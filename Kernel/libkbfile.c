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
int readKeyboard(inode* inode, char* buf, int count){
    int32_t counter = 0;
    int64_t c;
    //Si le paso count=-1 va a leer hasta el \n
    while((c = getChar()) != '\n'){
        if(counter < count || count == -1){
            if(c == '\b'){
                if(counter == 0)
                    continue;
                counter--;
            } else {
                buf[counter++] = c;
            }
        }
        else{
            if(c == '\b')
                counter--;
            else
                counter++;

        }
        //put_char(1, c);
    }

    if(counter <= count || count == -1){
        buf[counter] = 0;
        return counter;
    }
    buf[count] = 0;
    return count;
}