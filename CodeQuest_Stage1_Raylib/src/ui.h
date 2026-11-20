/* =============================================================================
 *  ui.h  --  Theme palette, reusable widgets, toast messages and the mascot.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *  Module  : Advanced Programming - Coding Exam 1 & 2 revision tool
 *
 *  Everything in this header is pure presentation: colours, buttons, the
 *  toast pop-up and the robot mascot. Keeping it separate from the screens
 *  means a screen only has to say "draw a button here", never how.
 * ========================================================================== */
#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

/* ---------------------------------------------------------------------------
 *  Window size the layout was designed for.
 * ------------------------------------------------------------------------- */
#define WIN_W 1920
#define WIN_H 1080

/* ---------------------------------------------------------------------------
 *  Palette. One place to change the look of the whole app.
 *  Written as compound literals so they can be used directly as Color args.
 *  (Prefixed COL_ because raylib already defines WHITE, BLUE, GOLD, ...)
 * ------------------------------------------------------------------------- */
#define COL_PAGE       CLITERAL(Color){ 245, 249, 253, 255 }  /* window backdrop  */
#define COL_WHITE      CLITERAL(Color){ 255, 255, 255, 255 }  /* card surface     */
#define COL_NAVY       CLITERAL(Color){  25,  54,  80, 255 }  /* headings         */
#define COL_INK        CLITERAL(Color){  38,  61,  82, 255 }  /* body text        */
#define COL_MUTED      CLITERAL(Color){ 123, 141, 160, 255 }  /* secondary text   */
#define COL_BLUE       CLITERAL(Color){  40, 127, 209, 255 }  /* accent           */
#define COL_BLUE_DARK  CLITERAL(Color){  23, 105, 178, 255 }
#define COL_BLUE_LIGHT CLITERAL(Color){ 234, 244, 255, 255 }
#define COL_CYAN_LIGHT CLITERAL(Color){ 232, 248, 252, 255 }
#define COL_PURPLE     CLITERAL(Color){ 242, 237, 255, 255 }
#define COL_LINE       CLITERAL(Color){ 229, 237, 245, 255 }  /* borders, rules   */
#define COL_TRACK      CLITERAL(Color){ 237, 242, 247, 255 }  /* empty progress   */
#define COL_GREEN      CLITERAL(Color){  55, 184, 135, 255 }
#define COL_GOLD       CLITERAL(Color){ 245, 189,  72, 255 }

/* --- interaction helpers -------------------------------------------------- */
bool UiIsHover(Rectangle r);
bool UiIsClicked(Rectangle r);

/* --- widgets -------------------------------------------------------------- */
/* Immediate mode: draws itself and returns true on the frame it is clicked. */
bool UiButton(Rectangle r, const char *label, Color base, Color hover,
              Color text, int fontSize);

/* Rounded track with a filled portion, t in 0..1. */
void UiProgressBar(Rectangle r, float t, Color fill);

/* Ring that fills clockwise from 12 o'clock, t in 0..1, with "NN%" inside. */
void UiProgressRing(Vector2 centre, float radius, float thickness, float t,
                    Color fill, Color hole);

/* Rounded pill with centred text, used for status labels. */
void UiBadge(Rectangle r, const char *label, Color text, Color fill, int fontSize);

/* --- text ----------------------------------------------------------------- */
/* Draws `text` breaking on spaces so no line is wider than `width`.
 * Returns the height used, so callers can lay out what comes below. */
float UiTextWrapped(const char *text, float x, float y, float width,
                    int fontSize, float lineGap, Color color);

/* --- scrolling ------------------------------------------------------------ */
/* Updates *scroll from the mouse wheel while the cursor is inside `view`,
 * clamps it to the content, and draws a slim track on the right edge when
 * the content is taller than the view. Draw the content inside
 * BeginScissorMode(view) offset by -*scroll. */
void UiScrollArea(Rectangle view, float contentHeight, float *scroll);

/* Tick and cross marks centred on (cx, cy), `s` is half their size. */
void UiTick(float cx, float cy, float s, Color c);
void UiCross(float cx, float cy, float s, Color c);

/* Colours for right and wrong. */
#define COL_GOOD_BG   CLITERAL(Color){ 225, 245, 236, 255 }
#define COL_GOOD_FG   CLITERAL(Color){  30, 140, 100, 255 }
#define COL_BAD_BG    CLITERAL(Color){ 253, 232, 232, 255 }
#define COL_BAD_FG    CLITERAL(Color){ 200,  60,  70, 255 }

/* --- page layout ---------------------------------------------------------- */
/* The area right of the sidebar that screens draw into, from the current
 * window size. */
Rectangle UiContentArea(void);

/* Small blue kicker line above a large title, as on every screen. */
void UiPageTitle(float x, const char *kicker, const char *title);

/* A text link such as "< Back to C Fundamentals". Returns true when clicked. */
bool UiLink(float x, float y, const char *label, int fontSize);

/* White rounded card. */
void UiCard(Rectangle r);

/* Heading with a short muted line under it; returns the height used. */
float UiSectionTitle(float x, float y, const char *title, const char *sub);

/* --- toast ---------------------------------------------------------------- */
/* A short message in the bottom-right corner that fades out by itself. */
void UiShowToast(const char *message);
void UiUpdateToast(float dt);
void UiDrawToast(void);

/* --- mascot --------------------------------------------------------------- */
/* Friendly robot drawn entirely with raylib shapes, roughly 106 x 120 px. */
void UiDrawRobot(float x, float y);

#endif /* UI_H */
