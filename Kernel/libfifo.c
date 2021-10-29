#include "include/libfifo.h"
#include "interruptions/roundRobin.h"
#include "include/semaphore.h"
#include "include/string.h"

int createFifo(inode* createdInode){
    createdInode->block = malloc(BLOCK_SIZE);
    
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;

    createdInode->fileType = 3;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;

    createdInode->rPassword = getAvailablePassword();
    createdInode->wPassword = getAvailablePassword();

    strcpy(createdInode->wSemId, createdInode->name); 
    strcat("W", createdInode->wSemId);
    int ret1 = (int) semOpen(createdInode->wSemId, 1);

    strcpy(createdInode->rSemId, createdInode->name); 
    strcat("R", createdInode->rSemId);
    int ret2 = (int) semOpen(createdInode->rSemId, 1);

    if(ret1 == -1 || ret2 == -1 || createdInode->rPassword == -1 || createdInode->wPassword == -1)
        return -1;

    return 0;
}
//Uso esta funcion para agregarle restricciones propias de un fifo a la apertura de un archivo
int openFifo(inode* inode, int inodeIndex, int mode){
    if(mode == 2)
        return -1; //Solo se puede o escribir o leer de un fifo
    int fd = openFileFromInode(inode, inodeIndex, mode);

    if(fd == -1)
        return -1;
    /*
    if(inode->writeOpenCount == 0){ //No hay escritores
        switch(mode){
            case 0: //Llega un lector
                blockMyself(inode->rPassword); //Bloqueo al lector 
                break;
            case 1: //Llega un escritor
                popAndUnblock(inode->rPassword); //Desbloque a todos los lectores
                break;
        }
    }

    else if(inode->writeOpenCount == inode->openCount){ //No hay lectores, recordemos que para fifos solo vale rdonly o wronly, entonces no hay problemas
        switch(mode){
            case 0: //Llega un lector
                popAndUnblock(inode->wPassword); //Desbloqueo a todos los escritores
                break;
            case 1: //Llega un escritor
                blockMyself(inode->wPassword); //Bloqueo al escritor
                break;
        }
    }
    */
    return fd; //En este caso no hubo ni que bloquear ni desbloquear a nadie
}


int readFifo(inode* readInode, char* buf, int count){

    if((int) semWait(readInode->rSemId) == -1)
        return -1;

    int i;
    for(i=0; i<count; i++){
        if((readInode->indexes[0]+i)%BLOCK_SIZE == readInode->indexes[1] || readInode->indexes[1] == -1){
            if(readInode->writeOpenCount == 0 && readInode->indexes[1] != -1){
                readInode->indexes[0] = readInode->indexes[0]+i;
                return 0; //Llegue al EOF
            }
            blockMyself(readInode->rPassword);
        }

        buf[i]=readInode->block[(readInode->indexes[0]+i)%BLOCK_SIZE];
    }

    popAndUnblock(readInode->wPassword);//TODO al tener un semaforo seria suficiente con que solo se desbloquee 1?
    
    readInode->indexes[0] = readInode->indexes[0]+i;

    if((int) semPost(readInode->rSemId) == -1)
        return -1;

    return i;
}

int writeFifo(inode* writtenInode, char* buf, int count){

    if((int) semWait(writtenInode->wSemId) == -1)
        return -1;

    int i;
    for(i=0; i<count; i++){
        if((writtenInode->indexes[1]+i)%BLOCK_SIZE == writtenInode->indexes[0] || writtenInode->indexes[0] == -1){
            if(writtenInode->openCount == writtenInode->writeOpenCount && writtenInode->indexes[0] != -1){
                writtenInode->indexes[1] = writtenInode->indexes[1]+i;
                return 0; //Llegue al EOF
            }
            blockMyself(writtenInode->wPassword);
        }
        writtenInode->block[(writtenInode->indexes[1]+i)%BLOCK_SIZE] = buf[i];
    }

    popAndUnblock(writtenInode->rPassword); //TODO al tener un semaforo seria suficiente con que solo se desbloquee 1?

    writtenInode->indexes[1] = writtenInode->indexes[1]+i;

    if((int) semPost(writtenInode->wSemId) == -1)
        return -1;

    return i;
}