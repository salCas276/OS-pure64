/* sampleCodeModule.c */
#include <stdint.h>
#include <lib.h>
#include <utils.h>
#include <exceptions.h>
#include <string.h>

#include <stdio.h>
//#include <stdlib.h>
#include <stdarg.h>

#define MAX_COMMAND 19 // Habria que achicarlo
#define MODULES_SIZE 8

typedef void (*commandType)(void);

static char * commandStrings[MODULES_SIZE] = {
	"help",
	"inforeg",
	"printmem",
	"printDate",
	"divisionByZero",
	"invalidOpcode",
	"printFeatures",
	"printQuadraticRoots"
};
static commandType commandFunctions[MODULES_SIZE] = {
	help,
	inforeg,
	printmem,
	printDate,
	throwDivisionByZeroException,
	throwInvalidOpcodeException,
	printFeatures,
	printQuadraticRoots
};

void checkModule(char * string);

int main() {
	char buffer[MAX_COMMAND + 1];
	//int32_t counter = 0;

	print_f(1, "Estamos en userland.\n");
	// help();


	print_f(1, "--------First round------\n");

	void * ary[20] = {0}; 
	void * aux; 
	int i = 0; 
	do {
		aux = memalloc(0x10000); 
		ary[i++] = aux; 
	} while (aux!=NULL); 

	for (int i=0; i<20; i++)
		print_f(2, "%d\n", ary[i]);

	for (int i=0; i<20; i++)
		memfree(ary[i]); 

	print_f(1, "--------Second round------\n");

	void * ary2[20] = {0}; 
	void * aux2; 
	int i2 = 0; 
	do {
		aux2 = memalloc(0x10000); 
		ary2[i2++] = aux2; 
	} while (aux2!=NULL); 

	for (int i2=0; i2<20; i2++)
		print_f(2, "%d\n", ary2[i2]);



	while(1) {
		print_f(2, "\n>> ");
		int64_t ans = get_s(buffer, MAX_COMMAND);
		if (ans != -1)
			checkModule(buffer);
		else
			print_f(1, "Comando no valido\n");
	}
}

void checkModule(char * string) {
	for (int i = 0; i < MODULES_SIZE; i++){
		if (!strcmp(string, commandStrings[i])){
			commandFunctions[i]();
			return;
		}
	}
	print_f(1, "Comando no valido\n");
}
