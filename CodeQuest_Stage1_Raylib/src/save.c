/* =============================================================================
 *  save.c  --  Loading, saving and updating codequest.sav.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Plain text, one "key values" line each, for example:
 *
 *      xp 460
 *      streak 3 20412
 *      today 20412 1
 *      lastpath 0
 *      lessons 0 8191
 *      mock 0 75
 *      quizzes 6
 *      perfect 1
 *
 *  `lessons` is a bitmask: bit n set means lesson n of that path is done.
 *  Days are counted from 1 Jan 1970 in local time, so "yesterday" is just
 *  today - 1. A missing or damaged file means starting from zero.
 * ========================================================================== */
#include "save.h"
#include "progress.h"

#include "raylib.h"          /* LoadFileText / SaveFileText / FileExists */

#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct SaveData {
    unsigned lessons[PATH_COUNT];   /* bitmask per path         */
    int      xp;
    int      streak;
    int      streakDay;             /* last day with activity   */
    int      questDay;              /* day `activities` is for  */
    int      activities;
    int      lastPath;
    int      bestMock[MOCK_COUNT];  /* percent, -1 = never      */
    int      quizzes;
    bool     perfect;
} SaveData;

static SaveData S;
static bool     readOnly = false;

/* ---------------------------------------------------------------------------
 *  Dates
 * ------------------------------------------------------------------------- */
/* Days since 1970-01-01 for a calendar date (Howard Hinnant's algorithm).
 * Doing it by hand avoids time zone and daylight saving surprises. */
static int DaysFromCivil(int y, int m, int d)
{
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

static int Today(void)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return DaysFromCivil(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
}

/* ---------------------------------------------------------------------------
 *  File
 * ------------------------------------------------------------------------- */
static void Defaults(void)
{
    memset(&S, 0, sizeof S);
    for (int i = 0; i < MOCK_COUNT; i++) S.bestMock[i] = -1;
}

static void Write(void)
{
    if (readOnly) return;
    char buf[1024];
    int  n = 0;
    n += snprintf(buf + n, sizeof buf - n, "xp %d\n", S.xp);
    n += snprintf(buf + n, sizeof buf - n, "streak %d %d\n", S.streak, S.streakDay);
    n += snprintf(buf + n, sizeof buf - n, "today %d %d\n", S.questDay, S.activities);
    n += snprintf(buf + n, sizeof buf - n, "lastpath %d\n", S.lastPath);
    for (int i = 0; i < PATH_COUNT; i++)
        n += snprintf(buf + n, sizeof buf - n, "lessons %d %u\n", i, S.lessons[i]);
    for (int i = 0; i < MOCK_COUNT; i++)
        n += snprintf(buf + n, sizeof buf - n, "mock %d %d\n", i, S.bestMock[i]);
    n += snprintf(buf + n, sizeof buf - n, "quizzes %d\n", S.quizzes);
    snprintf(buf + n, sizeof buf - n, "perfect %d\n", S.perfect ? 1 : 0);
    SaveFileText(SAVE_FILE, buf);
}

void SaveLoad(void)
{
    Defaults();
    ProgressLoad();
    if (!FileExists(SAVE_FILE)) return;

    char *text = LoadFileText(SAVE_FILE);
    if (!text) return;

    const char *p = text;
    while (*p) {
        char key[16];
        int a = 0, b = 0;
        unsigned u = 0;
        if (sscanf(p, "%15s", key) == 1) {
            if      (!strcmp(key, "xp"))       sscanf(p, "%*s %d", &S.xp);
            else if (!strcmp(key, "streak"))   sscanf(p, "%*s %d %d", &S.streak, &S.streakDay);
            else if (!strcmp(key, "today"))    sscanf(p, "%*s %d %d", &S.questDay, &S.activities);
            else if (!strcmp(key, "lastpath")) sscanf(p, "%*s %d", &S.lastPath);
            else if (!strcmp(key, "quizzes"))  sscanf(p, "%*s %d", &S.quizzes);
            else if (!strcmp(key, "perfect")) { sscanf(p, "%*s %d", &a); S.perfect = a != 0; }
            else if (!strcmp(key, "lessons") && sscanf(p, "%*s %d %u", &a, &u) == 2) {
                if (a >= 0 && a < PATH_COUNT) S.lessons[a] = u;
            }
            else if (!strcmp(key, "mock") && sscanf(p, "%*s %d %d", &a, &b) == 2) {
                if (a >= 0 && a < MOCK_COUNT) S.bestMock[a] = b;
            }
        }
        const char *nl = strchr(p, '\n');
        if (!nl) break;
        p = nl + 1;
    }
    UnloadFileText(text);

    if (S.lastPath < 0 || S.lastPath >= PATH_COUNT) S.lastPath = 0;
    if (S.xp < 0) S.xp = 0;
}

void SaveUseDemoData(void)
{
    readOnly = true;
    Defaults();
    S.lessons[PATH_C_FUNDAMENTALS] = (1u << 13) - 1;   /* first 13 lessons */
    S.lessons[PATH_EXAM1] = 3u;                        /* first 2 sets     */
    S.xp = 460;
    S.streak = 3;
    S.streakDay = Today();
    S.questDay = Today();
    S.activities = 1;
    S.bestMock[MOCK_EXAM1] = 62;
    S.quizzes = 16;
}

void SaveReset(void)
{
    Defaults();
    Write();
    ProgressReset();
    ProgressSave();
}

/* ---------------------------------------------------------------------------
 *  Lessons
 * ------------------------------------------------------------------------- */
bool SaveLessonDone(PathId path, int lesson)
{
    if ((int)path < 0 || path >= PATH_COUNT || lesson < 0 || lesson >= 32) return false;
    return (S.lessons[path] >> lesson) & 1u;
}

int SaveLessonsDone(PathId path)
{
    const LearningPath *p = PathGet(path);
    int n = 0;
    if (!p) return 0;
    for (int i = 0; i < p->lessonCount; i++) n += SaveLessonDone(path, i);
    return n;
}

int SaveLessonsDoneAll(void)
{
    int n = 0;
    for (int i = 0; i < PATH_COUNT; i++) n += SaveLessonsDone((PathId)i);
    return n;
}

bool SaveCompleteLesson(PathId path, int lesson)
{
    if (SaveLessonDone(path, lesson) || lesson < 0 || lesson >= 32) return false;
    S.lessons[path] |= 1u << lesson;
    S.xp += XP_LESSON;
    Write();                 /* the quiz that completed it already counted as activity */
    return true;
}

PathId SaveLastPath(void) { return (PathId)S.lastPath; }

void SaveSetLastPath(PathId path)
{
    if ((int)path == S.lastPath) return;
    S.lastPath = (int)path;
    Write();
}

/* ---------------------------------------------------------------------------
 *  XP and levels
 * ------------------------------------------------------------------------- */
void SaveAddXp(int xp)
{
    if (xp <= 0) return;
    S.xp += xp;
    Write();
}

int SaveXp(void)    { return S.xp; }
int SaveLevel(void) { return S.xp / XP_PER_LEVEL + 1; }

const char *SaveLevelTitle(void)
{
    static const char *titles[] = {
        "Beginner", "Explorer", "Apprentice", "Coder", "Problem Solver",
        "Debugger", "Expert", "Master"
    };
    int i = SaveLevel() - 1;
    int n = (int)(sizeof titles / sizeof *titles);
    return titles[i < n ? i : n - 1];
}

int   SaveXpToNextLevel(void) { return XP_PER_LEVEL - S.xp % XP_PER_LEVEL; }
float SaveLevelProgress(void) { return (float)(S.xp % XP_PER_LEVEL) / XP_PER_LEVEL; }

/* ---------------------------------------------------------------------------
 *  Streak and daily quest
 * ------------------------------------------------------------------------- */
void SaveActivity(void)
{
    int today = Today();

    if (S.streakDay != today) {
        S.streak = (S.streakDay == today - 1) ? S.streak + 1 : 1;
        S.streakDay = today;
    }

    if (S.questDay != today) { S.questDay = today; S.activities = 0; }
    S.activities++;
    if (S.activities == QUEST_TARGET) S.xp += XP_QUEST_BONUS;

    Write();
}

int SaveStreak(void)
{
    /* a streak survives until a whole day is missed */
    int today = Today();
    return (S.streakDay == today || S.streakDay == today - 1) ? S.streak : 0;
}

int SaveActivitiesToday(void)
{
    return (S.questDay == Today()) ? S.activities : 0;
}

/* ---------------------------------------------------------------------------
 *  Quizzes
 * ------------------------------------------------------------------------- */
void SaveRecordQuiz(int marks, int marksAvailable, int mock)
{
    S.quizzes++;
    if (marksAvailable > 0 && marks == marksAvailable) S.perfect = true;
    if (mock >= 0 && mock < MOCK_COUNT && marksAvailable > 0) {
        int pct = marks * 100 / marksAvailable;
        if (pct > S.bestMock[mock]) S.bestMock[mock] = pct;
    }
    ProgressSave();
    SaveActivity();          /* writes the file */
}

int  SaveBestMock(MockExam exam)  { return (exam < MOCK_COUNT) ? S.bestMock[exam] : -1; }
int  SaveQuizzesFinished(void)    { return S.quizzes; }
bool SaveHadPerfectQuiz(void)     { return S.perfect; }
