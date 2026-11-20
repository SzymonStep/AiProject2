/* =============================================================================
 *  codeview.c  --  Syntax-highlighted code blocks and the text input box.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 *
 *  Code is drawn in a monospaced TTF (Consolas, falling back to Cascadia
 *  Code, then raylib's built-in font) so columns line up the way they do in
 *  an editor. The rest of the interface keeps raylib's default font.
 *
 *  Code blocks and answer boxes use a dark "editor" palette on purpose: it
 *  makes code stand out from the light lesson pages around it.
 * ========================================================================== */
#include "codeview.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* --- editor palette -------------------------------------------------------- */
#define COL_CODE_BG   CLITERAL(Color){  22,  34,  48, 255 }
#define COL_BORDER    CLITERAL(Color){  48,  66,  88, 255 }
#define COL_TEXT      CLITERAL(Color){ 228, 234, 244, 255 }
#define COL_CMUTED    CLITERAL(Color){ 150, 165, 188, 255 }
#define COL_ACCENT    CLITERAL(Color){  94, 162, 255, 255 }
#define COL_KEYWORD   CLITERAL(Color){ 199, 146, 234, 255 }
#define COL_TYPE      CLITERAL(Color){  86, 203, 220, 255 }
#define COL_NUMBER    CLITERAL(Color){ 246, 172, 110, 255 }
#define COL_STRING    CLITERAL(Color){ 152, 210, 132, 255 }
#define COL_COMMENT   CLITERAL(Color){ 110, 126, 150, 255 }
#define COL_BLANK     CLITERAL(Color){ 255, 212,  96, 255 }

#define FONT_BASE 40          /* atlas size the TTF is rasterised at */

static Font gMono;
static bool gMonoLoaded = false;

/* ---------------------------------------------------------------------------
 *  Font
 * ------------------------------------------------------------------------- */
void CodeFontLoad(void)
{
    static const char *paths[] = { "C:/Windows/Fonts/consola.ttf",
                                   "C:/Windows/Fonts/CascadiaCode.ttf",
                                   "assets/mono.ttf" };
    gMono = GetFontDefault();
    gMonoLoaded = false;
    for (int i = 0; i < (int)(sizeof paths / sizeof *paths); i++) {
        if (!FileExists(paths[i])) continue;
        Font f = LoadFontEx(paths[i], FONT_BASE, NULL, 0);
        if (f.texture.id != 0 && f.glyphCount > 0) {
            SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
            gMono = f;
            gMonoLoaded = true;
            return;
        }
    }
}

void CodeFontUnload(void)
{
    if (gMonoLoaded) UnloadFont(gMono);
    gMonoLoaded = false;
}

/* raylib's built-in font needs extra spacing to stay readable; a TTF does not. */
static float Spacing(float size) { return gMonoLoaded ? 0.0f : size / 10.0f; }

void MonoText(const char *s, float x, float y, float size, Color c)
{
    DrawTextEx(gMonoLoaded ? gMono : GetFontDefault(), s, (Vector2){ x, y }, size, Spacing(size), c);
}

float MonoWidth(const char *s, float size)
{
    return MeasureTextEx(gMonoLoaded ? gMono : GetFontDefault(), s, size, Spacing(size)).x;
}

/* ---------------------------------------------------------------------------
 *  Code renderer with syntax highlighting
 *
 *  The source is walked one line at a time. Within a line we classify runs of
 *  characters into tokens and pick a colour per token. Blanks written in the
 *  content as ___answer___ are drawn as a highlighted slot so exam-style
 *  "fill in the blank" questions render without any extra markup language.
 * ------------------------------------------------------------------------- */

static const char *kKeywords[] = {
    "if", "else", "for", "while", "do", "return", "break", "continue",
    "switch", "case", "default", "struct", "typedef", "sizeof", "const",
    "static", "new", "delete", "class", "public", "private", "using",
    "namespace", "goto", "enum", "union", "true", "false", "NULL", "nullptr"
};
static const char *kTypes[] = {
    "int", "char", "float", "double", "void", "bool", "long", "short",
    "unsigned", "signed", "string", "size_t", "FILE"
};

static bool InWordList(const char *s, int len, const char *const *list, int n)
{
    for (int i = 0; i < n; i++) {
        if ((int)strlen(list[i]) == len && strncmp(s, list[i], (size_t)len) == 0)
            return true;
    }
    return false;
}

static bool IsIdentChar(char c) { return isalnum((unsigned char)c) || c == '_'; }

/* Counts the lines a code string will occupy. */
static int CountLines(const char *code)
{
    int n = 1;
    for (const char *p = code; *p; p++) if (*p == '\n') n++;
    return n;
}

float CodeViewHeight(const char *code, float size)
{
    return CountLines(code) * (size * 1.5f) + 20.0f;
}

/* Draws one already-isolated line. `y` is the top of the line. */
static void DrawCodeLine(const char *line, int len, float x, float y,
                         float size, bool hideBlankText, float clipRight)
{
    float cx = x;
    int   i  = 0;
    float spaceW = MonoWidth(" ", size);

    while (i < len) {
        char c = line[i];

        /* --- a blank slot: ___text___ ------------------------------------ */
        if (c == '_' && i + 2 < len && line[i+1] == '_' && line[i+2] == '_') {
            int bodyStart = i + 3;
            int j = bodyStart;
            while (j + 2 < len &&
                   !(line[j] == '_' && line[j+1] == '_' && line[j+2] == '_'))
                j++;
            int bodyLen = j - bodyStart;
            if (bodyLen < 0) bodyLen = 0;

            char body[128];
            int  copy = bodyLen < 127 ? bodyLen : 127;
            memcpy(body, line + bodyStart, (size_t)copy);
            body[copy] = '\0';

            const char *shown = hideBlankText ? "______" : body;
            float w  = MonoWidth(shown, size) + 10.0f;
            Rectangle slot = { cx, y - 2.0f, w, size + 5.0f };
            DrawRectangleRounded(slot, 0.25f, 6,
                                 ColorAlpha(COL_BLANK, hideBlankText ? 0.14f : 0.20f));
            DrawRectangleRoundedLines(slot, 0.25f, 6, ColorAlpha(COL_BLANK, 0.75f));
            MonoText(shown, cx + 5.0f, y, size, COL_BLANK);
            cx += w + 1.0f;
            i = (j + 2 < len) ? j + 3 : len;
            continue;
        }

        /* --- line comment ------------------------------------------------ */
        if (c == '/' && i + 1 < len && line[i+1] == '/') {
            char buf[512];
            int  n = len - i;
            if (n > 511) n = 511;
            memcpy(buf, line + i, (size_t)n); buf[n] = '\0';
            MonoText(buf, cx, y, size, COL_COMMENT);
            return;                              /* rest of the line is comment */
        }

        /* --- string / char literal --------------------------------------- */
        if (c == 34 || c == 39) {                /* 34 = double, 39 = single */
            char quote = c;
            int  j = i + 1;
            while (j < len) {
                if (line[j] == 92 && j + 1 < len) { j += 2; continue; }  /* 92 = backslash */
                if (line[j] == quote) { j++; break; }
                j++;
            }
            char buf[256];
            int  n = j - i;
            if (n > 255) n = 255;
            memcpy(buf, line + i, (size_t)n); buf[n] = '\0';
            MonoText(buf, cx, y, size, COL_STRING);
            cx += MonoWidth(buf, size);
            i = j;
            continue;
        }

        /* --- number ------------------------------------------------------ */
        if (isdigit((unsigned char)c)) {
            int j = i;
            while (j < len && (isalnum((unsigned char)line[j]) || line[j] == '.')) j++;
            char buf[64];
            int  n = j - i;
            if (n > 63) n = 63;
            memcpy(buf, line + i, (size_t)n); buf[n] = '\0';
            MonoText(buf, cx, y, size, COL_NUMBER);
            cx += MonoWidth(buf, size);
            i = j;
            continue;
        }

        /* --- identifier / keyword / type --------------------------------- */
        if (isalpha((unsigned char)c) || c == '_') {
            int j = i;
            while (j < len && IsIdentChar(line[j])) j++;
            int  n = j - i;
            char buf[128];
            int  cn = n < 127 ? n : 127;
            memcpy(buf, line + i, (size_t)cn); buf[cn] = '\0';

            Color col = COL_TEXT;
            if (InWordList(line + i, n, kKeywords,
                           (int)(sizeof kKeywords / sizeof *kKeywords)))
                col = COL_KEYWORD;
            else if (InWordList(line + i, n, kTypes,
                                (int)(sizeof kTypes / sizeof *kTypes)))
                col = COL_TYPE;
            else if (j < len && line[j] == '(')
                col = COL_ACCENT;               /* looks like a function call */

            MonoText(buf, cx, y, size, col);
            cx += MonoWidth(buf, size);
            i = j;
            continue;
        }

        /* --- whitespace -------------------------------------------------- */
        if (c == ' ' || c == '\t') {
            cx += (c == '\t') ? spaceW * 4.0f : spaceW;
            i++;
            continue;
        }

        /* --- single punctuation / operator ------------------------------- */
        {
            char buf[2] = { c, '\0' };
            MonoText(buf, cx, y, size, COL_CMUTED);
            cx += MonoWidth(buf, size);
            i++;
        }

        if (cx > clipRight) return;              /* nothing more is visible */
    }
}

float CodeView(const char *code, Rectangle r, float size, float scroll,
             bool hideBlankText)
{
    DrawRectangleRounded(r, 0.04f, 8, COL_CODE_BG);
    DrawRectangleRoundedLines(r, 0.04f, 8, COL_BORDER);

    BeginScissorMode((int)r.x, (int)r.y, (int)r.width, (int)r.height);

    float lineH  = size * 1.5f;
    float y      = r.y + 10.0f - scroll;
    float gutter = MonoWidth("88 ", size) + 8.0f;

    const char *p = code;
    int lineNo = 1;
    while (*p) {
        const char *eol = strchr(p, '\n');
        int len = eol ? (int)(eol - p) : (int)strlen(p);

        /* Only pay for lines that are actually on screen. */
        if (y + lineH > r.y && y < r.y + r.height) {
            char num[8];
            snprintf(num, sizeof num, "%2d", lineNo);
            MonoText(num, r.x + 10.0f, y, size, ColorAlpha(COL_COMMENT, 0.7f));
            DrawCodeLine(p, len, r.x + 10.0f + gutter, y, size,
                         hideBlankText, r.x + r.width);
        }

        y += lineH;
        lineNo++;
        if (!eol) break;
        p = eol + 1;
    }

    EndScissorMode();
    return CodeViewHeight(code, size);
}

/* ---------------------------------------------------------------------------
 *  Text input
 * ------------------------------------------------------------------------- */
void TextBoxInit(TextBox *tb, bool multiline)
{
    memset(tb, 0, sizeof *tb);
    tb->multiline = multiline;
}

void TextBoxClear(TextBox *tb)
{
    tb->text[0] = '\0';
    tb->len     = 0;
    tb->caret   = 0;
}

void TextBoxSet(TextBox *tb, const char *s)
{
    bool ml = tb->multiline;
    TextBoxInit(tb, ml);
    if (!s) return;
    int n = (int)strlen(s);
    if (n > TEXTBOX_CAP - 1) n = TEXTBOX_CAP - 1;
    memcpy(tb->text, s, (size_t)n);
    tb->text[n] = '\0';
    tb->len     = n;
    tb->caret   = n;
}

/* Inserts one character at the caret, shifting the tail right. */
static void Insert(TextBox *tb, char c)
{
    if (tb->len >= TEXTBOX_CAP - 1) return;
    for (int i = tb->len; i > tb->caret; i--) tb->text[i] = tb->text[i-1];
    tb->text[tb->caret] = c;
    tb->len++;
    tb->caret++;
    tb->text[tb->len] = '\0';
}

/* Deletes the character before the caret, shifting the tail left. */
static void Backspace(TextBox *tb)
{
    if (tb->caret <= 0) return;
    for (int i = tb->caret - 1; i < tb->len - 1; i++) tb->text[i] = tb->text[i+1];
    tb->len--;
    tb->caret--;
    tb->text[tb->len] = '\0';
}

static void DeleteFwd(TextBox *tb)
{
    if (tb->caret >= tb->len) return;
    for (int i = tb->caret; i < tb->len - 1; i++) tb->text[i] = tb->text[i+1];
    tb->len--;
    tb->text[tb->len] = '\0';
}

/* Holding a key should repeat it. raylib only reports the initial press for
 * most keys, so we run our own delay-then-rate timer. */
static bool KeyRepeat(TextBox *tb, int key)
{
    if (IsKeyPressed(key)) { tb->lastKey = key; tb->keyRepeat = 0.0f; return true; }
    if (IsKeyDown(key) && tb->lastKey == key) {
        tb->keyRepeat += GetFrameTime();
        if (tb->keyRepeat > 0.42f) {            /* initial delay passed */
            tb->keyRepeat -= 0.035f;            /* then ~28 chars/second */
            return true;
        }
    }
    return false;
}

bool TextBoxUpdateDraw(TextBox *tb, Rectangle r, const char *placeholder)
{
    bool over = CheckCollisionPointRec(GetMousePosition(), r);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) tb->focused = over;
    if (over) SetMouseCursor(MOUSE_CURSOR_IBEAM);

    bool  submitted = false;
    float size  = 18.0f;
    float lineH = size * 1.45f;

    if (tb->focused) {
        /* Printable characters arrive as a queue of codepoints. */
        int ch;
        while ((ch = GetCharPressed()) != 0)
            if (ch >= 32 && ch < 127) Insert(tb, (char)ch);

        if (KeyRepeat(tb, KEY_BACKSPACE)) Backspace(tb);
        if (KeyRepeat(tb, KEY_DELETE))    DeleteFwd(tb);
        if (KeyRepeat(tb, KEY_LEFT)  && tb->caret > 0)       tb->caret--;
        if (KeyRepeat(tb, KEY_RIGHT) && tb->caret < tb->len) tb->caret++;
        if (IsKeyPressed(KEY_HOME)) tb->caret = 0;
        if (IsKeyPressed(KEY_END))  tb->caret = tb->len;

        /* Ctrl+V paste - handy for the longer Section A answers. */
        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) &&
            IsKeyPressed(KEY_V)) {
            const char *clip = GetClipboardText();
            if (clip) {
                for (const char *q = clip; *q; q++) {
                    if (*q == '\n') { if (tb->multiline) Insert(tb, '\n'); }
                    else if (*q >= 32 && *q < 127) Insert(tb, *q);
                }
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (tb->multiline && !IsKeyDown(KEY_LEFT_CONTROL)) Insert(tb, '\n');
            else submitted = true;
        }

        tb->blink += GetFrameTime();
    } else {
        tb->blink = 0.0f;
    }

    /* --- draw ------------------------------------------------------------ */
    DrawRectangleRounded(r, 0.1f, 8, COL_CODE_BG);
    DrawRectangleRoundedLines(r, 0.1f, 8, tb->focused ? COL_ACCENT : COL_BORDER);

    BeginScissorMode((int)r.x + 2, (int)r.y + 2,
                     (int)r.width - 4, (int)r.height - 4);

    if (tb->len == 0 && !tb->focused && placeholder) {
        DrawText(placeholder, (int)(r.x + 12), (int)(r.y + 12), 14, COL_CMUTED);
    } else {
        /* Walk the buffer line by line so newlines render as real breaks and
         * the caret can be placed on the correct line. */
        float y = r.y + 9.0f;
        int   lineStart = 0;
        float caretX = r.x + 12.0f, caretY = y;
        bool  caretPlaced = false;

        for (int i = 0; i <= tb->len; i++) {
            if (i == tb->len || tb->text[i] == '\n') {
                char buf[TEXTBOX_CAP];
                int  n = i - lineStart;
                if (n > TEXTBOX_CAP - 1) n = TEXTBOX_CAP - 1;
                memcpy(buf, tb->text + lineStart, (size_t)n);
                buf[n] = '\0';
                MonoText(buf, r.x + 12.0f, y, size, COL_TEXT);

                if (!caretPlaced && tb->caret >= lineStart && tb->caret <= i) {
                    char upto[TEXTBOX_CAP];
                    int  m = tb->caret - lineStart;
                    if (m < 0) m = 0;
                    memcpy(upto, tb->text + lineStart, (size_t)m);
                    upto[m] = '\0';
                    caretX = r.x + 12.0f + MonoWidth(upto, size);
                    caretY = y;
                    caretPlaced = true;
                }

                y += lineH;
                lineStart = i + 1;
            }
        }

        if (tb->focused && fmodf(tb->blink, 1.0f) < 0.55f)
            DrawRectangle((int)caretX, (int)caretY, 2, (int)size, COL_ACCENT);
    }

    EndScissorMode();
    return submitted;
}
