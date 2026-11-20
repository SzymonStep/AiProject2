/* =============================================================================
 *  content.h  --  The revision material: lecture notes and the question bank.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  All content is static read-only data. Separating it from the UI and the
 *  quiz engine means a new question is a new table entry, not a code change.
 *
 *  Question formats mirror the real paper exactly:
 *    Section A - "What is the output / value?"  -> Q_OUTPUT
 *    Section B - "Fill in the blanks"           -> Q_BLANKS
 * ========================================================================== */
#ifndef CONTENT_H
#define CONTENT_H

#include <stdbool.h>

/* --------------------------------------------------------------------------
 *  Topics follow the three practical labs the exam is drawn from.
 * ------------------------------------------------------------------------ */
typedef enum Topic {
    TOPIC_LAB1 = 0,     /* core logic, loops, 1D arrays        */
    TOPIC_LAB2,         /* 2D arrays and grid / game mechanics */
    TOPIC_LAB3,         /* pointers and C strings              */
    TOPIC_COUNT
} Topic;

typedef enum QKind {
    Q_OUTPUT = 0,       /* Section A: type the expected output      */
    Q_BLANKS            /* Section B: type the missing code in gaps */
} QKind;

#define MAX_BLANKS 6
#define MAX_ALTS   6    /* accepted spellings per blank, NULL terminated */

/* One gap in a Section B snippet. `accepted` holds every spelling that is
 * marked correct; whitespace is ignored by the grader, so "-= 1" and "-=1"
 * do not both need listing. */
typedef struct Blank {
    const char *accepted[MAX_ALTS];
    const char *note;               /* shown during review */
} Blank;

typedef struct Question {
    const char *id;                 /* "L1-A3" - stable, used by progress */
    const char *title;
    const char *prompt;             /* what the student is asked to do    */
    const char *spec;               /* the function's specification       */
    const char *code;               /* snippet; ___x___ marks a blank     */
    Topic       topic;
    QKind       kind;
    const char *answer;             /* expected stdout (Q_OUTPUT only)    */
    const char *explain;            /* model explanation, shown in review */
    Blank       blanks[MAX_BLANKS];
    int         blankCount;
} Question;

extern const Question QUESTIONS[];
extern const int      QUESTION_COUNT;

const char *TopicName(Topic t);
const char *TopicShort(Topic t);
const char *TopicBlurb(Topic t);

/* How many questions a topic holds, and the index of its nth question. */
int TopicQuestionCount(Topic t);
int TopicQuestionIndex(Topic t, int nth);

/* --------------------------------------------------------------------------
 *  Lecture notes. Each note section is a heading, a prose body and an
 *  optional code example that renders through the syntax highlighter.
 * ------------------------------------------------------------------------ */
typedef struct Note {
    const char *heading;
    const char *body;
    const char *code;               /* may be NULL */
} Note;

#define NOTE_SECTION_COUNT 4        /* 3 labs + exam technique */

const char *NoteSectionName(int section);
const Note *NotesFor(int section, int *countOut);

#endif /* CONTENT_H */
