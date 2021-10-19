#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 19 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define _ARGUMENTS char argv[MAX_ARGC][MAX_COMMAND], int argc
#include <stdint.h>

typedef struct processDescriptor{
    uint8_t pid;
} processDescriptor;

void printDate(_ARGUMENTS);
void help(_ARGUMENTS);
void printmem(_ARGUMENTS);
void printFeatures(_ARGUMENTS);
void printQuadraticRoots(_ARGUMENTS);
void echo(_ARGUMENTS); 
void printProcessesData(_ARGUMENTS);
void printHola(_ARGUMENTS);
void nicecmd(_ARGUMENTS); 

#endif