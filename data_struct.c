#include "data_struct.h"

///\brief Simply gets the opposite Direction
Direction opposite_dir(const Direction dir){
	if(dir == LEFT)  return RIGHT;
	if(dir == DOWN)  return UP;
	if(dir == UP)    return DOWN;
	if(dir == RIGHT) return LEFT;
	return ERROR;
}

///\brief Cell constructor
Cell new_cell(bool top, bool left){
	Cell c;
	c.top = top;
	c.left = left;
	return c;
}

///\brief Constructor for Yx
Yx new_yx(int y, int x){
	Yx c;
	c.y = y;
	c.x = x;
	return c;
}

///\brief Get neighbor of given Yx in given Direction
Yx get_neigh(Yx c, Direction dir){
	switch(dir){
	case RIGHT:
		c.x++;
		break;
	case UP:
		c.y--;
		break;
	case LEFT:
		c.x--;
		break;
	case DOWN:
		c.y++;
		break;
	default:
		break;
	}
	return c;
}

///\brief Board constructor
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

///\brief Board destructor
void free_board(Board *b){
	int i;
	for(i=0; i<b->h; i++){
		free(b->cells[i]);
	}
	free(b->cells);
	free(b);
	return;
}

///\brief Player constructor
Player *new_player(Yx start_c){
	Player *plr = (Player*) malloc(sizeof(Player));
	plr->c = start_c;
	plr->nb_steps = 0;
	return plr;
}

///\brief Player destructor
void free_player(Player *plr){
	free(plr);
	return;
}

///\brief Sets wall correctly in the board
void set_wall(Board *b, Yx c, Direction side, bool val){
	//Wrong input: Yx out of Board limits
	if(!exists(b, c)) return;

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

///\brief Get a boolean indicating wether there is a wall around given cell.
bool get_wall(Board *b, Yx c, Direction side){
	//Wrong input: Yx out of Board limits
	if(!exists(b, c)) return false;

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
		ret = true;
	}
	return ret;
}

///\brief Indicates wether given coordinates are inside the board.
bool exists(Board *b, Yx c){
	return ((c.y >= 0) && (c.y < b->h) && (c.x >= 0) && (c.x < b->w));
}

///\brief Get a boolean indicating wether given cell is enclosed within walls.
bool is_alone(Board *b, Yx c){
	//Wrong input: Yx out of Board limits
	if(!exists(b, c)) return false;

	//Correct input
	return get_wall(b, c, LEFT) && get_wall(b, c, DOWN) && get_wall(b, c, UP) && get_wall(b, c, RIGHT);
}

bool has_not_alone_neighbor(Board *b, Yx c){
	return ( !is_alone(b, get_neigh(c, RIGHT)) && (c.x<b->w-1))
		|| (!is_alone(b, get_neigh(c, UP)) && (c.y>0))
		|| (!is_alone(b, get_neigh(c, LEFT)) && (c.x>0))
		|| (!is_alone(b, get_neigh(c, DOWN)) && (c.y<b->h-1));
}

