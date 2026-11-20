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
 *  Theme. Every colour the app uses comes from the current theme, so light
 *  and dark mode are just two filled-in copies of this struct. The COL_
 *  names read the active theme, which is why they work anywhere a Color is
 *  expected. (Prefixed COL_ because raylib already defines WHITE, BLUE ...)
 * ------------------------------------------------------------------------- */
typedef struct Theme {
    Color page;        /* window backdrop                         */
    Color surface;     /* cards, sidebar                          */
    Color navy;        /* headings                                */
    Color ink;         /* body text                               */
    Color muted;       /* secondary text                          */
    Color blue;        /* accent: buttons, bars, selection        */
    Color blueDark;    /* accent text on light-blue backgrounds   */
    Color blueLight;   /* soft accent background                  */
    Color cyanLight;
    Color purple;
    Color line;        /* borders, rules                          */
    Color track;       /* empty part of a progress bar            */
    Color green;
    Color gold;
    Color goldLight;   /* soft gold background                    */
    Color hero;        /* dashboard welcome panel                 */
    Color hoverLine;   /* card border on hover                    */
    Color goodBg, goodFg, badBg, badFg;
    Color ringTrack;   /* empty part of a progress ring           */
    Color scroll;      /* scrollbar thumb                         */
    Color helpBg;      /* sidebar help card                       */
    Color avatar;      /* profile circle                          */
    Color toastBg, toastFg;
    Color overlay;     /* dimming behind pop-ups                  */
} Theme;

extern Theme gTheme;

void UiSetDarkMode(bool dark);
bool UiIsDarkMode(void);

#define COL_PAGE       (gTheme.page)
#define COL_SURFACE    (gTheme.surface)
#define COL_NAVY       (gTheme.navy)
#define COL_INK        (gTheme.ink)
#define COL_MUTED      (gTheme.muted)
#define COL_BLUE       (gTheme.blue)
#define COL_BLUE_DARK  (gTheme.blueDark)
#define COL_BLUE_LIGHT (gTheme.blueLight)
#define COL_CYAN_LIGHT (gTheme.cyanLight)
#define COL_PURPLE     (gTheme.purple)
#define COL_LINE       (gTheme.line)
#define COL_TRACK      (gTheme.track)
#define COL_GREEN      (gTheme.green)
#define COL_GOLD       (gTheme.gold)
#define COL_GOLD_LIGHT (gTheme.goldLight)
#define COL_HERO       (gTheme.hero)
#define COL_HOVER_LINE (gTheme.hoverLine)
#define COL_RING_TRACK (gTheme.ringTrack)
#define COL_SCROLL     (gTheme.scroll)
#define COL_HELP_BG    (gTheme.helpBg)
#define COL_AVATAR     (gTheme.avatar)
#define COL_TOAST_BG   (gTheme.toastBg)
#define COL_TOAST_FG   (gTheme.toastFg)
#define COL_OVERLAY    (gTheme.overlay)
#define COL_GOOD_BG    (gTheme.goodBg)
#define COL_GOOD_FG    (gTheme.goodFg)
#define COL_BAD_BG     (gTheme.badBg)
#define COL_BAD_FG     (gTheme.badFg)

/* Text on a solid accent (blue, gold, red) background: white in both themes. */
#define COL_ON_ACCENT  CLITERAL(Color){ 255, 255, 255, 255 }

/* --- fonts ---------------------------------------------------------------- */
/* Loads Segoe UI for the interface (falls back to raylib's built-in font).
 * Call after InitWindow. */
void UiLoadFonts(void);
void UiUnloadFonts(void);

/* Interface text. `size` is the design size used throughout the layout; it
 * is scaled up slightly so small labels stay readable in a real font. Sizes
 * of 15 and above use the semibold weight for headings. */
void UiText(const char *text, int x, int y, int size, Color color);
int  UiMeasure(const char *text, int size);

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

/* A small on/off switch. Returns true on the frame it is clicked. */
bool UiSwitch(Rectangle r, bool on);

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
