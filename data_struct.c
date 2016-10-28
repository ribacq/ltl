#include "data_struct.h"

Direction opposite_dir(const Direction dir){
	if(dir == LEFT)  return RIGHT;
	if(dir == DOWN)  return UP;
	if(dir == UP)    return DOWN;
	if(dir == RIGHT) return LEFT;
	return ERROR;
}

Cell new_cell(bool top, bool left){
	Cell c;
	c.top = top;
	c.left = left;
	return c;
}

Yx new_yx(int y, int x){
	Yx c;
	c.y = y;
	c.x = x;
	return c;
}

Board *new_board(const int h, const int w, Yx start){
	Board *b = (Board *) malloc(sizeof(Board));
	b->h = h;
	b->w = w;
	b->start = start;
	b->end = b->start;
	b->cells = (Cell **) calloc(h, sizeof(Cell *));
	int i, j;
	for(i=0; i<h; i++){
		b->cells[i] = (Cell *) calloc(w, sizeof(Cell));
		for(j=0; j<w; j++){
			b->cells[i][j] = new_cell(true, true);
		}
	}
	return b;
}

Player *new_player(Yx start_c){
	Player *plr = (Player*) malloc(sizeof(Player));
	plr->c = start_c;
	plr->nb_steps = 0;
	return plr;
}

void set_wall(Board *b, Yx c, Direction side, bool val){
	//Wrong input: Yx out of Board limits
	if((c.y < 0) || (c.x < 0) || (c.y >= b->h) || (c.x >= b->w)) return;

	//Correct input
	if(side == LEFT){
		b->cells[c.y][c.x].left = val;
	}else if((side == DOWN) && (c.y < b->h-1)){
		b->cells[c.y+1][c.x].top = val;
	}else if(side == UP){
		b->cells[c.y][c.x].top = val;
	}else if((side == RIGHT) && (c.x < b->w-1)){
		b->cells[c.y][c.x+1].left = val;
	}
	return;
}

bool get_wall(Board *b, Yx c, Direction side){
	//Wrong input: Yx out of Board limits
	if((c.y < 0) || (c.x < 0) || (c.y >= b->h) || (c.x >= b->w)) return false;

	//Correct input
	bool ret;
	if(side == LEFT){
		ret = b->cells[c.y][c.x].left;
	}else if(side == DOWN){
		ret = (c.y == b->h-1) || b->cells[c.y+1][c.x].top;
	}else if(side == UP){
		ret = b->cells[c.y][c.x].top;
	}else if(side == RIGHT){
		ret = (c.x == b->w-1) || b->cells[c.y][c.x+1].left;
	}else{
		ret = false;
	}
	return ret;
}

bool is_alone(Board *b, Yx c){
	//Wrong input: Yx out of Board limits
	if((c.y < 0) || (c.x < 0) || (c.y >= b->h) || (c.x >= b->w)) return false;

	//Correct input
	return get_wall(b, c, LEFT) && get_wall(b, c, DOWN) && get_wall(b, c, UP) && get_wall(b, c, RIGHT);
}

