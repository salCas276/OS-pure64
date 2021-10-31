#include <semaphore.h>
#include <string.h>
#include "./interruptions/roundRobin.h"

#define SEMAPHORE_PASSWORD 2

#define LENGTH 20

typedef struct semaphore {
    uint64_t value;
    uint64_t lock;
    char * id ; 
    uint64_t attachedProcess;
} semaphore;

static semaphore * semaphores[LENGTH];

void acquire(uint64_t * lock);
void release(uint64_t * lock);

static int searchEmptySlot();
static int searchSemaphore(char * sem_id);
// static int8_t strcmp (const char *p1, const char *p2);


uint64_t semOpen(char * sem_id, uint64_t initialValue){
 
    int isYet = searchSemaphore(sem_id);
    if(isYet >= 0){
        semaphores[isYet]->attachedProcess++;
        return 0;
    }


    int emptyIndex = searchEmptySlot();
    if(emptyIndex < 0 ){
        return -1;
    }



    semaphores[emptyIndex] = malloc(sizeof(semaphore)); //falla de malloc

    if(semaphores[emptyIndex] == (void* )0)
        return -1;

    semaphores[emptyIndex]->id=sem_id;
    semaphores[emptyIndex]->lock = 0 ; 
    semaphores[emptyIndex]->attachedProcess = 1 ; 
    semaphores[emptyIndex]->value = initialValue;

    return 0; 
 

}

uint64_t semWait(char * sem_id){
    int semIndex = searchSemaphore(sem_id);
    if(semIndex < 0){
      return 0; //not found
    }

    semaphore *  currentSem = semaphores[semIndex];

    int loop = 1 ;
    while(loop){
        acquire(&currentSem->lock);
        if(currentSem->value > 0 ){
            currentSem->value--;
            loop=0;
        }else{
            release(&currentSem->lock);
            blockProcess(getCurrentPid(),SEMAPHORE_PASSWORD + semIndex);
            acquire(&currentSem->lock);
            if(currentSem -> value > 0 ){
                currentSem->value--;
                loop=0;
            }else{
                release(&currentSem->lock);
            }
        }
    }

    
    release(&currentSem->lock);
    return 0; //retorna uint64_t

}

uint64_t semPost(char * sem_id){
    int semIndex = searchSemaphore(sem_id);
    if(semIndex < 0){ //not found
      return 0;
    }
    semaphore *  currentSem = semaphores[semIndex];

    acquire(&currentSem->lock);
    currentSem->value++;
    if(currentSem->value==1)
        popAndUnblock(SEMAPHORE_PASSWORD + semIndex);
    release(&currentSem->lock);
    
    

    return 0; //retorna uint64_t

    
}

uint64_t semClose(char * sem_id){
    int semIndex = searchSemaphore(sem_id);
 
    if(semaphores[semIndex]->attachedProcess==1){
        free(semaphores[semIndex]);
        semaphores[semIndex]=(void*)0;
    }
    else 
        semaphores[semIndex]->attachedProcess--;

    return 0; //retorna uint64_t

    
}


int getSemaphoreData(uint64_t arrayPointer){
    int j = 0;
    semaphoreDescriptor * semaphoreDescriptorArray = (semaphoreDescriptor *) arrayPointer;
    for(int i=0; i< LENGTH ; i++){
        if(semaphores[i] != (void*)0 ){
            (semaphoreDescriptorArray+j)->sem_id=semaphores[i]->id;
            (semaphoreDescriptorArray+j)->value=semaphores[i]->value;
            (semaphoreDescriptorArray+j)->blocked=getBlockedBy(SEMAPHORE_PASSWORD+i,semaphores[i]->attachedProcess);
            j++;
        }
    }
    return j;
}

static int searchEmptySlot(){ 
    for(int i = 0  ; i < LENGTH ; i++){
        if(semaphores[i]==( void * ) 0){
            return i ; 
        }
    }
    return -1;//no deberia pasar nunca.
}


static int searchSemaphore(char * sem_id){
    for(int i = 0 ; i < LENGTH ; i ++ )
        if(semaphores[i]!=(void*)0 && !strcmp(sem_id,semaphores[i]->id))
            return i ; 
    return -1;//not found
}

int getSemBlokcedPids(char* sem_id, int* pidsBuff){
    int semIndex = searchSemaphore(sem_id);
    return getBlockedPidsByPass(SEMAPHORE_PASSWORD+semIndex, pidsBuff);
}



// static int8_t strcmp (const char *p1, const char *p2) {
//   const unsigned char *s1 = (const unsigned char *) p1;
//   const unsigned char *s2 = (const unsigned char *) p2;
//   unsigned char c1, c2;
//   do {
//       c1 = (unsigned char) *s1++;
//       c2 = (unsigned char) *s2++;
//       if (c1 == '\0')
//         return c1 - c2;
//     } while (c1 == c2);
//   return c1 - c2;
// }