#include "include/libkbfile.h"
#include "interruptions/keyboard.h"
#include "naiveConsole.h"
#include "include/semaphore.h"
#include "include/string.h"


int createKeyboard(inode* createdInode){
    createdInode->block = (char*) 0;
    createdInode->openCount = 0;
    createdInode->writeOpenCount = 0;
    createdInode->fileType = 0;
    createdInode->forUnlink = 0;

    for(int i=0; i<2; i++) createdInode->indexes[i] = -1;
    
    createdInode->rPassword = KEYBOARD_PASSWORD;

    strcpy(createdInode->rSemId, createdInode->name); 
    strcat("R", createdInode->rSemId);
    int ret2 = (int) semOpen(createdInode->rSemId, 1);

    if(ret2 == -1)
        return -1;

    return 0;
}

int openKeyboard(int pid, inode* openedInode, int inodeIndex, int mode){
    if(mode != 0)
        return -1; //Solo se puede leer de teclado
    return openFileFromInode(pid, openedInode, inodeIndex, mode);
}

//Este metdodo va a ser basicamente un read_s con un semaforo
int readKeyboard(inode* readInode, char* buf, int count){

   if((int) semWait(readInode->rSemId) == -1)
    return -1;

    int32_t counter = 0;
    int64_t c;
    count = count < 0 ? count : count-1;
    //Si le paso count=-1 va a leer hasta el \n
    while((c = getChar()) != '~' && (count != -1 || c != '\n')){
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
            else{
                buf[counter++] = c;
                ncPrintCharAtt(c, &WHITE, &BLACK);
                break;
            }
        }
        ncPrintCharAtt(c, &WHITE, &BLACK);
    }

    if(c == '~'){
        ncPrintCharAtt('\n', &WHITE, &BLACK);
        if((int) semPost(readInode->rSemId) == -1)
            return -1;
        return 0;
    }
    else ncPrintCharAtt(c, &WHITE, &BLACK);

    if((int) semPost(readInode->rSemId) == -1)
        return -1;

    if(counter <= count || count == -1){
        buf[counter] = 0;
        return counter;
    }

    buf[count] = 0;
    return count;
}