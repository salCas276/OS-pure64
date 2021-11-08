#include "./include/libshmem.h"
#include "./include/memoryManager.h"

typedef struct shmem {
    int qProcessesAttached; 
    void * memDirec; 
} shmem; 

#define SHMEMSIZE 100
#define MAXMEMS 3

shmem * memories[MAXMEMS] = {0}; 

// Devuelve null si no existe o el puntero a la estructura si existe 
static shmem * fetchShmemById(int id) {
    return memories[id]; 
}

void * createShmem(int id) {

    shmem * mem = fetchShmemById(id); 

    if ( !mem ) {
        // Asignar memoria dinamica
        memories[id] = (shmem *) malloc(sizeof(shmem)); 
        memories[id]->qProcessesAttached = 1; 
        memories[id]->memDirec = (void *) malloc(sizeof(SHMEMSIZE)); 
    } else {
        memories[id]->qProcessesAttached ++; 
    }

    return mem->memDirec; 
}


int unlinkShmem(int id) {
    
    shmem * mem = fetchShmemById(id); 

    // deberia validar ids
    if ( mem ) {
        memories[id]->qProcessesAttached --; 
        if ( ! memories[id]->qProcessesAttached ) {
            free(memories[id]->memDirec); 
            free(memories[id]); 
            memories[id] = 0; 
        }
        return 0; 
    }
    return -1; 
}