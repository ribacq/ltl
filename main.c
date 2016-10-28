#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "data_struct.h"
#include "text_ui.h" 
#include "gen.h"

/**
 * \mainpage CLI Maze game in C
 *
 * This software uses `ncurses.h`.
 */

/**
 * \file main.c
 * \brief Main file for ltl
 *
 * This file is where the main() function lives. It should be UI-independent.
 */

/**
 * \brief Main function.
 * 
 * Command-line parameters:
 * -h/--height N: sets board height to N
 * -w/--width N: sets board width to N
 * -r/--robot N: sets robot to play and lag to Ne-2 seconds.
 * N: sets board random seed.
 */
int main(int argc, char *argv[]){
	//Parameters that can be modified with the command-line parameters
	UI* ui = ui_init();
	//Board height and width
	int h, w;
	Yx start;
	h = (getmaxy(ui->main_win)-1)/2;
	w = (getmaxx(ui->main_win)-1)/3;
	//Played by human or robot and robot lag
	bool robot = false;
	struct timespec lag;
	lag.tv_sec = 0;
	lag.tv_nsec = 1e8;
	//Board random seed
	srand(time(NULL));
	int seed = rand();
	//Read throught parameters
	int i=1;
	while(i<argc){
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height")){
			if(i+1 < argc){
				i++;
				h = (int) strtol(argv[i], NULL, 10);
			}
		}else if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")){
			if(i+1 < argc){
				i++;
				w = (int) strtol(argv[2], NULL, 10);
			}
		}else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--robot")){
			robot = true;
			if(i+1 < argc){
				i++;
				lag.tv_nsec = 1e8*strtof(argv[i], NULL);
			}
		}else{
			seed = (int) strtol(argv[i], NULL, 10);
		}
		i++;
	}

	//Data instanciation
	start = new_yx(rand()%h, rand()%w);
	Board *b = new_board(h, w, start);
	int dist = gen_maze(b, seed);
	Player *plr = new_player(b->start);
	print_board(ui, b);
	print_player(ui, plr);
	Direction dir = RIGHT;
	Direction next = dir;
	while(ui->signal == CONTINUE){
		if(!robot){
			//Human
			dir = get_user_input(ui);
		}else{
			//Robot
			next = opposite_dir(dir);
			do{
				next = (next == ERROR-1) ? RIGHT : next+1;
			}while(get_wall(b, plr->c, next) && next != opposite_dir(dir));
			dir = next;
			nanosleep(&lag, NULL);
		}
		//Make move
		move_player(ui, b, plr, dir);
	}
	ui_terminate(ui);
	printf("Steps taken: %d\n", plr->nb_steps);
	printf("Minimal possible distance: %d\n", dist);
	printf("%.1f%% excess\n",  100*(plr->nb_steps - dist)/(1.0*dist));
	free(plr);
	return EXIT_SUCCESS;
}

