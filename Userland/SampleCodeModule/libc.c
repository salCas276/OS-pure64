// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "include/string.h"

#define BUFF_SIZE 256

static uint64_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2){
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

int print_f(uint8_t fd, const char * format, ...) {
    char writtingBuff[BUFF_SIZE] = {0};
    va_list arg;
    va_start(arg, format);
    
    int64_t i;
    char buff[24];
    char * s;
    const char * traverse;
    double g;
    
    for(traverse = format; *traverse != '\0'; traverse++) {
        while(*traverse != '%') {
            if(*traverse == '\0') {
                va_end(arg);
                write(-1, fd, writtingBuff, strlen(writtingBuff));
                return (traverse - format) / sizeof(traverse);
            }
            charcat(*traverse, writtingBuff);
            traverse++;
        }

        traverse++;

        switch(*traverse) {
            case 'c':
                i = va_arg(arg, int);
                charcat(i, writtingBuff);
                break;
            case 's':
                s = va_arg(arg, char *);
                strcat(s, writtingBuff);
                break;
            case 'd':
                i = (int32_t)va_arg(arg, int64_t);
                if (i < 0) {
                    i = -i;
                    charcat('-', writtingBuff);
                }
                if(uintToBase(i, buff, 10) > 0) strcat(buff, writtingBuff);
                break;
            case 'x':
                i = va_arg(arg, int64_t);
                if(uintToBase(i, buff, 16) > 0) strcat(buff, writtingBuff);
                break;
            case 'g':
                g = va_arg(arg, double);
                if (g < 0) {
                    g = -g;
                    charcat('-', writtingBuff);
                }
                i = (uint64_t)g;
                if (uintToBase(i, buff, 10) > 0) strcat(buff, writtingBuff);
                charcat('.', writtingBuff);
                g -= i;
                i = (uint64_t)(g * 1000); // cantidad de decimales significativos (1000 -> 3 decimales significativos)
                if (uintToBase(i, buff, 10) > 0) strcat(buff, writtingBuff);
                break;
            case '\0':
                va_end(arg);
                write(-1, fd, writtingBuff, strlen(writtingBuff));
                return (traverse - format) / sizeof(traverse);
        }

    }
    va_end(arg);
    write(-1, fd, writtingBuff, strlen(writtingBuff));
    return (traverse - format) / sizeof(traverse);
}


// https://code.woboq.org/gcc/libiberty/strtol.c.html
int64_t strtoint(const char *nptr, char **endptr, register int base) {
        register const char *s = nptr;
        register int64_t acc;
        register int c;
        register unsigned long cutoff;
        register int neg = 0, any, cutlim;

        do {
            c = *s++;
        } while (c == ' ');
        
        if (c == '-') {
                neg = 1;
                c = *s++;
        } else if (c == '+')
                c = *s++;
        if ((base == 0 || base == 16) &&
            c == '0' && (*s == 'x' || *s == 'X')) {
                c = s[1];
                s += 2;
                base = 16;
        }
        if (base == 0)
                base = c == '0' ? 8 : 10;

        cutoff = neg ? -(unsigned long) LONG_MIN : LONG_MAX;
        cutlim = cutoff % (unsigned long)base;
        cutoff /= (unsigned long)base;
        for (acc = 0, any = 0;; c = *s++) {
                if (ISDIGIT(c))
                        c -= '0';
                else if (ISALPHA(c))
                        c -= ISUPPER(c) ? 'A' - 10 : 'a' - 10;
                else
                        break;
                if (c >= base)
                        break;
                if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
                        any = -1;
                else {
                        any = 1;
                        acc *= base;
                        acc += c;
                }
        }
        if (any < 0) {
                acc = neg ? LONG_MIN : LONG_MAX;
        } else if (neg)
                acc = -acc;
        if (endptr != 0)
                *endptr = (char *) (any ? s - 1 : nptr);
        return (acc);
}
