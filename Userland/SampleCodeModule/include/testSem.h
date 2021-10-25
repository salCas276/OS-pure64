#ifndef __TESTSEM_H__
#define __TESTSEM_H__
#include <stdint.h>

uint64_t openSemaphore(char * sem_id , uint64_t initialValue );
uint64_t waitSemaphore(char * sem_id  );
uint64_t postSemaphore(char * sem_id  );
uint64_t closeSemaphore(char * sem_id  );

void printSemaphore();



#endif