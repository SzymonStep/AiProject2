/* =============================================================================
 *  paths.h  --  The learning paths and the lessons inside each one.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Pure data, no drawing. Every screen that shows a path (dashboard, learning
 *  path screen, later the lesson screen) reads it from here, so the numbers
 *  can never disagree between screens.
 *
 *  Which lessons are done lives in save.c; the helpers below read it so the
 *  screens never touch the save file directly.
 * ========================================================================== */
#ifndef PATHS_H
#define PATHS_H

typedef enum PathId {
    PATH_C_FUNDAMENTALS = 0,
    PATH_EXAM1,
    PATH_EXAM2,
    PATH_ADVANCED_CPP,
    PATH_COUNT
} PathId;

typedef struct Lesson {
    const char *title;
    const char *summary;    /* one line shown under the title */
    int         minutes;    /* rough time to work through it  */
} Lesson;

#define PATH_SKILLS 4

typedef struct LearningPath {
    const char   *icon;         /* single character until real icons */
    const char   *title;
    const char   *tagline;      /* short, fits on a dashboard card   */
    const char   *description;  /* a sentence or two for the detail  */
    const char   *unitWord;     /* "lessons" or "practice sets"      */
    const char   *badge;        /* status label on the dashboard     */
    const char   *skills[PATH_SKILLS];
    const Lesson *lessons;
    int           lessonCount;
} LearningPath;

const LearningPath *PathGet(PathId id);

/* Lessons finished in this path (from the save file). */
int PathCompleted(PathId id);

/* 0..1 share of lessons completed. */
float PathProgress(PathId id);

/* Total estimated minutes for the whole path. */
int PathTotalMinutes(PathId id);

/* Index of the first lesson not yet done, or -1 when the path is finished. */
int PathNextLesson(PathId id);

#endif /* PATHS_H */
