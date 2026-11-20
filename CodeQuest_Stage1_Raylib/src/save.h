/* =============================================================================
 *  save.h  --  The student's CodeQuest progress, saved between runs.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Two files are kept next to the executable:
 *    codequest.sav   lessons completed, XP, streak, quest, best scores (here)
 *    progress.txt    per-question history from the quiz engine (progress.c),
 *                    the same format CExam Trainer used
 *
 *  Every change is written straight away, so closing the window never loses
 *  anything.
 * ========================================================================== */
#ifndef SAVE_H
#define SAVE_H

#include "paths.h"
#include <stdbool.h>

#define SAVE_FILE "codequest.sav"

/* XP rewards, kept together so the balance is easy to tune. */
#define XP_LESSON        50   /* first time a lesson is completed   */
#define XP_CORRECT       10   /* each fully correct quiz answer     */
#define XP_QUEST_BONUS   20   /* finishing today's mini quest       */
#define XP_PER_LEVEL    250
#define QUEST_TARGET      2   /* learning activities per day        */

typedef enum MockExam { MOCK_EXAM1 = 0, MOCK_EXAM2, MOCK_COUNT } MockExam;

void SaveLoad(void);
void SaveReset(void);          /* wipes both save files */

/* Fills in sample progress and stops all writing, so `make shots` produces
 * screenshots with something on them without touching the real save. */
void SaveUseDemoData(void);

/* --- lessons --------------------------------------------------------------- */
bool SaveLessonDone(PathId path, int lesson);
int  SaveLessonsDone(PathId path);
int  SaveLessonsDoneAll(void);

/* Marks a lesson complete. Returns true the first time, when it also awards
 * XP. (The quiz that completes it is what counts as the day's activity.) */
bool SaveCompleteLesson(PathId path, int lesson);

/* The path the student last opened, so "Continue learning" goes back to it. */
PathId SaveLastPath(void);
void   SaveSetLastPath(PathId path);

/* --- XP and levels --------------------------------------------------------- */
void        SaveAddXp(int xp);
int         SaveXp(void);
int         SaveLevel(void);              /* 1, 2, 3 ...               */
const char *SaveLevelTitle(void);         /* "Beginner", "Explorer" ... */
int         SaveXpToNextLevel(void);
float       SaveLevelProgress(void);      /* 0..1 within this level    */

/* --- streak and daily quest ------------------------------------------------ */
/* Records one learning activity today (a lesson finished or a quiz done).
 * Updates the streak and pays the quest bonus when the target is reached. */
void SaveActivity(void);
int  SaveStreak(void);                    /* days in a row, 0 if broken */
int  SaveActivitiesToday(void);

/* --- quizzes ------------------------------------------------------------- */
/* Records a finished quiz. `mock` is MOCK_EXAM1/2 for a mock exam, or -1. */
void SaveRecordQuiz(int marks, int marksAvailable, int mock);
int  SaveBestMock(MockExam exam);         /* best percentage, -1 if never */
int  SaveQuizzesFinished(void);
bool SaveHadPerfectQuiz(void);


#endif /* SAVE_H */
