/* =============================================================================
 *  achievements.c  --  Badges, overall stats and resetting progress.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Badges are not stored: each one is a rule checked against the save file
 *  and the question history every frame, so they can never get out of step.
 * ========================================================================== */
#include "screens.h"
#include "app.h"
#include "save.h"
#include "progress.h"
#include "ui.h"

#include <stdio.h>

typedef struct Badge {
    const char *icon;
    const char *title;
    const char *how;
    int (*value)(void);   /* current progress towards `target` */
    int target;
} Badge;

static int Lessons(void)  { return SaveLessonsDoneAll(); }
static int Streak(void)   { return SaveStreak(); }
static int Xp(void)       { return SaveXp(); }
static int Solved(void)   { return ProgressSolvedAll(); }
static int Perfect(void)  { return SaveHadPerfectQuiz() ? 1 : 0; }
static int BestMock(void)
{
    int a = SaveBestMock(MOCK_EXAM1), b = SaveBestMock(MOCK_EXAM2);
    return a > b ? (a < 0 ? 0 : a) : (b < 0 ? 0 : b);
}
static int PathsDone(void)
{
    int n = 0;
    for (int i = 0; i < PATH_COUNT; i++)
        if (PathNextLesson((PathId)i) < 0) n++;
    return n;
}

static const Badge kBadges[] = {
    { "1",  "First Steps",      "Complete your first lesson",          Lessons,   1 },
    { "5",  "On a Roll",        "Complete 5 lessons",                  Lessons,   5 },
    { "25", "Halfway Hero",     "Complete 25 lessons",                 Lessons,  25 },
    { "P",  "Path Finisher",    "Finish every lesson in a path",       PathsDone, 1 },
    { "3",  "Warming Up",       "Keep a 3-day streak",                 Streak,    3 },
    { "7",  "Week Warrior",     "Keep a 7-day streak",                 Streak,    7 },
    { "XP", "Level Up",         "Earn 500 XP",                         Xp,      500 },
    { "1k", "XP Hoarder",       "Earn 1000 XP",                        Xp,     1000 },
    { "*",  "Flawless",         "Score 100% on any quiz",              Perfect,   1 },
    { "E",  "Exam Ready",       "Pass a mock paper (50%+)",            BestMock, 50 },
    { "A+", "Top Marks",        "Score 80% or more on a mock",         BestMock, 80 },
    { "Q",  "Question Crusher", "Solve 25 different exam questions",   Solved,   25 },
};
#define BADGE_COUNT ((int)(sizeof kBadges / sizeof kBadges[0]))

static bool confirmReset = false;

static void DrawBadge(Rectangle r, const Badge *b)
{
    int v = b->value();
    bool got = v >= b->target;
    float t = b->target ? (float)v / b->target : 0;

    UiCard(r);
    Vector2 c = { r.x + 44, r.y + r.height / 2 };
    DrawCircleV(c, 26, got ? COL_GOLD : COL_PAGE);
    DrawCircleLinesV(c, 26, got ? (Color){ 220, 160, 40, 255 } : COL_LINE);
    int iw = MeasureText(b->icon, 16);
    DrawText(b->icon, (int)c.x - iw / 2, (int)c.y - 8, 16, got ? COL_WHITE : COL_MUTED);

    float x = r.x + 86, w = r.width - 106;
    DrawText(b->title, (int)x, (int)(r.y + 20), 15, got ? COL_NAVY : COL_INK);
    DrawText(b->how, (int)x, (int)(r.y + 42), 10, COL_MUTED);
    if (got) {
        DrawText("UNLOCKED", (int)x, (int)(r.y + 64), 10, COL_GOOD_FG);
    } else {
        char prog[32];
        snprintf(prog, sizeof prog, "%d / %d", v, b->target);
        UiProgressBar((Rectangle){ x, r.y + 68, w - 60, 6 }, t, COL_BLUE);
        DrawText(prog, (int)(x + w - 50), (int)(r.y + 64), 10, COL_MUTED);
    }
}

static void DrawStatTile(Rectangle r, const char *value, const char *label)
{
    UiCard(r);
    DrawText(value, (int)(r.x + 20), (int)(r.y + 16), 24, COL_NAVY);
    DrawText(label, (int)(r.x + 20), (int)(r.y + 48), 10, COL_MUTED);
}

void AchievementsDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x;
    int unlocked = 0;
    for (int i = 0; i < BADGE_COUNT; i++)
        if (kBadges[i].value() >= kBadges[i].target) unlocked++;

    char title[48];
    snprintf(title, sizeof title, "%d of %d badges unlocked", unlocked, BADGE_COUNT);
    UiPageTitle(x, "ACHIEVEMENTS", title);

    /* stats row */
    char a[24], b[24], c[24], d[24], e[24];
    snprintf(a, sizeof a, "%d", SaveLevel());
    snprintf(b, sizeof b, "%d XP", SaveXp());
    snprintf(c, sizeof c, "%d", SaveLessonsDoneAll());
    snprintf(d, sizeof d, "%d", SaveQuizzesFinished());
    snprintf(e, sizeof e, "%d days", SaveStreak());
    const char *values[] = { a, b, c, d, e };
    const char *labels[] = { SaveLevelTitle(), "total experience", "lessons completed",
                             "quizzes finished", "current streak" };
    float tw = (area.width - 4 * 14) / 5;
    for (int i = 0; i < 5; i++)
        DrawStatTile((Rectangle){ x + i * (tw + 14), 100, tw, 72 }, values[i], i == 0 ? "level" : labels[i]);
    DrawText(labels[0], (int)(x + 20 + MeasureText(a, 24) + 10), 124, 12, COL_BLUE);

    /* badge grid */
    int cols = area.width > 1400 ? 4 : 3;
    float bw = (area.width - (cols - 1) * 14) / cols, bh = 96;
    for (int i = 0; i < BADGE_COUNT; i++) {
        int r = i / cols, col = i % cols;
        DrawBadge((Rectangle){ x + col * (bw + 14), 196 + r * (bh + 14), bw, bh }, &kBadges[i]);
    }

    /* reset, with a second click to confirm */
    float ry = area.y + area.height - 40;
    const char *label = confirmReset ? "Click again to wipe ALL progress" : "Reset progress";
    if (UiButton((Rectangle){ x, ry, confirmReset ? 260 : 150, 34 }, label,
                 confirmReset ? COL_BAD_FG : COL_WHITE, confirmReset ? (Color){ 170, 40, 50, 255 } : COL_BAD_BG,
                 confirmReset ? COL_WHITE : COL_BAD_FG, 12)) {
        if (confirmReset) { SaveReset(); UiShowToast("Progress reset. A fresh start!"); confirmReset = false; }
        else confirmReset = true;
    }
    if (confirmReset && UiLink(x + 276, ry + 10, "Cancel", 12)) confirmReset = false;
}
