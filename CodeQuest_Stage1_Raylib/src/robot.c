/* =============================================================================
 *  robot.c  --  "Ask the robot": a pop-up with study tips.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Drawn over whatever screen is showing. main.c moves the mouse out of reach
 *  of the screen underneath while it is open, so clicks only reach the pop-up.
 * ========================================================================== */
#include "screens.h"
#include "ui.h"

#include <stdlib.h>

static bool open = false;
static bool justOpened = false;   /* ignore the click that opened it */
static int  tip = 0;

static const char *kTips[] = {
    "Section A: trace the code on paper, one variable per column. Update a value only "
    "when the line that changes it runs.",
    "Section B: read the function's specification first, then ask what the blank must "
    "be for the loop to stop at the right place.",
    "Off-by-one check: for an array of n, valid indexes are 0 to n - 1. If your loop "
    "touches index n, something is wrong.",
    "When inserting into an array, shift from the end towards the gap. When deleting, "
    "shift from the gap towards the end.",
    "Integer division drops the fraction: 7 / 2 is 3. Make one side a double if you "
    "need the decimals.",
    "A C string is only finished when it has its '\\0'. If you build one by hand, write "
    "the terminator yourself.",
    "p + 1 moves a pointer one element, not one byte. That is why *(a + i) and a[i] "
    "are the same.",
    "Stuck on a question? Skip it and come back. In the real paper every mark is worth "
    "the same.",
    "Short sessions every day beat one long one. Keeping your streak alive is the "
    "easiest way to remember more.",
    "Got a question wrong? It is now in the Revision Centre. Drill it until it is "
    "mastered.",
    "printf output in Section A must match exactly, spaces and newlines included. Read "
    "every format string carefully.",
    "In C++, pass big objects by const reference to avoid copying them, and by "
    "reference when the function must change them.",
};
#define TIP_COUNT ((int)(sizeof kTips / sizeof kTips[0]))

bool RobotIsOpen(void) { return open; }

void RobotToggle(void)
{
    open = !open;
    if (open) { tip = rand() % TIP_COUNT; justOpened = true; }
}

void RobotDraw(void)
{
    if (!open) return;

    float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
    DrawRectangle(0, 0, (int)sw, (int)sh, COL_OVERLAY);

    Rectangle card = { (sw - 560) / 2, (sh - 300) / 2, 560, 300 };
    DrawRectangleRounded(card, 0.08f, 12, COL_SURFACE);
    UiDrawRobot(card.x + 30, card.y + 40);

    float x = card.x + 170, w = card.width - 200, y = card.y + 34;
    UiText("YOUR CODING ROBOT SAYS", (int)x, (int)y, 10, COL_BLUE);
    y += 24;
    UiTextWrapped(kTips[tip], x, y, w, 15, 8, COL_NAVY);

    float by = card.y + card.height - 64;
    if (UiButton((Rectangle){ x, by, 150, 38 }, "Another tip", COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 13))
        tip = (tip + 1 + rand() % (TIP_COUNT - 1)) % TIP_COUNT;
    if (UiButton((Rectangle){ x + 162, by, 120, 38 }, "Thanks!", COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
        open = false;

    /* clicking outside the card closes it too */
    if (!justOpened && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !UiIsHover(card)) open = false;
    if (IsKeyPressed(KEY_ESCAPE)) open = false;
    justOpened = false;
}
