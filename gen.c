#include "gen.h"

/**
 * \brief Generates a maze with given algorithm
 *
 * \param *b the board to set
 * \param alg choice of algorithm
 * \return the minimal distance from Board::start to Board::end
 */
int gen_maze(Board *b, GenAlgo alg){
	int end_dist = 0;
	switch(alg){
	case BRUTE:
		brute_gen(b, b->start, &(b->end), &end_dist);
		break;
	case SIMUL:
		simul_gen(b, b->start, &(b->end), &end_dist);
		break;
	}
	return end_dist;
}

/**
 * \brief A very simple generation algorithm
 *
 * \param *b Pointer to the Board
 * \param c Cell from which to go
 * \param end Coordinates for Board::end. The farthest cell from start.
 * \param *to_end distance to the current end; will be maximized
 *
 * Recursively looks up every yet unobserved direction.
 * Because the direction are looked up recursively one after the other, there
 * are relatively few dead ends and those are short and easy to recognize.
 * But the code is short and it ensures that no cell is left alone and that
 * there is no loop.
 */
void brute_gen(Board *b, Yx c, Yx *end_cell, int *end_dist){
	//Farthest cells in every direction
	Yx max_cell = c;
	Yx test_cell = max_cell;
	int max_dist = *end_dist;
	int test_dist = max_dist;

	//Test every direction in random order
	bool tested[4] = {false, false, false, false};
	Direction dir = rand()%4;
	while(!tested[LEFT] || !tested[DOWN] || !tested[UP] || !tested[RIGHT]){
		if(!tested[dir] && exists(b, get_neigh(c, dir)) && is_alone(b, get_neigh(c, dir))){
			set_wall(b, c, dir, false);
			test_dist++;
			brute_gen(b, get_neigh(c, dir), &test_cell, &test_dist);
			if(test_dist > max_dist){
				max_dist = test_dist;
				max_cell = test_cell;
			}
		}
		test_cell = c;
		test_dist = *end_dist;
		tested[dir] = true;
		dir = rand()%4;
	}
	*end_cell = max_cell;
	*end_dist = max_dist;
	return;
}

/**
 * \brief Another recursive algorithm
 *
 * \param *b The board to set
 * \param c The cell from which to start
 * \param *end_cell where to store the farthest found cell
 * \param *end_dist where to store the distance to end_cell
 *
 * During the first phase, branches are calculated simultaneously. The number of
 * simultaneous robots is limited to 4. If it increases, the different robots
 * tend to restrain each other, and this makes long, boring, paralell lines. If
 * it decreases, more cells are left empty, for the second phase to use. Because
 * the second phase is recursive, a low limit could result in something
 * equivalent to brute_gen().
 * 
 * In the second phase, this algorithm is launched again to fill the blanks and
 * link them with the paths generated before. The end cell is also updated
 * during this phase.
 * 
 * In the end, the strength of this algorithm resides in the fact that dead ends
 * are often very long and difficult to recognize at first glimpse.
 *
 */
void simul_gen(Board *b, Yx c, Yx *end_cell, int *end_dist){
	//A robot mining through the walls (chained list)
	typedef struct _simul_gen_robot_struct{
		Yx c;
		int dist;
		struct _simul_gen_robot_struct *next;
	} Robot;
	
	//The first robot
	Robot *first = (Robot *) malloc(sizeof(Robot));
	first->c = c;
	first->dist = *end_dist;
	first->next = first;

	//The string of robots
	int nb_robots = 1;
	Robot *last = first;

	//Variables for the end
	Yx max_cell = first->c;
	int max_dist = first->dist;
	int distances[b->h][b->w];
	int i, j;
	for(i=0; i<b->h; i++){
		for(j=0; j<b->w; j++){
			distances[i][j] = 0;
		}
	}
	
	//Variables for every single test applied on the robot
	bool tested[4];
	Direction dir;
	
	//Loop on robot
	Robot *entry = (Robot *) malloc(sizeof(Robot));
	entry->next = first;
	Robot *cur = entry;
	while(nb_robots > 0){
		//Test current robot
		tested[RIGHT] = false;
		tested[UP]    = false;
		tested[LEFT]  = false;
		tested[DOWN]  = false;
		dir = rand()%4;
		while(!tested[RIGHT] || !tested[UP] || !tested[LEFT] || !tested[DOWN]){
			if(!tested[dir] && is_alone(b, get_neigh(cur->next->c, dir)) && (nb_robots<4)){
				//Add a new robot in next pos and crush one wall
				set_wall(b, cur->next->c, dir, false);
				Robot *tmp = (Robot *) malloc(sizeof(Robot));
				tmp->c = get_neigh(cur->next->c, dir);
				tmp->dist = cur->next->dist+1;
				tmp->next = last->next;
				last->next = tmp;
				last = tmp;
				if(tmp->dist > max_dist){
					max_dist = tmp->dist;
					max_cell = tmp->c;
				}
				distances[tmp->c.y][tmp->c.x] = tmp->dist;
				nb_robots++;
			}
			tested[dir] = true;
			dir = rand()%4;
		}

		//Delete current robot
		nb_robots--;
		Robot *tmp = cur->next;
		cur->next = cur->next->next;
		free(tmp);
	}
	free(cur);

	//Second phase: join alone cells
	Yx p2_cur;
	Direction p2_dir;
	int p2_dist;
	bool p2_linked;
	for(i=0; i<b->h; i++){
		for(j=0; j<b->w; j++){
			p2_cur = new_yx(i, j);
			if(is_alone(b, p2_cur) && has_not_alone_neighbor(b, p2_cur)){
				p2_linked = false;
				p2_dir = RIGHT;
				while(!p2_linked && (p2_dir < ERROR)){
					if(exists(b, get_neigh(p2_cur, p2_dir)) && !is_alone(b, get_neigh(p2_cur, p2_dir))){
						set_wall(b, p2_cur, p2_dir, false);
						p2_linked = true;
						p2_dist = 1+distances[get_neigh(p2_cur, p2_dir).y][get_neigh(p2_cur, p2_dir).x];
						simul_gen(b, p2_cur, &p2_cur, &p2_dist);
						if(p2_dist > max_dist){
							max_dist = p2_dist;
							max_cell = p2_cur;
						}
					}
					p2_dir++;
				}
			}
		}
	}

	//Set found end
	*end_cell = max_cell;
	*end_dist = max_dist;
	return;
}

///\brief Trace a straight path between two given Yx locations
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

///\brief Trace an angle path between two given Yx locations
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

