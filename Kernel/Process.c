#include <Process.h>


static prompt_info Prompt;
static uint64_t pid = 1;



int64_t functionAddress;
void InitFirstProcess();



 #define SIZE 50
 processControlBlock * allProcesses[SIZE]; 

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
    allProcesses[pid-1] = task;

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
    allProcesses[pid-1] = task;

    addProcess(task); 

    return pid - 1 ;  

}

//Carga un conjunto de datos predefinidos (mirar la struct processDescriptor) en la direccion indicada
//Retorna la cantidad de procesos listados
int getProcessesData(uint64_t descriptorArray){
    int i = 0;
    processDescriptor * descriptorArrayAux = (processDescriptor *) descriptorArray;
    while(allProcesses[i]){
        (descriptorArrayAux+i)->pid=allProcesses[i]->pid;
        i++;
    }
    return i;
}
