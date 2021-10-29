#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "utils.h"

#define IN_RANGE(x, a, b) ((x) >= (a) && (x) <= (b))
#define ISUPPER(x) IN_RANGE(x, 'A', 'Z')
#define ISLOWER(x) IN_RANGE(x, 'a', 'z')
#define ISALPHA(x) (ISUPPER(x) || ISLOWER(x))
#define ISDIGIT(x) IN_RANGE(x, '0', '9')
//#define _ARGUMENTS int argc, char * argv[]

typedef struct dateType {
	uint8_t year, month, day;
	uint8_t hour, minute, second;
} dateType;

// C
int print_f(uint8_t fd, const char * format, ...);
int testPrint_f(uint8_t fd, const char * format, ...);
void put_char(uint8_t fd, const char character);
void put_s(uint8_t fd, const char * s);
int sscan(char * buff, char * fmt, ...) ;
int64_t strtoint(const char *nptr, char **endptr, register int base);
double strtodouble(char * start, char ** end);
int64_t get_s(char * buffer, uint64_t maxLength);

// ASM
void setReg(void);
uint64_t fillDate(dateType * pDate);
void inforeg(/*_ARGUMENTS*/);
int64_t fillMem(uint64_t src, uint8_t * dst, uint8_t size);
int _quadratic(double * a, double * b, double * c, double * root1, double * root2);
void * memalloc(uint64_t size); 
void memfree(void * pv); 
int getProcessesData(processDescriptor * despriptorArray);
int getpid();

int createReg(char* name);
int createFifo(char* name);
int open(int pid, char* name, int mode);
int close(int pid, int fd);
int read(int pid, int fd, char* buf, int count);
int write(int pid, int fd, char* buf, int count);
int unlink(char* name);
int getFileContent(char* name, char* buf);
int getFileInfo(char* name, fileInfo* buf);
int dup(int pid, int oldVirtualFd);
int dup2(int pid, int oldVirtualFd, int newVirtualFd);
int getFdTableByPid(int pid, int* fbBuff);

#endif