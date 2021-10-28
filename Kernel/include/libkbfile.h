#ifndef __LIBKBFILE_H__
#define __LIBKBFILE_H__

#include "fileSystem.h"


int createKeyboard(inode* createdInode);

int openKeyboard(inode* openedInode, int inodeIndex, int mode);

int readKeyboard();

#endif