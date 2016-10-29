/**
 * \file gen.h
 * \brief Maze generation algorithms
 */

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "data_struct.h"

///\brief Possible algorithims to choose from
typedef enum {BRUTE, SIMUL} GenAlgo;
void brute_gen(Board *, Yx, Yx *, int *);
void simul_gen(Board *, Yx, Yx *, int *);
int gen_maze(Board *, GenAlgo);

void straight_path(Board *, Yx, Yx);
void angle_path(Board *, Yx, Yx);
