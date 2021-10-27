#include "include/libkbfile.h"
#include "interruptions/keyboard.h"


int createKeyboard(inode* createdInode){
    createdInode->block = malloc(BLOCK_SIZE);
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 0;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    return 0;
}

int openKeyboard(inode* openedInode, int inodeIndex, int mode){
    if(mode != 0)
        return -1; //Solo se puede leer de teclado
    return openFileFromInode(openedInode, inodeIndex, mode);
}

//Este metdodo va a ser basicamente un read_s con un semaforo
int readKeyboard(){

}

