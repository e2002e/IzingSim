#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>

const int N = 3;//on récupère le premier argument du programme avec atoi(argv[1]) si on veut 
int row, col;
void *cycle(void*);
pthread_mutex_t mutex1;

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
    
    getmaxyx(stdscr, row, col);
    printw("rows:%d\ncol:%d", row, col);
    
    mutex1 = PTHREAD_MUTEX_INITIALIZER;  
    int Nthread = 8;
    pthread_t threads[Nthread];
    int id[Nthread];
    int i, j;
    //for(i=0; i < Nthread; i++) id[i] = pthread_create(&threads[i], NULL, cycle, NULL);
    //for(j=0; j < Nthread; j++) id[j] = pthread_join(threads[j], NULL);
    cycle(NULL);
    sleep(5);
    endwin();
    
}
void *cycle(void*)
{
    long int value;
    char A[4];  
    int spins[N][N][N];
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            for(int k=0;k<N;k++)
                spins[i][j][k]=0;
    value =(long int) pow(2.0, pow((double)N, 3.0));
    
    for(long int j=0; j < value; j++){
        int x, y, z;
        for(x=0; x < N; x++){
            for(y = 0; y < N; y++){
                for(z = 0; z < N; z++){
                    spins[x][y][z] =  (((j % ((long int)pow(2.0, (double)N)-1)) & ((long int)pow(2.0, (double)(x+1))-1)) 
                                        >> x) &
                                      (((j % ((long int)pow(2.0, pow((double)N, 2.0))-1)) & ((long int)pow(2.0, (double)(y+1)*(y+1))-1)) 
                                        >> y) &
                                      (((j % ((long int)pow(2.0, pow((double)N, 3.0))-1)) & ((long int)pow(2.0, pow((double)(z+1),3.0))-1))
                                        >> z)
                                      ;
                    snprintf(A, 2, "%d",  spins[x][y][z]);
                    attron(COLOR_PAIR(z+1));
                    mvprintw((y*3+row/4-z)+3, (x*4+z)+3, A);
                    attroff(COLOR_PAIR(z+1)); 
                }
            }
        }
        refresh();
    }
    return NULL;
}
 
