#include <Process.h>


static prompt_info Prompt;



int64_t functionAddress;
void InitFirstProcess();


#define MAX_PIDS 20
static int lastPid = 0;
static int freePidsCounter = MAX_PIDS;
processControlBlock * allProcesses[MAX_PIDS]; 

static int generateNextPid();

//First process created by the kernel.
void firstProcess(uint64_t functionAddress, prompt_info prompt) {

    uint64_t * basePointer = malloc(4096 * sizeof(uint64_t));

    Prompt = prompt;
    processControlBlock * task= malloc(sizeof(processControlBlock));
    freePidsCounter--;
    task->pid=0;
    task->prompt = Prompt;
    task->baseRSP =(uint64_t)&basePointer[4095] ;
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress);
    allProcesses[task->pid] = task;

    addProcess(task); 
    InitFirstProcess();
}

uint64_t createProcess(uint64_t functionAddress){
    uint64_t * basePointer = malloc(4096 * sizeof(uint64_t));

    processControlBlock * task= malloc(sizeof(processControlBlock));
    task->pid=generateNextPid();
    //if(foreground)
        task->prompt = Prompt;
    
    task->baseRSP = (uint64_t)&basePointer[4095] ; 
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress);
    allProcesses[task->pid] = task;

    addProcess(task); 

    return task->pid;  
}

//Carga un conjunto de datos predefinidos (mirar la struct processDescriptor) en la direccion indicada
//Retorna la cantidad de procesos listados
int getProcessesData(uint64_t descriptorArray){
    int j = 0;
    if(freePidsCounter==MAX_PIDS) return j;
    processDescriptor * descriptorArrayAux = (processDescriptor *) descriptorArray;
    for(int i=0; i<MAX_PIDS; i++){
        if(allProcesses[i]){
            (descriptorArrayAux+j)->pid=allProcesses[i]->pid;
            j++;
        }
    }
    return j;
}

//Busca ciclicamente el proximo (partiendo del ultimo) pid disponible
//Si no hay pids disponibles retorna -1
static int generateNextPid(){
    if(!freePidsCounter) return -1;
    freePidsCounter--;
    while(allProcesses[lastPid])
        lastPid = (lastPid+1)%MAX_PIDS;
    return lastPid;
}
