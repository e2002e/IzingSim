#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <curses.h>

int main(int argc, char *argv[]){
    initscr();cbreak();
	int row, col;
    getmaxyx(stdscr, row, col);
    printw("rows:%d\ncol:%d", row, col);
    refresh();
    int N = 4;//on récupère le premier argument du programme.
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
                            mvprintw((y*2+row/2-z)+3, (x*2+z)+3, A);
                            if(y != 0)
                                mvprintw((y*2+row/2-z)+2, (x*2+z)+3, "|");
                            mvprintw((y*2+row/2-z)+3, (x*2+z)+2, "-");
                            
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
