#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <cstring>
#include <ncurses.h>
#include <omp.h>

double doP(), doZ(), doH(), sumP();//Les fonctions servant aux calculs
void *cycle(void*), equilibrate(int iter), display();
int N;//la variable représentant le nombre de spins
double T, h, J;
int *spins;
char **spinstr;
double Beta;
bool longrange;
int circular;
int D;//dimentions
int spacer;
char *chaining = "strict chain";
bool dosump = 0;

void placeSpins() {
    if(D == 1) {
		for(int i=0; i<N; ++i) {
			spins[i] = atoi(spinstr[i]);
			if(spins[i] == 0) spins[i] = -1;
		}
	}
	else if(D == 2) {
		if(((int) sqrt(N) * sqrt(N)) != N) {//without cast to int this is always true.
			mvprintw(spacer+8, 0, "There must be a square number of spins");
			D = 1;
			goto out;
		}		
		for(int i=0; i<sqrt(N); i++) {//rows
			for(int j=0; j<sqrt(N); j++) {//columns
				spins[(int)(j*sqrt(N))+i] = atoi(spinstr[(int)(j*sqrt(N))+i]);
				if(spins[(int)(j*sqrt(N))+i] == 0) spins[(int)(j*sqrt(N))+i] = -1;
			}
	    }
	}
	out:
	clear();
}

int main(int argc, char *argv[]){
    srand(time(NULL));// random generator init
    //omp_set_nested(1);
    //init variables
    longrange = false;
    h = 0.0;
    J = 0.1;
    Beta = 1.0;
    circular = 0;
    D = 1;
    
	
	if((!(N = strlen(argv[1]))) || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		printf("You must pass a string representing the spins as argument:\n"
			   "  ./IzingSim 1111100101101001\n"
			   "  "
			   );
		exit(-1);
	}
    spins = new int[N];
    spinstr = new char*[N];
    
    for(int a=0; a<N; ++a) {
    	spinstr[a] = new char[2];
    	spinstr[a][0] = argv[1][a];
    	spinstr[a][1] = '\0';
    }
    placeSpins();
	initscr();
	cbreak();
    while(1)
    {
    	noecho();
    	
		display();
		
        refresh();
        
        int c = getch();
      	echo();
      	char str[64];
      	
        if(c < 49 || c > 57) continue;
        switch(c){
            case 49:
            	if(dosump) dosump = false;
            	else dosump = true;
            	break;
            case 50:
                if(longrange)
                    longrange = false;
                else longrange = true;
                break;
            case 51:
                mvprintw(spacer+8, 0, "Enter value for Beta: ");
                getstr(str);
                Beta = atof(str);
                break;
            case 52:
                if(circular){   
                	circular = 0;
                    strcpy(chaining, "strict chain");
                }
                else {   
                	circular = 1;
                    strcpy(chaining, "ring");
                }
            case 53:
                mvprintw(spacer+8, 0, "Enter the magnitude: ");
            	getstr(str);
            	J = atof(str);
				break;
            case 54:
            	mvprintw(spacer+8, 0, "Enter the number of dimensions of the model: ");
            	do {
            	getstr(str);
            	D = atoi(str);
            	} while(D < 1 || D > 2);
            	placeSpins();
            	break;
            case 55:
                break;
            	mvprintw(spacer+8, 0, "Enter the paramagnitude: ");
            	getstr(str);
           		h = atof(str);
            	break;
            case 56:
            	mvprintw(spacer+8, 0, "Enter the number of rounds to minimize temperature: ");
            	do
            		getstr(str);
            	while(!atoi(str));
            	equilibrate(atoi(str));
            	break;
            case 57:
            	endwin();
                exit(0);
            default:
                continue;
        }
        clear();
    }
}

double seekNeighbours(int i)
{
    double sum;
    int post = N - i;    //i est l'index du spin, N - i donnes les spins après donc variable nommée 'post'
    int pre  = N - post; //même logique, mais avec les spins d'avant.
    /*
    for(int j=0; j<pre; j++){
        sum += (double)spins[i]*((double)spins[j]/pow((double)pre+1-j, 3.0));
        if(circular)
            if(post<N/2)
                sum += (double)spins[i]*((double)spins[j]/pow((double)post+j, 3.0));
    }
    for(int j=N; j>post; j--){
        sum += (double)spins[i]*((double)spins[j]/pow((double)j-post, 3.0));
        if(circular)
            if(pre<N/2)
                sum += (double)spins[i]*((double)spins[j]/pow((double)j-pre, 3.0));
    }
    */
    for(int j = 0; j < N; ++j) {
        if(j<i) {
            sum += (double)spins[i] * ( (double)spins[j]/pow((double)i-j, 3.0));
            if(circular) {
                if(i > 0 && N-i+j < N)
                    sum += (double)spins[i] * ( (double)spins[j]/pow((double)N+j-i, 3.0));
            }
        }
        else if(j>i) {
            sum += (double)spins[i] * ( (double)spins[j]/pow((double)j-i, 3.0));
            if(circular) {
                if(i < N-1 && N-j+i < N)
                    sum += (double)spins[i] * ( (double)spins[j]/pow((double)N-j+i, 3.0));
            }
        }
    }
    //ces deux boucles additionnent le spin actuel avec ses voisins (d'avant et d'après) en multipliant ces derniers par 1/r^3
    return sum;
}

double doP() {
    double P = (1/doZ()) * exp(-Beta*doH());
    return P; 
}

void equilibrate(int iter) {
	display();
	sleep(1);
	for(int i=0; i<iter; ++i) {
		int eq = -1;
		double last = doH();
		double tests[N];
		
		if(last == 0.0f) break;
		
		for(int n=0; n<N; ++n) {//on testes chaques changement de spins, si la valeur se rapproche de 0, on la sauvegarde dans tests[]
			if(spins[n] == -1) spins[n] = 1;
			else spins[n] = -1;
			
			double now = doH();
			
			if(spins[n] == -1) spins[n] = 1;
			else spins[n] = -1;
			
			if(last < 0) {
				if(last + now > last && last + now < 0) {
					tests[n] = last + now;
				}
				else tests[n] = 0.0;
			}
			else if (last > 0) {
				if(last + now < last && last + now > 0) {
					tests[n] = last + now;
				}
				else tests[n] = 0.0;
			}
			else if(last + now == 0.0f) {// the configuration is stable
				eq = n;
				goto end;
			}
		}
		for(int n=0; n<N; ++n) {
			if(tests[n] == 0) continue;
			for(int n2=0; n2<N; ++n2) {
				if(tests[n2] == 0) continue;
				if(last < 0) {
					if(tests[n] >= tests[n2]) {
						eq = n;
					}
				}
				else if(last > 0) {
					if(tests[n] <= tests[n2]) {
						eq = n;
					}
				}
			}
		}
		end:
		if(eq != -1) {
			if(spins[eq] == -1) spins[eq] = 1;
			else spins[eq] = -1;
			display();
			sleep(1);
		} 
	}
}

void display() {
	if(D == 1) {
		for(int i=0; i<N; ++i) {
			if(spins[i] == -1) spins[i] = 0;
			mvprintw(0, i, "%d ", spins[i]);
			if(spins[i] == 0) spins[i] = -1;
		}
		spacer = 1;
	}
	else if(D == 2) {	
		for(int i=0; i<sqrt(N); i++) {//rows
			for(int j=0; j<sqrt(N); j++) {//columns
				if(spins[(int)(j*sqrt(N))+i] == -1) spins[(int)(j*sqrt(N))+i] = 0;
				mvprintw((int)sqrt(N)-j-1, i*2, "%d ", spins[(int)(j*sqrt(N))+i]);
				if(spins[(int)(j*sqrt(N))+i] == 0) spins[(int)(j*sqrt(N))+i] = -1;
			}
	    }
	    spacer = sqrt(N);
	}
	
	mvprintw(spacer,	0, 	"1)sum of P: \t%d\t\t2)longrange:\t\t%d", dosump, longrange);
    mvprintw(spacer+1,	0, 	"3)beta:\t\t%lf\t4)chaining method:\t%s", Beta, chaining);
    mvprintw(spacer+2,	0, 	"5)magnitude:\t%lf\t6)dimension:\t\t%d", J, D);
    mvprintw(spacer+3,	0, 	"7)paramagnitude:%lf\t8)equilibrate", h);
    mvprintw(spacer+4,	0,	"9)exit");
	mvprintw(spacer+5,	0, "Hamiltonian(C):\t\t%lf", doH());
    mvprintw(spacer+6,	0, "Normalized H:\t\t%lf", doZ());
    mvprintw(spacer+7,	0, "Proba(C):\t\t%lf", doP());
    if(dosump)
   		mvprintw(spacer+8,	0, "Sum of Probas: \t%f", sumP());
	refresh();
}

double doH()
{
	double H = 0;

	switch(D) {
	case 1:
		for(int i=0; i<N; ++i){
			if(longrange)
				H += -seekNeighbours(i)*J - (double)spins[i]*h;//moin les voisins * J - le spin * h
			else {
				if(circular) {
				    if(i < N-1)
				        H += -((double)spins[i]*spins[i+1])*J - (double) spins[i]*h;
				    else if(i == N-1)
				        H += -((double)spins[i]*spins[0])*J - (double) spins[i]*h;
				}
				else {   
			    	if(i < N-1)
						H += -((double)spins[i]*spins[i+1])*J - (double) spins[i]*h;
					
				}
			}
		}
		break;
	case 2:
		for(int i=0; i<sqrt(N); i++) {//rows
			for(int j=0; j<sqrt(N); j++) {//columns
				if(spins[(int)(j*sqrt(N))+i] == -1) spins[(int)(j*sqrt(N))+i] = 0;
				if(i < N-1)
					H += -((double)spins[(int)(j*sqrt(N))+i]*spins[(int)(j*sqrt(N))+i+1])*J - (double) spins[(int)(j*sqrt(N))+i]*h;
				if(i > 0)
					H += -((double)spins[(int)(j*sqrt(N))+i]*spins[(int)(j*sqrt(N))+i-1])*J - (double) spins[(int)(j*sqrt(N))+i]*h;//row
				if((int)(j*sqrt(N))+i < sqrt(N)-1 * sqrt(N))
					H += -((double)spins[(int)(j*sqrt(N))+i]*spins[(int)(j*sqrt(N))+i+(int)sqrt(N)])*J - (double) spins[(int)(j*sqrt(N))+i]*h;//column
				if((int)(j*sqrt(N))+i > sqrt(N))
					H += -((double)spins[(int)(j*sqrt(N))+i]*spins[(int)(j*sqrt(N))+i-(int)sqrt(N)])*J - (double) spins[(int)(j*sqrt(N))+i]*h;
				break;
			}
		}
		break;	
	}
	return H;
}
double doZ() 
{
    long int value = (long int) pow(2.0, (double)N);// Ce nombre sert de map, toutes les config sont testées grâce à lui 
    double Z = 0;
    int *save = new int[N];
    for(int i=0; i<N; i++)
        save[i] = spins[i];// je sauvegarde la config car elle va être remplacée
 	int mt = omp_get_max_threads();
 
	//for(int t=0; t<mt; ++t) {
	//#pragma omp parallel firstprivate(spins) num_threads(mt)
	//{
		for(long int a=0/*value*((double)t/mt)*/; a<value/*/(mt/((double)t+1))*/; ++a) {
			for(int i=0; i<N; ++i) {
				if(i == 0) {
		    		spins[0] = a & 1;
		    	}
		    	else {
				    spins[i] = (a & (long int)pow(2.0, (double)i)) >> i;
				}
				if(spins[i] == 0) spins[i] = -1;
			}
	//		#pragma omp critical
			Z += exp(-Beta*doH()); 
	//	}
	}
	//}
    for(int i=0; i<N; i++)
        spins[i] = save[i];
    return Z;
}
double sumP()
{
    double sum = 0;
    long int value = (long int) pow(2.0, (double)N);
    int *save = new int[N];
    for(int i=0; i<N; i++)
        save[i] = spins[i];
 	
    for(long int a = 0; a < value; a++){
        for(int i=0; i<N; ++i) {
			if(i == 0) {
	    		spins[0] = a & 1;
	    	}
	    	else {
			    spins[i] = (a & (long int)pow(2.0, (double)i)) >> i;
			}
			if(spins[i] == 0) spins[i] = -1;
		}
    	sum += doP();
    }
    for(int i=0; i<N; i++)
        spins[i] = save[i];
    return sum;
}
