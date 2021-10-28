#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <stdint.h>
#include <memoryManager.h>

uint64_t semOpen(char * sem_id, uint64_t initialValue);

uint64_t semWait(char * sem_id);

uint64_t semPost(char * sem_id);

uint64_t semClose(char * sem_id);

void printSemaphore();

#endif