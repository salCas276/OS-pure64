
#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <stdint.h>
#include <unistd.h>

#define MEMBASE 0x900000 

void *malloc(size_t size);
void free(void *pv);

#endif