/* =============================================================================
 *  lesson.h  --  One lesson: notes, example code and the way into its quiz.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 * ========================================================================== */
#ifndef LESSON_H
#define LESSON_H

#include "paths.h"
#include "raylib.h"
#include <stdbool.h>

void LessonScreenOpen(PathId path, int lesson);
void LessonScreenDraw(void);

/* Draws `code` inside `r` but only the part visible in `clip`, so a code
 * block can sit inside a scrolling page. Re-enables `clip` afterwards. */
void DrawCodeClipped(const char *code, Rectangle r, float size, Rectangle clip, bool hideBlanks);

/* Draws text split into paragraphs on blank lines; returns the height used. */
float DrawParagraphs(const char *text, float x, float y, float width, int fontSize, Color c);

#endif /* LESSON_H */
