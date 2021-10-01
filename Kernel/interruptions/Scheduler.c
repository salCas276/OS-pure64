#include <Scheduler.h>
#include "../include/ListFreeMemoryManager.h"

#define SIZE 20
static int currentIndex = 0 ; 
static int ProcessTotal = 0;

//active process list.
static processControlBlock * ActiveProcess[SIZE];

uint64_t _buildContext(uint64_t baseRSP, uint64_t functionAddress);
void InitFirstProcess();


//call when scheduler executes. Selects next Process.
void timer_handler(void){
	currentIndex = (currentIndex + 1) % ProcessTotal;
}

//First process created by the kernel.
void FirstProcess(uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt) {
    addProcess(functionAddress,baseRSP,prompt); // add to be scheduled
    InitFirstProcess();
}




//adding a process to be scheduled.
void addProcess( uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt){

	processControlBlock * task= malloc(sizeof(processControlBlock));
    task->pid=ProcessTotal;
    task->prompt = prompt;
    task->baseRSP = baseRSP;
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(baseRSP, functionAddress);

	ActiveProcess[ProcessTotal]=task;
	ProcessTotal++;
}


void removeProcess(int pid){
    return;
}


void setCurrentRSP(uint64_t rsp) {
    ActiveProcess[currentIndex]->taskRSP = rsp;
}

uint64_t getCurrentRSP(){
    return ActiveProcess[currentIndex]->taskRSP;
}


prompt_info * getCurrentPrompt() {
    return &ActiveProcess[currentIndex]->prompt;
}