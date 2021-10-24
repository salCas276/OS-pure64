#ifndef __PROCESSAPI_H__
#define __PROCESSAPI_H__

#include "utils.h"
#include <stdint.h>

uint64_t createProcessUserland(uint64_t function,_ARGUMENTS,int foreground);

uint64_t nice(int pid, int deltaNice); 

uint64_t kill(int signal, int pid); 

uint64_t renounceUserland(); 

void exitUserland();

void waitSon();

#endif