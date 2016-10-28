#include "gen.h"

int gen_maze(Board *b, int seed){
	srand(seed);
	int to_end = 0;
	b->end = brute_gen(b, b->start, LEFT, &to_end);
	return to_end;
}

/**
 * \brief A very simple generation algorithm
 *
 * \param *b Pointer to the Board
 * \param c Cell from which to go
 * \param from_dir The direction (relative to c) from where we are coming
 * \param *to_end distance to the current end; will be maximized
 * \return Coordinates for Board::end. The farthest cell from start.
 */
Yx brute_gen(Board *b, Yx c, Direction from_dir, int *to_end){
	//Array of neighboring cells
	Yx neighs[4];
	neighs[LEFT]  = new_yx(c.y, c.x-1);
	neighs[DOWN]  = new_yx(c.y+1, c.x);
	neighs[UP]    = new_yx(c.y-1, c.x);
	neighs[RIGHT] = new_yx(c.y, c.x+1);

	//Farthest cells in every direction
	Yx farthest[4] = {c, c, c, c};
	int distances[4] = {*to_end, *to_end, *to_end, *to_end};

	//Flags
	bool tested[4] = {false, false, false, false};
	tested[from_dir] = true;
	
	//Test every direction in random order
	Direction dir = rand()%4;
	while(!tested[LEFT] || !tested[DOWN] || !tested[UP] || !tested[RIGHT]){
		if(is_alone(b, neighs[dir]) && !tested[dir]){
			set_wall(b, c, dir, false);
			distances[dir]++;
			farthest[dir] = brute_gen(b, neighs[dir], opposite_dir(dir), &(distances[dir]));
		}
		tested[dir] = true;
		dir = (dir+1 == ERROR) ? RIGHT : dir+1;
	}

	//Return farthest
	Direction dir_to_end;
	if((distances[LEFT] > distances[DOWN]) && (distances[LEFT] > distances[UP]) && (distances[LEFT] > distances[RIGHT])){
		dir_to_end = LEFT;
	}else if((distances[DOWN] > distances[UP]) && (distances[DOWN] > distances[RIGHT])){
		dir_to_end = DOWN;
	}else if(distances[UP] > distances[RIGHT]){
		dir_to_end = UP;
	}else{
		dir_to_end = RIGHT;
	}
	*to_end = distances[dir_to_end];
	return farthest[dir_to_end];
}

void straight_path(Board *b, Yx from, Yx to){
	//Start by ordering from and to
	if((from.y > to.y) || (from.x > to.x)){
		Yx tmp = from;
		from = to;
		to = tmp;
	}
	Yx cur = from;
	Yx next = cur;
	int nb_steps = (abs(to.y-from.y)+abs(to.x-from.x));
	float delta_y = ((to.y*1.0)-(from.y*1.0))/(nb_steps*1.0);
	float delta_x = ((to.x*1.0)-(from.x*1.0))/(nb_steps*1.0);
	int i;
	for(i=1; i<=nb_steps; i++){
		next.y = (int) floorf(from.y + i*delta_y);
		next.x = (int) floorf(from.x + i*delta_x);
		angle_path(b, cur, next);
		cur = next;
	}
	return;
}

void angle_path(Board *b, Yx from, Yx to){
	Yx d;
	d.y = -1*((to.y-from.y) < 0) + 1*((to.y-from.y) > 0);
	Direction y_side = (d.y > 0) ? DOWN : UP;
	d.x = -1*((to.x-from.x) < 0) + 1*((to.x-from.x) > 0);
	Direction x_side = (d.x > 0) ? RIGHT : LEFT;
	
	Yx cur = from;	
	while(cur.y != to.y){
		set_wall(b, cur, y_side, false);
		cur.y += d.y;
	}
	while(cur.x != to.x){
		set_wall(b, cur, x_side, false);
		cur.x += d.x;
	}
	return;
}

