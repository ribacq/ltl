#ifndef _DATA_STRUCT_H_INCLUDED
#define _DATA_STRUCT_H_INCLUDED

/**
 * \file data_struct.h
 * \brief Data structures for ltl
 */

#include <stdbool.h>
#include <stdlib.h>

///\brief Directions in which the Player can move
typedef enum{RIGHT, UP, LEFT, DOWN, ERROR} Direction;
Direction opposite_dir(const Direction);

///\brief Cell data structure
typedef struct{
	bool top; ///< \brief True if there’s a wall on top of this cell
	bool left; ///< \brief True if there’s a wall on the left side
} Cell;
Cell new_cell(bool, bool);

///\brief Simple coordinates data structure
typedef struct{
	int y; ///< \brief Line number
	int x; ///< \brief Column number
} Yx;
Yx new_yx(int, int);
Yx get_neigh(Yx, Direction);

///\brief Board data structure
typedef struct{
	int h; ///< \brief Height: total number of lines
	int w; ///< \brief Width: total number of columns
	Yx start; ///< \brief Where the Player will start
	Yx end; ///< \brief Where the Player must end
	Cell** cells; ///< \brief Array[h][w] of cells
} Board;
Board *new_board(const int, const int, Yx);
void free_board(Board *);

///\brief Player data structure
typedef struct{
	Yx c; ///< \brief Coordinates
	int nb_steps; ///< \brief Number of steps already taken
	char str[2]; ///< \brief Characters to be printed
	bool robot; ///< \brief If the player is not human
} Player;
Player *new_player(Yx, bool);
void free_player(Player *);

void set_wall(Board *, Yx, Direction, bool);
bool get_wall(Board *, Yx, Direction);
bool exists(Board *, Yx);
bool is_alone(Board *, Yx);
bool has_not_alone_neighbor(Board *, Yx);
#endif //_DATA_STRUCT_H_INCLUDED

