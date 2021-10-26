#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 19 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define MAX_SIZE_BLOCK 256
#define _ARGUMENTS char argv[MAX_ARGC][MAX_COMMAND], int argc
#include <stdint.h>


typedef struct processDescriptor{
    uint8_t pid;
} processDescriptor;

typedef struct fileInfo{
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int fileType; //0 -> File, 1 -> Fifo
    int forUnlink; //Flag que me indica si se le hizo un unlink
}fileInfo;

void printDate(_ARGUMENTS);
void help(_ARGUMENTS);
void printmem(_ARGUMENTS);
void printFeatures(_ARGUMENTS);
void printQuadraticRoots(_ARGUMENTS);
void echo(_ARGUMENTS); 
void printProcessesData(_ARGUMENTS);
void printHola(_ARGUMENTS);
void nicecmd(_ARGUMENTS); 
void killcmd(_ARGUMENTS); 
void createFifo(_ARGUMENTS);
void createFile(_ARGUMENTS);
void printFileContent(_ARGUMENTS);
void printFileInfo(_ARGUMENTS);

#endif