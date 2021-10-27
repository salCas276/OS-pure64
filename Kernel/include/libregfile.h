#ifndef __LIBREGFILE_H__
#define __LIBREGFILE_H__

#include "fileSystem.h"

int createRegular(inode* createdInode);

int openRegular(inode* openedInode, int inodeIndex, int mode);

int writeRegular();

int readRegular();

#endif