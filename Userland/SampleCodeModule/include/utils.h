#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 100 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define MAX_SIZE_BLOCK 256
#define MAX_PRINTING_FILES 40
#define BUFFER_SIZE 128
#define _ARGUMENTS int argc , char * argv[]
// #define NULL 0
#include <stdint.h>
//#include <test_util.h>


typedef struct processDescriptor{
    char * name ;
    uint8_t pid;
    uint8_t priority;
    uint8_t foreground;
    int stackPointer;
    int basePointer;
} processDescriptor;

typedef struct semaphoreDescriptor{
    int value;
    char * sem_id;
    int * blocked;
} semaphoreDescriptor;

typedef struct fifoData{
    char name[BUFFER_SIZE];
    int indexes[2];
    int openCount, writeOpenCount;
    int forUnlink;
    int blockedPids[BUFFER_SIZE];
} fifoData;

typedef struct fileInfo{
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int fileType; //0 -> File, 1 -> Fifo
    int forUnlink; //Flag que me indica si se le hizo un unlink
} fileInfo;




void printDate(_ARGUMENTS);
void help(_ARGUMENTS,int foreground);
void printmem(_ARGUMENTS);
void printFeatures(_ARGUMENTS);
void printQuadraticRoots(_ARGUMENTS);
void echo(_ARGUMENTS,int foreground); 
void printProcessesData(_ARGUMENTS,int foreground);
void loop_wrapper(_ARGUMENTS,int foreground);
void nicecmd(_ARGUMENTS,int foreground); 
void killcmd(_ARGUMENTS,int foreground); 
void test_processes_wrapper(_ARGUMENTS,int foreground);
void cat_wrapper(_ARGUMENTS, int foreground); 
void filter_wrapper(_ARGUMENTS, int foreground); 
void wc_wrapper(_ARGUMENTS, int foreground); 
void printMemState(_ARGUMENTS, int foreground); 


void loop(_ARGUMENTS);
void test_processes(_ARGUMENTS);
void test_sync(_ARGUMENTS);
void test_mm(_ARGUMENTS);
void test_prio(_ARGUMENTS);
void cat(_ARGUMENTS);
void wc(_ARGUMENTS);
void filter(_ARGUMENTS);

void api_createFifo(_ARGUMENTS,int foreground);
void api_createReg(_ARGUMENTS,int foreground);
void api_unlink(_ARGUMENTS,int foreground);
void api_open(_ARGUMENTS,int foreground);
void api_close(_ARGUMENTS,int foreground);
void api_read(_ARGUMENTS,int foreground);
void api_write(_ARGUMENTS,int foreground);
void api_dup(_ARGUMENTS,int foreground);
void api_dup2(_ARGUMENTS,int foreground);
void api_printFileContent(_ARGUMENTS,int foreground);
void api_printFileInfo(_ARGUMENTS,int foreground);
void api_printFdTableByPid(_ARGUMENTS,int foreground);
void api_printFifosData(_ARGUMENTS,int foreground);


void sem(_ARGUMENTS,int foreground);
void test_sync_wrapper(_ARGUMENTS , int foreground );
void test_no_sync_wrapper(_ARGUMENTS , int foreground );
void test_mm_wrapper(_ARGUMENTS,int foreground);
void test_prio_wrapper(_ARGUMENTS,int foreground);

void defensa(_ARGUMENTS, int foreground); 
#endif