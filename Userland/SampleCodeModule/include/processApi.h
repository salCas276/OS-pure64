#ifndef __PROCESSAPI_H__
#define __PROCESSAPI_H__

#include "utils.h"
#include <stdint.h>

int createProcessUserland(uint64_t function,_ARGUMENTS,int foreground);

uint64_t nice(int pid, int deltaNice); 

uint64_t kill(int signal, int pid); 

uint64_t renounceUserland(); 

void exitUserland();

void waitSon();

uint64_t getPid();

#endif