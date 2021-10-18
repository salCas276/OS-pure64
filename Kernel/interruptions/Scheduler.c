#include <Scheduler.h>
#include "../include/memoryManager.h"




uint64_t _buildContext(uint64_t baseRSP, uint64_t functionAddress);
void InitFirstProcess();


//call when scheduler executes. Selects next Process.
void timer_handler(void){
    nextTask();
}

//First process created by the kernel.
void FirstProcess(uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt) {

    processControlBlock * task= malloc(sizeof(processControlBlock));
    task->pid=0;
    task->prompt = prompt;
    task->baseRSP = baseRSP;
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(baseRSP, functionAddress);

    addProcess(task); 
    InitFirstProcess();
}








