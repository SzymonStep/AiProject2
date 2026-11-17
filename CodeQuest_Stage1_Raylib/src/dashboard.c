/* =============================================================================
 *  dashboard.c  --  The home screen.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 * ========================================================================== */
#include "dashboard.h"
#include "ui.h"

#include <stdio.h>

/* ---------------------------------------------------------------------------
 *  Cards
 * ------------------------------------------------------------------------- */
static void DrawProgressCard(float x, float y)
{
    DrawRectangleRounded((Rectangle){ x, y, 360, 100 }, 0.12f, 12, COL_WHITE);

    /* ring: grey track, blue arc for 65%, white hole in the middle */
    Vector2 c = { x + 51, y + 50 };
    DrawCircleV(c, 29, (Color){ 233, 241, 248, 255 });
    DrawCircleSector(c, 29, -90, 144, 48, COL_BLUE);
    DrawCircleV(c, 22, COL_WHITE);

    DrawText("65%", (int)(x + 32), (int)(y + 44), 14, COL_NAVY);
    DrawText("CURRENT LEARNING PATH", (int)(x + 95), (int)(y + 24), 10, COL_MUTED);
    DrawText("C Fundamentals", (int)(x + 95), (int)(y + 43), 19, COL_NAVY);
    DrawText("13 of 20 lessons completed", (int)(x + 95), (int)(y + 70), 10, COL_MUTED);
}

static void DrawSmallStat(float x, float y, const char *icon, const char *label,
                          const char *value, const char *sub, Color iconBg)
{
    DrawRectangleRounded((Rectangle){ x, y, 245, 100 }, 0.12f, 12, COL_WHITE);
    DrawRectangleRounded((Rectangle){ x + 15, y + 28, 42, 42 }, 0.25f, 8, iconBg);
    DrawText(icon,  (int)(x + 28), (int)(y + 38), 20, COL_BLUE_DARK);
    DrawText(label, (int)(x + 70), (int)(y + 22), 10, COL_MUTED);
    DrawText(value, (int)(x + 70), (int)(y + 41), 20, COL_NAVY);
    DrawText(sub,   (int)(x + 70), (int)(y + 72),  9, COL_MUTED);
}

typedef struct PathCard {
    const char *icon;
    const char *title;
    const char *description;
    const char *status;
    float       progress;     /* 0..1 */
    Color       iconBg;
    const char *footerLeft;
} PathCard;

static void DrawPathCard(float x, float y, const PathCard *p)
{
    Rectangle card = { x, y, 272, 172 };
    bool over = UiIsHover(card);

    DrawRectangleRounded(card, 0.12f, 12, COL_WHITE);
    DrawRectangleRoundedLines(card, 0.12f, 12,
                              over ? (Color){ 190, 220, 245, 255 } : COL_LINE);

    DrawRectangleRounded((Rectangle){ x + 15, y + 14, 42, 42 }, 0.25f, 8, p->iconBg);
    DrawText(p->icon, (int)(x + 28), (int)(y + 24), 20, COL_BLUE_DARK);

    DrawRectangleRounded((Rectangle){ x + 163, y + 23, 94, 19 }, 0.4f, 8, COL_BLUE_LIGHT);
    DrawText(p->status, (int)(x + 171), (int)(y + 28), 8, COL_BLUE_DARK);

    DrawText(p->title,       (int)(x + 15), (int)(y + 69),  15, COL_NAVY);
    DrawText(p->description, (int)(x + 15), (int)(y + 94),   9, COL_MUTED);
    DrawText("Build skills one lesson at a time", (int)(x + 15), (int)(y + 108), 9, COL_MUTED);

    UiProgressBar((Rectangle){ x + 15, y + 133, 242, 6 }, p->progress, COL_BLUE);

    /* footer: count on the left, percentage right-aligned */
    char pct[8];
    snprintf(pct, sizeof pct, "%d%%", (int)(p->progress * 100.0f + 0.5f));
    DrawText(p->footerLeft, (int)(x + 15), (int)(y + 146), 8, COL_MUTED);
    DrawText(pct, (int)(x + 257) - MeasureText(pct, 8), (int)(y + 146), 8, COL_MUTED);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        char msg[96];
        snprintf(msg, sizeof msg, "%s selected. Lesson screens are coming next.", p->title);
        UiShowToast(msg);
    }
}

/* ---------------------------------------------------------------------------
 *  Sections
 * ------------------------------------------------------------------------- */
static void DrawHeader(void)
{
    DrawText("YOUR LEARNING HUB", 280, 30, 10, COL_BLUE);
    DrawText("Hey, Code Explorer!", 280, 51, 28, COL_NAVY);
    DrawText("7", 1178, 39, 16, COL_GOLD);
    DrawText("day streak", 1194, 42, 11, COL_INK);
    DrawText("*", 1301, 39, 18, COL_GOLD);
    DrawText("450 XP", 1320, 42, 11, COL_INK);
}

static void DrawWelcomePanel(void)
{
    DrawRectangleRounded((Rectangle){ 280, 105, 1118, 185 }, 0.12f, 16,
                         (Color){ 230, 243, 255, 255 });
    DrawText("YOUR NEXT QUEST IS READY", 310, 130, 10, COL_BLUE_DARK);
    DrawText("Small steps. Big coding wins.", 310, 153, 24, COL_NAVY);
    DrawText("Pick up where you left off and keep building your C and C++ skills.",
             310, 190, 12, COL_MUTED);
    DrawText("Every challenge gets you closer to exam-ready.", 310, 208, 12, COL_MUTED);

    if (UiButton((Rectangle){ 310, 239, 164, 34 }, "Continue learning  >",
                 COL_BLUE, COL_BLUE_DARK, COL_WHITE, 11))
        UiShowToast("Next up: Loops - repeat with confidence.");

    UiDrawRobot(1205, 140);
    DrawRectangleRounded((Rectangle){ 1270, 119, 102, 27 }, 0.3f, 8, COL_WHITE);
    DrawText("You've got this!", 1279, 127, 9, COL_BLUE_DARK);
}

static void DrawProgressRow(void)
{
    DrawText("YOUR PROGRESS", 280, 316, 16, COL_NAVY);
    DrawText("Look how far you've come.", 280, 337, 10, COL_MUTED);

    DrawProgressCard(280, 359);
    DrawSmallStat(655, 359, "!", "CURRENT STREAK", "7 days",
                  "One week of showing up!", (Color){ 255, 245, 223, 255 });
    DrawSmallStat(915, 359, "*", "TOTAL EXPERIENCE", "450 XP",
                  "50 XP to your next level", COL_BLUE_LIGHT);
}

static void DrawLearningPaths(void)
{
    static const PathCard paths[] = {
        { "C", "C Fundamentals", "Variables, conditions, loops and functions",
          "IN PROGRESS",   0.65f, COL_BLUE_LIGHT, "13 / 20 lessons" },
        { "E", "Coding Exam 1",  "Problem-solving, arrays, strings and functions",
          "EXAM PRACTICE", 0.20f, COL_CYAN_LIGHT, "2 / 10 practice sets" },
        { "+", "C++ Concepts",   "Classes, objects and memory management",
          "UP NEXT",       0.00f, COL_PURPLE,     "Not started" },
    };

    DrawText("CHOOSE YOUR LEARNING PATH", 280, 490, 16, COL_NAVY);
    DrawText("Build your skills one friendly lesson at a time.", 280, 511, 10, COL_MUTED);

    for (int i = 0; i < (int)(sizeof paths / sizeof paths[0]); ++i)
        DrawPathCard(280.0f + i * 285.0f, 533, &paths[i]);
}

static void DrawNextLesson(void)
{
    DrawText("PICK UP WHERE YOU LEFT OFF", 280, 734, 15, COL_NAVY);
    DrawText("YOUR NEXT BITE-SIZED LESSONS", 280, 755, 9, COL_MUTED);

    DrawRectangleRounded((Rectangle){ 280, 775, 690, 82 }, 0.12f, 10, COL_WHITE);
    DrawText("Loops: repeat with confidence", 300, 791, 12, COL_NAVY);
    DrawText("C Fundamentals  -  6 min", 300, 811, 9, COL_MUTED);

    if (UiButton((Rectangle){ 850, 795, 85, 29 }, "Start",
                 COL_BLUE, COL_BLUE_DARK, COL_WHITE, 10))
        UiShowToast("Lesson selected: Loops - repeat with confidence.");
}

static void DrawMiniQuest(void)
{
    DrawRectangleRounded((Rectangle){ 990, 734, 408, 123 }, 0.12f, 10, COL_WHITE);
    DrawText("TODAY'S MINI QUEST", 1010, 751, 13, COL_NAVY);
    DrawText("Complete 2 learning activities", 1010, 779, 11, COL_INK);
    DrawText("Earn 20 bonus XP when you finish.", 1010, 798, 9, COL_MUTED);
    UiProgressBar((Rectangle){ 1010, 824, 330, 7 }, 0.5f, COL_GREEN);
    DrawText("1 / 2", 1350, 819, 9, COL_MUTED);
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void DashboardDraw(void)
{
    DrawHeader();
    DrawWelcomePanel();
    DrawProgressRow();
    DrawLearningPaths();
    DrawNextLesson();
    DrawMiniQuest();
}
