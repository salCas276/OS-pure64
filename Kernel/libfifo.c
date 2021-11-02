// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

    char * Wmode = malloc(sizeof(char)*2);
    if(!Wmode)
        return -1;
    strcpy(Wmode,"W");


    char * Rmode = malloc(sizeof(char)*2);
    if(!Rmode){
        free(Wmode);
        return -1;
    }
    strcpy(Rmode,"R");


    strcat(Wmode, createdInode->wSemId);
    int ret1 = (int) semOpen(createdInode->wSemId, 1);

    strcpy(createdInode->rSemId, createdInode->name); 
    strcat(Rmode, createdInode->rSemId);
    int ret2 = (int) semOpen(createdInode->rSemId, 1);

    free(Wmode);
    free(Rmode);

    if(ret1 == -1 || ret2 == -1 || createdInode->rPassword == -1 || createdInode->wPassword == -1)
        return -1;

    return 0;
}
//Uso esta funcion para agregarle restricciones propias de un fifo a la apertura de un archivo
int openFifo(int pid, inode* inode, int inodeIndex, int mode){
    if(mode == 2)
        return -1; //Solo se puede o escribir o leer de un fifo

    int flag = 0;

    if(inode->indexes[0] == -1 && mode == 0){
        flag = 1; 
    }
    else if(inode->indexes[0] == -1 && mode == 1){
        flag = -1;
    }

    int fd = openFileFromInode(pid, inode, inodeIndex, mode);

    if(fd == -1)
        return -1;

    if(flag == 1)
        while(popAndUnblock(inode->wPassword) != -1);

    if(flag == -1)
        blockMyself(inode->wPassword);
    
    return fd;
}


int readFifo(inode* readInode, char* buf, int count){

    if((int) semWait(readInode->rSemId) == -1)
        return -1;

    int i;
    for(i=0; i<count; i++){
        if(readInode->indexes[0] == readInode->indexes[1] || readInode->indexes[1] == -1){
            if(readInode->writeOpenCount == 0 && readInode->indexes[1] != -1){
                if((int) semPost(readInode->rSemId) == -1)
                    return -1;
                buf[i] = 0;
                return 0; //Llegue al EOF
            }
            popAndUnblock(readInode->wPassword);
            blockMyself(readInode->rPassword);
        }

        buf[i]=readInode->block[readInode->indexes[0]];
        readInode->indexes[0] = (readInode->indexes[0]+1)%BLOCK_SIZE;
    }

    popAndUnblock(readInode->wPassword);
    

    if((int) semPost(readInode->rSemId) == -1)
        return -1;

    return i;
}

int writeFifo(inode* writtenInode, char* buf, int count){

    if((int) semWait(writtenInode->wSemId) == -1)
        return -1;

    int i;
    for(i=0; i<count; i++){
        if((writtenInode->indexes[1]+1)%BLOCK_SIZE == writtenInode->indexes[0] || buf[i] == '~'){
            if( (writtenInode->openCount == writtenInode->writeOpenCount && writtenInode->indexes[0] != -1) || buf[i] == '~'){
                if((int) semPost(writtenInode->wSemId) == -1)
                    return -1;
                return 0; //Llegue al EOF
            }
            popAndUnblock(writtenInode->rPassword);
            blockMyself(writtenInode->wPassword);
        }
        writtenInode->block[writtenInode->indexes[1]] = buf[i];
        writtenInode->indexes[1] = (writtenInode->indexes[1]+1)%BLOCK_SIZE;
    }

    popAndUnblock(writtenInode->rPassword);


    if((int) semPost(writtenInode->wSemId) == -1)
        return -1;

    return i;
}


int loadFifosData(fifoData* fifosBuf){
    inode** inodeBuf = malloc(sizeof(inode*)*MAX_FILES);
    if(!inodeBuf)
        return -1;
    int counter;

    if( (counter = getTypeInodes(inodeBuf, 3)) == -1){
        free(inodeBuf);
        return -1;
    }

    for(int i = 0; i < counter; i++){
            strcpy(fifosBuf[i].name, inodeBuf[i]->name);
            fifosBuf[i].forUnlink = inodeBuf[i]->forUnlink;
            fifosBuf[i].indexes[0] = inodeBuf[i]->indexes[0];
            fifosBuf[i].indexes[1] = inodeBuf[i]->indexes[1];
            fifosBuf[i].openCount = inodeBuf[i]->openCount;
            fifosBuf[i].writeOpenCount = inodeBuf[i]->writeOpenCount;
            if(getPidsBlocked(inodeBuf[i], fifosBuf[i].blockedPids) == -1)
                return -1;
    }

    free(inodeBuf);
    return counter;
}