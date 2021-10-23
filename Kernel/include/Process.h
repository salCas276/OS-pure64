#ifndef PROCESS_H
#define PROCESS_H

#include <roundRobin.h>
#include "../include/video.h"
#include "../include/memoryManager.h"


void firstProcess(uint64_t functionAddress, prompt_info prompt);

uint64_t createProcess(uint64_t functionAddress);

int getProcessesData(uint64_t descriptorsArray);
 
int deleteProcess(int pid);

void exit();

void wait();


#endif