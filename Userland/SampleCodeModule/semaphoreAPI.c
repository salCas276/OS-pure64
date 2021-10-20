#include <semaphoreAPI.h>


void aux1(){
    return;
}

// // uint64_t openSemAsm(char * sem_id , uint64_t initialValue );
// // uint64_t waitSemAsm(char * sem_id  );
// // uint64_t postSemAsm(char * sem_id );
// // uint64_t closeSemAsm(char * sem_id );

// uint64_t openSemaphore(char * sem_id , uint64_t initialValue ){
//     return 0;//openSemAsm(sem_id,initialValue);
// }
// uint64_t waitSemaphore(char * sem_id  ){
//     return 0;//waitSemaphore(sem_id);
// }
// uint64_t postSemaphore(char * sem_id  ){
//     return 0;//postSemaphore(sem_id);
// }
// uint64_t closeSemaphore(char * sem_id  ){
//     return 0;//closeSemAsm(sem_id);
// }