#ifndef __LIBFIFO_H__
#define __LIBFIFO_H__

#include "fileSystem.h"

typedef struct fifoData{
    char name[MAX_NAME];
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int forUnlink; //Flag que me indica si se le hizo un unlink
    int blockedPids[MAX_NAME];
}fifoData;

int createFifo(inode* createdInode);

int openFifo(int pid, inode* inode, int inodeIndex, int mode);

int readFifo(inode* readInode, char* buf, int count);

int writeFifo(inode* writtenInode, char* buf, int count);

int loadFifosData(fifoData* fifosBuf);

#endif