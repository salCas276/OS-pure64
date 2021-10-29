#ifndef __LIBCONFILE_H__
#define __LIBCONSFILE_H__

#include "fileSystem.h"

int createConsole(inode* createdInode);

int openConsole(int pid, inode* openedInode, int inodeIndex, int mode);

int writeConsole(inode* inode, char* buf, int count);

#endif