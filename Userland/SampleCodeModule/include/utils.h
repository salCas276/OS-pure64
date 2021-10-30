#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 100 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define MAX_SIZE_BLOCK 256
#define BUFFER_SIZE 128
#define _ARGUMENTS int argc , char * argv[]
#define NULL 0
#include <stdint.h>
//#include <test_util.h>


typedef struct processDescriptor{
    char * name;
    uint8_t pid;
    uint8_t priority;
    uint8_t foreground;
} processDescriptor;

typedef struct semaphoreDescriptor{
    int value;
    char * sem_id;
    int * blocked;
}semaphoreDescriptor;


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
void loop_wrapper(_ARGUMENTS,int foreground);
void nicecmd(_ARGUMENTS); 
void killcmd(_ARGUMENTS); 
void test_processes_wrapper(_ARGUMENTS,int foreground);
void cat_wrapper(_ARGUMENTS, int foreground); 
void filter_wrapper(_ARGUMENTS, int foreground); 
void wc_wrapper(_ARGUMENTS, int foreground); 

void api_createFifo(_ARGUMENTS);
void api_createReg(_ARGUMENTS);
void api_unlink(_ARGUMENTS);
void api_open(_ARGUMENTS);
void api_close(_ARGUMENTS);
void api_read(_ARGUMENTS);
void api_write(_ARGUMENTS);
void api_dup(_ARGUMENTS);
void api_dup2(_ARGUMENTS);
void api_printFileContent(_ARGUMENTS);
void api_printFileInfo(_ARGUMENTS);
void api_printFdTableByPid(_ARGUMENTS);


void sem(_ARGUMENTS,int foreground);
void test_sync_wrapper(_ARGUMENTS , int foreground );
void test_no_sync_wrapper(_ARGUMENTS , int foreground );
void test_mm_wrapper(_ARGUMENTS,int foreground);
void test_prio_wrapper(_ARGUMENTS,int foreground);
#endif