/* =============================================================================
 *  arena.c  --  Exam Arena: timed mock papers.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Coding Exam 1 draws from Labs 1 and 2, Coding Exam 2 from Lab 3. A quick
 *  mock is 10 questions split evenly between Section A and B; the full paper
 *  is every question for that exam.
 * ========================================================================== */
#include "screens.h"
#include "app.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>

typedef struct ExamDef {
    const char *title;
    const char *scope;
    Topic       topics[2];
    int         topicCount;
    MockExam    mock;
} ExamDef;

static const ExamDef kExams[MOCK_COUNT] = {
    { "Coding Exam 1", "Labs 1 and 2: core logic, 1D arrays, 2D grids",
      { TOPIC_LAB1, TOPIC_LAB2 }, 2, MOCK_EXAM1 },
    { "Coding Exam 2", "Lab 3: pointers and C strings",
      { TOPIC_LAB3, TOPIC_LAB3 }, 1, MOCK_EXAM2 },
};

static int BuildPool(const ExamDef *e, int *pool)
{
    int n = 0;
    for (int i = 0; i < QUESTION_COUNT && n < 256; i++)
        for (int t = 0; t < e->topicCount; t++)
            if (QUESTIONS[i].topic == e->topics[t]) { pool[n++] = i; break; }
    return n;
}

static void DrawExamCard(Rectangle r, const ExamDef *e)
{
    int pool[256];
    int n = BuildPool(e, pool);
    int best = SaveBestMock(e->mock);
    float x = r.x + 26, y = r.y + 24, w = r.width - 52;

    UiCard(r);
    DrawRectangleRounded((Rectangle){ x, y, 48, 48 }, 0.25f, 8, e->mock == MOCK_EXAM1 ? COL_CYAN_LIGHT
                         : (Color){ 255, 245, 223, 255 });
    DrawText(e->mock == MOCK_EXAM1 ? "1" : "2", (int)(x + 18), (int)(y + 13), 22, COL_BLUE_DARK);
    DrawText(e->title, (int)(x + 64), (int)(y + 4), 20, COL_NAVY);
    DrawText(e->scope, (int)(x + 64), (int)(y + 30), 11, COL_MUTED);
    y += 76;

    char line[64];
    if (best >= 0) snprintf(line, sizeof line, "Best mock score: %d%%", best);
    else snprintf(line, sizeof line, "No mock attempted yet");
    DrawText(line, (int)x, (int)y, 14, best >= 50 ? COL_GOOD_FG : COL_INK);
    if (best >= 0) UiProgressBar((Rectangle){ x, y + 24, w, 8 }, best / 100.0f, best >= 50 ? COL_GREEN : COL_GOLD);
    y += 48;

    snprintf(line, sizeof line, "%d questions in the bank for this exam.", n);
    DrawText(line, (int)x, (int)y, 12, COL_MUTED);
    y += 30;

    char title[64];
    snprintf(title, sizeof title, "%s - mock paper", e->title);
    if (UiButton((Rectangle){ x, y, w, 40 }, "Quick mock: 10 questions  >", COL_BLUE, COL_BLUE_DARK, COL_WHITE, 13))
        AppStartPractice(title, MODE_EXAM, pool, n, 10, true, SCR_ARENA, e->mock);
    y += 50;
    snprintf(line, sizeof line, "Full paper: all %d questions", n);
    snprintf(title, sizeof title, "%s - full paper", e->title);
    if (UiButton((Rectangle){ x, y, w, 40 }, line, COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 13))
        AppStartPractice(title, MODE_EXAM, pool, n, 0, true, SCR_ARENA, e->mock);
}

void ArenaDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x;
    UiPageTitle(x, "EXAM ARENA", "Sit a mock paper");
    DrawText("Practise under exam conditions. Your best score for each exam is saved.",
             (int)x, 92, 12, COL_MUTED);

    float cardW = (area.width - 18) / 2;
    if (cardW > 560) cardW = 560;
    for (int i = 0; i < MOCK_COUNT; i++)
        DrawExamCard((Rectangle){ x + i * (cardW + 18), 125, cardW, 330 }, &kExams[i]);

    Rectangle rules = { x, 475, cardW * 2 + 18, 190 };
    UiCard(rules);
    float rx = rules.x + 26, ry = rules.y + 22;
    DrawText("HOW THE MOCK WORKS", (int)rx, (int)ry, 10, COL_MUTED);
    ry += 24;
    static const char *kRules[] = {
        "Section A questions come first, then Section B, like the real paper.",
        "You get about 90 seconds per mark. The clock turns red in the last minute.",
        "Answers are marked at the end: no feedback while the clock is running.",
        "Skip anything you are stuck on; a skipped question scores zero.",
        "50% or more is a pass. The last lesson of each Coding Exam path is this same mock.",
    };
    for (int i = 0; i < 5; i++) {
        DrawCircle((int)(rx + 8), (int)(ry + 7), 8, COL_BLUE_LIGHT);
        UiTick(rx + 8, ry + 7, 3.5f, COL_BLUE);
        DrawText(kRules[i], (int)(rx + 26), (int)ry, 13, COL_INK);
        ry += 28;
    }
}
