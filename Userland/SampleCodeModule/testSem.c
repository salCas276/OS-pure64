// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <testSem.h>


int64_t openSemAsm(char * sem_id , uint64_t initialValue );
int64_t waitSemAsm(char * sem_id  );
int64_t postSemAsm(char * sem_id );
int64_t closeSemAsm(char * sem_id );
int getSemaphoreDataAsm(uint64_t semaphoreDescriptorArray );

int64_t openSemaphore(char * sem_id , uint64_t initialValue ){
    return openSemAsm(sem_id,initialValue);
}
int64_t waitSemaphore(char * sem_id  ){
    return waitSemAsm(sem_id);
}
int64_t postSemaphore(char * sem_id  ){
    return postSemAsm(sem_id);
}
int64_t closeSemaphore(char * sem_id  ){
    return closeSemAsm(sem_id);
}

int getSemaphoreData(uint64_t semaphoreDescriptorArray){
    return getSemaphoreDataAsm(semaphoreDescriptorArray);
}