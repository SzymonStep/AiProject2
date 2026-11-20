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
    DrawText("CQ", 26, 34, 18, COL_WHITE);
    DrawText("CodeQuest", 74, 25, 20, COL_NAVY);
    DrawText("LEARN  PRACTISE  MASTER", 75, 49, 8, COL_MUTED);
}

static void DrawMenu(NavItem active)
{
    DrawText("YOUR SPACE", 24, 105, 10, COL_MUTED);

    for (int i = 0; i < NAV_COUNT; ++i) {
        int y = 128 + i * 47;

        /* Group headings push the items below them down a little. */
        if (i == NAV_EXAM_ARENA) {
            DrawText("EXAM PREP", 24, y - 12, 10, COL_MUTED);
            y += 14;
        } else if (i == NAV_ACHIEVEMENTS) {
            DrawText("YOUR PROGRESS", 24, y - 12, 10, COL_MUTED);
            y += 14;
        }

        Rectangle item = { 12, (float)y, 224, 39 };
        bool selected = (i == (int)active);
        bool over = UiIsHover(item);

        if (selected)  DrawRectangleRounded(item, 0.22f, 8, COL_BLUE_LIGHT);
        else if (over) DrawRectangleRounded(item, 0.22f, 8, COL_PAGE);

        Color itemColor = selected ? COL_BLUE_DARK : COL_MUTED;
        DrawCircle(34, y + 19, 11, selected ? COL_WHITE : COL_PAGE);
        int iconW = MeasureText(NAV[i].icon, 12);
        DrawText(NAV[i].icon, 34 - iconW / 2, y + 13, 12, itemColor);
        DrawText(NAV[i].label, 55, y + 12, 13, itemColor);

        /* main.c decides what a click means; sections not built yet toast */
        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            AppNavigate((NavItem)i);
    }
}

static void DrawHelpCard(void)
{
    DrawRectangleRounded((Rectangle){ 16, 690, 216, 105 }, 0.12f, 10,
                         (Color){ 242, 248, 254, 255 });
    DrawText("NEED A LITTLE HELP?", 29, 704, 10, COL_NAVY);
    DrawText("Your coding robot is here", 29, 725, 10, COL_MUTED);
    DrawText("to help explain tricky ideas.", 29, 740, 10, COL_MUTED);

    if (UiButton((Rectangle){ 28, 759, 112, 25 }, "Ask the robot",
                 COL_WHITE, COL_BLUE_LIGHT, COL_BLUE_DARK, 10))
        AppToggleRobot();
}

static void DrawProfile(void)
{
    DrawLine(16, 818, 232, 818, COL_LINE);
    DrawCircle(39, 852, 17, (Color){ 223, 242, 255, 255 });
    DrawText("ME", 28, 846, 10, COL_BLUE_DARK);
    DrawText("Code Explorer", 65, 840, 12, COL_NAVY);
    char level[48];
    snprintf(level, sizeof level, "Level %d  -  %s", SaveLevel(), SaveLevelTitle());
    DrawText(level, 65, 857, 9, COL_MUTED);
}

void SidebarDraw(NavItem active)
{
    DrawRectangle(0, 0, SIDEBAR_W, GetScreenHeight(), COL_WHITE);
    DrawLine(SIDEBAR_W - 1, 0, SIDEBAR_W - 1, GetScreenHeight(), COL_LINE);

    DrawLogo();
    DrawMenu(active);
    DrawHelpCard();
    DrawProfile();
}

const char *SidebarLabel(NavItem item)
{
    if ((int)item < 0 || item >= NAV_COUNT) return "";
    return NAV[item].label;
}
