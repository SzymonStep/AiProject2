/* =============================================================================
 *  learnpath.c  --  The learning path screen.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Everything is laid out from the current window size rather than fixed
 *  coordinates, so the screen fills a 1440 x 900 or a 1920 x 1080 window.
 * ========================================================================== */
#include "learnpath.h"
#include "sidebar.h"
#include "ui.h"
#include "app.h"
#include "save.h"

#include <stdio.h>

#define MARGIN_L     32     /* gap between sidebar and content */
#define MARGIN_R     42
#define GAP          18
#define SELECTOR_Y   105
#define SELECTOR_H   118
#define DETAIL_Y     (SELECTOR_Y + SELECTOR_H + 24)
#define RIGHT_W      360
#define ROW_H        60

static PathId selected = PATH_C_FUNDAMENTALS;
static float  lessonScroll = 0.0f;

/* Icon tile colour per path, matching the dashboard cards. */
static Color PathTint(PathId id)
{
    switch (id) {
    case PATH_EXAM1:        return COL_CYAN_LIGHT;
    case PATH_EXAM2:        return (Color){ 255, 245, 223, 255 };
    case PATH_ADVANCED_CPP: return COL_PURPLE;
    default:                return COL_BLUE_LIGHT;
    }
}

void LearnPathSelect(PathId path)
{
    if ((int)path < 0 || path >= PATH_COUNT) return;
    selected = path;
    lessonScroll = 0.0f;
}

PathId LearnPathSelected(void)
{
    return selected;
}

/* ---------------------------------------------------------------------------
 *  Small pieces
 * ------------------------------------------------------------------------- */
static void DrawIconTile(Rectangle r, const char *icon, Color bg, int fontSize)
{
    DrawRectangleRounded(r, 0.25f, 8, bg);
    int w = MeasureText(icon, fontSize);
    DrawText(icon, (int)(r.x + (r.width - w) / 2),
             (int)(r.y + (r.height - fontSize) / 2), fontSize, COL_BLUE_DARK);
}

static void DrawTick(float cx, float cy, float s, Color c)
{
    DrawLineEx((Vector2){ cx - s, cy }, (Vector2){ cx - s * 0.3f, cy + s * 0.7f }, 2.5f, c);
    DrawLineEx((Vector2){ cx - s * 0.3f, cy + s * 0.7f }, (Vector2){ cx + s, cy - s * 0.6f }, 2.5f, c);
}

static void FormatDuration(int minutes, char *out, int cap)
{
    if (minutes < 60) snprintf(out, cap, "%d min", minutes);
    else if (minutes % 60 == 0) snprintf(out, cap, "%d h", minutes / 60);
    else snprintf(out, cap, "%d h %d min", minutes / 60, minutes % 60);
}

/* ---------------------------------------------------------------------------
 *  Header and path selector
 * ------------------------------------------------------------------------- */
static void DrawHeader(float x, float right)
{
    DrawText("LEARNING PATHS", (int)x, 30, 10, COL_BLUE);
    DrawText("Choose your path", (int)x, 51, 28, COL_NAVY);

    int done = 0, total = 0;
    for (int i = 0; i < PATH_COUNT; ++i) {
        done  += PathCompleted((PathId)i);
        total += PathGet((PathId)i)->lessonCount;
    }
    char summary[64];
    snprintf(summary, sizeof summary, "%d of %d lessons completed", done, total);
    DrawText(summary, (int)right - MeasureText(summary, 12), 44, 12, COL_INK);
}

static void DrawSelectorCard(Rectangle r, PathId id)
{
    const LearningPath *p = PathGet(id);
    bool isSel = (id == selected);
    bool over = UiIsHover(r);

    DrawRectangleRounded(r, 0.12f, 12, isSel ? COL_BLUE_LIGHT : COL_WHITE);
    if (isSel)
        DrawRectangleRoundedLinesEx(r, 0.12f, 12, 2.0f, COL_BLUE);
    else
        DrawRectangleRoundedLines(r, 0.12f, 12,
                                  over ? (Color){ 190, 220, 245, 255 } : COL_LINE);

    DrawIconTile((Rectangle){ r.x + 16, r.y + 16, 42, 42 }, p->icon,
                 isSel ? COL_WHITE : PathTint(id), 20);
    DrawText(p->title, (int)(r.x + 72), (int)(r.y + 20), 15, COL_NAVY);

    char count[48];
    snprintf(count, sizeof count, "%d / %d %s", PathCompleted(id), p->lessonCount, p->unitWord);
    DrawText(count, (int)(r.x + 72), (int)(r.y + 42), 10, COL_MUTED);

    float t = PathProgress(id);
    UiProgressBar((Rectangle){ r.x + 16, r.y + 82, r.width - 72, 6 }, t, COL_BLUE);
    char pct[8];
    snprintf(pct, sizeof pct, "%d%%", (int)(t * 100.0f + 0.5f));
    DrawText(pct, (int)(r.x + r.width - 16) - MeasureText(pct, 10), (int)(r.y + 80), 10, COL_MUTED);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isSel)
        LearnPathSelect(id);
}

static void DrawSelector(float x, float width)
{
    float w = (width - GAP * (PATH_COUNT - 1)) / PATH_COUNT;
    for (int i = 0; i < PATH_COUNT; ++i)
        DrawSelectorCard((Rectangle){ x + i * (w + GAP), SELECTOR_Y, w, SELECTOR_H }, (PathId)i);
}

/* ---------------------------------------------------------------------------
 *  Left panel: the selected path and its lessons
 * ------------------------------------------------------------------------- */
static void DrawLessonRow(Rectangle r, int index, const Lesson *l, int next,
                          bool interactive)
{
    bool done   = SaveLessonDone(selected, index);
    bool isNext = index == next;
    bool over   = interactive && UiIsHover(r);

    if (isNext)     DrawRectangleRounded(r, 0.18f, 8, COL_BLUE_LIGHT);
    else if (over)  DrawRectangleRounded(r, 0.18f, 8, COL_PAGE);

    /* number bubble: green tick when done, solid blue when next */
    Vector2 c = { r.x + 30, r.y + r.height / 2 };
    if (done) {
        DrawCircleV(c, 15, (Color){ 225, 245, 236, 255 });
        DrawTick(c.x, c.y, 6, COL_GREEN);
    } else {
        char num[8];
        snprintf(num, sizeof num, "%d", index + 1);
        DrawCircleV(c, 15, isNext ? COL_BLUE : COL_PAGE);
        DrawText(num, (int)(c.x - MeasureText(num, 12) / 2.0f), (int)(c.y - 6), 12,
                 isNext ? COL_WHITE : COL_MUTED);
    }

    DrawText(l->title,   (int)(r.x + 60), (int)(r.y + 13), 14, COL_NAVY);
    DrawText(l->summary, (int)(r.x + 60), (int)(r.y + 34), 10, COL_MUTED);

    /* right side: badge then duration */
    float rx = r.x + r.width - 16;
    char mins[16];
    snprintf(mins, sizeof mins, "%d min", l->minutes);
    rx -= MeasureText(mins, 10);
    DrawText(mins, (int)rx, (int)(r.y + r.height / 2 - 5), 10, COL_MUTED);

    if (done)
        UiBadge((Rectangle){ rx - 84, r.y + r.height / 2 - 10, 68, 20 }, "DONE",
                (Color){ 30, 140, 100, 255 }, (Color){ 225, 245, 236, 255 }, 9);
    else if (isNext)
        UiBadge((Rectangle){ rx - 84, r.y + r.height / 2 - 10, 68, 20 }, "UP NEXT",
                COL_WHITE, COL_BLUE, 9);

    /* separator */
    if (!isNext)
        DrawLine((int)(r.x + 60), (int)(r.y + r.height - 1),
                 (int)(r.x + r.width - 16), (int)(r.y + r.height - 1), COL_LINE);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AppOpenLesson(selected, index);
}

static void DrawStat(float x, float y, const char *value, const char *label)
{
    DrawText(value, (int)x, (int)y, 18, COL_NAVY);
    DrawText(label, (int)x, (int)(y + 24), 9, COL_MUTED);
}

static void DrawDetailPanel(Rectangle panel)
{
    const LearningPath *p = PathGet(selected);
    int next = PathNextLesson(selected);

    DrawRectangleRounded(panel, 0.04f, 12, COL_WHITE);

    float x = panel.x + 24, y = panel.y + 22;
    DrawIconTile((Rectangle){ x, y, 52, 52 }, p->icon, PathTint(selected), 24);
    DrawText(p->title, (int)(x + 68), (int)(y + 4), 22, COL_NAVY);
    DrawText(p->tagline, (int)(x + 68), (int)(y + 32), 11, COL_MUTED);
    y += 70;

    y += UiTextWrapped(p->description, x, y, panel.width - 48, 12, 6, COL_INK);
    y += 10;

    /* stats row */
    char a[24], b[24], c[24];
    snprintf(a, sizeof a, "%d", p->lessonCount);
    snprintf(b, sizeof b, "%d", PathCompleted(selected));
    FormatDuration(PathTotalMinutes(selected), c, sizeof c);
    DrawStat(x,       y, a, p->unitWord);
    DrawStat(x + 140, y, b, "completed");
    DrawStat(x + 280, y, c, "estimated time");
    y += 52;

    DrawLine((int)x, (int)y, (int)(panel.x + panel.width - 24), (int)y, COL_LINE);
    y += 8;

    /* scrollable lesson list */
    Rectangle view = { panel.x + 8, y, panel.width - 16, panel.y + panel.height - 12 - y };
    float contentH = p->lessonCount * (float)ROW_H;
    UiScrollArea(view, contentH, &lessonScroll);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    for (int i = 0; i < p->lessonCount; ++i) {
        Rectangle row = { view.x, view.y + i * ROW_H - lessonScroll, view.width - 12, ROW_H - 4 };
        if (row.y + row.height < view.y || row.y > view.y + view.height) continue;
        /* a row half scrolled out must not react to the mouse outside the list */
        DrawLessonRow(row, i, &p->lessons[i], next, UiIsHover(view));
    }
    EndScissorMode();
}

/* ---------------------------------------------------------------------------
 *  Right panel: skills, progress, next lesson, robot
 * ------------------------------------------------------------------------- */
static void DrawSidePanel(Rectangle panel)
{
    const LearningPath *p = PathGet(selected);
    int next = PathNextLesson(selected);

    DrawRectangleRounded(panel, 0.05f, 12, COL_WHITE);
    float x = panel.x + 24, y = panel.y + 24;

    DrawText("WHAT YOU'LL LEARN", (int)x, (int)y, 10, COL_MUTED);
    y += 24;
    for (int i = 0; i < PATH_SKILLS; ++i) {
        DrawCircle((int)(x + 9), (int)(y + 7), 9, COL_BLUE_LIGHT);
        DrawTick(x + 9, y + 7, 4, COL_BLUE);
        DrawText(p->skills[i], (int)(x + 28), (int)(y + 1), 12, COL_INK);
        y += 28;
    }

    y += 12;
    DrawLine((int)x, (int)y, (int)(panel.x + panel.width - 24), (int)y, COL_LINE);
    y += 20;

    DrawText("YOUR PROGRESS", (int)x, (int)y, 10, COL_MUTED);
    y += 22;
    UiProgressRing((Vector2){ x + 36, y + 36 }, 36, 8, PathProgress(selected),
                   COL_BLUE, COL_WHITE);
    char line1[48], line2[48];
    snprintf(line1, sizeof line1, "%d of %d %s", PathCompleted(selected), p->lessonCount, p->unitWord);
    snprintf(line2, sizeof line2, "%d to go", p->lessonCount - PathCompleted(selected));
    DrawText(line1, (int)(x + 92), (int)(y + 22), 14, COL_NAVY);
    DrawText(line2, (int)(x + 92), (int)(y + 42), 10, COL_MUTED);
    y += 92;

    DrawLine((int)x, (int)y, (int)(panel.x + panel.width - 24), (int)y, COL_LINE);
    y += 20;

    DrawText(next < 0 ? "PATH COMPLETE" : (PathCompleted(selected) ? "UP NEXT" : "START HERE"),
             (int)x, (int)y, 10, COL_MUTED);
    y += 22;
    if (next >= 0) {
        const Lesson *l = &p->lessons[next];
        DrawText(l->title, (int)x, (int)y, 14, COL_NAVY);
        char sub[48];
        snprintf(sub, sizeof sub, "Lesson %d  -  %d min", next + 1, l->minutes);
        DrawText(sub, (int)x, (int)(y + 20), 10, COL_MUTED);
        y += 44;

        const char *label = PathCompleted(selected) ? "Continue learning  >" : "Start this path  >";
        if (UiButton((Rectangle){ x, y, panel.width - 48, 36 }, label,
                     COL_BLUE, COL_BLUE_DARK, COL_WHITE, 12))
            AppOpenLesson(selected, next);
        y += 52;
    } else {
        DrawText("Every lesson done. Nice work!", (int)x, (int)y, 12, COL_INK);
        y += 30;
    }

    /* the robot only when there is room for him */
    float robotY = panel.y + panel.height - 140;
    if (robotY > y + 10) {
        float rx = panel.x + (panel.width - 106) / 2;
        UiDrawRobot(rx, robotY);
    }
}

/* ---------------------------------------------------------------------------
 *  Public
 * ------------------------------------------------------------------------- */
void LearnPathDraw(void)
{
    float x = SIDEBAR_W + MARGIN_L;
    float right = (float)GetScreenWidth() - MARGIN_R;
    float width = right - x;
    float bottom = (float)GetScreenHeight() - 24;

    DrawHeader(x, right);
    DrawSelector(x, width);

    Rectangle side = { right - RIGHT_W, DETAIL_Y, RIGHT_W, bottom - DETAIL_Y };
    Rectangle detail = { x, DETAIL_Y, width - RIGHT_W - GAP, bottom - DETAIL_Y };
    DrawDetailPanel(detail);
    DrawSidePanel(side);
}
