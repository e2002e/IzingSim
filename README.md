# IzingSim

Model of Izing computations test.  

COMPILE:  
make

USAGE:  
./izing <string representing the spins>  
./izing 100110001  

From then you have an interactive menu thanks to ncurses, which makes it a linux exclusive program.  
The code is verified and complete in dimention 1, but don't use ring and longrange in dimension 2 as it will yield false results.  

WARNING, calculating Z will be heavy with more than 16 spins and does so automatically when starting the program.  
Calculating the Sum of probabilities will recalculate Z for each config...  
The equilibrate function seeks for values of 0, but is not to be taken as a valid operation.  



