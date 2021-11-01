
#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <stdint.h>
#include <unistd.h>

#define MEMBASE 0x900000 

typedef struct memstateType {
    unsigned totalMemory; 
    unsigned occupied; 
    unsigned free; 
} memstateType; 

void *malloc(size_t size);
void free(void *pv);
void getMemState(memstateType * state); 


#endif