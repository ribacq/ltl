/**
 * \file gen.h
 * \brief Maze generation algorithms
 */

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "data_struct.h"

///\brief This is directly called from the outside
int gen_maze(Board *, int);
Yx brute_gen(Board *, Yx, Direction, int *);

///\brief Trace a straight path between two given Yx locations
void straight_path(Board *, Yx, Yx);
///\brief Trace an angle path between two given Yx locations
void angle_path(Board *, Yx, Yx);
