/* =============================================================================
 *  quizscreen.h  --  Running a quiz and showing its results.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Two kinds of quiz share this screen:
 *    - multiple choice, for C Fundamentals and Advanced C++ lessons
 *    - exam questions from the quiz engine: Section A (type the output) and
 *      Section B (fill in the blanks), optionally as a timed mock paper
 *
 *  When a quiz ends it awards XP, records the attempt in the save file and,
 *  for a lesson's quiz, completes the lesson on a score of 50% or more.
 * ========================================================================== */
#ifndef QUIZSCREEN_H
#define QUIZSCREEN_H

#include "app.h"

/* A lesson's multiple-choice check. */
void QuizScreenStartMcq(PathId path, int lesson);

/* Exam questions from QUESTIONS[] indexes. `lesson` is -1 when the quiz does
 * not belong to a lesson. */
void QuizScreenStartEngine(const char *title, QuizMode mode, const int *pool, int poolCount,
                           int limit, bool shuffle, Screen returnTo,
                           PathId path, int lesson, int mock);

void QuizScreenDraw(void);

#endif /* QUIZSCREEN_H */
