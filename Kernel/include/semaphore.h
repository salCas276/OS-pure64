#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <stdint.h>
#include <memoryManager.h>

typedef struct semaphoreDescriptor{
    int value;
    char * sem_id;
    int * blocked;
}semaphoreDescriptor;

int64_t semOpen(char * sem_id, uint64_t initialValue);

int64_t semWait(char * sem_id);

int64_t semPost(char * sem_id);

int64_t semClose(char * sem_id);

int getSemaphoreData(uint64_t arrayPointer);

int getSemBlokcedPids(char* sem_id, int* pidsBuff);

#endif