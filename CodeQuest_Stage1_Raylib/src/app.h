/* =============================================================================
 *  app.h  --  Screens and navigation requests from screens back to main.c.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Screens never switch screens themselves; they ask main.c to. That keeps
 *  the "which screen is on show" state in one place.
 * ========================================================================== */
#ifndef APP_H
#define APP_H

#include "sidebar.h"
#include "paths.h"
#include "quiz.h"

typedef enum Screen {
    SCR_DASHBOARD = 0,
    SCR_PATHS,
    SCR_LESSON,
    SCR_QUIZ,
    SCR_ARENA,
    SCR_CHALLENGES,
    SCR_REVISION,
    SCR_ACHIEVEMENTS
} Screen;

/* A sidebar click. */
void AppNavigate(NavItem item);

/* Show a screen without changing the sidebar highlight. */
void AppGoTo(Screen screen);

/* Open the learning path screen with one path selected. */
void AppOpenPath(PathId path);

/* Open one lesson's notes. */
void AppOpenLesson(PathId path, int lesson);

/* Start a lesson's check: multiple choice, exam questions or a mock paper. */
void AppStartLessonQuiz(PathId path, int lesson);

/* Start a quiz built from QUESTIONS[] indexes. When it ends, the results
 * offer a way back to `returnTo`. `mock` is MOCK_EXAM1/2 or -1. */
void AppStartPractice(const char *title, QuizMode mode, const int *pool, int poolCount,
                      int limit, bool shuffle, Screen returnTo, int mock);

/* The robot helper pop-up. */
void AppToggleRobot(void);

/* Borderless fullscreen on the current monitor, toggled by F11 or the
 * button in the sidebar. */
void AppToggleFullscreen(void);
bool AppIsFullscreen(void);

#endif /* APP_H */
