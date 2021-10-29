#include "include/libconsfile.h"
#include "include/fileSystem.h"
#include "include/naiveConsole.h"

int createConsole(inode* createdInode){
    createdInode->block = 0;
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 1;
    createdInode->forUnlink = 0;

    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    
    strcpy(createdInode->wSemId, createdInode->name); 
    strcat("W", createdInode->wSemId);
    int ret1 = (int) semOpen(createdInode->wSemId, 1);

    if(ret1 == -1)
        return -1;

    return 0;
}

int openConsole(inode* openedInode, int inodeIndex, int mode){
    if(mode != 1)
        return -1; //Solo se puede escribir en pantalla
    return openFileFromInode(openedInode, inodeIndex, mode);
}

//Este va a ser un print_s pero con un semaforo
int writeConsole(inode* writtenInode, char* buf, int count){

    if((int) semWait(writtenInode->wSemId) == -1)
        return -1;

    int counter = 0;
    while(*(buf+counter) && counter < count) ncPrintCharAtt(*(buf+counter++), &WHITE, &BLACK);

    if((int) semPost(writtenInode->wSemId) == -1)
        return -1;

    return counter;
}