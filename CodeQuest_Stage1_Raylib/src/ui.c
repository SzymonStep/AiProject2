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
