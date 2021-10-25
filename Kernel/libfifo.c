#include "include/libfifo.h"

//Uso esta funcion para agregarle restricciones propias de un fifo a la apertura de un archivo
int openFifoFromInode(inode* inode, int inodeIndex, int mode){
    openFileFromInode(inode, inodeIndex, mode);
    int flag = 0;
    if(inode->writeOpenCount == 0){ //No hay escritores
        switch(mode){
            case 0: //Llega un lector
                flag = -1; //Bloqueo al lector 
                break;
            case 1: //Llega un escritor
                flag = 1; //Desbloque a todos los lectores
                break;
        }
    }
    else if(inode->writeOpenCount == inode->openCount){ //No hay lectores
        switch(mode){
            case 0: //Llega un lector
                flag = 2; //Desbloqueo a todos los escritores
                break;
            case 1: //Llega un escritor
                flag = -2; //Bloqueo al escritor
                break;
        }
    }
    return flag; //En este caso no hubo ni que bloquear ni desbloquear a nadie
}

/*
int readFifo(int fd, char* buf, int count){
    if(!openedFileTable[fd]) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 1) //La apertura no permite esta operacion
        return -1;

    inode* targetInode = openedFileTable[fd]->inode;
    int i;

    for(i=0; i<count; i++){
        if((targetInode->indexes[0]+i)%BLOCK_SIZE == targetInode->indexes[1]){
            if(targetInode->writeOpenCount == 0){
                targetInode->indexes[0] = targetInode->indexes[0]+i;
                return 0; //Llegue al EOF
            }
            //TODO Block reader
        }

        buf[i]=targetInode->block[(targetInode->indexes[0]+i)%BLOCK_SIZE];
    }
    //TODO Unblock possible blocked writers
    targetInode->indexes[0] = targetInode->indexes[0]+i;
    return i;
}

int writeFifo(int fd, char* buf, int count){
    if(!openedFileTable[fd]) //El fd no apunta a una apertura existente
        return -1;
    if(openedFileTable[fd]->mode == 0) //La apertura no permite esta operacion
        return -1;
    inode* targetInode = openedFileTable[fd]->inode;
    int i;
    for(i=0; i<count; i++){
        if((targetInode->indexes[1]+i)%BLOCK_SIZE == targetInode->indexes[0]){
            //TODO Block writer
        }
        targetInode->block[(targetInode->indexes[1]+i)%BLOCK_SIZE] = buf[i];
    }
    //TODO Unblock possible blocked readers
    targetInode->indexes[1] = targetInode->indexes[1]+i;
    return i;
}
*/
