#ifndef __LIBFIFO_H__
#define __LIBFIFO_H__

#include "fileSystem.h"


int openFifoFromInode(inode* inode, int inodeIndex, int mode);

int readFifo(int fd, char* buf, int count);

int writeFifo(int fd, char* buf, int count);

#endif