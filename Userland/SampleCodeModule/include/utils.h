#ifndef __UTILS_H__
#define __UTILS_H__

#define BYTES_TO_READ 32
#include <stdint.h>

typedef struct processDescriptor{
    uint8_t pid;
} processDescriptor;

void printDate(void);
void help(void);
void printmem(void);
void printFeatures(void);
void printQuadraticRoots(void);
void printProcessesData(void);

void printHola(void);

#endif