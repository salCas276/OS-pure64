// #ifndef _SCHEDULER_H_
// #define _SCHEDULER_H_

#include <stdint.h>
#include "RoundRobin.h"
#include "../include/video.h"


void timer_handler(void);
void FirstProcess(uint64_t functionAddress, uint64_t baseRSP, prompt_info prompt);


// #endif
