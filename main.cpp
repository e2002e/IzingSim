#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <curses.h>

int main(int argc, char *argv[]){
    initscr();cbreak();
    if(has_colors() == false) mvprintw(0, 10, "Terminal doesn't support colors..\n");
    start_color();
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_WHITE);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_WHITE);
    init_pair(8, COLOR_RED, COLOR_BLACK);
    
    
	int row, col;
    getmaxyx(stdscr, row, col);
    printw("rows:%d\ncol:%d", row, col);
    refresh();
    
    
    int N = 8;//on récupère le premier argument du programme.
	int spins[N][N][N];
    double value;
    char A[20];
    value = pow(2.0, (double)N);
    while(1){
    for(int j=0; j < (int)value; j++){
        for(int k=0; k < (int)value; k++){
            for(int l=0; l < (int)value; l++){
                for(int x=0; x < N; x++){
                    for(int y=0; y < N; y++){
                        for(int z=0; z < N; z++){
                            spins[x][y][z] =  ((j & (int)pow(2.0, (double)x)) >> x) \
                                            & ((k & (int)pow(2.0, (double)y)) >> y) \
                                            & ((l & (int)pow(2.0, (double)z)) >> z);
                            sprintf(A,"%d", spins[x][y][z]);
                            attron(COLOR_PAIR(z+1));
                            mvprintw((y*3+row/2-z)+3, (x*4+z)+3, A);
                            attroff(COLOR_PAIR(z+1));/*
                            if(y != 0)
                                mvprintw((y*3+row/2-z)+2, (x*4+z)+3, "|");
                            if(x != 0 && x != N - 1)
                                mvprintw((y*3+row/2-z)+3, (x*4+z)+2, "-");
                            */
                        }
                    }
                }
                refresh();    
            }
        }
    }};
	endwin();
	return 0;
}
