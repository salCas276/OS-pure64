#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#define MAX_COMMAND 100 // Habria que achicarlo
#define MAX_ARGC 5 // El nombre del comando es un parametro... 
#define _ARGUMENTS int argc , char * argv[]
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
void sem(_ARGUMENTS,int foreground);
void test_sync_wrapper(_ARGUMENTS , int foreground );
void test_no_sync_wrapper(_ARGUMENTS , int foreground );
void test_mm_wrapper(_ARGUMENTS,int foreground);
void test_prio_wrapper(_ARGUMENTS,int foreground);
#endif