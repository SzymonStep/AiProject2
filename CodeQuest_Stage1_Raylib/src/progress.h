/* =============================================================================
 *  progress.h  --  Per-question history, saved between runs.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  Records are keyed by the question's id string ("L1-A3") rather than by its
 *  position in the bank, so adding or reordering questions never corrupts a
 *  saved file.
 * ========================================================================== */
#ifndef PROGRESS_H
#define PROGRESS_H

#include "content.h"
#include <stdbool.h>

#define PROGRESS_FILE "progress.txt"
#define PROGRESS_MAX  256
#define ID_CAP        16

typedef struct ProgressRec {
    char id[ID_CAP];
    int  seen;
    int  correct;
} ProgressRec;

void ProgressLoad(void);
void ProgressSave(void);
void ProgressReset(void);

/* Adds one attempt to a question's history. Call once per answered question. */
void ProgressRecord(const char *id, bool correct);

/* Looks up a question's history. Missing entries report 0 / 0. */
void ProgressGet(const char *id, int *seen, int *correct);

/* Fraction (0..1) of a topic's questions that have been answered correctly at
 * least once - the app's definition of "mastered". */
float TopicMastery(Topic t);
int   TopicSolved(Topic t);

/* Totals across every topic. */
int ProgressAttempts(void);
int ProgressCorrect(void);
int ProgressSolvedAll(void);

/* Index of the question the student has got wrong most often and not yet
 * mastered, or -1 if there is nothing to revisit. Drives "Practise weakest". */
int WeakestQuestion(void);

#endif /* PROGRESS_H */
