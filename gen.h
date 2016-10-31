/**
 * \file gen.h
 * \brief Maze generation algorithms
 */

#include <math.h>
#include <stdlib.h>
#include "data_struct.h"
#include "text_ui.h"

///\brief Possible algorithims to choose from
typedef enum {BRUTE, SIMUL} GenAlgo;
void brute_gen(UI *, float, Board *, Yx, Yx *, int *);
void simul_gen(UI *, float, Board *, Yx, Yx *, int *, int **);
int gen_maze(UI *, float, Board *, GenAlgo);

void straight_path(Board *, Yx, Yx);
void angle_path(Board *, Yx, Yx);
