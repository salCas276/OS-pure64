#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include "../include/video.h"


typedef struct processControlBlock {
    uint8_t pid;
    uint64_t taskRSP;
    uint64_t functionAddress; //no deberia ser necesario
    uint64_t baseRSP; //no deberia ser necesario 
    prompt_info prompt;
} processControlBlock;

void timer_handler(void);
void FirstProcess(uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt);
void addProcess(uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt); 
void removeProcess(int pid);
uint64_t getCurrentRSP();
void setCurrentRSP(uint64_t rsp);
prompt_info * getCurrentPrompt();


#endif
