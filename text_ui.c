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
	ui->signal = CONTINUE;
	
	//Colors
	if(has_colors() == FALSE){
		endwin();
		printf("Your terminal does not support color.\n");
		exit(1);
	}
	start_color();
	init_pair(CP_DEF,	COLOR_BLACK,	COLOR_WHITE);
	init_pair(CP_WALL,	COLOR_WHITE,	COLOR_WHITE);
	init_pair(CP_PLAYER,	COLOR_BLACK,	COLOR_BLUE);
	init_pair(CP_END,	COLOR_RED,	COLOR_RED);

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

//Board
///\brief Prints board properly
void print_board(UI *ui, Board *b){
	//Clean and set attributes
	ui_clear(ui);
	wattrset(ui->main_win, COLOR_PAIR(CP_WALL));

	//Walls defined in Board::cells
	int i, j;
	for(i=0; i<b->h; i++){
		for(j=0; j<b->w; j++){
			mvwaddch(ui->main_win, 2*i, 3*j, '+');
			if(b->cells[i][j].top)
				mvwaddstr(ui->main_win, 2*i, 3*j+1, "--");
			if(b->cells[i][j].left)
				mvwaddch(ui->main_win, 2*i+1, 3*j, '|');
		}
		//Wall on right of board
		mvwaddch(ui->main_win, 2*i, 3*b->w, '+');
		mvwaddch(ui->main_win, 2*i+1, 3*b->w, '|');
	}

	//Wall down of board
	for(j=0; j<b->w; j++){
		mvwaddstr(ui->main_win, 2*b->h, 3*j, "+--");
	}

	//Bottom-right corner
	mvwaddch(ui->main_win, 2*b->h, 3*b->w, '+');

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
	if(plr->nb_steps % 100){
		wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER) | A_REVERSE);
		mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "::");
	}else{
		wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER));
		mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "% 2X", plr->nb_steps/100);
	}
	return;
}

///\brief Prints Player from screen
void print_player(UI* ui, Player *plr){
	wattrset(ui->main_win, COLOR_PAIR(CP_PLAYER));
	mvwprintw(ui->main_win, 2*plr->c.y+1, 3*plr->c.x+1, "::");
	return;
}

///\brief Moves Player on screen
void move_player(UI *ui, Board *b, Player *plr, Direction dir){
	//Set direction coordinates
	Yx d = new_yx(0, 0);
	switch(dir){
	case LEFT:
		d.x--;
		break;
	case DOWN:
		d.y++;
		break;
	case UP:
		d.y--;
		break;
	case RIGHT:
		d.x++;
		break;
	default:
		break;
	}

	//Move
	if(!get_wall(b, plr->c, dir)){
		erase_player(ui, plr);
		plr->c.y += d.y;
		plr->c.x += d.x;
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

