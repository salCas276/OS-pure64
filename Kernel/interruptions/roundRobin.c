// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <roundRobin.h>
#include "../include/naiveConsole.h"
#include "../include/memoryManager.h"

#define SIZE 20
#define QHEADERS (MAXBLOCKTYPES+1)
#define KEYBOARD_PASSWORD 1
#define WAITING 2

static int processTotal = 0;
static processControlBlock * currentProcess = (processControlBlock *)(0); 
static processControlBlock * next = (processControlBlock * ) 0;
static processControlBlock * idle = (processControlBlock*)(0);

static prompt_info shellPrompt ; 
static prompt_info backgroundPrompt;

static int passIndex = 30;

static processControlBlock * headers[QHEADERS] ={0}; 

void timerTickInterrupt();
void _hlt();
uint64_t _buildContext(uint64_t basePointer , uint64_t functionAddress,_ARGUMENTS );

static processControlBlock * unlinkProcess(processControlBlock * process, int pid, processControlBlock ** p) ; 
static void pushProcess( processControlBlock ** header, processControlBlock * process);
static int changeNicenessRec(processControlBlock * header, int pid, int bonus);
static void restartRoundRobin(processControlBlock * header);
// Debugging
void printChain(processControlBlock * c) {
   
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



int * getBlockedBy(int password , int maxQ){
    //chequear password incorrecta

    int * pidArray = malloc((maxQ+1)*(sizeof(uint64_t)));

    if(!pidArray)
        return (void*)0;

    processControlBlock * currentProcess = headers[password+1];

    int index = 0;
    while(currentProcess){
        pidArray[index++]=currentProcess->pid;
        currentProcess = currentProcess -> tail;
    }
    
    pidArray[index] = -1;

    return pidArray;
}



void idleProcessFunction(){
    while (1)
    {
        _hlt();
    }
    
}



// El agregado de procesos se hace al estilo stack, se lo pushea en una lista simplemente encadenada de procesos 
// En la iteracion de la lista, si llegue al final, vuelvo a empezar (RR)
// Agregar un proceso no cambia el proceso que actualmente se esta ejecutando 
void addProcess(processControlBlock * process){

    // No hay procesos 
    if ( headers[0] == 0 ) {
        headers[0] = process; 
        headers[0]->tail = (processControlBlock *)(0); 
        currentProcess = process; 

        uint64_t * basePointerIdle = malloc(256 * sizeof(uint64_t));
        processControlBlock * idleProcess = malloc(sizeof(processControlBlock));
        
        if( !idleProcess || !basePointerIdle ){
            free(basePointerIdle);
            free(idleProcess);  
            return ; 
        }

        idleProcess->baseRSP = (uint64_t)&basePointerIdle[255] ;
        idleProcess->functionAddress= (uint64_t)&idleProcessFunction;
        idleProcess->taskRSP = _buildContext(idleProcess->baseRSP, idleProcess->functionAddress,0,0);
        idle=idleProcess;
        return; 
    }

    pushProcess(&headers[0], process);
	processTotal++;

}

void setPrompt(prompt_info shellPromptP , prompt_info backgroundPromptP){
    shellPrompt = shellPromptP;
    backgroundPrompt = backgroundPromptP;
}


static void pushProcess( processControlBlock ** header, processControlBlock * process) {
    process->tail = (*header); 
    (*header) = process; 
}


static processControlBlock * unlinkProcess(processControlBlock * process, int pid, processControlBlock ** p) {


    if (process == 0) {
        (*p) = 0; 
        return process; 
    }

    if ( process->pid == pid) {
        processControlBlock * aux = process->tail;
        process->tail = (void*)0; //para que no quede enganchado a la lista 
        (*p) = process;  
        return aux; 
    }

    process->tail = unlinkProcess(process->tail, pid, p); 
    return process; 
}




// Te mata un proceso que esta en la cola de listos. Falta borrar de process.c
int killProcess(int pid) {

    if (pid == 0) return -1; // No puedes matar al primer proceso!
    processControlBlock * p = 0;
    int i = 0; 
    do { 
        headers[i] = unlinkProcess(headers[i], pid, &p); 
        i++; 
    } while( p == 0 && i<QHEADERS); 
    
    if ( p == 0 ) return -1; 


    free(( (uint64_t * ) p->baseRSP ) - 4095);
    free(p);
    processTotal --; 
    
    if(pid == getCurrentPid()){
        currentProcess = (void*) 0 ;
        timerTickInterrupt();
    }
    
    return 0; 
}

int blockMyself(int password){
    return blockProcess(getCurrentPid(), password);
}

int blockProcess(int pid, int password) {

    if (pid == 0 && password != KEYBOARD_PASSWORD && password!=WAITING) return -1; // No puedes bloquear al primer proceso, solo puede bloquearlo el teclado o algun proceso hijo

    if ( password < 0 || password > MAXBLOCKTYPES) return -1; 

    if( next != (void*) 0 && next->pid == pid){ //si estan por bloquear al proximo.
        next = next -> tail; 
    }

    processControlBlock * p = 0; 

    //lo saco de la lista de procesos listos 
    headers[0] = unlinkProcess(headers[0], pid, &p);

    if (p == 0) return -1; 

    pushProcess( &headers[password+1], p); 

    if(pid == getCurrentPid()){
        currentProcess->currentPushes = WORSTPRIORITY + 1 - currentProcess->priority; //null
        timerTickInterrupt();

    }


    return 0; 
}

int unblockProcess(int pid, int password) {
    
    if ( password < 0 || password > MAXBLOCKTYPES) return -1; 
    
    processControlBlock * p; 

    if(password + 1 > MAXBLOCKTYPES ){
        return -1;
    }

    headers[password+1] = unlinkProcess(headers[password+1], pid, &p); 

    if (p == 0) return -1; 
    
    pushProcess( &headers[0], p); 

    
        
    return 0; 
}

static void restartRoundRobin(processControlBlock * header){
    processControlBlock * current = header;
    while(current != (void*) 0){
        current->currentPushes=0;
        current = current -> tail;
    }
}


int popAndUnblock(int password) { 
    //chequear password 
     if (headers[password+1] == 0) return -1;

     return unblockProcess(headers[password+1]->pid, password);
}



void nextTask(){

    if(headers[0] == (void*)0){
        currentProcess = idle ; 
        next=(void*)0;
        return;
    }
    else if(currentProcess == idle && headers[0]!=(void*)0 )
        currentProcess = (void*)0;

    if(currentProcess == (void*)0){
        if(next == (void*) 0 ){
            restartRoundRobin(headers[0]);
            currentProcess = headers[0];
        }else 
            currentProcess = next;
    }
    else if (currentProcess->currentPushes < WORSTPRIORITY + 1 - currentProcess->priority ) {
       currentProcess->currentPushes ++; 
   } else {
        currentProcess->currentPushes = 0; 
        if(next == (void*) 0 ){
            restartRoundRobin(headers[0]);
            currentProcess = headers[0];
        }else 
            currentProcess =  next; 
    } 
    next = currentProcess -> tail; 

    if(currentProcess->quantityWaiting > 0){
        currentProcess-> currentPushes = WORSTPRIORITY + 1 - currentProcess->priority;
            nextTask();
    }

}



int renounce() {
    currentProcess->currentPushes = WORSTPRIORITY + 1 - currentProcess->priority;
    timerTickInterrupt(); 
    return 0; 
}



processControlBlock * getCurrentTask(){
    return currentProcess;
}



void setCurrentRSP(uint64_t rsp) {
    processControlBlock * current = getCurrentTask();
    if(current != (void*)0 )
        current->taskRSP = rsp;
 
}

uint64_t getCurrentRSP(){
    processControlBlock * current = getCurrentTask();
    if(current != (void*)0 )
        return current->taskRSP;
    else return -1;
}

prompt_info * getCurrentPrompt() {
    if( getCurrentTask()->parentPid >= 0 ) //foreground
        return &shellPrompt;
    else 
        return &backgroundPrompt;
}

int getCurrentPid(){
    processControlBlock * current = getCurrentTask();
    if(current != (void*)0 )
        return current->pid;
    else return -1;
}

static int changeNicenessRec(processControlBlock * header, int pid, int deltaNice) {
    
    if (header == 0) 
        return 1; 

    if (header->pid == pid) {
        int aux = header->priority + deltaNice; 
        if ( aux > WORSTPRIORITY ) aux = WORSTPRIORITY; 
        else if ( aux < 0 ) aux = 0; 
        header->priority = aux; 
        return 0; 
    }

    return changeNicenessRec(header->tail, pid, deltaNice); 
    
}

int changeNicenessBy(uint64_t pid, uint64_t deltaNice) {
    int f = 1; 
    for (int i=0; i<QHEADERS && f; i++) 
        f = changeNicenessRec(headers[i], pid, deltaNice); 
    return 0; 
}

int getBlockedPidsByPass(int password, int* pidsBuf){
    if(password < 0)
        return -1;
    processControlBlock* next = headers[password+1];
    int counter=0;
    while(next){
        pidsBuf[counter++] = next->pid;
        next = next->tail;
    }
    return counter;
}

int getAvailablePassword(){
    return passIndex++;
}
