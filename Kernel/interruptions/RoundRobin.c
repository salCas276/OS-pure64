
#include <roundRobin.h>
#include "../include/naiveConsole.h"

#define SIZE 20
static processControlBlock * ActiveProcess[SIZE];
static int currentIndex = 0 ; 
static int ProcessTotal = 0;


void addProcess(processControlBlock * process){
    ActiveProcess[ProcessTotal]=process;
    // char c[] = { 'p', 'r', 'o', 'c', 'e', 's', 's', 'P', 'R', ':', ' ', '0'+process->priority, '\n', 0}; 
    //ncPrint(c);
	ProcessTotal++;
}


void nextTask(){
    if ( ActiveProcess[currentIndex]->currentPushes < WORSTPRIORITY + 1 - ActiveProcess[currentIndex]->priority ) {
        ActiveProcess[currentIndex]->currentPushes ++; 

    } else {
        ActiveProcess[currentIndex]->currentPushes = 0; 
        currentIndex = (currentIndex + 1) % ProcessTotal;
    } 
}

processControlBlock * getCurrentTask(){
    return ActiveProcess[currentIndex];
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
    return ActiveProcess[currentIndex]->pid;
}

int changeNicenessBy(uint64_t pid, uint64_t deltaNice) {

    if (ActiveProcess[pid] == (processControlBlock *)0) return 1; 


    // ncPrint("Vieja prioridad era: ");
    // char c[] = { ActiveProcess[pid]->priority+'0', 0}; 
    // ncPrint(c);
    // ncPrint("\n");   


    int aux = ActiveProcess[pid]->priority+deltaNice; 

    // Si no anda sacar esto     
    if ( aux > WORSTPRIORITY ) aux = WORSTPRIORITY; 
    else if ( aux < 0 ) aux = 0; 

    ActiveProcess[pid]->priority = aux; 

    // ncPrint("Nueva prioridad es: ");
    // char d[] = { ActiveProcess[pid]->priority+'0', 0}; 
    // ncPrint(d);
    // ncPrint("\n");   

    // ActiveProcess[pid]->currentPushes = 0; // Podria traer inanicion. Me hago un deltaNice = 0 a mi mismo siempre y me los chicho a todos 
    return 0; 
}