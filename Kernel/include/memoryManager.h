
#ifndef _MEMORYMANAGER_H_
#define _MEMORYMANAGER_H_

#include <stdint.h>
#include <unistd.h>

void *malloc(unsigned int size);
void free(void *pv);

#endif