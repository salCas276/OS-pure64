#ifndef __LIBCONFILE_H__
#define __LIBCONSFILE_H__

#include "fileSystem.h"

int createConsole(inode* createdInode);

int openConsole(inode* openedInode, int inodeIndex, int mode);

int writeConsole();

#endif