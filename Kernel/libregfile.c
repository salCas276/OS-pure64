#include "include/libregfile.h"


int createRegular(inode* createdInode){
    createdInode->block = malloc(REG_BLOCK_SIZE);
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 2;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    createdInode->rPassword = createdInode->wPassword = -1;
    createdInode->wSemId[0] = createdInode->rSemId[0] = 0;
    return 0;
}

int openRegular(inode* openedInode, int inodeIndex, int mode){
    return openFileFromInode(openedInode, inodeIndex, mode);
}

int writeRegular(inode* writtenInode, char* buf, int count){

    int i;
    for(i=0; i<count; i++)
        writtenInode->block[(writtenInode->indexes[1]+i)%REG_BLOCK_SIZE] = buf[i];

    writtenInode->indexes[1] = writtenInode->indexes[1]+i;

    return i;
}

int readRegular(inode* readInode, char* buf, int count){

    int i;
    for(i=0; i<count; i++){
        if((readInode->indexes[0]+i)%REG_BLOCK_SIZE == readInode->indexes[1] || readInode->indexes[1] == -1){
            readInode->indexes[0] = readInode->indexes[0]+i;
            return 0; //Llegue al EOF
        }

        buf[i]=readInode->block[(readInode->indexes[0]+i)%REG_BLOCK_SIZE];
    }

    readInode->indexes[0] = readInode->indexes[0]+i;

    return i;
}