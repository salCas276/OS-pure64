// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <phylo.h>
#include <testSem.h> 
#include <lib.h>
#include <processApi.h>
#include <utils.h>

int qPhylos = 0; 
char * mutex = "MUTEXPHYLO";
char * sems[MAXPHYLO]; 
dateType * randomizer; 
int phyloPids[MAXPHYLO]; 

typedef enum{
    hungry, 
    eating, 
    thinking
} state;

state states[MAXPHYLO];
 

#define LEFT (phnum + qPhylos - 1) % qPhylos
#define RIGHT (phnum + 1) % qPhylos

void testito();
void printPhyloArray();

void test(int phnum) {
    if (states[phnum] == hungry && states[LEFT] != eating && states[RIGHT] != eating) {
        // state that eating
        states[phnum] = eating;
       postSemaphore(sems[phnum]);
    }
}
 
// take up chopsticks
void take_fork(int phnum)
{
    waitSemaphore("MUTEXPHYLO");

 
    // state that hungry
    states[phnum] = hungry;
 
    test(phnum);
 
    postSemaphore("MUTEXPHYLO");
 
    waitSemaphore(sems[phnum]);

    
 
}
 
// put down chopsticks
void put_fork(int phnum)
{

    waitSemaphore("MUTEXPHYLO");
 
    // state that thinking
    states[phnum] = thinking;
 
    test(LEFT);
    test(RIGHT);
 
    postSemaphore("MUTEXPHYLO");
}


void think(int n) {
    for (int i=0; i<n; i++){
        renounceUserland();
    }
}

static void lifeOfAPhylo(int argc, char * argv[], int foreground) {
    int i = argv[0][1]-'0';
    while (1) {
        if (i>=qPhylos) kill(0, phyloPids[i]); 
        fillDate(randomizer); 
        think( randomizer->second % ( i + 2) ); 
        take_fork(i);
        put_fork(i);
    }
    print_f(1, "Phylo %d se levanto de la mesa!\n", i); 
    exitUserland(); 
}

void addPhylo() {
    print_f(1, "ADDING PHYLO ! \n"); 
    if ( qPhylos < MAXPHYLO ){
        phyloPids[qPhylos]=createProcessUserland( (uint64_t) & lifeOfAPhylo, 1 , &sems[qPhylos] , 0 );
        qPhylos++;
    }

}

void rmvPhylo() {
    print_f(1, "REMOVING PHYLO ! \n"); 
 
     if ( qPhylos > 0) --qPhylos; 
}


static char buffer[3] = {0}; 

void phyloKeyboard( int argc, char * argv, int foreground) {
    do {
        read(-1, 0, buffer, -1); 
        if ( buffer[0] == 'a') addPhylo(); 
        else if (  buffer[0] =='r') rmvPhylo(); 
    } while( buffer[0] != '1');

     for (int i=0; i<qPhylos; i++)
        kill(0, phyloPids[i]);
    
    exitUserland(); 
}

void printPhyloArray(){
    for(int i=0; i<qPhylos; i++)
        print_f(1, " %s ", (states[i] == eating ?  "E": "-")); 
    print_f(1, "\n"); 
}


void phylo(int argc, char * argv[], int foreground) {


    buffer[0] = 0; 
    openSemaphore("MUTEXPHYLO", 1);
    for ( int i = 0; i < MAXPHYLO; i++) {
        sems[i] = memalloc(3);
        sems[i][0] = 'P'; 
        sems[i][1] = '0'+i;  
        sems[i][2] = 0; 
        if (  openSemaphore(sems[i], 0) < 0 )
            print_f(1, "EL SEMAFOROOOOO\n");
    }
    
    randomizer = memalloc(sizeof(dateType)); 
    for (int i=0; i<MAXPHYLO; i++)
        phyloPids[i] = -1; 

    // Creation of phylos
    for (int i = 0; i < MAXPHYLO; i++) {
        addPhylo();
        print_f(1, "phylo %d is alive!\n", phyloPids[i]);
    }

    qPhylos = MAXPHYLO; 
    
    char * kybprocess= "PhyloKeyboard"; 
   createProcessUserland((uint64_t) &phyloKeyboard, 1, &kybprocess,  0);

    print_f(1, "El problema de los filosofos\n");
    print_f(1, "Ingrese a para agregar phylo, r para quitar, o 1 para salir SEGUIDO DE ENTER\n"); 

    while( buffer[0] != '1' ) {
        if (qPhylos > 0) printPhyloArray(); 
    }

    print_f(1, "------------------------------\n");

    
    for(int i = 0; i < MAXPHYLO; i++){
        closeSemaphore(sems[i]);
        memfree(sems[i]);
    }
    memfree(randomizer); 

    closeSemaphore("MUTEXPHYLO");

}

