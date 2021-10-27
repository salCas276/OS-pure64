#ifndef __LIBREGFILE_H__
#define __LIBREGFILE_H__

#include "fileSystem.h"

int createRegular(inode* createdInode);

int openRegular(inode* openedInode, int inodeIndex, int mode);

int writeRegular(inode* writtenInode, char* buf, int count);

int readRegular(inode* readInode, char* buf, int count);

#endif