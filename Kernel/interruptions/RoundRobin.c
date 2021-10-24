
#include <roundRobin.h>
#include "../include/naiveConsole.h"
#include "../include/memoryManager.h"

#define SIZE 20
#define QHEADERS MAXBLOCKTYPES+1
static int processTotal = 0;
static processControlBlock * currentProcess = (processControlBlock *)(0); 

static processControlBlock * headers[QHEADERS] ={0}; // ColaS de bloqueados donde el id es el pswd


static processControlBlock * unlinkProcess(processControlBlock * process, int pid, processControlBlock ** p) ; 
static void pushProcess( processControlBlock ** header, processControlBlock * process);
static int changeNicenessRec(processControlBlock * header, int pid, int bonus);

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
    if ( headers[0] == 0 ) {
        headers[0] = process; 
        headers[0]->tail = (processControlBlock *)(0); 
        currentProcess = process; 
        printChain(headers[0]); 
        return; 
    }

    // push
    pushProcess(&headers[0], process);
	processTotal++;
    printChain(headers[0]); 

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
        (*p) = process;  
        return aux; 
    }

    process->tail = unlinkProcess(process->tail, pid, p); 
    return process; 
}

// Te mata un proceso que esta en la cola de listos 
int killProcess(int pid) {

    if (pid == 0) return -1; // No puedes matar al primer proceso!
    processControlBlock * p = 0;
    int i = 0; 
    do { 
        headers[i] = unlinkProcess(headers[i], pid, &p);
        i++; 
    } while( p == 0 && i<QHEADERS); 
    
    if ( p == 0 ) return 1; 
    free(p); // Efectivamente lo borra
    processTotal --; 
    return 0; 
}

int blockProcess(int pid, int password) {

    printChain(headers[0]);
    printChain(headers[password+1]); 

    if (pid == 0) return -1; // No puedes bloquear al primer proceso 

    if ( password < 0 || password > MAXBLOCKTYPES) return -1; 

    processControlBlock * p = 0; 
    headers[0] = unlinkProcess(headers[0], pid, &p);

    if (p == 0) return 1; 

    pushProcess( &headers[password+1], p); // Deberia ser password
    
    printChain(headers[0]);
    printChain(headers[password+1]); 
    return 0; 
}

int unblockProcess(int pid, int password) {

    if ( password < 0 || password > MAXBLOCKTYPES) return -1; 
    
    processControlBlock * p; 
    headers[password+1] = unlinkProcess(headers[password+1], pid, &p); 

    if (p == 0) return 1; 
    pushProcess( &headers[0], p); 
    printChain(headers[0]);
    printChain(headers[password+1]); 

    return 0; 
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
        currentProcess = ( currentProcess->tail == (processControlBlock *)(0) ? headers[0] : currentProcess->tail); 
    } 
//     str[0] = '0'+ currentProcess->pid;
//     ncPrint(str); 
//     ncPrint("\n"); 
}

processControlBlock * getCurrentTask(){
    return currentProcess;
}

int renounce() {
    currentProcess->currentPushes = WORSTPRIORITY + 1 - currentProcess->priority; 
    return 0; 
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

int getCurrentMinFd(){
    processControlBlock* process = getCurrentTask();
    for(int i=0; i<MAX_PFD; i++){
        if(process->processFileDescriptors[i] == -1)
            return i;
    }
    return -1;
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