/* =============================================================================
 *  challenges.c  --  Quick drills from the question bank.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Pick a lab (or all of them), then one of three drills. Untimed, with
 *  feedback after every answer.
 * ========================================================================== */
#include "screens.h"
#include "app.h"
#include "progress.h"
#include "ui.h"

#include <stdio.h>

static int topicFilter = -1;   /* -1 = every lab */

typedef struct Drill {
    const char *icon;
    const char *title;
    const char *blurb;
    QuizMode    mode;
    int         length;
} Drill;

static const Drill kDrills[] = {
    { "A", "Output sprint",   "Section A only. Read the code, type exactly what it prints.", MODE_SECTION_A, 8 },
    { "B", "Fill the blanks", "Section B only. Supply the missing operators, bounds and expressions.", MODE_SECTION_B, 6 },
    { "?", "Mixed bag",       "Both sections, shuffled. The closest thing to revising for real.", MODE_MIXED, 10 },
};

static int BuildPool(int *pool)
{
    int n = 0;
    for (int i = 0; i < QUESTION_COUNT; i++)
        if (topicFilter < 0 || (int)QUESTIONS[i].topic == topicFilter) pool[n++] = i;
    return n;
}

static void DrawTopicChips(float x, float y)
{
    static const char *labels[] = { "All labs", "Lab 1", "Lab 2", "Lab 3" };
    for (int i = 0; i < 4; i++) {
        int value = i - 1;
        float w = (float)MeasureText(labels[i], 13) + 36;
        Rectangle r = { x, y, w, 34 };
        bool sel = value == topicFilter;
        bool over = UiIsHover(r);
        DrawRectangleRounded(r, 0.5f, 8, sel ? COL_BLUE : (over ? COL_BLUE_LIGHT : COL_WHITE));
        DrawText(labels[i], (int)(x + 18), (int)(y + 10), 13, sel ? COL_WHITE : COL_BLUE_DARK);
        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) topicFilter = value;
        x += w + 10;
    }
}

static void DrawDrillCard(Rectangle r, const Drill *d)
{
    int pool[256];
    int n = BuildPool(pool);
    int available = 0;
    for (int i = 0; i < n; i++) {
        QKind k = QUESTIONS[pool[i]].kind;
        if (d->mode == MODE_MIXED || (d->mode == MODE_SECTION_A) == (k == Q_OUTPUT)) available++;
    }
    int length = available < d->length ? available : d->length;

    UiCard(r);
    float x = r.x + 24, y = r.y + 24, w = r.width - 48;
    DrawRectangleRounded((Rectangle){ x, y, 46, 46 }, 0.25f, 8, COL_BLUE_LIGHT);
    DrawText(d->icon, (int)(x + 23) - MeasureText(d->icon, 22) / 2, (int)(y + 12), 22, COL_BLUE_DARK);
    y += 62;
    DrawText(d->title, (int)x, (int)y, 18, COL_NAVY);
    y += 28;
    y += UiTextWrapped(d->blurb, x, y, w, 12, 5, COL_MUTED) + 10;

    char line[48];
    snprintf(line, sizeof line, "%d questions  -  +%d XP each", length, 10);
    DrawText(line, (int)x, (int)y, 11, COL_INK);

    Rectangle btn = { x, r.y + r.height - 64, w, 40 };
    if (length == 0) {
        DrawText("No questions of this kind here.", (int)x, (int)btn.y + 12, 12, COL_MUTED);
    } else if (UiButton(btn, "Start  >", COL_BLUE, COL_BLUE_DARK, COL_WHITE, 13)) {
        AppStartPractice(d->title, d->mode, pool, n, d->length, true, SCR_CHALLENGES, -1);
    }
}

static void DrawRecord(Rectangle r)
{
    UiCard(r);
    float x = r.x + 24, y = r.y + 22, w = r.width - 48;
    DrawText("YOUR RECORD BY LAB", (int)x, (int)y, 10, COL_MUTED);
    y += 26;
    for (int t = 0; t < TOPIC_COUNT; t++) {
        int solved = TopicSolved((Topic)t), total = TopicQuestionCount((Topic)t);
        char line[64];
        snprintf(line, sizeof line, "%d / %d solved", solved, total);
        DrawText(TopicName((Topic)t), (int)x, (int)y, 13, COL_NAVY);
        DrawText(line, (int)(x + w) - MeasureText(line, 11), (int)(y + 2), 11, COL_MUTED);
        UiProgressBar((Rectangle){ x, y + 22, w, 7 }, total ? (float)solved / total : 0, COL_BLUE);
        y += 50;
    }
    char all[64];
    snprintf(all, sizeof all, "%d answers given, %d fully correct", ProgressAttempts(), ProgressCorrect());
    DrawText(all, (int)x, (int)y, 11, COL_MUTED);
}

void ChallengesDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x;
    UiPageTitle(x, "CHALLENGES", "Quick drills");
    DrawText("Short, untimed sets from the exam question bank. Pick a lab, then a drill.",
             (int)x, 92, 12, COL_MUTED);

    DrawTopicChips(x, 118);

    float recordW = 380;
    float cardsW = area.width - recordW - 18;
    float cardW = (cardsW - 2 * 18) / 3;
    for (int i = 0; i < 3; i++)
        DrawDrillCard((Rectangle){ x + i * (cardW + 18), 172, cardW, 300 }, &kDrills[i]);
    DrawRecord((Rectangle){ x + cardsW + 18, 172, recordW, 300 });
}
