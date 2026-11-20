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
 *  Themes
 * ------------------------------------------------------------------------- */
#define RGB(r, g, b) { r, g, b, 255 }

static const Theme kLight = {
    .page      = RGB(245, 249, 253), .surface   = RGB(255, 255, 255),
    .navy      = RGB( 25,  54,  80), .ink       = RGB( 38,  61,  82),
    .muted     = RGB(110, 128, 148), .blue      = RGB( 40, 127, 209),
    .blueDark  = RGB( 23, 105, 178), .blueLight = RGB(234, 244, 255),
    .cyanLight = RGB(232, 248, 252), .purple    = RGB(242, 237, 255),
    .line      = RGB(229, 237, 245), .track     = RGB(237, 242, 247),
    .green     = RGB( 55, 184, 135), .gold      = RGB(245, 189,  72),
    .goldLight = RGB(255, 245, 223), .hero      = RGB(230, 243, 255),
    .hoverLine = RGB(190, 220, 245),
    .goodBg    = RGB(225, 245, 236), .goodFg    = RGB( 30, 140, 100),
    .badBg     = RGB(253, 232, 232), .badFg     = RGB(200,  60,  70),
    .ringTrack = RGB(233, 241, 248), .scroll    = RGB(190, 210, 230),
    .helpBg    = RGB(242, 248, 254), .avatar    = RGB(223, 242, 255),
    .toastBg   = RGB( 25,  54,  80), .toastFg   = RGB(255, 255, 255),
    .overlay   = { 20, 40, 60, 110 },
};

/* Dark mode: deep navy surfaces, soft blue accents, never pure black or
 * pure white so long sessions stay easy on the eyes. */
static const Theme kDark = {
    .page      = RGB( 14,  21,  32), .surface   = RGB( 23,  33,  48),
    .navy      = RGB(233, 240, 250), .ink       = RGB(203, 214, 228),
    .muted     = RGB(140, 157, 178), .blue      = RGB( 62, 146, 232),
    .blueDark  = RGB(132, 192, 255), .blueLight = RGB( 31,  50,  77),
    .cyanLight = RGB( 24,  56,  68), .purple    = RGB( 46,  40,  76),
    .line      = RGB( 41,  56,  78), .track     = RGB( 38,  51,  70),
    .green     = RGB( 64, 200, 146), .gold      = RGB(245, 189,  72),
    .goldLight = RGB( 66,  53,  28), .hero      = RGB( 27,  45,  72),
    .hoverLine = RGB( 72, 112, 162),
    .goodBg    = RGB( 22,  60,  46), .goodFg    = RGB( 96, 218, 162),
    .badBg     = RGB( 72,  30,  38), .badFg     = RGB(246, 116, 126),
    .ringTrack = RGB( 38,  51,  70), .scroll    = RGB( 74,  94, 120),
    .helpBg    = RGB( 29,  42,  60), .avatar    = RGB( 36,  60,  92),
    .toastBg   = RGB(233, 240, 250), .toastFg   = RGB( 20,  30,  45),
    .overlay   = { 0, 0, 0, 150 },
};

Theme gTheme = {
    .page      = RGB(245, 249, 253), .surface   = RGB(255, 255, 255),
};
static bool darkMode = false;

void UiSetDarkMode(bool dark)
{
    darkMode = dark;
    gTheme = dark ? kDark : kLight;
}

bool UiIsDarkMode(void) { return darkMode; }

/* ---------------------------------------------------------------------------
 *  Fonts
 *
 *  Each weight is rasterised twice: a small atlas for body text and a large
 *  one for headings, so neither is scaled far from the size it was made at
 *  (which is what makes small text blurry).
 * ------------------------------------------------------------------------- */
static Font fRegSmall, fRegLarge, fBoldSmall, fBoldLarge;
static bool fontsLoaded = false;

static Font LoadFirst(const char *const *paths, int count, int size, bool *ok)
{
    for (int i = 0; i < count; i++) {
        if (!FileExists(paths[i])) continue;
        Font f = LoadFontEx(paths[i], size, NULL, 0);
        if (f.texture.id != 0 && f.glyphCount > 0) {
            SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
            *ok = true;
            return f;
        }
    }
    *ok = false;
    return GetFontDefault();
}

void UiLoadFonts(void)
{
    static const char *reg[]  = { "C:/Windows/Fonts/segoeui.ttf",  "assets/ui.ttf" };
    static const char *bold[] = { "C:/Windows/Fonts/seguisb.ttf",  "C:/Windows/Fonts/segoeuib.ttf",
                                  "C:/Windows/Fonts/segoeui.ttf",  "assets/ui-bold.ttf" };
    bool a, b, c, d;
    fRegSmall  = LoadFirst(reg, 2, 22, &a);
    fRegLarge  = LoadFirst(reg, 2, 44, &b);
    fBoldSmall = LoadFirst(bold, 4, 22, &c);
    fBoldLarge = LoadFirst(bold, 4, 44, &d);
    fontsLoaded = a && b && c && d;
    if (!fontsLoaded) UiUnloadFonts();   /* all or nothing, keeps metrics consistent */
}

void UiUnloadFonts(void)
{
    Font *all[] = { &fRegSmall, &fRegLarge, &fBoldSmall, &fBoldLarge };
    for (int i = 0; i < 4; i++) {
        if (all[i]->texture.id != 0 && all[i]->texture.id != GetFontDefault().texture.id)
            UnloadFont(*all[i]);
        all[i]->texture.id = 0;
    }
    fontsLoaded = false;
}

/* Layout sizes were chosen for raylib's chunky bitmap font; a real font
 * looks smaller at the same pixel size, so small sizes are nudged up. */
static float RealSize(int size)
{
    if (size <= 10) return size + 4.0f;
    if (size <= 14) return size + 3.0f;
    return size * 1.12f;
}

static Font PickFont(float px, bool bold)
{
    if (bold) return px > 26 ? fBoldLarge : fBoldSmall;
    return px > 26 ? fRegLarge : fRegSmall;
}

void UiText(const char *text, int x, int y, int size, Color color)
{
    if (!fontsLoaded) { DrawText(text, x, y, size, color); return; }
    float px = RealSize(size);
    /* centre the taller real-font line on the box the layout reserved */
    float dy = (size - px) * 0.55f;
    DrawTextEx(PickFont(px, size >= 15), text, (Vector2){ (float)x, y + dy }, px, 0.0f, color);
}

int UiMeasure(const char *text, int size)
{
    if (!fontsLoaded) return MeasureText(text, size);
    float px = RealSize(size);
    return (int)(MeasureTextEx(PickFont(px, size >= 15), text, px, 0.0f).x + 0.5f);
}

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

    int textWidth = UiMeasure(label, fontSize);
    UiText(label, (int)(r.x + (r.width - textWidth) / 2),
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

    DrawCircleV(centre, radius, COL_RING_TRACK);
    if (t > 0.0f)
        DrawCircleSector(centre, radius, -90.0f, -90.0f + 360.0f * t, 64, fill);
    DrawCircleV(centre, radius - thickness, hole);

    char pct[8];
    int size = (int)(radius * 0.5f);
    snprintf(pct, sizeof pct, "%d%%", (int)(t * 100.0f + 0.5f));
    UiText(pct, (int)(centre.x - UiMeasure(pct, size) / 2.0f),
             (int)(centre.y - size / 2.0f), size, COL_NAVY);
}

void UiBadge(Rectangle r, const char *label, Color text, Color fill, int fontSize)
{
    DrawRectangleRounded(r, 0.5f, 8, fill);
    int w = UiMeasure(label, fontSize);
    UiText(label, (int)(r.x + (r.width - w) / 2),
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

        if (lineLen && UiMeasure(trial, fontSize) > width) {
            UiText(line, (int)x, (int)cy, fontSize, color);
            cy += lineH;
            snprintf(line, sizeof line, "%.*s", wordLen, p);
        } else {
            snprintf(line, sizeof line, "%s", trial);
        }
        lineLen = (int)strlen(line);

        p = (*end == ' ') ? end + 1 : end;
    }

    if (lineLen) {
        UiText(line, (int)x, (int)cy, fontSize, color);
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
                         1.0f, 4, COL_SCROLL);
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

bool UiSwitch(Rectangle r, bool on)
{
    bool over = UiIsHover(r);
    DrawRectangleRounded(r, 1.0f, 12, on ? COL_BLUE : (over ? COL_HOVER_LINE : COL_TRACK));
    float knob = r.height - 6;
    float kx = on ? r.x + r.width - knob - 3 : r.x + 3;
    DrawCircle((int)(kx + knob / 2), (int)(r.y + r.height / 2), knob / 2, COL_ON_ACCENT);
    return over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
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
    UiText(kicker, (int)x, 30, 10, COL_BLUE);
    UiText(title, (int)x, 51, 28, COL_NAVY);
}

bool UiLink(float x, float y, const char *label, int fontSize)
{
    Rectangle r = { x, y, (float)UiMeasure(label, fontSize), (float)fontSize + 4 };
    bool over = UiIsHover(r);
    UiText(label, (int)x, (int)y, fontSize, over ? COL_BLUE_DARK : COL_BLUE);
    if (over) DrawLine((int)x, (int)(y + fontSize + 2), (int)(x + r.width), (int)(y + fontSize + 2), COL_BLUE);
    return over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UiCard(Rectangle r)
{
    DrawRectangleRounded(r, r.height > 300 ? 0.03f : 0.1f, 12, COL_SURFACE);
}

float UiSectionTitle(float x, float y, const char *title, const char *sub)
{
    UiText(title, (int)x, (int)y, 16, COL_NAVY);
    if (!sub) return 24;
    UiText(sub, (int)x, (int)(y + 21), 10, COL_MUTED);
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

    int width = UiMeasure(toastText, 12) + 34;
    Rectangle box = {
        (float)GetScreenWidth() - width - 24,
        (float)GetScreenHeight() - 58,
        (float)width, 38
    };
    DrawRectangleRounded(box, 0.25f, 10, COL_TOAST_BG);
    UiText(toastText, GetScreenWidth() - width - 7,
             GetScreenHeight() - 45, 12, COL_TOAST_FG);
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
