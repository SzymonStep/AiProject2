/* =============================================================================
 *  lesson.c  --  The lesson screen.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Left: the notes, the example and the key point, in a scrolling card.
 *  Right: whether the lesson is done, what the check involves, the button to
 *  start it, and previous / next lesson.
 * ========================================================================== */
#include "lesson.h"
#include "lessons.h"
#include "codeview.h"
#include "save.h"
#include "app.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>

#define SIDE_W   340
#define CODE_PT  15.0f

static PathId curPath = PATH_C_FUNDAMENTALS;
static int    curLesson = 0;
static float  scroll = 0.0f;
static float  contentH = 0.0f;   /* measured while drawing, used next frame */

void LessonScreenOpen(PathId path, int lesson)
{
    const LearningPath *p = PathGet(path);
    if (!p || lesson < 0 || lesson >= p->lessonCount) return;
    curPath = path;
    curLesson = lesson;
    scroll = 0.0f;
    SaveSetLastPath(path);
}

/* ---------------------------------------------------------------------------
 *  Shared drawing helpers
 * ------------------------------------------------------------------------- */
void DrawCodeClipped(const char *code, Rectangle r, float size, Rectangle clip, bool hideBlanks)
{
    float top = r.y > clip.y ? r.y : clip.y;
    float bottom = (r.y + r.height < clip.y + clip.height) ? r.y + r.height : clip.y + clip.height;
    if (bottom > top) {
        Rectangle visible = { r.x, top, r.width, bottom - top };
        CodeView(code, visible, size, top - r.y, hideBlanks);
    }
    /* CodeView ends its own scissor; put the caller's back */
    BeginScissorMode((int)clip.x, (int)clip.y, (int)clip.width, (int)clip.height);
}

float DrawParagraphs(const char *text, float x, float y, float width, int fontSize, Color c)
{
    char para[1200];
    float startY = y;
    const char *p = text;

    while (p && *p) {
        const char *end = strstr(p, "\n\n");
        int len = end ? (int)(end - p) : (int)strlen(p);
        if (len > (int)sizeof para - 1) len = (int)sizeof para - 1;
        memcpy(para, p, (size_t)len);
        para[len] = '\0';
        /* stray single newlines become spaces so the wrapper sees one line */
        for (int i = 0; i < len; i++) if (para[i] == '\n') para[i] = ' ';

        y += UiTextWrapped(para, x, y, width, fontSize, 7, c);
        p = end ? end + 2 : NULL;
        if (p) y += 10;
    }
    return y - startY;
}

/* ---------------------------------------------------------------------------
 *  Notes card
 * ------------------------------------------------------------------------- */
static void DrawNotes(Rectangle card)
{
    const LessonContent *c = LessonContentGet(curPath, curLesson);
    const char *heading = LessonHeading(curPath, curLesson);
    const char *notes = LessonNotes(curPath, curLesson);
    const char *code = LessonCode(curPath, curLesson);

    UiCard(card);
    Rectangle view = { card.x + 4, card.y + 4, card.width - 8, card.height - 8 };
    UiScrollArea(view, contentH, &scroll);

    float x = card.x + 30, w = card.width - 64;
    float y = card.y + 28 - scroll;
    float startY = y;

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    if (heading) {
        DrawText("FROM YOUR LECTURE NOTES", (int)x, (int)y, 10, COL_MUTED);
        y += 18;
        DrawText(heading, (int)x, (int)y, 18, COL_NAVY);
        y += 34;
    } else {
        DrawText("THE IDEA", (int)x, (int)y, 10, COL_MUTED);
        y += 22;
    }

    y += DrawParagraphs(notes, x, y, w, 14, COL_INK);
    y += 18;

    if (code) {
        DrawText("EXAMPLE", (int)x, (int)y, 10, COL_MUTED);
        y += 20;
        float h = CodeViewHeight(code, CODE_PT);
        DrawCodeClipped(code, (Rectangle){ x, y, w, h }, CODE_PT, view, false);
        y += h + 22;
    }

    if (c && c->keyPoint) {
        float boxH = 64;
        DrawRectangleRounded((Rectangle){ x, y, w, boxH }, 0.2f, 8, COL_BLUE_LIGHT);
        DrawRectangle((int)x, (int)(y + 10), 4, (int)boxH - 20, COL_BLUE);
        DrawText("KEY POINT", (int)(x + 20), (int)(y + 14), 10, COL_BLUE_DARK);
        DrawText(c->keyPoint, (int)(x + 20), (int)(y + 32), 13, COL_NAVY);
        y += boxH + 10;
    }

    EndScissorMode();
    contentH = (y - startY) + 40;
}

/* ---------------------------------------------------------------------------
 *  Side panel
 * ------------------------------------------------------------------------- */
static void DrawSide(Rectangle card)
{
    const LearningPath *p = PathGet(curPath);
    const LessonContent *c = LessonContentGet(curPath, curLesson);
    bool done = SaveLessonDone(curPath, curLesson);
    float x = card.x + 24, w = card.width - 48, y = card.y + 24;

    UiCard(card);

    if (done)
        UiBadge((Rectangle){ x, y, 104, 22 }, "COMPLETED", (Color){ 30, 140, 100, 255 },
                (Color){ 225, 245, 236, 255 }, 10);
    else
        UiBadge((Rectangle){ x, y, 104, 22 }, "NOT DONE YET", COL_BLUE_DARK, COL_BLUE_LIGHT, 10);
    y += 42;

    DrawText("CHECK YOUR UNDERSTANDING", (int)x, (int)y, 10, COL_MUTED);
    y += 22;

    char line[160];
    const char *button;
    if (c && c->mock >= 0) {
        DrawText("Timed mock paper", (int)x, (int)y, 16, COL_NAVY);
        y += 26;
        y += UiTextWrapped("10 questions in the real paper's format: Section A first, then "
                           "Section B, at about 90 seconds a mark. Answers are marked at the end.",
                           x, y, w, 12, 5, COL_INK);
        button = "Start the mock  >";
    } else if (c && c->mcqCount == 0) {
        int n = LessonQuestionCount(curPath, curLesson);
        snprintf(line, sizeof line, "%d exam questions", n);
        DrawText(line, (int)x, (int)y, 16, COL_NAVY);
        y += 26;
        y += UiTextWrapped("Section A: type exactly what the code prints. Section B: fill in "
                           "the missing code. Each answer is marked straight away.",
                           x, y, w, 12, 5, COL_INK);
        button = "Start practice  >";
    } else {
        snprintf(line, sizeof line, "%d quick questions", c ? c->mcqCount : 0);
        DrawText(line, (int)x, (int)y, 16, COL_NAVY);
        y += 26;
        y += UiTextWrapped("Multiple choice on this lesson, with an explanation after each "
                           "answer.", x, y, w, 12, 5, COL_INK);
        button = "Start quiz  >";
    }
    y += 10;
    snprintf(line, sizeof line, "Score 50%% or more to complete the lesson (+%d XP).", XP_LESSON);
    y += UiTextWrapped(line, x, y, w, 10, 4, COL_MUTED);
    y += 14;

    if (UiButton((Rectangle){ x, y, w, 40 }, done ? "Practise again  >" : button,
                 COL_BLUE, COL_BLUE_DARK, COL_WHITE, 13))
        AppStartLessonQuiz(curPath, curLesson);
    y += 62;

    DrawLine((int)x, (int)y, (int)(x + w), (int)y, COL_LINE);
    y += 18;

    /* previous / next */
    float half = (w - 12) / 2;
    if (curLesson > 0 &&
        UiButton((Rectangle){ x, y, half, 34 }, "<  Previous", COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 12))
        LessonScreenOpen(curPath, curLesson - 1);
    if (curLesson < p->lessonCount - 1 &&
        UiButton((Rectangle){ x + half + 12, y, half, 34 }, "Next  >", COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 12))
        LessonScreenOpen(curPath, curLesson + 1);
    y += 50;

    float robotY = card.y + card.height - 140;
    if (robotY > y) UiDrawRobot(card.x + (card.width - 106) / 2, robotY);
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void LessonScreenDraw(void)
{
    const LearningPath *p = PathGet(curPath);
    const Lesson *l = &p->lessons[curLesson];
    Rectangle area = UiContentArea();
    float x = area.x, right = area.x + area.width;

    char back[64], kicker[64];
    snprintf(back, sizeof back, "<  %s", p->title);
    if (UiLink(x, 28, back, 12)) AppOpenPath(curPath);

    snprintf(kicker, sizeof kicker, "LESSON %d OF %d  -  %d MIN", curLesson + 1, p->lessonCount, l->minutes);
    DrawText(kicker, (int)x, 54, 10, COL_BLUE);
    DrawText(l->title, (int)x, 72, 28, COL_NAVY);
    DrawText(l->summary, (int)x, 106, 12, COL_MUTED);

    float top = 136, bottom = area.y + area.height;
    DrawNotes((Rectangle){ x, top, area.width - SIDE_W - 20, bottom - top });
    DrawSide((Rectangle){ right - SIDE_W, top, SIDE_W, bottom - top });
}
