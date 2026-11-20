/* =============================================================================
 *  learnpath.h  --  The learning path screen: pick a path, see its lessons.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Layout
 *    top     a selector card for each of the four paths
 *    left    the selected path: description, stats, scrollable lesson list
 *    right   what you'll learn, progress ring, next lesson, the robot
 * ========================================================================== */
#ifndef LEARNPATH_H
#define LEARNPATH_H

#include "paths.h"

/* Selects which path is shown and scrolls its lesson list to the top. */
void LearnPathSelect(PathId path);

/* The path currently shown. */
PathId LearnPathSelected(void);

/* Draws the screen to the right of the sidebar and handles its clicks. */
void LearnPathDraw(void);

#endif /* LEARNPATH_H */
