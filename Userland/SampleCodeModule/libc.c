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

// http://www.firmcodes.com/write-printf-function-c/
int testPrint_f(uint8_t fd, const char * format, ...) {
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
                return (traverse - format) / sizeof(traverse);
            }
            put_char(fd, *traverse);
            traverse++;
        }

        traverse++;

        switch(*traverse) {
            case 'c':
                i = va_arg(arg, int);
                put_char(fd, i);
                break;
            case 's':
                s = va_arg(arg, char *);
                put_s(fd, s);
                break;
            case 'd':
                i = (int32_t)va_arg(arg, int64_t);
                if (i < 0) {
                    i = -i;
                    put_char(fd, '-');
                }
                if(uintToBase(i, buff, 10) > 0) put_s(fd, buff);
                break;
            case 'x':
                i = va_arg(arg, int64_t);
                if(uintToBase(i, buff, 16) > 0) put_s(fd, buff);
                break;
            case 'g':
                g = va_arg(arg, double);
                if (g < 0) {
                    g = -g;
                    put_char(fd, '-');
                }
                i = (uint64_t)g;
                if (uintToBase(i, buff, 10) > 0) put_s(fd, buff);
                put_char(fd, '.');
                g -= i;
                i = (uint64_t)(g * 1000); // cantidad de decimales significativos (1000 -> 3 decimales significativos)
                if (uintToBase(i, buff, 10) > 0) put_s(fd, buff);
                break;
            case '\0':
                va_end(arg);
                return (traverse - format) / sizeof(traverse);
        }

    }
    va_end(arg);
    return (traverse - format) / sizeof(traverse);
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
                write(-1, fd, writtingBuff, BUFF_SIZE);
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
                write(-1, fd, writtingBuff, BUFF_SIZE);
                return (traverse - format) / sizeof(traverse);
        }

    }
    va_end(arg);
    write(-1, fd, writtingBuff, BUFF_SIZE);
    return (traverse - format) / sizeof(traverse);
}

void put_char(uint8_t fd,  char character) {
    write(-1, fd, &character, 1);
}

void put_s(uint8_t fd, const char * s) {
    write(-1, fd, s, BUFF_SIZE);
}

// maxLength = Letras a leer sin contar el \0 al final

int64_t get_s(char * buffer, uint64_t maxLength) {
    int32_t counter = 0;
    int64_t c[2];
    while ((*c = read(-1, 0, c, 1)) != '\n') {
        if (counter < maxLength) {
            if (*c == '\b') { // Backspace
                if (counter == 0)
                    continue;
                counter--;
            } else {
                buffer[counter++] = *c;
            }
            put_char(1, *c);
        } else {
            if (*c == '\b')
                counter--;
            else
                counter++;
            put_char(1, *c);
        }
    }

    put_char(1, *c);

    if (counter > maxLength) {
        buffer[maxLength] = '\0';
        return -1;
    }

    buffer[counter] = '\0';
    return counter;
}

// https://iq.opengenus.org/how-printf-and-scanf-function-works-in-c-internally/
int sscan(char * buff, char * fmt, ...) {
    va_list vl;
    int i = 0, j = 0, ret = 0;
    char *out_loc;
 	va_start(vl, fmt);
 	while (fmt && fmt[i]) {
 	    if (fmt[i] == '%') {
 	       i++;
 	       switch (fmt[i]) {
 	            case 'c': {
	 	            *(char *)va_arg(vl, char*) = buff[j];
	 	            j++;
	 	            ret ++;
	 	            break;
 	            }
 	            case 'd': {
	 	            *(int64_t *)va_arg(vl, int64_t*) = strtoint(&buff[j], &out_loc, 10);
	 	            j += out_loc -&buff[j];
	 	            ret++;
	 	            break;
 	            }
 	            case 'x': {
                    *(int64_t *)va_arg(vl, int64_t*) = strtoint(&buff[j], &out_loc, 16);
                    j += out_loc -&buff[j];
                    ret++;
	 	            break;
 	            }
                case 'g': {
                    *(double *)va_arg(vl, double*) = strtodouble(&buff[j], &out_loc);
                    j += out_loc - &buff[j];
                    ret++;
                    break;
                }
                
 	        }
 	    } 
 	    else {
 	        buff[j] = fmt[i];
            j++;
        }
        i++;
    }
    va_end(vl);

    return ret;
}

double strtodouble( char * start, char ** end) {
    char flagOnDecimal = 0;
    char flagNegative = 0;
    double ans = 0;
    double decimalPower = 1;
    
    while (*start == ' ') {
        start++;
    }

    if (*start == '-') {
        flagNegative = 1;
        start++;
    }
    while (ISDIGIT(*start) || *start == '.') {
        if (*start == '.') {
            if (flagOnDecimal)
                break;
            else
                flagOnDecimal = 1;
        } else { // Digito
            if (! flagOnDecimal) {
                ans = ans * 10 + (*start - '0');
            } else {
                decimalPower /= 10;
                ans += (*start - '0') * decimalPower;
            }
        }
        start++;
    }
    *end = start;
    return (flagNegative) ? -ans : ans;
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
