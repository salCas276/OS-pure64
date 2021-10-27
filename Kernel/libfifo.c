#include "include/libfifo.h"

int createFifo(inode* createdInode){
    createdInode->block = malloc(BLOCK_SIZE);
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 2;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    return 0;
}
//Uso esta funcion para agregarle restricciones propias de un fifo a la apertura de un archivo
int openFifo(inode* inode, int inodeIndex, int mode){
    if(mode == 2)
        return -1; //Solo se puede o escribir o leer de un fifo
    return openFileFromInode(inode, inodeIndex, mode);
}
    /*
    int flag = 0;

    if(inode->writeOpenCount == 0){ //No hay escritores
        switch(mode){
            case 0: //Llega un lector
                blockMyself(writerPassword); //Bloqueo al lector 
                break;
            case 1: //Llega un escritor
                unblockAll(readerPassword); //Desbloque a todos los lectores
                break;
        }
    }

    else if(inode->writeOpenCount == inode->openCount){ //No hay lectores, recordemos que para fifos solo vale rdonly o wronly, entonces no hay problemas
        switch(mode){
            case 0: //Llega un lector
                unblockAll(readerPassword); //Desbloqueo a todos los escritores
                break;
            case 1: //Llega un escritor
                blockMyself(writerPassword); //Bloqueo al escritor
                break;
        }
    }

    return flag; //En este caso no hubo ni que bloquear ni desbloquear a nadie
}

int readFifo(int fd, char* buf, int count){
    if(!openedFileTable[fd]) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 1) //La apertura no permite esta operacion
        return -1;

    semWait(redersSemId);

    inode* targetInode = openedFileTable[fd]->inode;
    int i;

    for(i=0; i<count; i++){
        if((targetInode->indexes[0]+i)%BLOCK_SIZE == targetInode->indexes[1]){
            if(targetInode->writeOpenCount == 0){
                targetInode->indexes[0] = targetInode->indexes[0]+i;
                return 0; //Llegue al EOF
            }
            blockMyself(readerPassword);
        }

        buf[i]=targetInode->block[(targetInode->indexes[0]+i)%BLOCK_SIZE];
    }

    unblockAll(writerPassword);//TODO al tener un semaforo seria suficiente con que solo se desbloquee 1?
    
    targetInode->indexes[0] = targetInode->indexes[0]+i;

    semPost(readersSemId);

    return i;
}

int writeFifo(int fd, char* buf, int count){
    if(!openedFileTable[fd]) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 0) //La apertura no permite esta operacion
        return -1;

    semWait(writersSemId);

    inode* targetInode = openedFileTable[fd]->inode;
    int i;

    for(i=0; i<count; i++){
        if((targetInode->indexes[1]+i)%BLOCK_SIZE == targetInode->indexes[0]){
            if(targetInode->openCount == targetInode->writeOpenCount){
                targetInode->indexes[1] = targetInode->indexes[1]+i;
                return 0; //Llegue al EOF
            }
            blockMyself(writerPassword);
        }
        targetInode->block[(targetInode->indexes[1]+i)%BLOCK_SIZE] = buf[i];
    }

    unblockAll(readPassword); //TODO al tener un semaforo seria suficiente con que solo se desbloquee 1?

    targetInode->indexes[1] = targetInode->indexes[1]+i;

    semPost(writersSemId);

    return i;
}
*/
