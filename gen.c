#include "gen.h"

/**
 * \brief Generates a maze with given algorithm
 *
 * \param *ui A user interface on which to display the constructing board
 * \param disp_lag interval in milliseconds for the display
 * \param *b the board to set
 * \param alg choice of algorithm
 * \return the minimal distance from Board::start to Board::end
 */
int gen_maze(UI *ui, float disp_lag, Board *b, GenAlgo alg){
	int end_dist = 0;
	int **distances = (int **) calloc(b->h, sizeof(int *));
	int i, j;
	switch(alg){
	case BRUTE:
		brute_gen(ui, disp_lag, b, b->start, &(b->end), &end_dist);
		break;
	case SIMUL:
		for(i=0; i<b->h; i++){
			distances[i] = (int *) calloc(b->w, sizeof(int));
			for(j=0; j<b->w; j++){
				distances[i][j] = 0;
			}
		}
		simul_gen(ui, disp_lag, b, b->start, &(b->end), &end_dist, distances);
		for(i=0; i<b->h; i++){
			free(distances[i]);
		}
		free(distances);
		break;
	}
	return end_dist;
}

/**
 * \brief A very simple generation algorithm
 *
 * \param *ui A user interface on which to display the constructing board
 * \param disp_lag interval in milliseconds for the display
 * \param *b Pointer to the Board
 * \param c Cell from which to go
 * \param *end_cell Coordinates for Board::end. The farthest cell from start.
 * \param *end_dist distance to the current end; will be maximized
 *
 * Recursively looks up every yet unobserved direction.
 * Because the direction are looked up recursively one after the other, there
 * are relatively few dead ends and those are short and easy to recognize.
 * But the code is short and it ensures that no cell is left alone and that
 * there is no loop.
 */
void brute_gen(UI *ui, float disp_lag, Board *b, Yx c, Yx *end_cell, int *end_dist){
	//Print WIP board
	if(disp_lag > 0){
		print_board(ui, b);
		msleep(disp_lag);
	}

	//Farthest cells in every direction
	Yx max_cell = c;
	Yx test_cell = max_cell;
	int max_dist = *end_dist;
	int test_dist = max_dist;

	//Test every direction in random order
	bool tested[4] = {false, false, false, false};
	Direction dir = rand()%4;
	while(!tested[LEFT] || !tested[DOWN] || !tested[UP] || !tested[RIGHT]){
		if(!tested[dir] && exists(b, get_neigh(b, c, dir)) && is_alone(b, get_neigh(b, c, dir))){
			set_wall(b, c, dir, false);
			test_dist++;
			brute_gen(ui, disp_lag, b, get_neigh(b, c, dir), &test_cell, &test_dist);
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
 * \param *ui A user interface on which to display the constructing board
 * \param disp_lag interval in milliseconds for the display
 * \param *b The board to set
 * \param c The cell from which to start
 * \param *end_cell where to store the farthest found cell
 * \param *end_dist where to store the distance to end_cell
 * \param **distances an array of size [b->h][b->w] in which to store the distances to all cells. It is transmitted recursively. Before the first call, initialize it to all 0.
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
void simul_gen(UI *ui, float disp_lag, Board *b, Yx c, Yx *end_cell, int *end_dist, int **distances){
	//A robot mining through the walls (chained list)
	typedef struct _simul_gen_robot_struct{
		Yx c;
		int dist;
		int energy;
		struct _simul_gen_robot_struct *next;
	} Robot;
	
	//The first robot
	Robot *first = (Robot *) malloc(sizeof(Robot));
	first->c = c;
	first->dist = *end_dist;
	first->energy = 42;
	first->next = first;

	//The string of robots
	int nb_robots = 1;
	const int MAX_ROBOTS = 1;
	Robot *last = first;

	//Variables for the end
	Yx max_cell = first->c;
	int max_dist = first->dist;
	
	//Variables for every single test applied on the robot
	bool tested[4];
	Direction dir;
	
	//Loop on robot
	Robot *cur = (Robot *) malloc(sizeof(Robot));
	cur->next = first;
	while(nb_robots > 0){
		//Print WIP board
		if(disp_lag > 0){
			print_board(ui, b);
			msleep(disp_lag);
		}

		//Test current robot
		tested[RIGHT] = false;
		tested[UP]    = false;
		tested[LEFT]  = false;
		tested[DOWN]  = false;
		dir = rand()%4;
		while((cur->next->energy > 0) && (!tested[RIGHT] || !tested[UP] || !tested[LEFT] || !tested[DOWN])){
			if(!tested[dir] && is_alone(b, get_neigh(b, cur->next->c, dir)) && (nb_robots<=MAX_ROBOTS)){
				//Add a new robot in next pos and crush one wall
				set_wall(b, cur->next->c, dir, false);
				Robot *tmp = (Robot *) malloc(sizeof(Robot));
				tmp->c = get_neigh(b, cur->next->c, dir);
				tmp->dist = cur->next->dist+1;
				tmp->energy = --cur->next->energy;
				tmp->next = last->next;
				last->next = tmp;
				last = tmp;
				if(tmp->dist > max_dist){
					//Possibly save a new end cell
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
	Yx p2_start = max_cell;
	Yx p2_cur = new_yx((p2_start.y+1)%b->h, (p2_start.x+1)%b->w);
	Direction p2_dir;
	int p2_dist;
	bool p2_linked;
	while((p2_cur.y != p2_start.y) || (p2_cur.x != p2_start.x)){
		p2_cur.y = (p2_cur.y+1)%b->h;
		p2_cur.x = (p2_cur.x+1)%b->w;
		if(is_alone(b, p2_cur) && has_not_alone_neighbor(b, p2_cur)){
			p2_linked = false;
			p2_dir = RIGHT;
			while(!p2_linked && (p2_dir < ERROR)){
				if(exists(b, get_neigh(b, p2_cur, p2_dir)) && !is_alone(b, get_neigh(b, p2_cur, p2_dir))){
					set_wall(b, p2_cur, p2_dir, false);
					p2_linked = true;
					p2_dist = 1+distances[get_neigh(b, p2_cur, p2_dir).y][get_neigh(b, p2_cur, p2_dir).x];
					simul_gen(ui, disp_lag, b, p2_cur, &p2_cur, &p2_dist, distances);
					if(p2_dist > max_dist){
						max_dist = p2_dist;
						max_cell = p2_cur;
					}
				}
				p2_dir++;
			}
		}
	}

	//Set found end
	*end_cell = max_cell;
	*end_dist = max_dist;
	return;
}

