/* =============================================================================
 *  main.c  --  CodeQuest: learn, practise and master C / C++.
 *
 *  Project : CodeQuest (raylib)
 *  Author  : Szymon Stepniak
 *  Module  : Advanced Programming - revision for Coding Exam 1 & 2
 *
 *  What this file does
 *  -------------------
 *  Owns the window, the application state and the main loop. Everything else
 *  is a module: widgets (ui.c, codeview.c), content (paths.c, lessons.c,
 *  content.c), the quiz engine (quiz.c), saved progress (save.c,
 *  progress.c) and one file per screen. This file only decides which screen
 *  is on show; screens ask to change screen through app.h.
 *
 *  Screens
 *    Dashboard         home, real progress, next lesson, daily quest
 *    Learning Path     the four paths and their lessons
 *    Lesson            notes and example, then the lesson's check
 *    Quiz              multiple choice or exam questions, then results
 *    Exam Arena        timed mock papers
 *    Challenges        quick drills by section and lab
 *    Revision Centre   mastery, weakest questions, lecture notes
 *    Achievements      badges, stats, reset
 *
 *  Build: see Makefile, or build.bat on Windows.
 * ========================================================================== */
#include "raylib.h"

#include "app.h"
#include "ui.h"
#include "codeview.h"
#include "sidebar.h"
#include "dashboard.h"
#include "learnpath.h"
#include "lesson.h"
#include "lessons.h"
#include "quizscreen.h"
#include "screens.h"
#include "save.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct App {
    Screen  screen;    /* what is drawn            */
    NavItem nav;       /* what the sidebar shows   */
} App;

static App app = { SCR_DASHBOARD, NAV_DASHBOARD };

/* ---------------------------------------------------------------------------
 *  Navigation (app.h)
 * ------------------------------------------------------------------------- */
void AppNavigate(NavItem item)
{
    app.nav = item;
    switch (item) {
    case NAV_DASHBOARD:     app.screen = SCR_DASHBOARD;    break;
    case NAV_LEARNING_PATH: app.screen = SCR_PATHS;        break;
    case NAV_LEARN_C:       LearnPathSelect(PATH_C_FUNDAMENTALS); app.screen = SCR_PATHS; break;
    case NAV_LEARN_CPP:     LearnPathSelect(PATH_ADVANCED_CPP);   app.screen = SCR_PATHS; break;
    case NAV_CHALLENGES:    app.screen = SCR_CHALLENGES;   break;
    case NAV_EXAM_ARENA:    app.screen = SCR_ARENA;        break;
    case NAV_REVISION:      app.screen = SCR_REVISION;     break;
    case NAV_ACHIEVEMENTS:  app.screen = SCR_ACHIEVEMENTS; break;
    default: break;
    }
}

void AppGoTo(Screen screen)
{
    app.screen = screen;
}

void AppOpenPath(PathId path)
{
    LearnPathSelect(path);
    app.screen = SCR_PATHS;
    app.nav = NAV_LEARNING_PATH;
}

void AppOpenLesson(PathId path, int lesson)
{
    LearnPathSelect(path);
    LessonScreenOpen(path, lesson);
    app.screen = SCR_LESSON;
    if (app.nav != NAV_LEARN_C && app.nav != NAV_LEARN_CPP) app.nav = NAV_LEARNING_PATH;
}

void AppStartLessonQuiz(PathId path, int lesson)
{
    const LessonContent *c = LessonContentGet(path, lesson);
    const char *title = PathGet(path)->lessons[lesson].title;
    if (!c) return;

    if (c->mcqCount > 0) {
        QuizScreenStartMcq(path, lesson);
    } else if (c->mock >= 0) {
        /* a mock lesson: 10 timed questions from that exam's labs */
        int pool[256], n = 0;
        for (int i = 0; i < QUESTION_COUNT; i++) {
            Topic t = QUESTIONS[i].topic;
            bool in = (c->mock == MOCK_EXAM1) ? (t == TOPIC_LAB1 || t == TOPIC_LAB2) : (t == TOPIC_LAB3);
            if (in) pool[n++] = i;
        }
        QuizScreenStartEngine(title, MODE_EXAM, pool, n, 10, true, SCR_LESSON, path, lesson, c->mock);
    } else {
        int pool[LESSON_QIDS], n = 0;
        for (int i = 0; i < LESSON_QIDS && c->questionIds[i]; i++) {
            int qi = QuestionIndexById(c->questionIds[i]);
            if (qi >= 0) pool[n++] = qi;
        }
        QuizScreenStartEngine(title, MODE_MIXED, pool, n, 0, false, SCR_LESSON, path, lesson, -1);
    }
    app.screen = SCR_QUIZ;
}

void AppStartPractice(const char *title, QuizMode mode, const int *pool, int poolCount,
                      int limit, bool shuffle, Screen returnTo, int mock)
{
    QuizScreenStartEngine(title, mode, pool, poolCount, limit, shuffle, returnTo,
                          PATH_C_FUNDAMENTALS, -1, mock);
    app.screen = SCR_QUIZ;
}

void AppToggleRobot(void)
{
    RobotToggle();
}

/* Borderless windowed rather than exclusive fullscreen: it switches
 * instantly, keeps the desktop resolution and Alt+Tab works normally. */
void AppToggleFullscreen(void)
{
    ToggleBorderlessWindowed();
}

bool AppIsFullscreen(void)
{
    return IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
}

/* ---------------------------------------------------------------------------
 *  Frame
 * ------------------------------------------------------------------------- */
/* Picking a different path while "Learn C" is highlighted would leave the
 * sidebar pointing at the wrong item, so fall back to "Learning Path". */
static void SyncSidebarWithPath(void)
{
    if ((app.nav == NAV_LEARN_C   && LearnPathSelected() != PATH_C_FUNDAMENTALS) ||
        (app.nav == NAV_LEARN_CPP && LearnPathSelected() != PATH_ADVANCED_CPP))
        app.nav = NAV_LEARNING_PATH;
}

static void DrawScreen(void)
{
    switch (app.screen) {
    case SCR_PATHS:        LearnPathDraw();    break;
    case SCR_LESSON:       LessonScreenDraw(); break;
    case SCR_QUIZ:         QuizScreenDraw();   break;
    case SCR_ARENA:        ArenaDraw();        break;
    case SCR_CHALLENGES:   ChallengesDraw();   break;
    case SCR_REVISION:     RevisionDraw();     break;
    case SCR_ACHIEVEMENTS: AchievementsDraw(); break;
    case SCR_DASHBOARD:
    default:               DashboardDraw();    break;
    }
}

/* --shot mode: render each screen, save it to a PNG and quit. Uses a
 * throwaway save so real progress is never touched.
 * `make shots` moves the files into shots/. */
static bool RunShotStep(int frame)
{
    static const struct { int frame; const char *file; } steps[] = {
        {  5, "01-dashboard.png"     }, { 10, "02-learning-path.png" },
        { 15, "03-lesson.png"        }, { 20, "04-quiz-mcq.png"      },
        { 25, "05-quiz-exam.png"     }, { 30, "06-exam-arena.png"    },
        { 35, "07-challenges.png"    }, { 40, "08-revision.png"      },
        { 45, "09-achievements.png"  },
    };
    for (int i = 0; i < (int)(sizeof steps / sizeof steps[0]); ++i)
        if (frame == steps[i].frame) TakeScreenshot(steps[i].file);

    switch (frame) {
    case 6:  AppOpenPath(PATH_C_FUNDAMENTALS); break;
    case 11: AppOpenLesson(PATH_C_FUNDAMENTALS, 14); break;
    case 16: AppStartLessonQuiz(PATH_C_FUNDAMENTALS, 14); break;
    case 21: AppStartLessonQuiz(PATH_EXAM1, 1); break;
    case 26: AppNavigate(NAV_EXAM_ARENA); break;
    case 31: AppNavigate(NAV_CHALLENGES); break;
    case 36: AppNavigate(NAV_REVISION); break;
    case 41: AppNavigate(NAV_ACHIEVEMENTS); break;
    default: break;
    }
    return frame >= 45;
}

int main(int argc, char **argv)
{
    bool shotMode = (argc > 1 && strcmp(argv[1], "--shot") == 0);
    int  frame = 0;

    srand((unsigned)time(NULL));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(WIN_W, WIN_H, "CodeQuest - Learn. Practise. Master.");
    SetWindowMinSize(1280, 760);
    SetExitKey(KEY_NULL);          /* Esc closes pop-ups, not the app */
    SetTargetFPS(60);
    UiLoadFonts();
    CodeFontLoad();
    if (shotMode) SaveUseDemoData(); else SaveLoad();
    UiSetDarkMode(true);           /* CodeQuest always runs in dark mode */
    if (!shotMode) MaximizeWindow();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) AppToggleFullscreen();
        UiUpdateToast(GetFrameTime());
        SyncSidebarWithPath();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        BeginDrawing();
        ClearBackground(COL_PAGE);

        /* while the robot is open, park the mouse far away for everything
         * underneath so only the pop-up reacts to it */
        bool modal = RobotIsOpen();
        if (modal) SetMouseOffset(-100000, -100000);
        SidebarDraw(app.nav);
        DrawScreen();
        if (modal) SetMouseOffset(0, 0);

        RobotDraw();
        UiDrawToast();

        EndDrawing();

        if (shotMode && RunShotStep(++frame)) break;
    }

    CodeFontUnload();
    UiUnloadFonts();
    CloseWindow();
    return 0;
}
