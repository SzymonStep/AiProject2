/* =============================================================================
 *  sidebar.c  --  Left-hand navigation.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 * ========================================================================== */
#include "sidebar.h"
#include "ui.h"
#include "app.h"
#include "save.h"

#include <stdio.h>

typedef struct NavEntry {
    const char *label;
    const char *icon;   /* single character until real icons are added */
} NavEntry;

static const NavEntry NAV[NAV_COUNT] = {
    [NAV_DASHBOARD]     = { "Dashboard",       "H" },
    [NAV_LEARNING_PATH] = { "Learning Path",   "P" },
    [NAV_LEARN_C]       = { "Learn C",         "C" },
    [NAV_LEARN_CPP]     = { "Learn C++",       "+" },
    [NAV_CHALLENGES]    = { "Challenges",      "!" },
    [NAV_EXAM_ARENA]    = { "Exam Arena",      "E" },
    [NAV_REVISION]      = { "Revision Centre", "R" },
    [NAV_ACHIEVEMENTS]  = { "Achievements",    "*" },
};

static void DrawLogo(void)
{
    DrawRectangleRounded((Rectangle){ 20, 22, 43, 43 }, 0.28f, 8, COL_BLUE);
    UiText("CQ", 26, 34, 18, COL_ON_ACCENT);
    UiText("CodeQuest", 74, 25, 20, COL_NAVY);
    UiText("LEARN  PRACTISE  MASTER", 75, 49, 8, COL_MUTED);
}

#define ITEM_H     40      /* height of one menu item (and its click area) */
#define ITEM_GAP    6
#define GROUP_GAP  30      /* room for a group heading above an item       */
#define PROFILE_H  76      /* profile row pinned to the bottom              */
#define HELP_H     92

/* Draws a group heading and returns how far it pushes everything below. */
static int GroupHeading(const char *text, int y)
{
    UiText(text, 24, y + 10, 10, COL_MUTED);
    return GROUP_GAP;
}

/* Returns the y just below the last item, so the help card can avoid it. */
static int DrawMenu(NavItem active)
{
    /* y runs down the list and every heading adds to it, so a heading
     * pushes ALL later items down, not just the one under it. That keeps
     * each item's drawn box and its click area exactly the same. */
    int y = 92;
    y += GroupHeading("YOUR SPACE", y);

    for (int i = 0; i < NAV_COUNT; ++i) {
        if (i == NAV_EXAM_ARENA)   y += GroupHeading("EXAM PREP", y);
        if (i == NAV_ACHIEVEMENTS) y += GroupHeading("YOUR PROGRESS", y);

        Rectangle item = { 12, (float)y, SIDEBAR_W - 24, ITEM_H };
        bool selected = (i == (int)active);
        bool over = UiIsHover(item);

        if (selected)  DrawRectangleRounded(item, 0.22f, 8, COL_BLUE_LIGHT);
        else if (over) DrawRectangleRounded(item, 0.22f, 8, COL_PAGE);

        Color itemColor = selected ? COL_BLUE_DARK : COL_INK;
        int cy = y + ITEM_H / 2;
        DrawCircle(34, cy, 12, selected ? COL_SURFACE : COL_PAGE);
        int iconW = UiMeasure(NAV[i].icon, 12);
        UiText(NAV[i].icon, 34 - iconW / 2, cy - 7, 12, selected ? COL_BLUE_DARK : COL_MUTED);
        UiText(NAV[i].label, 56, cy - 8, 14, itemColor);

        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            AppNavigate((NavItem)i);

        y += ITEM_H + ITEM_GAP;
    }
    return y;
}

static void DrawHelpCard(int menuBottom)
{
    int y = GetScreenHeight() - PROFILE_H - 14 - HELP_H;
    if (y < menuBottom + 6) return;            /* window too short: skip it */

    DrawRectangleRounded((Rectangle){ 16, (float)y, SIDEBAR_W - 32, HELP_H }, 0.12f, 10, COL_HELP_BG);
    UiText("NEED A LITTLE HELP?", 30, y + 14, 10, COL_NAVY);
    UiText("Ask your coding robot for a tip.", 30, y + 34, 10, COL_MUTED);

    if (UiButton((Rectangle){ 30, (float)y + 56, 124, 26 }, "Ask the robot",
                 COL_SURFACE, COL_BLUE_LIGHT, COL_BLUE_DARK, 11))
        AppToggleRobot();
}

/* Four corner brackets: the usual "fullscreen" symbol. */
static void DrawFullscreenIcon(float cx, float cy, Color c)
{
    float s = 8, l = 4;
    for (int i = 0; i < 4; i++) {
        float sx = (i & 1) ? 1.0f : -1.0f, sy = (i & 2) ? 1.0f : -1.0f;
        Vector2 corner = { cx + sx * s, cy + sy * s };
        DrawLineEx(corner, (Vector2){ corner.x - sx * l, corner.y }, 2.0f, c);
        DrawLineEx(corner, (Vector2){ corner.x, corner.y - sy * l }, 2.0f, c);
    }
}

/* Pinned to the very bottom of the sidebar, with the fullscreen button. */
static void DrawProfile(void)
{
    int top = GetScreenHeight() - PROFILE_H;
    int cy = top + PROFILE_H / 2;

    DrawLine(16, top, SIDEBAR_W - 16, top, COL_LINE);
    DrawCircle(39, cy, 18, COL_AVATAR);
    UiText("ME", 39 - UiMeasure("ME", 11) / 2, cy - 7, 11, COL_BLUE_DARK);
    UiText("Code Explorer", 66, cy - 17, 13, COL_NAVY);
    char level[48];
    snprintf(level, sizeof level, "Level %d - %s", SaveLevel(), SaveLevelTitle());
    UiText(level, 66, cy + 3, 9, COL_MUTED);

    /* fullscreen button (F11 does the same) */
    Rectangle btn = { SIDEBAR_W - 54, (float)cy - 17, 34, 34 };
    bool on = AppIsFullscreen(), over = UiIsHover(btn);
    DrawRectangleRounded(btn, 0.3f, 8, on ? COL_BLUE : (over ? COL_BLUE_LIGHT : COL_PAGE));
    DrawFullscreenIcon(btn.x + 17, btn.y + 17, on ? COL_ON_ACCENT : COL_BLUE_DARK);
    if (over) UiText("F11", (int)btn.x + 4, (int)btn.y - 18, 9, COL_MUTED);
    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) AppToggleFullscreen();
}

void SidebarDraw(NavItem active)
{
    DrawRectangle(0, 0, SIDEBAR_W, GetScreenHeight(), COL_SURFACE);
    DrawLine(SIDEBAR_W - 1, 0, SIDEBAR_W - 1, GetScreenHeight(), COL_LINE);

    DrawLogo();
    int menuBottom = DrawMenu(active);
    DrawHelpCard(menuBottom);
    DrawProfile();
}

const char *SidebarLabel(NavItem item)
{
    if ((int)item < 0 || item >= NAV_COUNT) return "";
    return NAV[item].label;
}
