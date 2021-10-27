#include "include/libregfile.h"


int createRegular(inode* createdInode){
    createdInode->block = malloc(BLOCK_SIZE);
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 2;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    return 0;
}

int openRegular(inode* openedInode, int inodeIndex, int mode){
    return openFileFromInode(openedInode, inodeIndex, mode);
}

int writeRegular(){

}

int readRegular(){

}