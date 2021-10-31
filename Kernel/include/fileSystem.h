#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "memoryManager.h"

#define MAX_FILES 40
#define RD_ONLY 0
#define WRT_ONLY 1
#define RD_WRT 2
#define MAX_OPEN_FILES 40
#define BLOCK_SIZE 256
#define MAX_NAME 64

//Representa un archivo creado
typedef struct inode{
    char name[MAX_NAME]; //Se usa un nombre para poder abrir el archivo desde diferentes procesos
    char* block; //Esta es la memoria almacenada por el archivo
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int fileType; //0 -> File, 1 -> Fifo
    int forUnlink; //Flag que me indica si se le hizo un unlink
    int rPassword, wPassword;
    char rSemId[MAX_NAME+1], wSemId[MAX_NAME+1];
}inode;


typedef struct fileInfo{
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int fileType; //0 -> File, 1 -> Fifo
    int forUnlink; //Flag que me indica si se le hizo un unlink
}fileInfo;

//Representa la apertura de un archivo preexistente
typedef struct openedFile{
    int mode;
    inode* inode;
    int inodeIndex; 
}openedFile;

int createFile(char* name, int fileType);

int openFile(int pid, char* name, int mode);

int openFileFromInode(int pid, inode* inode, int inodeIndex, int mode);

int closeFile(int pid, int fd);

int unlinkFile(char* name);

int readFile(int pid, int fd, char* buf, int count);

int writeFile(int pid, int fd, char* buf, int count);

int dupp(int pid, int oldfd);

int dupp2(int pid, int oldfd, int newfd);

inode* getInode(char* name, int* inodeIndex);

int getTypeInodes(inode* inodeBuf[], int fileType);

int getPidsBlocked(inode* targetInode, int* pidsBuf);

#endif