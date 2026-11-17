/* =============================================================================
 *  sidebar.h  --  Left-hand navigation: logo, menu, help card, profile.
 *
 *  Project : CodeQuest - learn, practise and master C / C++ (raylib)
 *  Author  : Szymon Stepniak
 * ========================================================================== */
#ifndef SIDEBAR_H
#define SIDEBAR_H

#define SIDEBAR_W 248

/* One entry per menu item, in the order they appear. */
typedef enum NavItem {
    NAV_DASHBOARD = 0,
    NAV_LEARNING_PATH,
    NAV_LEARN_C,
    NAV_LEARN_CPP,
    NAV_CHALLENGES,
    NAV_EXAM_ARENA,
    NAV_REVISION,
    NAV_ACHIEVEMENTS,
    NAV_COUNT
} NavItem;

/* Draws the sidebar and handles clicks. Writes the newly chosen item to
 * *active when the user clicks one. */
void SidebarDraw(NavItem *active);

#endif /* SIDEBAR_H */
