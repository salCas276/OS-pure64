#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 19 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define _ARGUMENTS int argc , char * argv[]
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
void printHola(_ARGUMENTS,int foreground);
void nicecmd(_ARGUMENTS); 
void killcmd(_ARGUMENTS); 
void test_sync();
void test_no_sync();
#endif