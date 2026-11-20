/* =============================================================================
 *  codeview.h  --  Syntax-highlighted code blocks and the text input box.
 *
 *  Project : CodeQuest (ported from CExam Trainer)
 *  Author  : Szymon Stepniak
 * ========================================================================== */
#ifndef CODEVIEW_H
#define CODEVIEW_H

#include "raylib.h"
#include <stdbool.h>

/* Loads Consolas (or a fallback) for code. Call after InitWindow. */
void CodeFontLoad(void);
void CodeFontUnload(void);

/* Text in the code font, for answers and inline code. */
void  MonoText(const char *s, float x, float y, float size, Color c);
float MonoWidth(const char *s, float size);

/* Draws C/C++ source with keyword/number/string/comment colouring and line
 * numbers. Blanks written as ___text___ are drawn as highlighted slots;
 * `hideBlankText` shows them empty. Returns the block's full height. */
float CodeView(const char *code, Rectangle r, float size, float scroll, bool hideBlankText);

/* Height the same code would occupy, for layout and scroll maths. */
float CodeViewHeight(const char *code, float size);

/* --- text input ----------------------------------------------------------- */
#define TEXTBOX_CAP 1024

typedef struct TextBox {
    char  text[TEXTBOX_CAP];
    int   len;
    int   caret;          /* caret index into text[]                  */
    bool  focused;
    bool  multiline;      /* Enter inserts '\n' instead of submitting */
    float blink;          /* caret blink accumulator                  */
    float keyRepeat;      /* backspace / arrow auto-repeat timer      */
    int   lastKey;
} TextBox;

void TextBoxInit(TextBox *tb, bool multiline);
void TextBoxClear(TextBox *tb);
void TextBoxSet(TextBox *tb, const char *s);

/* Handles typing while focused and draws the box. Returns true when Enter is
 * pressed in a single-line box, or Ctrl+Enter in a multi-line one. */
bool TextBoxUpdateDraw(TextBox *tb, Rectangle r, const char *placeholder);

#endif /* CODEVIEW_H */
