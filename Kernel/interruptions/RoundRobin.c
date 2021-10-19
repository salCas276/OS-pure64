
#include <roundRobin.h>

#define SIZE 20
static processControlBlock * ActiveProcess[SIZE];
static int currentIndex = 0 ; 
static int ProcessTotal = 0;


void addProcess(processControlBlock * process){
    ActiveProcess[ProcessTotal]=process;
	ProcessTotal++;
}


void nextTask(){
    if ( ActiveProcess[currentIndex]->currentPushes < WORSTPRIORITY + 1 - ActiveProcess[currentIndex]->priority ) 
        ActiveProcess[currentIndex]->currentPushes ++; 
    else {
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