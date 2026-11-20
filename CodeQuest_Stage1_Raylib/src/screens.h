/* =============================================================================
 *  screens.h  --  The smaller sidebar screens and the robot helper.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 *
 *  Each lives in its own .c file; they share one header because each only
 *  exposes a draw function.
 *
 *    arena.c         Exam Arena: timed mock papers for Coding Exam 1 and 2
 *    challenges.c    Challenges: quick drills by section and lab
 *    revision.c      Revision Centre: mastery, weakest questions, notes
 *    achievements.c  Achievements: badges, stats and resetting progress
 *    robot.c         "Ask the robot": a pop-up of study tips
 * ========================================================================== */
#ifndef SCREENS_H
#define SCREENS_H

#include <stdbool.h>

void ArenaDraw(void);
void ChallengesDraw(void);
void RevisionDraw(void);
void AchievementsDraw(void);

bool RobotIsOpen(void);
void RobotToggle(void);
void RobotDraw(void);

#endif /* SCREENS_H */
