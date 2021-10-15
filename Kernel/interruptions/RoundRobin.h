#ifndef _ROUNDROBIN_H_
#define _ROUNROBIN_H_



#include <stdint.h>
#include "../include/video.h"


typedef struct processControlBlock {
    uint8_t pid;
    uint64_t taskRSP;
    uint64_t functionAddress; //no deberia ser necesario
    uint64_t baseRSP; //no deberia ser necesario 
    prompt_info prompt;
} processControlBlock;



void addProcess(processControlBlock * process);

//void removeProcess(uint64_t pid);

void nextTask();

processControlBlock * getCurrentTask();

void setCurrentRSP(uint64_t rsp) ;

uint64_t getCurrentRSP();

prompt_info * getCurrentPrompt();


#endif
