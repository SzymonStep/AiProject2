/* =============================================================================
 *  quiz.c  --  Session building, shuffling and marking.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  The only subtle part is grading. A student typing out a grid of numbers
 *  should not lose the mark for using two spaces instead of one, or for a
 *  trailing newline, so both the expected and the given answer are put
 *  through the same normalisation before comparison:
 *
 *    - each line is trimmed at both ends
 *    - runs of spaces or tabs inside a line collapse to one space
 *    - empty lines at the end are dropped
 *
 *  Everything else - the values, their order and which line they are on - has
 *  to match, because that is what the exam marks.
 * ========================================================================== */
#include "quiz.h"
#include "progress.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "raylib.h"          /* GetTime() for the exam clock */

/* ---------------------------------------------------------------------------
 *  Normalisation
 * ------------------------------------------------------------------------- */

/* Copies `in` into `out` applying the rules described at the top of the file. */
static void Normalise(const char *in, char *out, int cap)
{
    char tmp[ANSWER_CAP * 2];
    int  n = 0;
    bool atLineStart = true;
    bool pendingSpace = false;

    for (const char *p = in; *p && n < (int)sizeof tmp - 2; p++) {
        char c = *p;

        if (c == '\r') continue;                /* Windows line endings */

        if (c == '\n') {
            /* Drop any space we were holding - it would be trailing. */
            pendingSpace = false;
            tmp[n++] = '\n';
            atLineStart = true;
            continue;
        }

        if (c == ' ' || c == '\t') {
            if (!atLineStart) pendingSpace = true;   /* collapse, defer */
            continue;                                /* leading: discard */
        }

        if (pendingSpace) { tmp[n++] = ' '; pendingSpace = false; }
        tmp[n++] = c;
        atLineStart = false;
    }
    tmp[n] = '\0';

    /* Drop trailing newlines (and therefore trailing empty lines). */
    while (n > 0 && tmp[n-1] == '\n') tmp[--n] = '\0';

    int copy = n < cap - 1 ? n : cap - 1;
    memcpy(out, tmp, (size_t)copy);
    out[copy] = '\0';
}

/* Strips every space and tab - used for the short blank answers, where
 * internal spacing is never meaningful. */
static void StripSpaces(const char *in, char *out, int cap)
{
    int n = 0;
    for (const char *p = in; *p && n < cap - 1; p++)
        if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') out[n++] = *p;
    out[n] = '\0';
}

bool GradeOutput(const char *expected, const char *given)
{
    char a[ANSWER_CAP], b[ANSWER_CAP];
    Normalise(expected, a, sizeof a);
    Normalise(given,    b, sizeof b);
    if (a[0] == '\0' || b[0] == '\0') return false;   /* empty is never right */
    return strcmp(a, b) == 0;
}

bool GradeBlank(const Question *qu, int blankIdx, const char *given)
{
    if (blankIdx < 0 || blankIdx >= qu->blankCount) return false;

    char g[BLANK_CAP * 2];
    StripSpaces(given, g, sizeof g);
    if (g[0] == '\0') return false;

    const Blank *b = &qu->blanks[blankIdx];
    for (int i = 0; i < MAX_ALTS && b->accepted[i]; i++) {
        char a[BLANK_CAP * 2];
        StripSpaces(b->accepted[i], a, sizeof a);
        if (strcmp(a, g) == 0) return true;
    }
    return false;
}

void BlankModelAnswer(const Question *qu, int blankIdx, char *out, int cap)
{
    out[0] = '\0';
    if (blankIdx < 0 || blankIdx >= qu->blankCount) return;
    const char *s = qu->blanks[blankIdx].accepted[0];
    if (!s) return;
    int n = (int)strlen(s);
    if (n > cap - 1) n = cap - 1;
    memcpy(out, s, (size_t)n);
    out[n] = '\0';
}

/* ---------------------------------------------------------------------------
 *  Session building
 * ------------------------------------------------------------------------- */

/* Fisher-Yates: walk from the end, swapping each slot with a random earlier
 * one. Every ordering is equally likely, unlike the "swap two random slots
 * n times" approach. */
static void Shuffle(int *a, int n)
{
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

static bool ModeAccepts(QuizMode mode, QKind kind)
{
    switch (mode) {
        case MODE_SECTION_A: return kind == Q_OUTPUT;
        case MODE_SECTION_B: return kind == Q_BLANKS;
        default:             return true;       /* mixed and exam take both */
    }
}

static int MarksFor(const Question *qu)
{
    return (qu->kind == Q_BLANKS && qu->blankCount > 0) ? qu->blankCount : 1;
}

void QuizBuild(Quiz *q, QuizMode mode, int topic, int limit, unsigned seed)
{
    /* Collect every question in the topic; the mode filter is applied by
     * QuizBuildFromPool. */
    int pool[256];
    int poolCount = 0;
    for (int i = 0; i < QUESTION_COUNT && poolCount < 256; i++) {
        if (topic >= 0 && (int)QUESTIONS[i].topic != topic) continue;
        pool[poolCount++] = i;
    }
    QuizBuildFromPool(q, mode, pool, poolCount, limit, seed, true);
}

int QuestionIndexById(const char *id)
{
    for (int i = 0; i < QUESTION_COUNT; i++)
        if (strcmp(QUESTIONS[i].id, id) == 0) return i;
    return -1;
}

void QuizBuildFromPool(Quiz *q, QuizMode mode, const int *candidates, int candidateCount,
                       int limit, unsigned seed, bool shuffle)
{
    memset(q, 0, sizeof *q);
    q->mode = mode;
    srand(seed);

    int pool[256];
    int poolCount = 0;
    for (int i = 0; i < candidateCount && poolCount < 256; i++) {
        int qi = candidates[i];
        if (qi < 0 || qi >= QUESTION_COUNT)        continue;
        if (!ModeAccepts(mode, QUESTIONS[qi].kind)) continue;
        pool[poolCount++] = qi;
    }

    if (shuffle) Shuffle(pool, poolCount);

    /* A limited exam keeps the paper's balance: roughly half the questions
     * from each section rather than whatever the shuffle put first. */
    if (mode == MODE_EXAM && limit > 0 && limit < poolCount) {
        int a[256], b[256], na = 0, nb = 0;
        for (int i = 0; i < poolCount; i++) {
            if (QUESTIONS[pool[i]].kind == Q_OUTPUT) a[na++] = pool[i];
            else                                     b[nb++] = pool[i];
        }
        int wantA = (limit + 1) / 2, wantB = limit - wantA;
        if (wantA > na) { wantB += wantA - na; wantA = na; }
        if (wantB > nb) { wantA += wantB - nb; wantB = nb; }
        if (wantA > na) wantA = na;
        poolCount = 0;
        for (int i = 0; i < wantA; i++) pool[poolCount++] = a[i];
        for (int i = 0; i < wantB; i++) pool[poolCount++] = b[i];
    }

    /* In exam mode the paper runs Section A first and then Section B, which
     * is how the real paper is laid out - so sort the shuffled pool by kind
     * while keeping the random order within each section. */
    if (mode == MODE_EXAM) {
        int ordered[256];
        int n = 0;
        for (int pass = 0; pass < 2; pass++) {
            QKind want = (pass == 0) ? Q_OUTPUT : Q_BLANKS;
            for (int i = 0; i < poolCount; i++)
                if (QUESTIONS[pool[i]].kind == want) ordered[n++] = pool[i];
        }
        memcpy(pool, ordered, sizeof(int) * (size_t)n);
        poolCount = n;
    }

    if (limit > 0 && limit < poolCount) poolCount = limit;
    if (poolCount > MAX_SESSION)        poolCount = MAX_SESSION;

    q->count = poolCount;
    for (int i = 0; i < poolCount; i++) {
        Attempt *a = &q->attempts[i];
        memset(a, 0, sizeof *a);
        a->qIndex         = pool[i];
        a->marksAvailable = MarksFor(&QUESTIONS[pool[i]]);
        q->marksAvailable += a->marksAvailable;
    }

    q->current   = 0;
    q->startTime = GetTime();
    /* 90 seconds per mark is close to the real paper's pace. */
    q->limitSeconds = (mode == MODE_EXAM) ? q->marksAvailable * 90.0 : 0.0;
}

const Question *QuizCurrentQuestion(const Quiz *q)
{
    if (q->count == 0) return NULL;
    int c = q->current;
    if (c < 0 || c >= q->count) return NULL;
    return &QUESTIONS[q->attempts[c].qIndex];
}

Attempt *QuizCurrentAttempt(Quiz *q)
{
    if (q->count == 0) return NULL;
    int c = q->current;
    if (c < 0 || c >= q->count) return NULL;
    return &q->attempts[c];
}

/* ---------------------------------------------------------------------------
 *  Submitting
 * ------------------------------------------------------------------------- */
void QuizSubmitOutput(Quiz *q, const char *given)
{
    Attempt *a = QuizCurrentAttempt(q);
    if (!a || a->answered) return;
    const Question *qu = &QUESTIONS[a->qIndex];

    int n = (int)strlen(given);
    if (n > ANSWER_CAP - 1) n = ANSWER_CAP - 1;
    memcpy(a->given, given, (size_t)n);
    a->given[n] = '\0';

    a->correct  = GradeOutput(qu->answer, given);
    a->marks    = a->correct ? a->marksAvailable : 0;
    a->answered = true;
    q->marks   += a->marks;

    ProgressRecord(qu->id, a->correct);
}

void QuizSubmitBlanks(Quiz *q, const char blanks[MAX_BLANKS][BLANK_CAP])
{
    Attempt *a = QuizCurrentAttempt(q);
    if (!a || a->answered) return;
    const Question *qu = &QUESTIONS[a->qIndex];

    int got = 0;
    for (int i = 0; i < qu->blankCount; i++) {
        /* strncpy with an explicit terminator: the source is a fixed-size
         * buffer that may not be null terminated if it was filled exactly. */
        memcpy(a->givenBlanks[i], blanks[i], BLANK_CAP);
        a->givenBlanks[i][BLANK_CAP - 1] = '\0';

        a->blankCorrect[i] = GradeBlank(qu, i, a->givenBlanks[i]);
        if (a->blankCorrect[i]) got++;
    }

    a->marks    = got;
    a->correct  = (got == qu->blankCount);
    a->answered = true;
    q->marks   += got;

    ProgressRecord(qu->id, a->correct);
}

void QuizReveal(Quiz *q)
{
    Attempt *a = QuizCurrentAttempt(q);
    if (!a || a->answered) return;
    const Question *qu = &QUESTIONS[a->qIndex];

    a->revealed = true;
    a->answered = true;
    a->correct  = false;
    a->marks    = 0;
    ProgressRecord(qu->id, false);
}

/* ---------------------------------------------------------------------------
 *  Navigation
 * ------------------------------------------------------------------------- */
bool QuizAdvance(Quiz *q)
{
    if (q->current + 1 < q->count) { q->current++; return true; }
    q->finished = true;
    return false;
}

bool QuizBack(Quiz *q)
{
    if (q->current > 0) { q->current--; return true; }
    return false;
}

double QuizTimeLeft(const Quiz *q)
{
    if (q->limitSeconds <= 0.0) return -1.0;
    double left = q->limitSeconds - (GetTime() - q->startTime);
    return left > 0.0 ? left : 0.0;
}
