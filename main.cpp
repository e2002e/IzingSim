#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

double doP(), doZ(), doH(), sumP();//Les fonctions servant aux calculs
void *cycle(void*);//ma fonction principal qui devrais plutôt s'appeller "doCalc" ou un truc ds le genre
int N;//la variable qui sert a determiner le nombre de spins
double T, h, J;
int *spins;
double Beta;
bool longrange;
int circular;

int main(int argc, char *argv[]){
    srand(time(NULL));// random generator init
    //init variables
    longrange = false;
    h = 0.0;
    J = 1.0;
    Beta = 1.0;
    circular = 0;
    char chaining[13] = {"strict chain"};

    N = atoi(argv[1]);//N est le premier argument du programme, segfault si omit
    spins = new int[N];
    for(int i=0; i<N; i++)
    {
        spins[i] = 1;//juste une initialisation random a la con  
        if(spins[i] == 0)
            spins[i] = -1;
        printf("%d ", spins[i]);
    }
    spins[1] = -1;
    //Le module de menu, à dévelloper
    char c[10];
    while(1)
    {
        printf( "\n1)calculate\t\t4)longrange %d\n2)Beta:\t%lf\t5)chaining method: %s\n"\
                "3)magnitude:%lf\t6)exit\nparamagnitude:%lf\n", longrange, Beta, chaining, J, h);
        scanf("%s", c);
        if(atoi(c) < 1 || atoi(c) > 6) continue;
        switch(atoi(c)){
            case 1:
                cycle(NULL);
                break;
            case 2:
                printf("Enter value for Beta\n");
                scanf("%s", c);
                Beta = atof(c);
                break;
            case 4:
                if(longrange)
                    longrange = 0;
                else longrange = 1;
                break;
            case 5:
                if(circular)
                {   circular = 0;
                    strcpy(chaining, "strict chain");
                }
                else
                {   circular = 1;
                    strcpy(chaining, "ring");
                }
                break;
            case 6:
                exit(0);
            default:
                continue;
        }
    }
}
void *cycle(void*)
{
    double P = doP();//doP() appelle les autres fonctions
    printf("Hamiltonian(C):\t\t%lf\nNormalized H:\t\t%lf\nProba(C):\t\t%lf\n",doH(), doZ(), P);
    printf("Sum of all P:%lf\n", sumP());
    return NULL;
}
double seekNeighbours(int i)
{
    double sum;
    int post = N - i;    //i est l'index du spin, N - i donnes les spins après donc variable nommée 'post'
    int pre  = N - post; //même logique, mais avec les spins d'avant.
    for(int j=0; j<pre; j++){
        sum += (double)spins[i]*((double)spins[j]/pow((double)pre+j, 3.0));
        if(circular)
            if(post<N/2)
                sum += (double)spins[i]*((double)spins[j]/pow((double)post+j, 3.0));
    }
    for(int j=N; j>post; j--){
        sum += (double)spins[i]*((double)spins[j]/pow((double)j-post, 3.0));
        if(circular)
            if(pre<N/2)
                sum += (double)spins[i]*((double)spins[j]/pow((double)pre-j, 3.0));
    }
    //ces deux boucles additionnent le spin actuel avec ses voisins (d'avant et d'après) en multipliant ces derniers par 1/r^3
    return sum;
}

double doP()
{
    double P = (1/doZ()) * exp(-Beta*doH()); //la formule est plutôt explicite.
    return P; 
}

double doH()
{
    double H = 0;
    for(int i=0; i<N; i++){
        if(longrange)
            H += -seekNeighbours(i)*J - (double)spins[i]*h;//moin les voisins * J - le spin * h
        else
        {
            if(circular)
            {
                if(i < N-1)
                    H += -((double)spins[i]*spins[i+1])*J - (double) spins[i]*h;
                else if(i == N-1)
                    H += -((double)spins[i]*spins[0])*J - (double) spins[i]*h;
            }
            else
            {   
                if(i < N-1)
                    H += -((double)spins[i]*spins[i+1])*J - (double) spins[i]*h;
            }
        }
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
        for(int i=0;i<N;i++)
        {
            if(i==0)
                spins[i] = a & 1;   //un AND (binaire) avec 2^i montre si le bit(mapé sur l'index
                                    //du tableau) est utilisé, suivit d'un shift i pour avoir tjr 0 ou 1
            else
                spins[i] = (a & (long int)pow(2.0, (double)i)) >> i;
            if(spins[i] == 0)
                spins[i] = -1; //on convertit les zéro en -1
        }
        Z += exp(-Beta*doH());
    }
    
    for(int i=0; i<N; i++)
        spins[i] = save[i]; // on reprend la config originale.
    return Z;
}
double sumP()
{
    double sum;
    long int value = (long int) pow(2.0, (double)N);
    int *save = new int[N];
    for(int i=0; i<N; i++)
        save[i] = spins[i]; //je sauvegarde la config car elle va être remplacée
    for(long int a = 0; a < value; a++)
    {
        for(int i=0;i<N;i++)
        {
            if(i==0)
                spins[i] = a & 1;   //un AND (binaire) avec 2^i montre si le bit(mapé sur l'index
                                    //du tableau) est utilisé, suivit d'un shift i pour avoir tjr 0 ou 1
            else
                spins[i] = (a & (long int)pow(2.0, (double)i)) >> i;
            if(spins[i] == 0)
                spins[i] = -1; //on convertit les zéro en -1
        }
        sum += doP();// à ce moment la on fait P pour chaques config ce qui indui une loop sur chaques config suplémentaire 
                    // pour trouver Z dans doZ(), ça peut être lourd avec ne serait-ce qu'une dixaine de spins.
    }
    for(int i=0; i<N; i++)
        spins[i] = save[i];
    return sum;
}
