#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <curses.h>
//#include <pthread.h>
#include <unistd.h>

double doP(), doZ(), doH();//Les fonctions servant aux calculs 
int row, col;//des variables pour ncurses
void *cycle(void*);//ma fonction principal qui devrais plutôt s'appeller "doCalc" ou un truc ds le genre
//pthread_mutex_t mutex1;
int N;//la variable qui sert a determiner le nombre de spins
double T, h, J;
int *spins;
double Boltz;

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
    /*threads (not used)
    mutex1 = PTHREAD_MUTEX_INITIALIZER;
    int Nthread = 8;
    pthread_t threads[Nthread];
    int id[Nthread];
    int i, j;
    //for(i=0; i < Nthread; i++) id[i] = pthread_create(&threads[i], NULL, cycle, NULL);
    //for(j=0; j < Nthread; j++) id[j] = pthread_join(threads[j], NULL);
    */
    //init variables
    T = 0.0;
    h = 0.0;
    J = 0.5;
    Boltz = (1.0/(1.38064852*pow(10.0, -23.0))) * (double)T;

    N = atoi(argv[1]);//N est le premier argument du programme, segfault si omit
    spins = new int[N];
    for(int i=0; i<N; i++)
    {
        spins[i] = rand()%2;//juste une initialisation random a la con  
        if(spins[i] == 0)
            spins[i] = -1;
    }
    //Le module de menu, à dévelloper
    char c[10];
    while(1){
        mvprintw(0,0,"1)calculate\n2)system temperature:\t%12lf\n3)magnitude:\t\t%12lf\nand paramagnitude:\t%12lf\n4)exit", T, J, h );
        getstr(c);
        if(atoi(c) < 1 || atoi(c) > 4) continue;
        switch(atoi(c)){
            case 1:
                cycle(NULL);
                break;
            case 2:
                mvprintw(1,0,"Enter temperature then Enter                   ");
                getstr(c);
                T = atof(c);
                Boltz = (1.0/(1.38064852*pow(10.0, -23.0))) * (double)T;
                break;
            case 4:
                endwin();
                exit(0);
            default:
                continue;
        }
    }
    endwin(); 
}
void *cycle(void*)
{
    double P = doP();//doP() appelle les autres fonctions
    mvprintw(5, 0, "Proba(C):\t\t%12lf", P);
    mvprintw(6, 0, "Hamiltonian(C):\t\t%12lf", doH());
    refresh();
    return NULL;
}
double seekNeighbours(int i)
{
    double sum;
    int post = N - i; //i est l'index du spin, N - i donnes les spins après donc variable nommée 'post'
    int pre  = N - post; //même logique, mais avec les spins d'avant.
    for(int j=0; j<pre; j++)
        sum += (double)spins[i]*((double)spins[j]*(1/pow((double)pre-j, 3.0)));
    for(int j=N; j>post; j--)
        sum += (double)spins[i]*((double)spins[j]*(1/pow((double)j-post, 3.0)));
    //ces deux boucles additionnent le spin actuel avec ses voisins (d'avant et d'après) en multipliant ces derniers par 1/r^3
    return sum;
}

double doP()
{
    double P = (1/doZ()) * exp(-Boltz*doH()); //la formule est plutôt explicite.
    return P; 
}

double doH()
{
    double H = 0;
    for(int i=0; i<N; i++){
        H += -seekNeighbours(i)*J - (double)spins[i]*h;//Les voisins * J - le spin * h
    }
    return H;
}
double doZ()
{
    long int value = (long int) pow(2.0, (double)N); // Ce nombre sert de map, toutes les config sont testées grâce à lui 
    double Z = 0;
    int *save = new int[N];
    for(int i=0; i<N; i++)
        save[i] = spins[i]; // je sauvegarde la config car elle va être remplacée
    for(long int a=0;a<value;a++)
    {
        double sum = 0;
        for(int i=0;i<N;i++)
        {
            spins[i] = a & (long int)pow(2.0, (double)i) >> i;  //un AND (binaire) avec 2^i montre si le bit(mapé sur l'index du tableau)
                                                                //est utilisé, suivit d'un shift i pour avoir tjr 0 ou 1
            if(spins[i] == 0)
                spins[i] = -1; //on convertit les zéro en -1
            sum += seekNeighbours(i);
        }
        Z += sum * exp(-Boltz*doH());
    }
    for(int i=0; i<N; i++)
        spins[i] = save[i]; // on reprend la config originale.
    mvprintw(7, 0, "Normalized Hamiltonian:\t%12lf", Z);//on output ici au lieu de rappeller la fonction comme pour H(C) car
                                                    //soucis de performances
    return Z;
}
