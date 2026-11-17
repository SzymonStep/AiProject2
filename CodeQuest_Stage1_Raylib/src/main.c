/* =============================================================================
 *  main.c  --  CodeQuest: learn, practise and master C / C++.
 *
 *  Project : CodeQuest (raylib)
 *  Author  : Szymon Stepniak
 *  Module  : Advanced Programming - revision for Coding Exam 1 & 2
 *
 *  What this file does
 *  -------------------
 *  Owns the window, the application state and the main loop. The widgets
 *  (ui.c), the navigation (sidebar.c) and each screen (dashboard.c) are
 *  separate, so this file only decides which screen is on show.
 *
 *  Stage 1: only the Dashboard exists. The other sidebar items show a toast
 *  and leave the dashboard on screen until their stage is built.
 *
 *  Build: see Makefile, or build.bat on Windows.
 * ========================================================================== */
#include "raylib.h"

#include "ui.h"
#include "sidebar.h"
#include "dashboard.h"

#include <string.h>

typedef struct App {
    NavItem screen;    /* which sidebar item is selected */
} App;

static App app = { NAV_DASHBOARD };

static void DrawScreen(void)
{
    switch (app.screen) {
    case NAV_DASHBOARD:
    default:
        /* Later stages add a case per screen here. */
        DashboardDraw();
        break;
    }
}

int main(int argc, char **argv)
{
    /* --shot: draw a few frames, save a screenshot and quit (make shots). */
    bool shotMode = (argc > 1 && strcmp(argv[1], "--shot") == 0);
    int  frame = 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(WIN_W, WIN_H, "CodeQuest - Learn. Practise. Master.");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (shotMode && ++frame == 5) {
            TakeScreenshot("01-dashboard.png");
            break;
        }

        UiUpdateToast(GetFrameTime());

        BeginDrawing();
        ClearBackground(COL_PAGE);

        SidebarDraw(&app.screen);
        DrawScreen();
        UiDrawToast();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
