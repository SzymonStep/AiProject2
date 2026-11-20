/* =============================================================================
 *  revision.c  --  Revision Centre: mastery, weak spots and lecture notes.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Left: how much of each lab is mastered, and a drill of the questions you
 *  have got wrong most often. Right: every CExam Trainer lecture note,
 *  browsable by section, with its code example.
 * ========================================================================== */
#include "screens.h"
#include "app.h"
#include "progress.h"
#include "lesson.h"
#include "codeview.h"
#include "ui.h"

#include <stdio.h>

#define LEFT_W    380
#define LIST_W    270
#define WEAK_MAX  10

static int   section = 0;
static int   noteIdx = 0;
static float noteScroll = 0.0f;
static float noteContentH = 0.0f;

/* Questions answered wrong at least once and never answered right,
 * worst first. */
static int BuildWeakPool(int *pool)
{
    int n = 0;
    int misses[256];
    for (int i = 0; i < QUESTION_COUNT && n < 256; i++) {
        int seen = 0, correct = 0;
        ProgressGet(QUESTIONS[i].id, &seen, &correct);
        if (seen > correct && correct == 0) { pool[n] = i; misses[n] = seen; n++; }
    }
    /* insertion sort by misses, descending */
    for (int i = 1; i < n; i++) {
        int p = pool[i], m = misses[i], j = i - 1;
        while (j >= 0 && misses[j] < m) { pool[j + 1] = pool[j]; misses[j + 1] = misses[j]; j--; }
        pool[j + 1] = p; misses[j + 1] = m;
    }
    return n;
}

static void DrawLeft(Rectangle col)
{
    Rectangle mastery = { col.x, col.y, col.width, 240 };
    UiCard(mastery);
    float x = mastery.x + 24, y = mastery.y + 22, w = mastery.width - 48;
    UiText("MASTERY BY LAB", (int)x, (int)y, 10, COL_MUTED);
    y += 26;
    for (int t = 0; t < TOPIC_COUNT; t++) {
        float m = TopicMastery((Topic)t);
        char pct[8];
        snprintf(pct, sizeof pct, "%d%%", (int)(m * 100 + 0.5f));
        UiText(TopicName((Topic)t), (int)x, (int)y, 13, COL_NAVY);
        UiText(pct, (int)(x + w) - UiMeasure(pct, 12), (int)(y + 1), 12, COL_MUTED);
        UiProgressBar((Rectangle){ x, y + 22, w, 7 }, m, m >= 0.8f ? COL_GREEN : COL_BLUE);
        y += 52;
    }
    UiText("Mastered = answered fully right at least once.", (int)x, (int)y, 10, COL_MUTED);

    Rectangle weak = { col.x, col.y + 258, col.width, 230 };
    UiCard(weak);
    x = weak.x + 24; y = weak.y + 22;
    UiText("PRACTISE YOUR WEAKEST", (int)x, (int)y, 10, COL_MUTED);
    y += 26;

    int pool[256];
    int n = BuildWeakPool(pool);
    if (n == 0) {
        y += UiTextWrapped("Nothing to revise yet. Questions you get wrong in lessons, "
                           "challenges or mocks will collect here.", x, y, w, 13, 6, COL_INK);
        return;
    }

    char line[64];
    snprintf(line, sizeof line, "%d question%s still to crack", n, n == 1 ? "" : "s");
    UiText(line, (int)x, (int)y, 16, COL_NAVY);
    y += 28;
    for (int i = 0; i < n && i < 3; i++) {
        UiText(QUESTIONS[pool[i]].title, (int)(x + 12), (int)y, 12, COL_INK);
        DrawCircle((int)(x + 3), (int)(y + 6), 3, COL_BAD_FG);
        y += 20;
    }
    if (n > 3) { snprintf(line, sizeof line, "and %d more", n - 3); UiText(line, (int)(x + 12), (int)y, 11, COL_MUTED); }

    if (UiButton((Rectangle){ x, weak.y + weak.height - 62, w, 40 }, "Drill these  >",
                 COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
        AppStartPractice("Revision - weakest questions", MODE_MIXED, pool, n, WEAK_MAX, false,
                         SCR_REVISION, -1);
}

static void DrawNotes(Rectangle card)
{
    UiCard(card);
    float x = card.x + 20, y = card.y + 18;

    /* section tabs */
    for (int s = 0; s < NOTE_SECTION_COUNT; s++) {
        const char *name = NoteSectionName(s);
        float w = (float)UiMeasure(name, 12) + 28;
        Rectangle tab = { x, y, w, 32 };
        bool sel = s == section, over = UiIsHover(tab);
        DrawRectangleRounded(tab, 0.5f, 8, sel ? COL_BLUE : (over ? COL_BLUE_LIGHT : COL_PAGE));
        UiText(name, (int)(x + 14), (int)(y + 10), 12, sel ? COL_ON_ACCENT : COL_BLUE_DARK);
        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !sel) {
            section = s; noteIdx = 0; noteScroll = 0;
        }
        x += w + 8;
    }
    y += 48;

    int count = 0;
    const Note *notes = NotesFor(section, &count);
    if (noteIdx >= count) noteIdx = 0;

    /* heading list */
    float lx = card.x + 20;
    for (int i = 0; i < count; i++) {
        Rectangle row = { lx, y + i * 44, LIST_W, 40 };
        bool sel = i == noteIdx, over = UiIsHover(row);
        if (sel || over) DrawRectangleRounded(row, 0.2f, 8, sel ? COL_BLUE_LIGHT : COL_PAGE);
        UiTextWrapped(notes[i].heading, row.x + 12, row.y + 7, LIST_W - 24, 11, 3,
                      sel ? COL_BLUE_DARK : COL_INK);
        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { noteIdx = i; noteScroll = 0; }
    }

    /* selected note */
    Rectangle view = { lx + LIST_W + 20, y, card.x + card.width - (lx + LIST_W + 20) - 12,
                       card.y + card.height - y - 12 };
    UiScrollArea(view, noteContentH, &noteScroll);
    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    const Note *n = &notes[noteIdx];
    float nx = view.x + 6, ny = view.y - noteScroll, nw = view.width - 30, start = ny;
    UiText(n->heading, (int)nx, (int)ny, 18, COL_NAVY);
    ny += 32;
    ny += DrawParagraphs(n->body, nx, ny, nw, 13, COL_INK) + 16;
    if (n->code) {
        float h = CodeViewHeight(n->code, 14);
        DrawCodeClipped(n->code, (Rectangle){ nx, ny, nw, h }, 14, view, false);
        ny += h + 16;
    }
    EndScissorMode();
    noteContentH = ny - start + 20;
}

void RevisionDraw(void)
{
    Rectangle area = UiContentArea();
    float x = area.x;
    UiPageTitle(x, "REVISION CENTRE", "Revise and fix weak spots");
    UiText("Your mastery from every quiz, the questions to go back to, and all of your lecture notes.",
             (int)x, 92, 12, COL_MUTED);

    float top = 125, bottom = area.y + area.height;
    DrawLeft((Rectangle){ x, top, LEFT_W, bottom - top });
    DrawNotes((Rectangle){ x + LEFT_W + 18, top, area.width - LEFT_W - 18, bottom - top });
}
