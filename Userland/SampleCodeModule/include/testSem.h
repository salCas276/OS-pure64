#ifndef __TESTSEM_H__
#define __TESTSEM_H__
#include <stdint.h>

int64_t openSemaphore(char * sem_id , uint64_t initialValue );
int64_t waitSemaphore(char * sem_id  );
int64_t postSemaphore(char * sem_id  );
int64_t closeSemaphore(char * sem_id  );

int getSemaphoreData(uint64_t semaphoreDescriptorArray);


#endif