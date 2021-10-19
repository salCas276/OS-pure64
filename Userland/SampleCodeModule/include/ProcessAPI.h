#ifndef __PROCESSAPI_H__
#define __PROCESSAPI_H__

#include <stdint.h>

uint64_t createProcessUserland(uint64_t function);

uint64_t nice(int pid, int deltaNice); 

uint64_t kill(int signal, int pid); 


#endif