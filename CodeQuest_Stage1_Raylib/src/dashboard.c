/* =============================================================================
 *  dashboard.c  --  The home screen.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Everything shown comes from the save file: the path you were last on,
 *  your streak and XP, the next lesson and today's mini quest. The layout is
 *  worked out from the window width.
 * ========================================================================== */
#include "dashboard.h"
#include "app.h"
#include "paths.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>

#define GAP 18.0f

/* Icon tile colour per path, matching the learning path screen. */
static Color PathTint(PathId id)
{
    switch (id) {
    case PATH_EXAM1:        return COL_CYAN_LIGHT;
    case PATH_EXAM2:        return (Color){ 255, 245, 223, 255 };
    case PATH_ADVANCED_CPP: return COL_PURPLE;
    default:                return COL_BLUE_LIGHT;
    }
}

/* Where "Continue learning" should go: the next lesson of the last path,
 * or the path screen if that path is finished. */
static void ContinueLearning(void)
{
    PathId path = SaveLastPath();
    int next = PathNextLesson(path);
    if (next >= 0) AppOpenLesson(path, next);
    else AppOpenPath(path);
}

/* ---------------------------------------------------------------------------
 *  Header and welcome
 * ------------------------------------------------------------------------- */
static void DrawHeader(float x, float right)
{
    DrawText("YOUR LEARNING HUB", (int)x, 30, 10, COL_BLUE);
    DrawText("Hey, Code Explorer!", (int)x, 51, 28, COL_NAVY);

    char xp[32], streak[32];
    snprintf(xp, sizeof xp, "%d XP", SaveXp());
    snprintf(streak, sizeof streak, "%d day streak", SaveStreak());
    float rx = right - MeasureText(xp, 12);
    DrawText(xp, (int)rx, 44, 12, COL_INK);
    DrawText("*", (int)rx - 18, 41, 18, COL_GOLD);
    rx -= 50 + MeasureText(streak, 12);
    DrawText(streak, (int)rx, 44, 12, COL_INK);
    DrawCircle((int)rx - 12, 50, 5, COL_GOLD);
}

static void DrawWelcome(Rectangle r)
{
    DrawRectangleRounded(r, 0.12f, 16, (Color){ 230, 243, 255, 255 });
    float x = r.x + 30, y = r.y + 25;

    int done = SaveLessonsDoneAll();
    const char *kicker = done == 0 ? "YOUR FIRST QUEST IS READY" : "YOUR NEXT QUEST IS READY";
    const char *headline = done == 0 ? "Welcome! Let's write some C." : "Small steps. Big coding wins.";
    DrawText(kicker, (int)x, (int)y, 10, COL_BLUE_DARK);
    DrawText(headline, (int)x, (int)(y + 23), 24, COL_NAVY);

    const LearningPath *p = PathGet(SaveLastPath());
    int next = PathNextLesson(SaveLastPath());
    char line[128];
    if (next >= 0)
        snprintf(line, sizeof line, "Next up in %s: %s.", p->title, p->lessons[next].title);
    else
        snprintf(line, sizeof line, "You finished %s. Pick a new path below!", p->title);
    DrawText(line, (int)x, (int)(y + 60), 12, COL_MUTED);
    DrawText("Every challenge gets you closer to exam-ready.", (int)x, (int)(y + 78), 12, COL_MUTED);

    if (UiButton((Rectangle){ x, y + 109, 180, 34 }, done == 0 ? "Start learning  >" : "Continue learning  >",
                 COL_BLUE, COL_BLUE_DARK, COL_WHITE, 11))
        ContinueLearning();

    float robotX = r.x + r.width - 190;
    UiDrawRobot(robotX, r.y + 35);
    DrawRectangleRounded((Rectangle){ robotX + 65, r.y + 14, 108, 27 }, 0.3f, 8, COL_WHITE);
    DrawText(SaveStreak() >= 3 ? "On fire! Keep going!" : "You've got this!", (int)robotX + 74, (int)r.y + 22, 9,
             COL_BLUE_DARK);
}

/* ---------------------------------------------------------------------------
 *  Progress row
 * ------------------------------------------------------------------------- */
static void DrawProgressCard(Rectangle card)
{
    PathId id = SaveLastPath();
    const LearningPath *p = PathGet(id);
    float x = card.x, y = card.y;

    UiCard(card);
    UiProgressRing((Vector2){ x + 51, y + 50 }, 29, 7, PathProgress(id), COL_BLUE, COL_WHITE);

    char sub[64];
    snprintf(sub, sizeof sub, "%d of %d %s completed", PathCompleted(id), p->lessonCount, p->unitWord);
    DrawText("CURRENT LEARNING PATH", (int)(x + 95), (int)(y + 24), 10, COL_MUTED);
    DrawText(p->title, (int)(x + 95), (int)(y + 43), 19, COL_NAVY);
    DrawText(sub, (int)(x + 95), (int)(y + 70), 10, COL_MUTED);

    if (UiIsClicked(card)) AppOpenPath(id);
}

static void DrawSmallStat(Rectangle card, const char *icon, const char *label,
                          const char *value, const char *sub, Color iconBg)
{
    float x = card.x, y = card.y;
    UiCard(card);
    DrawRectangleRounded((Rectangle){ x + 15, y + 28, 42, 42 }, 0.25f, 8, iconBg);
    DrawText(icon,  (int)(x + 36) - MeasureText(icon, 20) / 2, (int)(y + 38), 20, COL_BLUE_DARK);
    DrawText(label, (int)(x + 70), (int)(y + 22), 10, COL_MUTED);
    DrawText(value, (int)(x + 70), (int)(y + 41), 20, COL_NAVY);
    DrawText(sub,   (int)(x + 70), (int)(y + 72),  9, COL_MUTED);
}

static void DrawProgressRow(float x, float y, float width)
{
    DrawText("YOUR PROGRESS", (int)x, (int)y, 16, COL_NAVY);
    DrawText("Look how far you've come.", (int)x, (int)(y + 21), 10, COL_MUTED);
    y += 43;

    float progW = width * 0.38f, statW = (width - progW - 2 * GAP) / 2;
    DrawProgressCard((Rectangle){ x, y, progW, 100 });

    char streak[24], streakSub[48], xp[24], xpSub[48];
    int s = SaveStreak();
    snprintf(streak, sizeof streak, "%d day%s", s, s == 1 ? "" : "s");
    snprintf(streakSub, sizeof streakSub, "%s", s == 0 ? "Finish a quiz to start one!"
                                         : s >= 7 ? "A whole week. Amazing!" : "Come back tomorrow!");
    snprintf(xp, sizeof xp, "%d XP", SaveXp());
    snprintf(xpSub, sizeof xpSub, "Level %d %s - %d XP to next", SaveLevel(), SaveLevelTitle(),
             SaveXpToNextLevel());

    DrawSmallStat((Rectangle){ x + progW + GAP, y, statW, 100 }, "!", "CURRENT STREAK", streak, streakSub,
                  (Color){ 255, 245, 223, 255 });
    Rectangle xpCard = { x + progW + statW + 2 * GAP, y, statW, 100 };
    DrawSmallStat(xpCard, "*", "TOTAL EXPERIENCE", xp, xpSub, COL_BLUE_LIGHT);
    UiProgressBar((Rectangle){ xpCard.x + 70, y + 88, xpCard.width - 90, 4 }, SaveLevelProgress(), COL_GOLD);
}

/* ---------------------------------------------------------------------------
 *  Learning paths
 * ------------------------------------------------------------------------- */
static void DrawPathCard(Rectangle card, PathId id)
{
    const LearningPath *p = PathGet(id);
    float x = card.x, y = card.y, w = card.width;
    bool over = UiIsHover(card);
    int done = PathCompleted(id);

    DrawRectangleRounded(card, 0.12f, 12, COL_WHITE);
    DrawRectangleRoundedLines(card, 0.12f, 12, over ? (Color){ 190, 220, 245, 255 } : COL_LINE);

    DrawRectangleRounded((Rectangle){ x + 15, y + 14, 42, 42 }, 0.25f, 8, PathTint(id));
    DrawText(p->icon, (int)(x + 36) - MeasureText(p->icon, 20) / 2, (int)(y + 24), 20, COL_BLUE_DARK);

    const char *badge = done == p->lessonCount ? "COMPLETE" : done > 0 ? "IN PROGRESS" : p->badge;
    UiBadge((Rectangle){ x + w - 109, y + 23, 94, 19 }, badge,
            done == p->lessonCount ? COL_GOOD_FG : COL_BLUE_DARK,
            done == p->lessonCount ? COL_GOOD_BG : COL_BLUE_LIGHT, 8);

    DrawText(p->title,   (int)(x + 15), (int)(y + 69), 15, COL_NAVY);
    DrawText(p->tagline, (int)(x + 15), (int)(y + 94),  9, COL_MUTED);
    char meta[48];
    snprintf(meta, sizeof meta, "%d %s", p->lessonCount, p->unitWord);
    DrawText(meta, (int)(x + 15), (int)(y + 108), 9, COL_MUTED);

    float t = PathProgress(id);
    UiProgressBar((Rectangle){ x + 15, y + 133, w - 30, 6 }, t, COL_BLUE);

    char count[48], pct[8];
    if (done == 0) snprintf(count, sizeof count, "Not started");
    else snprintf(count, sizeof count, "%d / %d %s", done, p->lessonCount, p->unitWord);
    snprintf(pct, sizeof pct, "%d%%", (int)(t * 100.0f + 0.5f));
    DrawText(count, (int)(x + 15), (int)(y + 146), 8, COL_MUTED);
    DrawText(pct, (int)(x + w - 15) - MeasureText(pct, 8), (int)(y + 146), 8, COL_MUTED);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AppOpenPath(id);
}

static void DrawLearningPaths(float x, float y, float width)
{
    DrawText("CHOOSE YOUR LEARNING PATH", (int)x, (int)y, 16, COL_NAVY);
    DrawText("Build your skills one friendly lesson at a time.", (int)x, (int)(y + 21), 10, COL_MUTED);
    float w = (width - GAP * (PATH_COUNT - 1)) / PATH_COUNT;
    for (int i = 0; i < PATH_COUNT; i++)
        DrawPathCard((Rectangle){ x + i * (w + GAP), y + 43, w, 172 }, (PathId)i);
}

/* ---------------------------------------------------------------------------
 *  Next lesson and mini quest
 * ------------------------------------------------------------------------- */
static void DrawNextLesson(float x, float y, float w)
{
    DrawText("PICK UP WHERE YOU LEFT OFF", (int)x, (int)y, 15, COL_NAVY);
    DrawText("YOUR NEXT BITE-SIZED LESSON", (int)x, (int)(y + 21), 9, COL_MUTED);

    Rectangle card = { x, y + 41, w, 82 };
    UiCard(card);

    PathId path = SaveLastPath();
    const LearningPath *p = PathGet(path);
    int next = PathNextLesson(path);
    if (next < 0) {
        DrawText("Path complete! Pick a new one above.", (int)x + 20, (int)card.y + 32, 13, COL_NAVY);
        return;
    }

    char sub[96];
    snprintf(sub, sizeof sub, "%s  -  Lesson %d of %d  -  %d min",
             p->title, next + 1, p->lessonCount, p->lessons[next].minutes);
    DrawText(p->lessons[next].title, (int)x + 20, (int)card.y + 22, 14, COL_NAVY);
    DrawText(sub, (int)x + 20, (int)card.y + 46, 10, COL_MUTED);

    if (UiButton((Rectangle){ x + w - 115, card.y + 24, 95, 34 }, "Start", COL_BLUE, COL_BLUE_DARK, COL_WHITE, 12))
        AppOpenLesson(path, next);
}

static void DrawMiniQuest(float x, float y, float w)
{
    Rectangle card = { x, y, w, 123 };
    UiCard(card);
    int done = SaveActivitiesToday();
    if (done > QUEST_TARGET) done = QUEST_TARGET;

    DrawText("TODAY'S MINI QUEST", (int)x + 20, (int)y + 17, 13, COL_NAVY);
    char goal[64], bonus[64], count[16];
    snprintf(goal, sizeof goal, "Finish %d quizzes or practice sets", QUEST_TARGET);
    if (done >= QUEST_TARGET) snprintf(bonus, sizeof bonus, "Quest complete: +%d XP earned!", XP_QUEST_BONUS);
    else snprintf(bonus, sizeof bonus, "Earn %d bonus XP when you finish.", XP_QUEST_BONUS);
    snprintf(count, sizeof count, "%d / %d", done, QUEST_TARGET);

    DrawText(goal, (int)x + 20, (int)y + 45, 11, COL_INK);
    DrawText(bonus, (int)x + 20, (int)y + 64, 9, done >= QUEST_TARGET ? COL_GOOD_FG : COL_MUTED);
    UiProgressBar((Rectangle){ x + 20, y + 90, w - 90, 7 }, (float)done / QUEST_TARGET, COL_GREEN);
    DrawText(count, (int)(x + w - 55), (int)y + 85, 10, COL_MUTED);
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void DashboardDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x, w = area.width;

    DrawHeader(x, x + w);
    DrawWelcome((Rectangle){ x, 105, w, 175 });
    DrawProgressRow(x, 306, w);
    DrawLearningPaths(x, 480, w);

    float questW = w * 0.36f;
    DrawNextLesson(x, 724, w - questW - GAP);
    DrawMiniQuest(x + w - questW, 724, questW);
}
