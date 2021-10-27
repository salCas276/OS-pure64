#include "include/libconsfile.h"
#include "include/fileSystem.h"

int createConsole(inode* createdInode){
    createdInode->block = malloc(BLOCK_SIZE);
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 1;
    createdInode->forUnlink = 0;
    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    return 0;
}

int openConsole(inode* openedInode, int inodeIndex, int mode){
    if(mode != 1)
        return -1; //Solo se puede escribir en pantalla
    return openFileFromInode(openedInode, inodeIndex, mode);
}

//Este va a ser un print_s pero con un semaforo
int writeConsole(inode* writtenInode, char* buf, int count){

}