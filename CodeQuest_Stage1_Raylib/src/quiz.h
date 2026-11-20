/* =============================================================================
 *  quiz.h  --  Quiz session building and answer grading.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  A Quiz is one sitting: an ordered list of question indexes plus the
 *  student's attempt at each. It holds no UI state, so the same engine drives
 *  the practice drills and the timed exam.
 *
 *  Marking follows the real paper: a Section A question is worth 1 mark, a
 *  Section B question is worth 1 mark per blank, so partial credit is
 *  possible on the fill-in questions.
 * ========================================================================== */
#ifndef QUIZ_H
#define QUIZ_H

#include "content.h"
#include <stdbool.h>

#define MAX_SESSION   64
#define ANSWER_CAP    1024
#define BLANK_CAP     64

typedef enum QuizMode {
    MODE_SECTION_A = 0,   /* output questions only           */
    MODE_SECTION_B,       /* fill-in-the-blank questions only */
    MODE_MIXED,           /* both, shuffled                  */
    MODE_EXAM             /* both, shuffled, timed, no reveal until the end */
} QuizMode;

/* One question's worth of the student's work. */
typedef struct Attempt {
    int   qIndex;                               /* into QUESTIONS[]   */
    bool  answered;
    bool  correct;                              /* fully correct      */
    int   marks;                                /* marks earned       */
    int   marksAvailable;
    char  given[ANSWER_CAP];                    /* Q_OUTPUT response  */
    char  givenBlanks[MAX_BLANKS][BLANK_CAP];   /* Q_BLANKS responses */
    bool  blankCorrect[MAX_BLANKS];
    bool  revealed;                             /* student gave up    */
} Attempt;

typedef struct Quiz {
    QuizMode mode;
    int      count;
    int      current;
    Attempt  attempts[MAX_SESSION];
    bool     finished;
    double   startTime;        /* GetTime() when the sitting began    */
    double   limitSeconds;     /* 0 means untimed                     */
    int      marks;            /* running total                       */
    int      marksAvailable;
} Quiz;

/* Builds a shuffled session. `topic` is a Topic value, or -1 for every topic.
 * `limit` caps the number of questions (clamped to MAX_SESSION). */
void QuizBuild(Quiz *q, QuizMode mode, int topic, int limit, unsigned seed);

/* Same, from an explicit list of QUESTIONS[] indexes (a practice set, or
 * several topics at once). Questions the mode does not accept are skipped.
 * `shuffle` false keeps the list's order. */
void QuizBuildFromPool(Quiz *q, QuizMode mode, const int *candidates, int candidateCount,
                       int limit, unsigned seed, bool shuffle);

/* Index into QUESTIONS[] of the question with this id ("L1-A3"), or -1. */
int QuestionIndexById(const char *id);

const Question *QuizCurrentQuestion(const Quiz *q);
Attempt        *QuizCurrentAttempt(Quiz *q);

/* Marks the current question from the supplied answer and advances the
 * running total. Safe to call once per question; later calls are ignored. */
void QuizSubmitOutput(Quiz *q, const char *given);
void QuizSubmitBlanks(Quiz *q, const char blanks[MAX_BLANKS][BLANK_CAP]);

/* Gives up on the current question: zero marks, answer shown. */
void QuizReveal(Quiz *q);

bool QuizAdvance(Quiz *q);     /* true if there was a next question */
bool QuizBack(Quiz *q);

/* Seconds remaining, or -1 when the session is untimed. */
double QuizTimeLeft(const Quiz *q);

/* --- grading primitives (exposed so the review screen can explain) -------- */

/* Compares two multi-line outputs ignoring trailing spaces, blank lines at
 * the end, and runs of spaces inside a line. */
bool GradeOutput(const char *expected, const char *given);

/* True if `given` matches any accepted spelling of that blank. All whitespace
 * is ignored, so "-= 1" and "-=1" are the same answer. */
bool GradeBlank(const Question *qu, int blankIdx, const char *given);

/* Writes the model answer for one blank into `out` (first accepted spelling). */
void BlankModelAnswer(const Question *qu, int blankIdx, char *out, int cap);

#endif /* QUIZ_H */
