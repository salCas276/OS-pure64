/* sampleCodeModule.c */
#include <stdint.h>
#include <lib.h>
#include <utils.h>
#include <exceptions.h>
#include "include/processApi.h"
#include <string.h>

#include <stdio.h>
//#include <stdlib.h>
#include <stdarg.h>

#define MODULES_SIZE 15

typedef void (*commandType)(char argv[MAX_ARGC][MAX_COMMAND], int argc);

static char * commandStrings[MODULES_SIZE] = {
	"help",
	"inforeg",
	"printmem",
	"printDate",
	// "divisionByZero",
	// "invalidOpcode",
	"printFeatures",
	"printQuadraticRoots",
	"echo",
	"printHola",
	"ps", 
	"nice", 
	"kill",
	"mkfifo",
	"mkfile",
	"printFileContent",
	"printFileInfo"
};
static commandType commandFunctions[MODULES_SIZE] = {
	help,
	inforeg,
	printmem,
	printDate,
	// throwDivisionByZeroException,
	// throwInvalidOpcodeException,
	printFeatures,
	printQuadraticRoots,
	echo, 
	printHola,
	printProcessesData,
	nicecmd, 
	killcmd,
	createFifo,
	createFile,
	printFileContent,
	printFileInfo
};

void checkModule(char argv[MAX_ARGC][MAX_COMMAND], int argc); 
static int parseArgv(char * buffer, char argv[MAX_ARGC][MAX_COMMAND]); 


int main() {
	char buffer[MAX_COMMAND + 1];
	char argv[MAX_ARGC][MAX_COMMAND]; 
	//int32_t counter = 0;

	// Queremos que nuestra shell tenga prioridad maxima 
	nice(0, -20); 
	nice(0, -20); 

	print_f(1, "Estamos en userland.\n");
	help(argv, 0);

	while(1) {
		print_f(2, "\n>> ");
		int64_t ans = get_s(buffer, MAX_COMMAND);
		if (ans != -1) {
			int argc = parseArgv(buffer, argv);
			checkModule(argv, argc);
		} else
			print_f(1, "Comando no valido\n");
	}
}

void checkModule(char argv[MAX_ARGC][MAX_COMMAND], int argc) {
	for (int i = 0; i < MODULES_SIZE; i++){
		if (!strcmp(argv[0], commandStrings[i])){
			commandFunctions[i](argv, argc);
			return;
		}
	}; 
	print_f(1, "Comando no valido\n");
}

static int parseArgv(char * buffer, char argv[MAX_ARGC][MAX_COMMAND]) { 
	int currentArgv = 0, currentBuffPos = 0, currentArgvPos = 0; 
	while( buffer[currentBuffPos] != 0) {
		// End of the current argv
		if (buffer[currentBuffPos] == ' ') {
			argv[currentArgv][currentArgvPos] = 0; // cierro el parametro actual 
			currentArgvPos = 0; // reseteo el indicador de posicion 
			currentBuffPos++;  // Me muevo un lugar en el buffer 
			currentArgv ++; // Me muevo un argumento hacia la derecha
			if( currentArgv >= MAX_ARGC) return currentArgv; 
		}
		argv[currentArgv][currentArgvPos++] = buffer[currentBuffPos++]; 
	}
	argv[currentArgv][currentArgvPos] = 0;
	return currentArgv + 1; 
}


