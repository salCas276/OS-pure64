#ifndef _ROUNDROBIN_H_
#define _ROUNROBIN_H_



#include <stdint.h>
#include "../include/video.h"

#define WORSTPRIORITY 40

typedef struct processControlBlock {
    uint8_t pid;
    uint64_t taskRSP;
    uint64_t functionAddress; //no deberia ser necesario
    uint64_t baseRSP; //no deberia ser necesario 
    char priority; // between 0 and WORSTPRIORITY
    char currentPushes; 
    prompt_info prompt;
} processControlBlock;

typedef struct processDescriptor{
    uint8_t pid;
} processDescriptor;

void addProcess(processControlBlock * process);

//void removeProcess(uint64_t pid);

void nextTask();

processControlBlock * getCurrentTask();

void setCurrentRSP(uint64_t rsp) ;

uint64_t getCurrentRSP();

prompt_info * getCurrentPrompt();

int getCurrentPid();


#endif
