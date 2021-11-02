#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>

uint8_t strcmp(const char *p1, const char *p2);
uint8_t strlen(const char *str);
char * strcpy(char *strDest, const char *strSrc);
int strcat(const char* src, char* dest);
int charcat(char c, char* dest);

#endif