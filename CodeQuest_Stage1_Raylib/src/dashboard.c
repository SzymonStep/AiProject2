/* =============================================================================
 *  dashboard.c  --  The home screen.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Everything shown comes from the save file: the path you were last on,
 *  your streak and XP, the next lesson and today's mini quest. The layout is
 *  worked out from the window width, and the page scrolls when the window is
 *  too short to show it all.
 * ========================================================================== */
#include "dashboard.h"
#include "app.h"
#include "paths.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>

#define GAP      24.0f   /* between cards              */
#define SECTION  44.0f   /* between sections           */
#define TITLE_H  48.0f   /* section title + subtitle   */
#define STAT_H   116
#define PATH_H   190
#define BOTTOM_H 104

static float scroll = 0.0f;
static float contentH = 0.0f;   /* measured each frame for the scrollbar */

/* Icon tile colour per path, matching the learning path screen. */
static Color PathTint(PathId id)
{
    switch (id) {
    case PATH_EXAM1:        return COL_CYAN_LIGHT;
    case PATH_EXAM2:        return COL_GOLD_LIGHT;
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

static void SectionTitle(float x, float y, const char *title, const char *sub)
{
    UiText(title, (int)x, (int)y, 17, COL_NAVY);
    UiText(sub, (int)x, (int)(y + 25), 11, COL_MUTED);
}

/* ---------------------------------------------------------------------------
 *  Header and welcome
 * ------------------------------------------------------------------------- */
static void DrawHeader(float x, float y, float right)
{
    UiText("YOUR LEARNING HUB", (int)x, (int)y, 10, COL_BLUE);
    UiText("Hey, Code Explorer!", (int)x, (int)y + 21, 28, COL_NAVY);

    /* streak and XP pills on the right */
    char xp[32], streak[32];
    snprintf(xp, sizeof xp, "%d XP", SaveXp());
    snprintf(streak, sizeof streak, "%d day streak", SaveStreak());
    float pillH = 34, py = y + 10;
    float xpW = (float)UiMeasure(xp, 12) + 44, stW = (float)UiMeasure(streak, 12) + 44;
    Rectangle xpPill = { right - xpW, py, xpW, pillH };
    Rectangle stPill = { xpPill.x - 12 - stW, py, stW, pillH };
    DrawRectangleRounded(xpPill, 1.0f, 12, COL_SURFACE);
    DrawRectangleRounded(stPill, 1.0f, 12, COL_SURFACE);
    DrawCircle((int)xpPill.x + 18, (int)(py + pillH / 2), 6, COL_GOLD);
    UiText(xp, (int)xpPill.x + 32, (int)py + 10, 12, COL_INK);
    DrawCircle((int)stPill.x + 18, (int)(py + pillH / 2), 6, COL_GREEN);
    UiText(streak, (int)stPill.x + 32, (int)py + 10, 12, COL_INK);
}

static void DrawWelcome(Rectangle r)
{
    DrawRectangleRounded(r, 0.12f, 16, COL_HERO);
    float x = r.x + 36, y = r.y + 30;

    int done = SaveLessonsDoneAll();
    const char *kicker = done == 0 ? "YOUR FIRST QUEST IS READY" : "YOUR NEXT QUEST IS READY";
    const char *headline = done == 0 ? "Welcome! Let's write some C." : "Small steps. Big coding wins.";
    UiText(kicker, (int)x, (int)y, 10, COL_BLUE_DARK);
    UiText(headline, (int)x, (int)(y + 24), 26, COL_NAVY);

    const LearningPath *p = PathGet(SaveLastPath());
    int next = PathNextLesson(SaveLastPath());
    char line[128];
    if (next >= 0)
        snprintf(line, sizeof line, "Next up in %s: %s.", p->title, p->lessons[next].title);
    else
        snprintf(line, sizeof line, "You finished %s. Pick a new path below!", p->title);
    UiText(line, (int)x, (int)(y + 68), 13, COL_INK);

    if (UiButton((Rectangle){ x, y + 106, 200, 40 }, done == 0 ? "Start learning  >" : "Continue learning  >",
                 COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
        ContinueLearning();

    float robotX = r.x + r.width - 200;
    UiDrawRobot(robotX, r.y + 44);
    const char *bubble = SaveStreak() >= 3 ? "On fire! Keep going!" : "You've got this!";
    float bw = (float)UiMeasure(bubble, 10) + 24;
    DrawRectangleRounded((Rectangle){ robotX + 60, r.y + 18, bw, 28 }, 0.5f, 8, COL_SURFACE);
    UiText(bubble, (int)(robotX + 72), (int)r.y + 25, 10, COL_BLUE_DARK);
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
    if (UiIsHover(card)) DrawRectangleRoundedLines(card, 0.1f, 12, COL_HOVER_LINE);
    UiProgressRing((Vector2){ x + 60, y + STAT_H / 2.0f }, 34, 8, PathProgress(id), COL_BLUE, COL_SURFACE);

    char sub[64];
    snprintf(sub, sizeof sub, "%d of %d %s completed", PathCompleted(id), p->lessonCount, p->unitWord);
    UiText("CURRENT LEARNING PATH", (int)(x + 114), (int)(y + 26), 10, COL_MUTED);
    UiText(p->title, (int)(x + 114), (int)(y + 46), 19, COL_NAVY);
    UiText(sub, (int)(x + 114), (int)(y + 78), 11, COL_MUTED);

    if (UiIsClicked(card)) AppOpenPath(id);
}

static void DrawSmallStat(Rectangle card, const char *icon, const char *label,
                          const char *value, const char *sub, Color iconBg)
{
    float x = card.x, y = card.y;
    UiCard(card);
    DrawRectangleRounded((Rectangle){ x + 20, y + 33, 50, 50 }, 0.25f, 8, iconBg);
    UiText(icon,  (int)(x + 45) - UiMeasure(icon, 20) / 2, (int)(y + 46), 20, COL_BLUE_DARK);
    UiText(label, (int)(x + 88), (int)(y + 24), 10, COL_MUTED);
    UiText(value, (int)(x + 88), (int)(y + 44), 20, COL_NAVY);
    UiText(sub,   (int)(x + 88), (int)(y + 78), 11, COL_MUTED);
}

static float DrawProgressRow(float x, float y, float width)
{
    SectionTitle(x, y, "YOUR PROGRESS", "Look how far you've come.");
    y += TITLE_H;

    float progW = width * 0.38f, statW = (width - progW - 2 * GAP) / 2;
    DrawProgressCard((Rectangle){ x, y, progW, STAT_H });

    char streak[24], streakSub[48], xp[24], xpSub[48];
    int s = SaveStreak();
    snprintf(streak, sizeof streak, "%d day%s", s, s == 1 ? "" : "s");
    snprintf(streakSub, sizeof streakSub, "%s", s == 0 ? "Finish a quiz to start one!"
                                         : s >= 7 ? "A whole week. Amazing!" : "Come back tomorrow!");
    snprintf(xp, sizeof xp, "%d XP", SaveXp());
    snprintf(xpSub, sizeof xpSub, "Level %d - %d XP to next", SaveLevel(), SaveXpToNextLevel());

    DrawSmallStat((Rectangle){ x + progW + GAP, y, statW, STAT_H }, "!", "CURRENT STREAK", streak, streakSub,
                  COL_GOLD_LIGHT);
    Rectangle xpCard = { x + progW + statW + 2 * GAP, y, statW, STAT_H };
    DrawSmallStat(xpCard, "*", "TOTAL EXPERIENCE", xp, xpSub, COL_BLUE_LIGHT);
    UiProgressBar((Rectangle){ xpCard.x + 88, y + STAT_H - 14, xpCard.width - 112, 5 }, SaveLevelProgress(), COL_GOLD);
    return y + STAT_H;
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

    DrawRectangleRounded(card, 0.1f, 12, COL_SURFACE);
    DrawRectangleRoundedLines(card, 0.1f, 12, over ? COL_HOVER_LINE : COL_LINE);

    DrawRectangleRounded((Rectangle){ x + 20, y + 20, 46, 46 }, 0.25f, 8, PathTint(id));
    UiText(p->icon, (int)(x + 43) - UiMeasure(p->icon, 20) / 2, (int)(y + 32), 20, COL_BLUE_DARK);

    const char *badge = done == p->lessonCount ? "COMPLETE" : done > 0 ? "IN PROGRESS" : p->badge;
    float bw = (float)UiMeasure(badge, 9) + 24;
    UiBadge((Rectangle){ x + w - 20 - bw, y + 32, bw, 22 }, badge,
            done == p->lessonCount ? COL_GOOD_FG : COL_BLUE_DARK,
            done == p->lessonCount ? COL_GOOD_BG : COL_BLUE_LIGHT, 9);

    UiText(p->title,   (int)(x + 20), (int)(y + 84), 16, COL_NAVY);
    UiText(p->tagline, (int)(x + 20), (int)(y + 112), 10, COL_MUTED);

    float t = PathProgress(id);
    UiProgressBar((Rectangle){ x + 20, y + 146, w - 40, 7 }, t, COL_BLUE);

    char count[48], pct[8];
    if (done == 0) snprintf(count, sizeof count, "Not started  -  %d %s", p->lessonCount, p->unitWord);
    else snprintf(count, sizeof count, "%d / %d %s", done, p->lessonCount, p->unitWord);
    snprintf(pct, sizeof pct, "%d%%", (int)(t * 100.0f + 0.5f));
    UiText(count, (int)(x + 20), (int)(y + 161), 10, COL_MUTED);
    UiText(pct, (int)(x + w - 20) - UiMeasure(pct, 10), (int)(y + 161), 10, COL_MUTED);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AppOpenPath(id);
}

static float DrawLearningPaths(float x, float y, float width)
{
    SectionTitle(x, y, "CHOOSE YOUR LEARNING PATH", "Build your skills one friendly lesson at a time.");
    y += TITLE_H;
    float w = (width - GAP * (PATH_COUNT - 1)) / PATH_COUNT;
    for (int i = 0; i < PATH_COUNT; i++)
        DrawPathCard((Rectangle){ x + i * (w + GAP), y, w, PATH_H }, (PathId)i);
    return y + PATH_H;
}

/* ---------------------------------------------------------------------------
 *  Next lesson and mini quest
 * ------------------------------------------------------------------------- */
static void DrawNextLesson(float x, float y, float w)
{
    SectionTitle(x, y, "PICK UP WHERE YOU LEFT OFF", "Your next bite-sized lesson.");
    Rectangle card = { x, y + TITLE_H, w, BOTTOM_H };
    UiCard(card);

    PathId path = SaveLastPath();
    const LearningPath *p = PathGet(path);
    int next = PathNextLesson(path);
    if (next < 0) {
        UiText("Path complete! Pick a new one above.", (int)x + 24, (int)card.y + 40, 14, COL_NAVY);
        return;
    }

    char sub[96];
    snprintf(sub, sizeof sub, "%s  -  Lesson %d of %d  -  %d min",
             p->title, next + 1, p->lessonCount, p->lessons[next].minutes);
    UiText(p->lessons[next].title, (int)x + 24, (int)card.y + 28, 16, COL_NAVY);
    UiText(sub, (int)x + 24, (int)card.y + 58, 11, COL_MUTED);

    if (UiButton((Rectangle){ x + w - 140, card.y + 32, 116, 40 }, "Start  >",
                 COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
        AppOpenLesson(path, next);
}

static void DrawMiniQuest(float x, float y, float w)
{
    SectionTitle(x, y, "TODAY'S MINI QUEST", "A little bonus for showing up.");
    Rectangle card = { x, y + TITLE_H, w, BOTTOM_H };
    UiCard(card);
    int done = SaveActivitiesToday();
    if (done > QUEST_TARGET) done = QUEST_TARGET;

    char goal[64], bonus[64], count[16];
    snprintf(goal, sizeof goal, "Finish %d quizzes or practice sets", QUEST_TARGET);
    if (done >= QUEST_TARGET) snprintf(bonus, sizeof bonus, "Quest complete: +%d XP earned!", XP_QUEST_BONUS);
    else snprintf(bonus, sizeof bonus, "Earn %d bonus XP when you finish.", XP_QUEST_BONUS);
    snprintf(count, sizeof count, "%d / %d", done, QUEST_TARGET);

    UiText(goal, (int)x + 24, (int)card.y + 22, 13, COL_INK);
    UiText(bonus, (int)x + 24, (int)card.y + 46, 11, done >= QUEST_TARGET ? COL_GOOD_FG : COL_MUTED);
    UiProgressBar((Rectangle){ x + 24, card.y + 76, w - 110, 8 }, (float)done / QUEST_TARGET, COL_GREEN);
    UiText(count, (int)(x + w - 70), (int)card.y + 72, 11, COL_MUTED);
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void DashboardDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x, w = area.width;

    /* the whole page scrolls when the window is too short for it */
    Rectangle view = { x - 16, 0, w + 40, (float)GetScreenHeight() };
    UiScrollArea(view, contentH, &scroll);

    float top = 30 - scroll;
    float y = top;
    DrawHeader(x, y, x + w);
    y += 84;

    DrawWelcome((Rectangle){ x, y, w, 190 });
    y += 190 + SECTION;

    y = DrawProgressRow(x, y, w) + SECTION;
    y = DrawLearningPaths(x, y, w) + SECTION;

    float questW = w * 0.36f;
    DrawNextLesson(x, y, w - questW - GAP);
    DrawMiniQuest(x + w - questW, y, questW);
    y += TITLE_H + BOTTOM_H;

    contentH = (y - top) + 54;
}
