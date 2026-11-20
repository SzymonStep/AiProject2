/* =============================================================================
 *  ui.c  --  Implementation of the widgets, toast and mascot.
 *
 *  Project : CodeQuest
 *  Author  : Szymon Stepniak
 *
 *  Notes on things not covered in class:
 *   - "Immediate mode" UI: a widget is a function that both draws itself and
 *     reports the user's interaction for this frame. There is no stored widget
 *     object, which is why every widget takes its Rectangle as an argument.
 * ========================================================================== */
#include "ui.h"

#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 *  Interaction helpers
 * ------------------------------------------------------------------------- */
bool UiIsHover(Rectangle r)
{
    return CheckCollisionPointRec(GetMousePosition(), r);
}

bool UiIsClicked(Rectangle r)
{
    return UiIsHover(r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* ---------------------------------------------------------------------------
 *  Widgets
 * ------------------------------------------------------------------------- */
bool UiButton(Rectangle r, const char *label, Color base, Color hover,
              Color text, int fontSize)
{
    bool over = UiIsHover(r);
    DrawRectangleRounded(r, 0.25f, 10, over ? hover : base);

    int textWidth = MeasureText(label, fontSize);
    DrawText(label, (int)(r.x + (r.width - textWidth) / 2),
             (int)(r.y + (r.height - fontSize) / 2 - 1), fontSize, text);

    return over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UiProgressBar(Rectangle r, float t, Color fill)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    DrawRectangleRounded(r, 0.5f, 8, COL_TRACK);
    if (t > 0.0f) {
        Rectangle done = r;
        done.width = r.width * t;
        DrawRectangleRounded(done, 0.5f, 8, fill);
    }
}

void UiProgressRing(Vector2 centre, float radius, float thickness, float t,
                    Color fill, Color hole)
{
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    DrawCircleV(centre, radius, (Color){ 233, 241, 248, 255 });
    if (t > 0.0f)
        DrawCircleSector(centre, radius, -90.0f, -90.0f + 360.0f * t, 64, fill);
    DrawCircleV(centre, radius - thickness, hole);

    char pct[8];
    int size = (int)(radius * 0.5f);
    snprintf(pct, sizeof pct, "%d%%", (int)(t * 100.0f + 0.5f));
    DrawText(pct, (int)(centre.x - MeasureText(pct, size) / 2.0f),
             (int)(centre.y - size / 2.0f), size, COL_NAVY);
}

void UiBadge(Rectangle r, const char *label, Color text, Color fill, int fontSize)
{
    DrawRectangleRounded(r, 0.5f, 8, fill);
    int w = MeasureText(label, fontSize);
    DrawText(label, (int)(r.x + (r.width - w) / 2),
             (int)(r.y + (r.height - fontSize) / 2), fontSize, text);
}

/* ---------------------------------------------------------------------------
 *  Text
 * ------------------------------------------------------------------------- */
float UiTextWrapped(const char *text, float x, float y, float width,
                    int fontSize, float lineGap, Color color)
{
    char line[256] = "";
    int  lineLen = 0;
    float lineH = fontSize + lineGap;
    float cy = y;
    const char *p = text;

    while (*p) {
        /* next word, up to the following space */
        const char *end = p;
        while (*end && *end != ' ') end++;
        int wordLen = (int)(end - p);

        char trial[256];
        snprintf(trial, sizeof trial, "%.*s%s%.*s",
                 lineLen, line, lineLen ? " " : "", wordLen, p);

        if (lineLen && MeasureText(trial, fontSize) > width) {
            DrawText(line, (int)x, (int)cy, fontSize, color);
            cy += lineH;
            snprintf(line, sizeof line, "%.*s", wordLen, p);
        } else {
            snprintf(line, sizeof line, "%s", trial);
        }
        lineLen = (int)strlen(line);

        p = (*end == ' ') ? end + 1 : end;
    }

    if (lineLen) {
        DrawText(line, (int)x, (int)cy, fontSize, color);
        cy += lineH;
    }
    return cy - y;
}

/* ---------------------------------------------------------------------------
 *  Scrolling
 * ------------------------------------------------------------------------- */
#define SCROLL_STEP 48.0f

void UiScrollArea(Rectangle view, float contentHeight, float *scroll)
{
    float maxScroll = contentHeight - view.height;
    if (maxScroll < 0.0f) maxScroll = 0.0f;

    if (UiIsHover(view)) *scroll -= GetMouseWheelMove() * SCROLL_STEP;
    if (*scroll < 0.0f)      *scroll = 0.0f;
    if (*scroll > maxScroll) *scroll = maxScroll;

    if (maxScroll <= 0.0f) return;

    /* thumb size is the visible share of the content */
    Rectangle track = { view.x + view.width - 5, view.y + 4, 4, view.height - 8 };
    float thumbH = track.height * (view.height / contentHeight);
    float thumbY = track.y + (track.height - thumbH) * (*scroll / maxScroll);
    DrawRectangleRounded(track, 1.0f, 4, COL_TRACK);
    DrawRectangleRounded((Rectangle){ track.x, thumbY, track.width, thumbH },
                         1.0f, 4, (Color){ 190, 210, 230, 255 });
}

void UiTick(float cx, float cy, float s, Color c)
{
    DrawLineEx((Vector2){ cx - s, cy }, (Vector2){ cx - s * 0.3f, cy + s * 0.7f }, 2.5f, c);
    DrawLineEx((Vector2){ cx - s * 0.3f, cy + s * 0.7f }, (Vector2){ cx + s, cy - s * 0.6f }, 2.5f, c);
}

void UiCross(float cx, float cy, float s, Color c)
{
    DrawLineEx((Vector2){ cx - s * 0.7f, cy - s * 0.7f }, (Vector2){ cx + s * 0.7f, cy + s * 0.7f }, 2.5f, c);
    DrawLineEx((Vector2){ cx + s * 0.7f, cy - s * 0.7f }, (Vector2){ cx - s * 0.7f, cy + s * 0.7f }, 2.5f, c);
}

/* ---------------------------------------------------------------------------
 *  Page layout
 * ------------------------------------------------------------------------- */
#define SIDEBAR_SPACE 248   /* matches SIDEBAR_W in sidebar.h */

Rectangle UiContentArea(void)
{
    float x = SIDEBAR_SPACE + 32;
    return (Rectangle){ x, 30, (float)GetScreenWidth() - 42 - x, (float)GetScreenHeight() - 54 };
}

void UiPageTitle(float x, const char *kicker, const char *title)
{
    DrawText(kicker, (int)x, 30, 10, COL_BLUE);
    DrawText(title, (int)x, 51, 28, COL_NAVY);
}

bool UiLink(float x, float y, const char *label, int fontSize)
{
    Rectangle r = { x, y, (float)MeasureText(label, fontSize), (float)fontSize + 4 };
    bool over = UiIsHover(r);
    DrawText(label, (int)x, (int)y, fontSize, over ? COL_BLUE_DARK : COL_BLUE);
    if (over) DrawLine((int)x, (int)(y + fontSize + 2), (int)(x + r.width), (int)(y + fontSize + 2), COL_BLUE);
    return over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UiCard(Rectangle r)
{
    DrawRectangleRounded(r, r.height > 300 ? 0.03f : 0.1f, 12, COL_WHITE);
}

float UiSectionTitle(float x, float y, const char *title, const char *sub)
{
    DrawText(title, (int)x, (int)y, 16, COL_NAVY);
    if (!sub) return 24;
    DrawText(sub, (int)x, (int)(y + 21), 10, COL_MUTED);
    return 40;
}

/* ---------------------------------------------------------------------------
 *  Toast
 * ------------------------------------------------------------------------- */
#define TOAST_CAP      160
#define TOAST_SECONDS  2.5f

static char  toastText[TOAST_CAP];
static float toastTimer = 0.0f;

void UiShowToast(const char *message)
{
    snprintf(toastText, sizeof toastText, "%s", message);
    toastTimer = TOAST_SECONDS;
}

void UiUpdateToast(float dt)
{
    if (toastTimer > 0.0f) toastTimer -= dt;
}

void UiDrawToast(void)
{
    if (toastTimer <= 0.0f) return;

    int width = MeasureText(toastText, 12) + 34;
    Rectangle box = {
        (float)GetScreenWidth() - width - 24,
        (float)GetScreenHeight() - 58,
        (float)width, 38
    };
    DrawRectangleRounded(box, 0.25f, 10, COL_NAVY);
    DrawText(toastText, GetScreenWidth() - width - 7,
             GetScreenHeight() - 45, 12, COL_WHITE);
}

/* ---------------------------------------------------------------------------
 *  Mascot
 * ------------------------------------------------------------------------- */
void UiDrawRobot(float x, float y)
{
    const Color body  = { 110, 180, 232, 255 };
    const Color limb  = {  98, 184, 233, 255 };
    const Color face  = { 245, 251, 255, 255 };
    const Color visor = { 217, 239, 255, 255 };

    /* shadow, antenna */
    DrawEllipse((int)(x + 53), (int)(y + 115), 62, 9, (Color){ 45, 115, 170, 28 });
    DrawCircle((int)(x + 53), (int)(y + 4), 5, COL_GOLD);
    DrawRectangleRounded((Rectangle){ x + 49, y + 6, 8, 17 }, 0.4f, 6, limb);

    /* head */
    DrawRectangleRounded((Rectangle){ x + 9,  y + 20, 88, 88 }, 0.28f, 12, body);
    DrawRectangleRounded((Rectangle){ x + 15, y + 26, 76, 76 }, 0.25f, 12, face);
    DrawRectangleRounded((Rectangle){ x + 25, y + 43, 56, 42 }, 0.25f, 10, visor);

    /* eyes and smile */
    DrawCircle((int)(x + 42), (int)(y + 59), 4, COL_BLUE);
    DrawCircle((int)(x + 64), (int)(y + 59), 4, COL_BLUE);
    DrawLineEx((Vector2){ x + 44, y + 74 }, (Vector2){ x + 52, y + 78 }, 2.5f, COL_BLUE);
    DrawLineEx((Vector2){ x + 52, y + 78 }, (Vector2){ x + 61, y + 74 }, 2.5f, COL_BLUE);

    /* ears */
    DrawRectangleRounded((Rectangle){ x + 3,  y + 51, 8, 22 }, 0.4f, 5, limb);
    DrawRectangleRounded((Rectangle){ x + 95, y + 51, 8, 22 }, 0.4f, 5, limb);
}
