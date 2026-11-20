/* =============================================================================
 *  quizscreen.c  --  The quiz screen and its results.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Marking is done by the quiz engine (quiz.c) for exam questions and by a
 *  simple index compare for multiple choice. This file only decides what is
 *  on screen and when to move on.
 * ========================================================================== */
#include "quizscreen.h"
#include "lessons.h"
#include "lesson.h"
#include "codeview.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define ANSWER_W  440
#define CODE_PT   15.0f

typedef enum Kind { KIND_MCQ, KIND_ENGINE } Kind;

static struct {
    Kind   kind;
    char   title[96];
    Screen returnTo;
    PathId path;
    int    lesson;          /* -1 when not part of a lesson */
    int    mock;

    /* multiple choice */
    const LessonContent *lc;
    int    cur;
    int    chosen[LESSON_MCQ];

    /* exam questions */
    Quiz     quiz;
    QuizMode mode;
    int      pool[256];
    int      poolCount;
    int      limit;
    bool     shuffle;
    TextBox  out;
    TextBox  blanks[MAX_BLANKS];
    int      boxesFor;      /* question index the boxes were set up for */
    float    codeScroll;

    /* results */
    bool   finished;
    int    marks, available, correct, total, xp;
    bool   passed, completedLesson;
    float  resultScroll;
} Q;

/* ---------------------------------------------------------------------------
 *  Starting and finishing
 * ------------------------------------------------------------------------- */
static void ResetCommon(void)
{
    Q.finished = false;
    Q.completedLesson = false;
    Q.resultScroll = 0.0f;
    Q.boxesFor = -1;
    Q.codeScroll = 0.0f;
}

void QuizScreenStartMcq(PathId path, int lesson)
{
    const LessonContent *lc = LessonContentGet(path, lesson);
    if (!lc || lc->mcqCount == 0) return;

    ResetCommon();
    Q.kind = KIND_MCQ;
    Q.lc = lc;
    Q.cur = 0;
    for (int i = 0; i < LESSON_MCQ; i++) Q.chosen[i] = -1;
    Q.path = path;
    Q.lesson = lesson;
    Q.mock = -1;
    Q.returnTo = SCR_LESSON;
    snprintf(Q.title, sizeof Q.title, "%s", PathGet(path)->lessons[lesson].title);
}

void QuizScreenStartEngine(const char *title, QuizMode mode, const int *pool, int poolCount,
                           int limit, bool shuffle, Screen returnTo,
                           PathId path, int lesson, int mock)
{
    ResetCommon();
    Q.kind = KIND_ENGINE;
    snprintf(Q.title, sizeof Q.title, "%s", title);
    Q.mode = mode;
    Q.poolCount = poolCount < 256 ? poolCount : 256;
    memcpy(Q.pool, pool, sizeof(int) * (size_t)Q.poolCount);
    Q.limit = limit;
    Q.shuffle = shuffle;
    Q.returnTo = returnTo;
    Q.path = path;
    Q.lesson = lesson;
    Q.mock = mock;
    QuizBuildFromPool(&Q.quiz, mode, Q.pool, Q.poolCount, limit, (unsigned)time(NULL), shuffle);
}

static void Restart(void)
{
    if (Q.kind == KIND_MCQ) QuizScreenStartMcq(Q.path, Q.lesson);
    else QuizScreenStartEngine(Q.title, Q.mode, Q.pool, Q.poolCount, Q.limit, Q.shuffle,
                               Q.returnTo, Q.path, Q.lesson, Q.mock);
}

static void Finish(void)
{
    if (Q.finished) return;

    if (Q.kind == KIND_MCQ) {
        Q.total = Q.lc->mcqCount;
        Q.correct = 0;
        for (int i = 0; i < Q.total; i++)
            if (Q.chosen[i] == Q.lc->mcq[i].answer) Q.correct++;
        Q.marks = Q.correct;
        Q.available = Q.total;
    } else {
        Q.quiz.finished = true;
        Q.total = Q.quiz.count;
        Q.correct = 0;
        for (int i = 0; i < Q.quiz.count; i++)
            if (Q.quiz.attempts[i].correct) Q.correct++;
        Q.marks = Q.quiz.marks;
        Q.available = Q.quiz.marksAvailable;
    }

    Q.xp = Q.correct * XP_CORRECT;
    SaveAddXp(Q.xp);
    SaveRecordQuiz(Q.marks, Q.available, Q.mock);

    Q.passed = Q.available > 0 && Q.marks * 2 >= Q.available;
    if (Q.passed && Q.lesson >= 0 && SaveCompleteLesson(Q.path, Q.lesson)) {
        Q.completedLesson = true;
        Q.xp += XP_LESSON;
    }
    Q.finished = true;
}

/* ---------------------------------------------------------------------------
 *  Header
 * ------------------------------------------------------------------------- */
static void DrawHeader(Rectangle area, int index, int count, double timeLeft)
{
    float x = area.x, right = area.x + area.width;

    if (UiLink(x, 28, "<  Leave quiz", 12)) AppGoTo(Q.returnTo);
    UiText(Q.title, (int)x, 54, 10, COL_BLUE);

    char head[48];
    snprintf(head, sizeof head, "Question %d of %d", index + 1, count);
    UiText(head, (int)x, 72, 28, COL_NAVY);

    if (timeLeft >= 0) {
        char clock[32];
        int s = (int)timeLeft;
        snprintf(clock, sizeof clock, "%02d:%02d left", s / 60, s % 60);
        int w = UiMeasure(clock, 20);
        Color c = s < 60 ? COL_BAD_FG : COL_NAVY;
        UiText(clock, (int)right - w, 78, 20, c);
    }

    UiProgressBar((Rectangle){ x, 112, area.width, 6 }, count ? (float)index / count : 0, COL_BLUE);
}

/* ---------------------------------------------------------------------------
 *  Multiple choice
 * ------------------------------------------------------------------------- */
static void DrawMcq(Rectangle area)
{
    const Mcq *m = &Q.lc->mcq[Q.cur];
    int chosen = Q.chosen[Q.cur];
    bool answered = chosen >= 0;

    DrawHeader(area, Q.cur, Q.lc->mcqCount, -1);

    float w = area.width < 900 ? area.width : 900;
    Rectangle card = { area.x, 140, w, area.y + area.height - 140 };
    UiCard(card);

    float x = card.x + 32, y = card.y + 30, iw = card.width - 64;
    y += UiTextWrapped(m->question, x, y, iw, 20, 8, COL_NAVY) + 18;

    for (int i = 0; i < MCQ_OPTIONS; i++) {
        Rectangle row = { x, y, iw, 52 };
        bool over = !answered && UiIsHover(row);
        Color bg = over ? COL_BLUE_LIGHT : COL_PAGE, fg = COL_INK;
        if (answered && i == m->answer)  { bg = COL_GOOD_BG; fg = COL_GOOD_FG; }
        else if (answered && i == chosen) { bg = COL_BAD_BG;  fg = COL_BAD_FG; }

        DrawRectangleRounded(row, 0.2f, 8, bg);
        char letter[2] = { (char)('A' + i), '\0' };
        DrawCircle((int)(x + 28), (int)(y + 26), 14, COL_SURFACE);
        UiText(letter, (int)(x + 28) - UiMeasure(letter, 14) / 2, (int)(y + 19), 14, fg);
        UiText(m->options[i], (int)(x + 56), (int)(y + 18), 15, fg);
        if (answered && i == m->answer)  UiTick(x + iw - 28, y + 26, 7, COL_GOOD_FG);
        else if (answered && i == chosen) UiCross(x + iw - 28, y + 26, 7, COL_BAD_FG);

        if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) Q.chosen[Q.cur] = i;
        y += 62;
    }

    if (!answered) {
        UiText("Pick an answer.", (int)x, (int)(y + 10), 12, COL_MUTED);
        return;
    }

    y += 10;
    bool right = chosen == m->answer;
    UiText(right ? "Correct!" : "Not quite.", (int)x, (int)y, 18, right ? COL_GOOD_FG : COL_BAD_FG);
    y += 30;
    y += UiTextWrapped(m->explain, x, y, iw, 14, 6, COL_INK) + 20;

    bool last = Q.cur == Q.lc->mcqCount - 1;
    if (UiButton((Rectangle){ x, y, 200, 40 }, last ? "See results  >" : "Next question  >",
                 COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13)) {
        if (last) Finish(); else Q.cur++;
    }
}

/* ---------------------------------------------------------------------------
 *  Exam questions
 * ------------------------------------------------------------------------- */
static void SetUpBoxes(const Question *qu, const Attempt *a)
{
    TextBoxInit(&Q.out, true);
    for (int i = 0; i < MAX_BLANKS; i++) TextBoxInit(&Q.blanks[i], false);
    if (a->answered && !a->revealed) {
        TextBoxSet(&Q.out, a->given);
        for (int i = 0; i < qu->blankCount; i++) TextBoxSet(&Q.blanks[i], a->givenBlanks[i]);
    }
    if (qu->kind == Q_OUTPUT) Q.out.focused = true;
    else Q.blanks[0].focused = true;
    Q.boxesFor = Q.quiz.current;
    Q.codeScroll = 0.0f;
}

static void Submit(const Question *qu)
{
    if (qu->kind == Q_OUTPUT) {
        QuizSubmitOutput(&Q.quiz, Q.out.text);
    } else {
        char given[MAX_BLANKS][BLANK_CAP];
        memset(given, 0, sizeof given);
        for (int i = 0; i < qu->blankCount; i++)
            snprintf(given[i], BLANK_CAP, "%s", Q.blanks[i].text);
        QuizSubmitBlanks(&Q.quiz, given);
    }
}

static void NextOrFinish(void)
{
    if (!QuizAdvance(&Q.quiz)) Finish();
}

static void DrawQuestionCard(Rectangle card, const Question *qu, const Attempt *a, bool exam)
{
    UiCard(card);
    float x = card.x + 26, y = card.y + 22, w = card.width - 52;

    const char *section = qu->kind == Q_OUTPUT ? "SECTION A  -  WHAT IS THE OUTPUT?"
                                               : "SECTION B  -  FILL IN THE BLANKS";
    UiBadge((Rectangle){ x, y, (float)UiMeasure(section, 9) + 24, 20 }, section,
            COL_BLUE_DARK, COL_BLUE_LIGHT, 9);
    y += 32;
    UiText(qu->title, (int)x, (int)y, 18, COL_NAVY);
    y += 28;
    if (qu->prompt) y += UiTextWrapped(qu->prompt, x, y, w, 13, 6, COL_INK) + 6;
    if (qu->spec)   y += UiTextWrapped(qu->spec, x, y, w, 11, 5, COL_MUTED) + 6;
    y += 6;

    float full = CodeViewHeight(qu->code, CODE_PT);
    float room = card.y + card.height - 20 - y;
    Rectangle code = { x, y, w, full < room ? full : room };   /* fit short snippets */
    float maxScroll = full - code.height;
    if (maxScroll < 0) maxScroll = 0;
    if (UiIsHover(code)) Q.codeScroll -= GetMouseWheelMove() * 40.0f;
    if (Q.codeScroll < 0) Q.codeScroll = 0;
    if (Q.codeScroll > maxScroll) Q.codeScroll = maxScroll;

    bool hideBlanks = !(a->answered && !exam);
    CodeView(qu->code, code, CODE_PT, Q.codeScroll, hideBlanks);
    if (maxScroll > 0)
        UiText("scroll for more", (int)(code.x + code.width - 110), (int)(code.y + code.height - 18), 10,
                 (Color){ 150, 165, 188, 255 });
}

static float DrawFeedback(float x, float y, float w, const Question *qu, const Attempt *a)
{
    const char *verdict = a->revealed ? "Answer shown" : a->correct ? "Correct!"
                        : a->marks > 0 ? "Partly right" : "Not quite";
    Color c = a->correct ? COL_GOOD_FG : (a->marks > 0 ? COL_GOLD : COL_BAD_FG);
    char marks[32];
    snprintf(marks, sizeof marks, "%d / %d", a->marks, a->marksAvailable);
    UiText(verdict, (int)x, (int)y, 18, c);
    UiText(marks, (int)(x + w) - UiMeasure(marks, 14), (int)(y + 3), 14, COL_MUTED);
    y += 30;

    if (qu->kind == Q_OUTPUT) {
        UiText("EXPECTED OUTPUT", (int)x, (int)y, 10, COL_MUTED);
        y += 18;
        const char *p = qu->answer;
        char line[256];
        while (p && *p) {
            const char *nl = strchr(p, '\n');
            int len = nl ? (int)(nl - p) : (int)strlen(p);
            if (len > 255) len = 255;
            memcpy(line, p, (size_t)len);
            line[len] = '\0';
            MonoText(line, x, y, 15, COL_NAVY);
            y += 19;
            p = nl ? nl + 1 : NULL;
        }
        y += 8;
    }
    if (qu->explain) y += UiTextWrapped(qu->explain, x, y, w, 12, 5, COL_INK) + 8;
    return y;
}

static void DrawAnswerCard(Rectangle card, const Question *qu, const Attempt *a, bool exam)
{
    UiCard(card);
    float x = card.x + 24, w = card.width - 48, y = card.y + 22;
    BeginScissorMode((int)card.x, (int)card.y, (int)card.width, (int)card.height);

    bool locked = a->answered;
    bool submit = false;

    if (qu->kind == Q_OUTPUT) {
        UiText("YOUR OUTPUT", (int)x, (int)y, 10, COL_MUTED);
        y += 18;
        Rectangle box = { x, y, w, 170 };
        if (locked) Q.out.focused = false;
        bool enter = TextBoxUpdateDraw(&Q.out, box, "Type exactly what the program prints...");
        if (!locked && enter) submit = true;
        y += 178;
        UiText("Each printed line on its own line. Ctrl+Enter submits.", (int)x, (int)y, 10, COL_MUTED);
        y += 22;
    } else {
        for (int i = 0; i < qu->blankCount; i++) {
            char label[32];
            snprintf(label, sizeof label, "BLANK %d", i + 1);
            UiText(label, (int)x, (int)(y + 12), 10, COL_MUTED);
            Rectangle box = { x + 70, y, w - 70 - (locked ? 34 : 0), 36 };
            if (locked) Q.blanks[i].focused = false;
            if (TextBoxUpdateDraw(&Q.blanks[i], box, NULL) && !locked) {
                /* Enter moves to the next blank; on the last one it submits */
                Q.blanks[i].focused = false;
                if (i + 1 < qu->blankCount) Q.blanks[i + 1].focused = true;
                else submit = true;
            }
            if (locked) {
                if (a->blankCorrect[i]) UiTick(x + w - 14, y + 18, 7, COL_GOOD_FG);
                else UiCross(x + w - 14, y + 18, 7, COL_BAD_FG);
            }
            y += 44;
            if (locked && !exam && !a->blankCorrect[i]) {
                char model[BLANK_CAP + 16];
                char ans[BLANK_CAP];
                BlankModelAnswer(qu, i, ans, sizeof ans);
                snprintf(model, sizeof model, "answer: %s", ans);
                MonoText(model, x + 70, y - 4, 13, COL_GOOD_FG);
                y += 18;
            }
        }
        UiText("Enter moves to the next blank.", (int)x, (int)y, 10, COL_MUTED);
        y += 22;
    }

    if (!locked) {
        float half = (w - 12) / 2;
        if (UiButton((Rectangle){ x, y, half, 40 }, exam ? "Save answer  >" : "Submit",
                     COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
            submit = true;
        if (UiButton((Rectangle){ x + half + 12, y, half, 40 }, exam ? "Skip" : "Show answer",
                     COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 13)) {
            if (exam) { QuizReveal(&Q.quiz); NextOrFinish(); }
            else QuizReveal(&Q.quiz);
        }
        if (submit) {
            Submit(qu);
            if (exam) NextOrFinish();
        }
    } else if (!exam) {
        y += 4;
        bool last = Q.quiz.current == Q.quiz.count - 1;
        if (UiButton((Rectangle){ x, y, w, 40 }, last ? "See results  >" : "Next question  >",
                     COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
            NextOrFinish();
        y += 58;
        DrawLine((int)x, (int)y, (int)(x + w), (int)y, COL_LINE);
        y += 16;
        DrawFeedback(x, y, w, qu, a);
    }

    EndScissorMode();
}

static void DrawEngine(Rectangle area)
{
    bool exam = Q.mode == MODE_EXAM;
    double left = QuizTimeLeft(&Q.quiz);
    if (exam && left == 0.0) { Finish(); return; }

    const Question *qu = QuizCurrentQuestion(&Q.quiz);
    Attempt *a = QuizCurrentAttempt(&Q.quiz);
    if (!qu || !a) { Finish(); return; }
    if (Q.boxesFor != Q.quiz.current) SetUpBoxes(qu, a);

    DrawHeader(area, Q.quiz.current, Q.quiz.count, exam ? left : -1);

    float top = 140, bottom = area.y + area.height;
    DrawQuestionCard((Rectangle){ area.x, top, area.width - ANSWER_W - 20, bottom - top }, qu, a, exam);
    DrawAnswerCard((Rectangle){ area.x + area.width - ANSWER_W, top, ANSWER_W, bottom - top }, qu, a, exam);
}

/* ---------------------------------------------------------------------------
 *  Results
 * ------------------------------------------------------------------------- */
static void DrawResults(Rectangle area)
{
    float x = area.x;
    UiPageTitle(x, Q.title, Q.mock >= 0 ? "Mock paper marked" : "Quiz complete");

    float w = area.width < 1000 ? area.width : 1000;
    Rectangle top = { x, 105, w, 170 };
    UiCard(top);

    float pct = Q.available ? (float)Q.marks / Q.available : 0;
    UiProgressRing((Vector2){ top.x + 90, top.y + 85 }, 56, 12, pct,
                   Q.passed ? COL_GREEN : COL_GOLD, COL_SURFACE);

    float tx = top.x + 180, ty = top.y + 34;
    char line[96];
    snprintf(line, sizeof line, "%d / %d marks", Q.marks, Q.available);
    UiText(line, (int)tx, (int)ty, 26, COL_NAVY);
    ty += 38;
    const char *msg = Q.passed ? (pct >= 0.999f ? "Perfect score. Brilliant work!" : "Nice work, that's a pass.")
                               : "Not a pass yet. Read the explanations and try again.";
    UiText(msg, (int)tx, (int)ty, 14, COL_INK);
    ty += 28;
    snprintf(line, sizeof line, "+%d XP earned", Q.xp);
    UiText(line, (int)tx, (int)ty, 14, COL_GOLD);
    if (Q.completedLesson) {
        UiText("Lesson completed!", (int)tx + UiMeasure(line, 14) + 24, (int)ty, 14, COL_GOOD_FG);
    }

    /* buttons */
    float btnY = top.y + top.height + 18;
    float bx = x;
    if (Q.lesson >= 0) {
        if (UiButton((Rectangle){ bx, btnY, 170, 40 }, "Back to lesson", COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
            AppOpenLesson(Q.path, Q.lesson);
        bx += 182;
        const LearningPath *p = PathGet(Q.path);
        if (Q.passed && Q.lesson + 1 < p->lessonCount) {
            if (UiButton((Rectangle){ bx, btnY, 170, 40 }, "Next lesson  >", COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
                AppOpenLesson(Q.path, Q.lesson + 1);
            bx += 182;
        }
    } else {
        if (UiButton((Rectangle){ bx, btnY, 170, 40 }, "Done", COL_BLUE, COL_BLUE_DARK, COL_ON_ACCENT, 13))
            AppGoTo(Q.returnTo);
        bx += 182;
    }
    if (UiButton((Rectangle){ bx, btnY, 140, 40 }, "Try again", COL_BLUE_LIGHT, COL_LINE, COL_BLUE_DARK, 13))
        Restart();

    /* question list */
    float listTop = btnY + 64;
    UiText("YOUR ANSWERS", (int)x, (int)listTop, 10, COL_MUTED);
    Rectangle view = { x, listTop + 20, w, area.y + area.height - listTop - 20 };
    int rows = Q.total;
    float rowH = 46;
    UiScrollArea(view, rows * rowH, &Q.resultScroll);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    for (int i = 0; i < rows; i++) {
        Rectangle r = { view.x, view.y + i * rowH - Q.resultScroll, view.width - 14, rowH - 6 };
        bool ok;
        const char *label;
        char marks[24] = "";
        if (Q.kind == KIND_MCQ) {
            ok = Q.chosen[i] == Q.lc->mcq[i].answer;
            label = Q.lc->mcq[i].question;
        } else {
            const Attempt *at = &Q.quiz.attempts[i];
            ok = at->correct;
            label = QUESTIONS[at->qIndex].title;
            snprintf(marks, sizeof marks, "%d / %d", at->marks, at->marksAvailable);
        }
        DrawRectangleRounded(r, 0.2f, 8, COL_SURFACE);
        DrawCircle((int)(r.x + 24), (int)(r.y + r.height / 2), 12, ok ? COL_GOOD_BG : COL_BAD_BG);
        if (ok) UiTick(r.x + 24, r.y + r.height / 2, 5, COL_GOOD_FG);
        else    UiCross(r.x + 24, r.y + r.height / 2, 5, COL_BAD_FG);
        UiText(label, (int)(r.x + 50), (int)(r.y + r.height / 2 - 7), 13, COL_NAVY);
        UiText(marks, (int)(r.x + r.width - 16) - UiMeasure(marks, 12), (int)(r.y + r.height / 2 - 6), 12, COL_MUTED);
    }
    EndScissorMode();
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void QuizScreenDraw(void)
{
    Rectangle area = UiContentArea();
    if (Q.finished)              DrawResults(area);
    else if (Q.kind == KIND_MCQ) DrawMcq(area);
    else                         DrawEngine(area);
}
