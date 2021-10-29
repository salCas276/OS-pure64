#ifndef __LIBFIFO_H__
#define __LIBFIFO_H__

#include "fileSystem.h"

int createFifo(inode* createdInode);

int openFifo(int pid, inode* inode, int inodeIndex, int mode);

int readFifo(inode* readInode, char* buf, int count);

int writeFifo(inode* writtenInode, char* buf, int count);

#endif