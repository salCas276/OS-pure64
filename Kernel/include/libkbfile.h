#ifndef __LIBKBFILE_H__
#define __LIBKBFILE_H__

#include "fileSystem.h"


int createKeyboard(inode* createdInode);

int openKeyboard(int pid, inode* openedInode, int inodeIndex, int mode);

int readKeyboard();

#endif