#ifndef _TEXT_UI_H_INCLUDED
#define _TEXT_UI_H_INCLUDED

/* User Interface
 * The functions described here must not depend on the technology used in the UI.
 * (Except for static ones)
 */

#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "data_struct.h"

/**
 * \file text_ui.h
 * \brief Text User Interface definition
 *
 * The functions and data here use ncurses.h in order to display information in
 * the GNU/Linux terminal.
 */

///\brief Names for the color pairs
typedef enum {
	CP_NULL,
	CP_DEF,
	CP_WALL,
	CP_PLAYER,
	CP_END
} CP_name;

///\defgroup controls_text_ui_gp Key Bindings in the Text UI
///\{
static const int CTRLS_LEFT[]  = {'t', KEY_LEFT};
static const int CTRLS_DOWN[]  = {'s', KEY_DOWN};
static const int CTRLS_UP[]    = {'r', KEY_UP};
static const int CTRLS_RIGHT[] = {'n', KEY_RIGHT};
static const int CTRLS_QUIT[]  = {'q', 'q'};
///\}

///\defgroup general_text_ui_gp General Text UI functions and data
///\{

///\brief User Interface data structure
typedef struct{
	WINDOW *main_win; ///< \brief Pointer to main curses window.
	enum { CONTINUE, QUIT } signal; ///< \brief message to the outside.
} UI;
UI *ui_init();
void ui_terminate(UI *);
void ui_clear(UI *);
///\}

void msleep(float);

//Board-related
void print_board(UI *, Board *);

//Player-related
void erase_player(UI *, Player *);
void print_player(UI *, Player *);
void erase_fill(UI *, Yx, Direction);
Direction get_user_input(UI *);
void move_player(UI *, Board *, Player *, Direction);

#endif //_TEXT_UI_H_INCLUDED

