#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <curses.h>
#include <pthread.h>
#include <unistd.h>

double doP(), doZ(), doH();
//on récupère le premier argument du programme avec atoi(argv[1]) si on veut 
int row, col;
void *cycle(void*);
pthread_mutex_t mutex1;
int N, y;
double T = 1.0;
int *spins;
double h = 1.0;
double J = 1.0;
const double Boltz = (1.0/(1.38064852*-pow(10.0, 23.0))) * (double)T;

int main(int argc, char *argv[]){
    srand(time(NULL));// random generator init
    // all the ncurses init
    initscr();cbreak();noecho();
    if(has_colors() == false) mvprintw(0, 10, "Terminal doesn't support colors..\n");
    start_color();
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    getmaxyx(stdscr, row, col);
    //
    //threads (not used)
    mutex1 = PTHREAD_MUTEX_INITIALIZER;
    int Nthread = 8;
    pthread_t threads[Nthread];
    int id[Nthread];
    int i, j;
    //for(i=0; i < Nthread; i++) id[i] = pthread_create(&threads[i], NULL, cycle, NULL);
    //for(j=0; j < Nthread; j++) id[j] = pthread_join(threads[j], NULL);
    //
    
    N = atoi(argv[1]);
    spins = new int[N];
    for(int i=0; i<N; i++)
    {
        spins[i] = -1;//juste une initialisation random a la con  
        if(spins[i] == 0)
            spins[i] = -1;
    }
    char *c;
    while(1){
        mvprintw(0,0,"1)calculate\n2)system temperature:%lf\n3)magnitude:%lf and paramagnitude:%lf", T, J, h );
        getstr(c);
        if(atoi(c) < 1 || atoi(c) > 3) continue;
        switch(atoi(c)){
            case 1:
                cycle(NULL);
                break;
            case 2:
                mvprintw(1,0,"Enter temperature then Enter");
                char d[10];
                getstr(d);
                T = (double) atoi(d);
                break;
            default:
                continue;
        }
    }
    endwin();    
}
void *cycle(void*)
{
    for(int j=0; j<N; j++)
    {
        y=0;
    }
    double P = doP();
    mvprintw(4+y, 0, "Proba(C):%lf", P);
    refresh();
    return NULL;
}
double seekNeighbours(int i)
{
    double sum;
    int post = N - i;
    int pre  = N - post;
    for(int j=0; j<pre; j++)
        sum += (double)spins[i]*((double)spins[j]*(1/pow((double)j, 3.0)));
    for(int j=post; j<N; j++)
        sum += (double)spins[i]*((double)spins[j]*(1/pow((double)j-post, 3.0)));
    return sum;
}

double doP()
{
    double P = (1/doZ()) * (double)exp(-Boltz*doH());
    return P; 
}

double doH()
{
    double H = 0;
    for(int i=0; i<N; i++){
        H += -seekNeighbours(i)*J-(double)spins[i]*h;
    }
    mvprintw(5+y, 0, "Hamiltonian: %lf", H);
    return H;
}
double doZ()
{
    long int value = pow(2.0, (double)N);
    double sum = 0.0;
    double Z = 0;
    for(long int a=0;a<value;a++)
    {
        for(int i=0;i<N;i++)
        {
            spins[i] = a & (long int)pow(2.0, (double)i) >> i;
            sum += (double)spins[i];
        }
        Z = sum * (double)exp(-Boltz*doH());
    }
    mvprintw(6+y, 0, "Normalized Hamiltonian: %lf", Z);
    return Z;
}
