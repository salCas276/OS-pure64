#include <Process.h>


static prompt_info Prompt;
static uint64_t pid = 1;



uint64_t _buildContext(uint64_t baseRSP, uint64_t functionAddress);
void InitFirstProcess();




//First process created by the kernel.
void firstProcess(uint64_t functionAddress, prompt_info prompt) {

    uint64_t * basePointer = malloc(4096 * sizeof(uint64_t));

    Prompt = prompt;
    processControlBlock * task= malloc(sizeof(processControlBlock));
    task->pid=0;
    task->prompt = Prompt;
    task->baseRSP =(uint64_t)&basePointer[4095] ;
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress);

    addProcess(task); 
    InitFirstProcess();
}

uint64_t createProcess(uint64_t functionAddress){
    uint64_t * basePointer = malloc(4096 * sizeof(uint64_t));

    processControlBlock * task= malloc(sizeof(processControlBlock));
    task->pid=pid;
    pid++;
    //if(foreground)
        task->prompt = Prompt;
    
    task->baseRSP = (uint64_t)&basePointer[4095] ; 
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress);

    addProcess(task); 

    return pid - 1 ;  

}
