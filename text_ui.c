#include "text_ui.h"

//General UI functions
///\brief UI initialization
UI *ui_init(){
	initscr();			//Starts ncurses mode
	raw();				//Disables line buffering
	noecho();			//Disables input echo
	curs_set(FALSE);		//Hides cursor
	
	//UI struct init.
	UI *ui = (UI *) malloc(sizeof(UI));
	ui->main_win = newwin(LINES, COLS, 0, 0);
	keypad(ui->main_win, TRUE);	//Enable special keys
	nodelay(ui->main_win, TRUE);    //Don’t block for user input
	ui->signal = CONTINUE;
	
	//Colors
	if(has_colors() == FALSE){
		endwin();
		printf("Your terminal does not support color.\n");
		exit(1);
	}
	start_color();
	init_pair(CP_DEF,	COLOR_WHITE,	COLOR_WHITE);
	init_pair(CP_WALL,	COLOR_BLACK,	COLOR_BLACK);
	init_pair(CP_PLAYER,	COLOR_WHITE,	COLOR_BLUE);
	init_pair(CP_END,	COLOR_WHITE,	COLOR_RED);

	//Display
	wrefresh(ui->main_win);
	return ui;
}

///\brief UI termination
void ui_terminate(UI *ui){
	ui_clear(ui);
	delwin(ui->main_win);
	endwin();
	return;
}

///\brief Correctly clear screen
void ui_clear(UI *ui){
	wstandend(ui->main_win);
	wclear(ui->main_win);
	wrefresh(ui->main_win);
	return;
}

///\brief Waits for n miliseconds
void msleep(float n){
	struct timespec lag;
	lag.tv_sec = 0;
	lag.tv_nsec = n*1e6;
	nanosleep(&lag, NULL);
	return;
}

//Board
///\brief Prints board properly
void print_board(UI *ui, Board *b){
	//Walls defined in Board::cells
	int i, j;
	CP_name color;
	for(i=0; i<b->h; i++){
		for(j=0; j<b->w; j++){
			//Cell up-left corner
			wattrset(ui->main_win, COLOR_PAIR(CP_WALL));
			mvwaddch(ui->main_win, 2*i, 3*j, ' ');

			//Cell up wall
			color = get_wall(b, new_yx(i, j), UP) ? CP_WALL : CP_DEF;
			wattrset(ui->main_win, COLOR_PAIR(color));
			mvwprintw(ui->main_win, 2*i, 3*j+1, "  ");
			
			//Cell left wall
			color = get_wall(b, new_yx(i, j), LEFT) ? CP_WALL : CP_DEF;
			wattrset(ui->main_win, COLOR_PAIR(color));
			mvwaddch(ui->main_win, 2*i+1, 3*j, ' ');
			
			//Cell content
			color = is_alone(b, new_yx(i, j)) ? CP_WALL : CP_DEF;
			wattrset(ui->main_win, COLOR_PAIR(color));
			mvwprintw(ui->main_win, 2*i+1, 3*j+1, "  ");
		}
		//Cell right wall on last column
		color = get_wall(b, new_yx(i, 0), LEFT) ? CP_WALL : CP_DEF;
		wattrset(ui->main_win, COLOR_PAIR(color));
		mvwaddch(ui->main_win, 2*i+1, 3*b->w, ' ');
	}
	//Cell down wall on last line
	for(j=0; j<b->w; j++){
		color = get_wall(b, new_yx(0, j), UP) ? CP_WALL : CP_DEF;
		wattrset(ui->main_win, COLOR_PAIR(color));
		mvwprintw(ui->main_win, 2*b->h, 3*j+1, "  ");
	}

	//Mark goal
	wattrset(ui->main_win, COLOR_PAIR(CP_END));
	mvwaddstr(ui->main_win, 2*b->end.y+1, 3*b->end.x+1, "  ");

	//Display this
	wrefresh(ui->main_win);
	return;
}

///\brief Returns keyboard input as a Direction
Direction get_user_input(UI *ui){
	Direction ret = ERROR;
	int ch = wgetch(ui->main_win);
	if(ch == CTRLS_LEFT[0] || ch == CTRLS_LEFT[1]){
		ret = LEFT;
	}else if(ch == CTRLS_DOWN[0] || ch == CTRLS_DOWN[1]){
		ret = DOWN;
	}else if(ch == CTRLS_UP[0] || ch == CTRLS_UP[1]){
		ret = UP;
	}else if(ch == CTRLS_RIGHT[0] || ch == CTRLS_RIGHT[1]){
		ret = RIGHT;
	}else if(ch == CTRLS_QUIT[0] || ch == CTRLS_QUIT[1]){
		ui->signal = QUIT;
	}
	return ret;
}

///\brief Erases Player from screen
void erase_player(UI* ui, Player *plr){
	wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER));
	if(plr->nb_steps % 100){
		mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "  ");
	}else{
		mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "%02d", (plr->nb_steps/100)%100);
	}
	return;
}

///\brief Prints Player from screen
void print_player(UI* ui, Player *plr){
	wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER) | A_BOLD);
	mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "::");
	return;
}

///\brief Erases filling between unvisited cells
void erase_fill(UI *ui, Yx c, Direction dir){
	//Correct input
	wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER));
	switch(dir){
	case LEFT:
		mvwaddch(ui->main_win, 2*c.y+1, 3*c.x, ' ');
		break;
	case DOWN:
		mvwaddstr(ui->main_win, 2*c.y+2, 3*c.x+1, "  ");
		break;
	case UP:
		mvwaddstr(ui->main_win, 2*c.y, 3*c.x+1, "  ");
		break;
	case RIGHT:
		mvwaddch(ui->main_win, 2*c.y+1, 3*c.x+3, ' ');
		break;
	default:
		break;
	}
	return;
}

///\brief Moves Player on screen
void move_player(UI *ui, Board *b, Player *plr, Direction dir){
	//Move
	if(!get_wall(b, plr->c, dir)){
		erase_player(ui, plr);
		erase_fill(ui, plr->c, dir);
		plr->c = get_neigh(b, plr->c, dir);
		if(((dir == LEFT) && (plr->c.x == b->w-1)) || ((dir == DOWN) && (plr->c.y == 0)) || ((dir == UP) && (plr->c.y == b->h-1)) || ((dir == RIGHT) && (plr->c.x == 0))){
			erase_fill(ui, plr->c, opposite_dir(dir));
		}
		plr->nb_steps++;
		print_player(ui, plr);
	}

	//Success
	if((plr->c.y == b->end.y) && (plr->c.x == b->end.x)){
		ui->signal = QUIT;
	}

	//Display this
	wrefresh(ui->main_win);
	return;
}

