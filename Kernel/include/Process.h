#ifndef PROCESS_H
#define PROCESS_H

#include <RoundRobin.h>
#include "../include/video.h"
#include "../include/ListFreeMemoryManager.h"


void firstProcess(uint64_t functionAddress, prompt_info prompt);

uint64_t createProcess(uint64_t functionAddress);



#endif