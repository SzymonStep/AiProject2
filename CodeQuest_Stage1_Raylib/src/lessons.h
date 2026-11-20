/* =============================================================================
 *  lessons.h  --  What is inside each lesson: notes, example code and a quiz.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Two kinds of lesson:
 *    - C Fundamentals and Advanced C++ lessons carry their own notes, a code
 *      example and a short multiple-choice check.
 *    - Coding Exam practice sets reuse a CExam Trainer lecture note and a
 *      list of real exam-style questions (Section A output, Section B blanks)
 *      that are marked by the quiz engine. The last set of each exam path is
 *      a timed mock paper.
 * ========================================================================== */
#ifndef LESSONS_H
#define LESSONS_H

#include "paths.h"

#define MCQ_OPTIONS  4
#define LESSON_MCQ   3
#define LESSON_QIDS  8   /* up to 7 questions plus the NULL end marker */

typedef struct Mcq {
    const char *question;
    const char *options[MCQ_OPTIONS];
    int         answer;          /* index of the correct option */
    const char *explain;
} Mcq;

typedef struct LessonContent {
    /* Own notes. When `notes` is NULL the lesson shows CExam Trainer note
     * `noteIndex` of note section `noteSection` instead. */
    const char *notes;           /* paragraphs separated by "\n\n" */
    const char *code;            /* example, may be NULL           */
    const char *keyPoint;        /* one-line takeaway, may be NULL */
    int         noteSection;
    int         noteIndex;

    Mcq         mcq[LESSON_MCQ];
    int         mcqCount;

    const char *questionIds[LESSON_QIDS];   /* exam questions, NULL-ended */
    int         mock;            /* MOCK_EXAM1/2 for a mock paper, else -1 */
} LessonContent;

const LessonContent *LessonContentGet(PathId path, int lesson);

/* Resolved notes for a lesson, whichever source they come from. */
const char *LessonNotes(PathId path, int lesson);
const char *LessonCode(PathId path, int lesson);
const char *LessonHeading(PathId path, int lesson);   /* note heading or NULL */

/* How many exam questions a lesson's quiz has (0 for MCQ lessons). */
int LessonQuestionCount(PathId path, int lesson);

#endif /* LESSONS_H */
