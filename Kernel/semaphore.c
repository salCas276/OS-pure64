#include <semaphore.h>

#define LENGTH 20

typedef struct semaphore {
    uint64_t value;
    uint64_t lock;
    char * id ; 
    uint64_t attachedProcess;
} semaphore;

static semaphore * semaphores[LENGTH];

void acquire(uint64_t lock);
void release(uint64_t * lock);

static int searchEmptySlot();
static int searchSemaphore(char * sem_id);
static int strcmp(char * string1 , char * string2);



uint64_t semOpen(char * sem_id, uint64_t initialValue){
 
    int isYet = searchSemaphore(sem_id);
    if(isYet > 0){
        semaphores[isYet]->attachedProcess++;
        return 0;
    }


    int emptyIndex = searchEmptySlot();

    semaphores[emptyIndex] = malloc(sizeof(semaphore));
    semaphores[emptyIndex]->id=sem_id;
    semaphores[emptyIndex]->lock = 0 ; 
    semaphores[emptyIndex]->attachedProcess = 1 ; 
    semaphores[emptyIndex]->value = initialValue;

    return 0; //retorna uint64_t
 

}

uint64_t semWait(char * sem_id){
    int semIndex = searchSemaphore(sem_id);
    semaphore *  currentSem = semaphores[semIndex];

    acquire(currentSem->lock);

    if(currentSem->value > 0 )
        currentSem->value--;
    else{
        release(&currentSem->lock);
        //blockmyself(); //wait 
        acquire(currentSem->lock);
        currentSem->value--;
    }
    release(&currentSem->lock);
    return 0; //retorna uint64_t

}

uint64_t semPost(char * sem_id){
    int semIndex = searchSemaphore(sem_id);
    semaphore *  currentSem = semaphores[semIndex];

    acquire(currentSem->lock);
    currentSem->value++;
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
        if(strcmp(sem_id,semaphores[i]->id))
            return i ; 
    return -1;//not found
}


static int strcmp(char * string1 , char * string2){
    int i = 0 ; 
    while(string1[i]!=0 && string2[i]!=0 && string2[i]==string1[i]){
        i++;
    }
   if(string1[i]==0 && string2[i]==0)    
        return 1;
    else 
        return 0 ; 
}