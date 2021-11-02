// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include "include/fileSystem.h"
#include "include/string.h"
#include "include/naiveConsole.h"

#define WAITING 2



int64_t functionAddress;
void InitFirstProcess();
uint64_t _buildContext(uint64_t basePointer , uint64_t functionAddress,_ARGUMENTS );

 uint8_t strlen(const char *str);
 char * strcpy(char *strDest, const char *strSrc);



#define MAX_PIDS 20
static int lastPid = 0;
static int freePidsCounter = MAX_PIDS;
processControlBlock * allProcesses[MAX_PIDS]; 


static int generateNextPid();

//First process created by the kernel.
void firstProcess(uint64_t functionAddress, prompt_info shellPrompt , prompt_info backgroundPrompt) {

    uint64_t * basePointer = malloc(4096 * sizeof(uint64_t));
    processControlBlock * task= malloc(sizeof(processControlBlock));

    if(!task || ! basePointer){
        free(task);
        free(basePointer);
        return ; 
    }

    task->name = "Shell";
    freePidsCounter--;
    task->pid=0;
    task->quantityWaiting = 0; 
    task->baseRSP =(uint64_t)&basePointer[4095] ;
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress,0,0);
    task->tail = (processControlBlock *) 0; 
    for(int i=0; i<MAX_PIDS; i++)
        task->processFileDescriptors[i] = i < 3 ? i : -1;

    createFile("keyboard", 0);
    createFile("console", 1);
    openFile(-1, "keyboard", 0);
    openFile(-1, "console", 1);
    openFile(-1, "console", 1);
    unlinkFile("keyboard");
    unlinkFile("console");
    // Processes are created with the worst possible priority.
    task->priority = WORSTPRIORITY; 
    task->currentPushes = 0; 
    
    allProcesses[task->pid] = task;

    setPrompt(shellPrompt,backgroundPrompt);

    addProcess(task);

    InitFirstProcess();
}

int createProcess(uint64_t functionAddress,_ARGUMENTS,int foreground){
    uint64_t * basePointer = (uint64_t*)malloc(4096 * sizeof(uint64_t));
    processControlBlock * task= (processControlBlock*) malloc(sizeof(processControlBlock));
    char * nameAux = malloc(strlen(argv[0])+1);

    if(!task || !basePointer || !nameAux){
        free(task);
        free(basePointer);
        free(nameAux);
        return -1; 
    }
    
    strcpy(nameAux,argv[0]);
    task->name = nameAux;
    task->pid=generateNextPid();
    if(task->pid < 0 ){
        free(basePointer);
        free(task);
        free(nameAux);
        return -1; 
    }

    
    if(foreground)
        task->parentPid = getCurrentPid();
    else 
        task->parentPid = -1; 

    task->quantityWaiting = 0; 
    task->baseRSP = (uint64_t)&basePointer[4095] ; 
    task->functionAddress = functionAddress;
    task->taskRSP = _buildContext(task->baseRSP, functionAddress,argc,argv);
    allProcesses[task->pid] = task;

    // Processes are created with the worst possible priority 
    task->priority = WORSTPRIORITY; 
    task->currentPushes = 0;
    task->tail = (processControlBlock *) 0; 

    for(int i=0; i<MAX_PFD; i++){
        task->processFileDescriptors[i] = task->parentPid >= 0 ? getProcessByPid(task->parentPid)->processFileDescriptors[i] : -1;
        addFd(task->processFileDescriptors[i]);
    }
    
    if(task->parentPid < 0){
        openFile(task->pid, "keyboard", 0);
        openFile(task->pid, "console", 1);
        openFile(task->pid, "console", 1);
    }

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
        if(allProcesses[i] != (void*)0 ){
            (descriptorArrayAux+j)->name=allProcesses[i]->name;
            (descriptorArrayAux+j)->pid=allProcesses[i]->pid;
            (descriptorArrayAux+j)->priority=allProcesses[i]->priority;
            (descriptorArrayAux+j)->stackPointer=allProcesses[i]->taskRSP;
            (descriptorArrayAux+j)->basePointer=allProcesses[i]->baseRSP;
            (descriptorArrayAux+j)->foreground=( !allProcesses[i]->pid || allProcesses[i]->parentPid > 0 );
            j++;
        }
    }
    return j;
}

//Busca ciclicamente el proximo (partiendo del ultimo) pid disponible
//Si no hay pids disponibles retorna -1
static int generateNextPid(){
    lastPid = 0 ; 
    if(!freePidsCounter) return -1;
    freePidsCounter--;
    while(allProcesses[lastPid] != (void*)0 )
        lastPid = (lastPid+1)%MAX_PIDS;
    return lastPid;
}

int deleteProcess(int pid){
    if(pid == 0 )
        return 0; //la shell no puede eliminarse

    for(int i = 0; i < MAX_PFD; i++)
        closeFile(pid, i);

    if(allProcesses[pid]->parentPid >= 0 ){
        processControlBlock * parent = allProcesses[allProcesses[pid]->parentPid] ;        
        parent->quantityWaiting -- ; 
        if(parent->quantityWaiting == 0 )
            unblockProcess(parent->pid,WAITING);
    }
  
    allProcesses[pid]=(void*)0;
    freePidsCounter++;

    return killProcess(pid);
}

processControlBlock* getProcessByPid(int pid){
    if(pid < -1 || pid >=MAX_PIDS)
        return (processControlBlock*)0;
    if(pid == -1)
        return getCurrentTask();
    
    return allProcesses[pid];
}

int getMinFdByPid(int pid){
    processControlBlock* process = getProcessByPid(pid);

    if(!process)
        return -1;

    for(int i=0; i<MAX_PFD; i++){
        if(process->processFileDescriptors[i] == -1)
            return i;
    }
    return -1;
}

void exit(){
    deleteProcess(getCurrentPid());
}


void wait(){

    int currentPid = getCurrentPid();
    allProcesses[currentPid]->quantityWaiting++;
    
    if( allProcesses[currentPid]->quantityWaiting == 1 )
        blockProcess(currentPid,WAITING);
}

