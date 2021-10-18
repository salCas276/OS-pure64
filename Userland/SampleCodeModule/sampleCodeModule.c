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
	help();

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
