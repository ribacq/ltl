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
	ui_clear(ui);
	float disp_lag = 1;
	
	//Board height and width
	int h, w;
	Yx start;
	h = (getmaxy(ui->main_win)-1)/2;
	w = (getmaxx(ui->main_win)-1)/3;
	
	//Played by human or robot and robot lag
	bool robot = false;
	float robot_lag = 100;
	
	//Board random seed and algorithm
	int seed = time(NULL);
	GenAlgo alg = SIMUL;
	
	//Read throught parameters
	int i=1;
	char *conv_test;
	while(i<argc){
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height")){
			if(i+1 < argc){
				strtol(argv[i+1], &conv_test, 10);
				if(*conv_test == '\0'){
					i++;
					h = (int) strtol(argv[i], NULL, 10);
				}
			}
		}else if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")){
			if(i+1 < argc){
				strtol(argv[i+1], &conv_test, 10);
				if(*conv_test == '\0'){
					i++;
					w = (int) strtol(argv[i], NULL, 10);
				}
			}
		}else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--robot")){
			robot = true;
			if(i+1 < argc){
				strtof(argv[i+1], &conv_test);
				if(*conv_test == '\0'){
					i++;
					robot_lag = strtof(argv[i], NULL);
				}
			}
		}else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lag")){
			if(i+1 < argc){
				strtof(argv[i+1], &conv_test);
				if(*conv_test == '\0'){
					i++;
					disp_lag = strtof(argv[i], NULL);
				}
			}
		}else if(!strcmp(argv[i], "-b") || !strcmp(argv[i], "--brute")){
			alg = BRUTE;
		}else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "--simul")){
			alg = SIMUL;
		}else{
			seed = (int) strtol(argv[i], NULL, 10);
		}
		i++;
	}

	//Data instanciation
	srand(seed);
	start = new_yx(rand()%h, rand()%w);
	Board *b = new_board(h, w, start);
	int to_end = gen_maze(ui, disp_lag, b, alg);
	Player *plr = new_player(b->start, robot);
	print_board(ui, b);
	print_player(ui, plr);

	//Main loop
	Direction dir = LEFT;
	while(ui->signal == CONTINUE){
		if(plr->robot){
			//Robot
			dir = opposite_dir(dir);
			i = 0;
			do{
				dir = (dir == ERROR-1) ? RIGHT : dir+1;
				i++;
			}while(i<4 && get_wall(b, plr->c, dir));
			msleep(robot_lag);
			get_user_input(ui);
		}else{
			//Human
			dir = get_user_input(ui);
		}
		//Make move
		move_player(ui, b, plr, dir);
	}
	ui_terminate(ui);
	printf("You have taken %d steps to leave the labyrinth.\n", plr->nb_steps);
	printf("It was possible in %d steps.\n", to_end);
	printf("This means you have taken %.1f%% more steps than was necessary.\n",  100*(plr->nb_steps - to_end)/(1.0*to_end));

	printf("\nBoard size: %d(h) × %d(w) = %d cells\n", b->h, b->w, b->h*b->w);
	printf("Min. path rate: %.2f%%\n", (to_end*100.0)/(b->h*b->w));
	printf("Seed: %d\n", seed);
	free_player(plr);
	free_board(b);
	return EXIT_SUCCESS;
}

