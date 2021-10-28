#ifndef __LIBREGFILE_H__
#define __LIBREGFILE_H__

#include "fileSystem.h"

#define REG_BLOCK_SIZE 1024

int createRegular(inode* createdInode);

int openRegular(inode* openedInode, int inodeIndex, int mode);

int writeRegular(inode* writtenInode, char* buf, int count);

int readRegular(inode* readInode, char* buf, int count);

#endif