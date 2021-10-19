
#include <roundRobin.h>
#include "../include/naiveConsole.h"
#include "../include/memoryManager.h"

#define SIZE 20
// static processControlBlock * ActiveProcess[SIZE];
static int currentIndex = 0 ; // idx in the list 
static int processTotal = 0;
static processControlBlock * currentProcess = (processControlBlock *)(0); 
static processControlBlock * header = (processControlBlock *)(0); 


static processControlBlock * blockHeaders[MAXBLOCKTYPES]; 


static processControlBlock * unlinkProcess(processControlBlock * process, int pid, int * flags, processControlBlock ** p) ; 
static void pushProcess( processControlBlock ** header, processControlBlock * process);

// Debugging
static void printChain(processControlBlock * c) {
   
   if ( c ==0 ) {
       ncPrint("[EOP]\n"); 
       return; 
   } 

   char str[] = {2, 0}; 
   str[0] = c->pid + '0'; 
   ncPrint(str); 
   ncPrint("->"); 
   printChain(c->tail); 
}


// El agregado de procesos se hace al estilo stack, se lo pushea en una lista simplemente encadenada de procesos 
// En la iteracion de la lista, si llegue al final, vuelvo a empezar (RR)
// Agregar un proceso no cambia el proceso que actualmente se esta ejecutando 
void addProcess(processControlBlock * process){

    // No hay procesos 
    if ( header == 0 ) {
        header = process; 
        header->tail = (processControlBlock *)(0); 
        currentProcess = process; 
        printChain(header); 
        return; 
    }

    // push 
    pushProcess(&header, process);
	processTotal++;
    printChain(header); 

}

static void pushProcess( processControlBlock ** header, processControlBlock * process) {
    process->tail = (*header); 
    (*header) = process; 
}


static processControlBlock * unlinkProcess(processControlBlock * process, int pid, int * flags, processControlBlock ** p) {

    if (process == 0) {
        *flags = 1; 
        return process; 
    }

    if ( process->pid == pid) {
        processControlBlock * aux = process->tail;
        *p = process;  
        return aux; 
    }

    process->tail = unlinkProcess(process->tail, pid, flags, p); 
    return process; 
}


int killProcess(int pid) {
    int flags = 0;
    processControlBlock * p = 0;  
    header = unlinkProcess(header, pid, &flags, &p);
    if (!flags) {
        free(p); // Efectivamente lo borra
        if (!flags) processTotal --; 
    }
    return flags; 
}


void nextTask(){
    //FOR DEBUGGING CHAMPAGNE
    // char str[] = {'0', 0}; 
    // ncPrint("CS "); 
    // str[0] = '0'+currentProcess->pid;
    // ncPrint(str); 
    // ncPrint("("); 
    // str[0] = '0' + currentProcess->priority; 
    // ncPrint(str); 
    // ncPrint(")->"); 

    if ( currentProcess->currentPushes < WORSTPRIORITY + 1 - currentProcess->priority ) {
        currentProcess->currentPushes ++; 

    } else {
        currentProcess->currentPushes = 0; 
        currentProcess = ( currentProcess->tail == (processControlBlock *)(0) ? header : currentProcess->tail); 
    } 
//     str[0] = '0'+ currentProcess->pid;
//     ncPrint(str); 
//     ncPrint("\n"); 
}

processControlBlock * getCurrentTask(){
    return currentProcess;
}


void setCurrentRSP(uint64_t rsp) {
    getCurrentTask()->taskRSP = rsp;
}

uint64_t getCurrentRSP(){
    return getCurrentTask()->taskRSP;
}

prompt_info * getCurrentPrompt() {
    return &(getCurrentTask()->prompt);
}

int getCurrentPid(){
    return currentProcess->pid;
}

// Se podria mejorar. Conformarse con O(N) y despues lo mejoramos si alcanza el tiempo. 
static processControlBlock * getProcessFromPID(int pid) {

    processControlBlock * c = header; 
    while ( c!=0 && c->pid != pid ) c = c->tail; 
    return c; 
    
}

int changeNicenessBy(uint64_t pid, uint64_t deltaNice) {

    processControlBlock * p = getProcessFromPID(pid); 

    if ( p == 0) return 1; 


    // ncPrint("Vieja prioridad era: ");
    // char c[] = { ActiveProcess[pid]->priority+'0', 0}; 
    // ncPrint(c);
    // ncPrint("\n");   


    int aux = p->priority+deltaNice; 

    //Si no anda sacar esto     
    if ( aux > WORSTPRIORITY ) aux = WORSTPRIORITY; 
    else if ( aux < 0 ) aux = 0; 

    p->priority = aux; 

    // ncPrint("Nueva prioridad es: ");
    // char d[] = { ActiveProcess[pid]->priority+'0', 0}; 
    // ncPrint(d);
    // ncPrint("\n");   

    // ActiveProcess[pid]->currentPushes = 0; // Podria traer inanicion. Me hago un deltaNice = 0 a mi mismo siempre y me los chicho a todos 
    return 0; 
}