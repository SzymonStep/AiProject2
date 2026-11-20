/* =============================================================================
 *  progress.c  --  Loading, saving and querying the student's history.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  The save format is deliberately plain text, one record per line:
 *
 *      L1-A3 4 3
 *      L2-B1 2 0
 *
 *  i.e. id, times attempted, times fully correct. It is readable, trivially
 *  diffable, and a corrupt or missing file just means starting from zero
 *  rather than crashing.
 * ========================================================================== */
#include "progress.h"

#include <stdio.h>
#include <string.h>
#include "raylib.h"          /* LoadFileText / SaveFileText / FileExists */

static ProgressRec gRecs[PROGRESS_MAX];
static int         gCount = 0;

/* Returns the record for `id`, creating it if there is room. NULL when the
 * table is full, which the callers treat as "do not record". */
static ProgressRec *Find(const char *id, bool create)
{
    for (int i = 0; i < gCount; i++)
        if (strncmp(gRecs[i].id, id, ID_CAP) == 0) return &gRecs[i];

    if (!create || gCount >= PROGRESS_MAX) return NULL;

    ProgressRec *r = &gRecs[gCount++];
    memset(r, 0, sizeof *r);
    strncpy(r->id, id, ID_CAP - 1);
    r->id[ID_CAP - 1] = '\0';
    return r;
}

void ProgressLoad(void)
{
    gCount = 0;
    if (!FileExists(PROGRESS_FILE)) return;

    char *text = LoadFileText(PROGRESS_FILE);
    if (!text) return;

    /* Walk the buffer line by line without modifying it. */
    const char *p = text;
    while (*p && gCount < PROGRESS_MAX) {
        char id[ID_CAP];
        int  seen = 0, correct = 0;

        /* sscanf stops at whitespace, so one call reads a whole record.
         * The width limit on %s is what keeps a long id from overflowing. */
        if (sscanf(p, "%15s %d %d", id, &seen, &correct) == 3) {
            ProgressRec *r = Find(id, true);
            if (r) { r->seen = seen; r->correct = correct; }
        }

        const char *nl = strchr(p, '\n');
        if (!nl) break;
        p = nl + 1;
    }

    UnloadFileText(text);
}

void ProgressSave(void)
{
    /* Build the whole file in one buffer: 32 bytes per record is ample for
     * "id seen correct\n". */
    char buf[PROGRESS_MAX * 32];
    int  n = 0;

    for (int i = 0; i < gCount; i++) {
        int written = snprintf(buf + n, sizeof buf - (size_t)n, "%s %d %d\n",
                               gRecs[i].id, gRecs[i].seen, gRecs[i].correct);
        if (written <= 0 || n + written >= (int)sizeof buf) break;
        n += written;
    }
    buf[n] = '\0';

    SaveFileText(PROGRESS_FILE, buf);
}

void ProgressReset(void)
{
    gCount = 0;
    SaveFileText(PROGRESS_FILE, "");
}

void ProgressRecord(const char *id, bool correct)
{
    ProgressRec *r = Find(id, true);
    if (!r) return;
    r->seen++;
    if (correct) r->correct++;
    ProgressSave();            /* small file: saving every answer is fine */
}

void ProgressGet(const char *id, int *seen, int *correct)
{
    ProgressRec *r = Find(id, false);
    if (seen)    *seen    = r ? r->seen    : 0;
    if (correct) *correct = r ? r->correct : 0;
}

int TopicSolved(Topic t)
{
    int solved = 0;
    for (int i = 0; i < QUESTION_COUNT; i++) {
        if (QUESTIONS[i].topic != t) continue;
        int seen = 0, correct = 0;
        ProgressGet(QUESTIONS[i].id, &seen, &correct);
        if (correct > 0) solved++;
    }
    return solved;
}

float TopicMastery(Topic t)
{
    int total = TopicQuestionCount(t);
    if (total == 0) return 0.0f;
    return (float)TopicSolved(t) / (float)total;
}

int ProgressAttempts(void)
{
    int n = 0;
    for (int i = 0; i < gCount; i++) n += gRecs[i].seen;
    return n;
}

int ProgressCorrect(void)
{
    int n = 0;
    for (int i = 0; i < gCount; i++) n += gRecs[i].correct;
    return n;
}

int ProgressSolvedAll(void)
{
    int n = 0;
    for (int t = 0; t < TOPIC_COUNT; t++) n += TopicSolved((Topic)t);
    return n;
}

int WeakestQuestion(void)
{
    int best = -1, bestMisses = 0;

    for (int i = 0; i < QUESTION_COUNT; i++) {
        int seen = 0, correct = 0;
        ProgressGet(QUESTIONS[i].id, &seen, &correct);
        if (correct > 0) continue;              /* already mastered once */

        /* Prefer the question that has been attempted and failed the most.
         * A never-attempted question scores 0 misses, so an actually failed
         * one always wins - which is the behaviour we want. */
        int misses = seen - correct;
        if (misses > bestMisses) { bestMisses = misses; best = i; }
    }
    return best;
}
