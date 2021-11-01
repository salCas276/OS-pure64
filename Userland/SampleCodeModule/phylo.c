#include <phylo.h>
#include <testSem.h> 
#include <lib.h>
#include <processApi.h>


static int initialized = 0; 
int qPhylos = MINPHYLO; 
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


void test(int phnum) {
    if (states[phnum] == hungry && states[LEFT] != eating && states[RIGHT] != eating) {
        // state that eating
        states[phnum] = eating;

        // for (int i=0; i<4000000; i++); 
  
        // printf("Philosopher %d takes fork %d and %d\n",
        //               phnum + 1, LEFT + 1, phnum + 1);
 
        // printf("Philosopher %d is Eating\n", phnum + 1);
 
        // sem_post(&S[phnum]) has no effect
        // during takefork
        // used to wake up hungry philosophers
        // during putfork
        postSemaphore(sems[phnum]);
    }
}
 
// take up chopsticks
void take_fork(int phnum)
{
 
    waitSemaphore("MUTEXPHYLO");

 
    // state that hungry
    states[phnum] = hungry;
 
    // printf("Philosopher %d is Hungry\n", phnum + 1);
 
    // eat if neighbours are not eating
    test(phnum);
 
    postSemaphore("MUTEXPHYLO");
 
    // if unable to eat wait to be signalled
    waitSemaphore(sems[phnum]);

    // for (int i=0; i<4000000; i++); 

 
}
 
// put down chopsticks
void put_fork(int phnum)
{
 
    waitSemaphore("MUTEXPHYLO");
 
    // state that thinking
    states[phnum] = thinking;
 
    // printf("Philosopher %d putting fork %d and %d down\n",
    //        phnum + 1, LEFT + 1, phnum + 1);
    // printf("Philosopher %d is thinking\n", phnum + 1);
 
    test(LEFT);
    test(RIGHT);
 
    postSemaphore("MUTEXPHYLO");
}


void think(int n) {
    for (int i=0; i<1000000*n; i++); 
}

static void lifeOfAPhylo(int argc, char * argv[], int foreground) {
    int i = argv[0][1]-'0';
    while (1) {
        if ( i >= qPhylos ) kill(0, getPid()); 
        fillDate(randomizer); 
        think( randomizer->second % ( i + 2) ); // They think more as their phmun auments
        take_fork(i);
        put_fork(i);
    }
}

void addPhylo() {
    
    if ( qPhylos < MAXPHYLO ) {
        phyloPids[qPhylos] = createProcessUserland((uint64_t) &lifeOfAPhylo, 1, &sems[qPhylos], 0);
        qPhylos ++; 
    }
    
}

void rmvPhylo() {
    qPhylos -= ( MINPHYLO < qPhylos); 
}


static char buffer[3]; 
void phyloKeyboard( int argc, char * argv, int foreground) {
    
    do {
        read(-1, 0, buffer, -1); 
        if ( buffer[0] == 'a') addPhylo(); 
        else if (  buffer[0] =='r') rmvPhylo(); 
    } while( buffer[0] != '~');

    // for (int i=0; i<qPhylos; i++)
    //     kill(0, phyloPids[i]); 

    for (int i=0; i<qPhylos; i++) 
        kill(0, phyloPids[i]);  
    
    exitUserland(); 
}

void phyloPrinter(int argc, char * argv, int foreground) {
    while( buffer[0] != '~') {
        for (int i=0; i<100000000; i++);
        for(int i=0; i<qPhylos; i++)
            print_f(1, " %s ", (states[i] == eating ?  "E": "-")); 
        print_f(1, "\n"); 
    }
    exitUserland(); 
}

void phyloSetup(int argc, char * argv, int foreground) {

    buffer[0] = 0; 
    // Initialization if needed
    if ( !initialized ) {
        // Initialize semaphores
        openSemaphore("MUTEXPHYLO", 1);
        for ( int i = 0; i < MAXPHYLO; i++) {
            sems[i] = memalloc(3);
            sems[i][0] = 'P'; 
            sems[i][1] = '0'+i;  
            sems[i][2] = 0; 
            openSemaphore(sems[i], 0);
            randomizer = memalloc(sizeof(dateType)); 
        }
        initialized = 1; 
    }

    // Creation of phylo
    for (int i = 0; i < qPhylos; i++) {
        phyloPids[i] = createProcessUserland((uint64_t) &lifeOfAPhylo, 1, &sems[i], 0);
    }

    exitUserland(); 
}


void phylo(int argc, char * argv[], int foreground) {

    char * args[3] = { "PhyloSetup", "PhyloKeyboard", "PhyloPrinter"}; 
    
    createProcessUserland((uint64_t) &phyloSetup, 1, &args[0], 1);
    waitSon();
    createProcessUserland((uint64_t) &phyloPrinter, 1, &args[2], 0);
    createProcessUserland((uint64_t) &phyloKeyboard, 1, &args[1], 1);
    waitSon(); 

}

