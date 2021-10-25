/* sampleCodeModule.c */
#include <stdint.h>
#include <lib.h>
#include <utils.h>
#include <exceptions.h>
#include <ProcessApi.h>
#include <string.h>
#include "./include/lib.h"

#include <stdio.h>
//#include <stdlib.h>
#include <stdarg.h>

#define MODULES_SIZE 13

typedef void (*commandType)(int argc, char * argv[],int foreground);

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
	"testProcess",
	"sem"
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
	test_processes_wrapper,
	sem
	// test_sync,
	// test_no_sync
};

// void checkModule(char argv[MAX_ARGC][MAX_COMMAND], int argc); 
// static int parseArgv(char * buffer, char argv[MAX_ARGC][MAX_COMMAND]); 
static int parseArgvPtr(char * buffer , char *argv[MAX_ARGC+1]);
void checkModulePtr(char *argv[], int argc,int foreground);


int main() {
	 char buffer[MAX_COMMAND + 1];
	// char argv[MAX_ARGC][MAX_COMMAND]; 
	//int32_t counter = 0;

	// Queremos que nuestra shell tenga prioridad maxima 
	nice(0, -5); 
	// nice(0, -20); 


	print_f(1, "Estamos en userland.\n");

	//help(argv, 0);

	

	sem(0,(char**)0,0);

	while(1) {
		print_f(2, "\n>> ");
		int64_t ans = get_s(buffer, MAX_COMMAND);
		if (ans != -1) {
			char ** argv = (char**) memalloc( (MAX_ARGC+1) * sizeof(char*));	
			int argc = parseArgvPtr(buffer, argv);
			if(argc < 0 ){
				print_f(1,"Cantidad de parámetros inválida");
				for(int i = 0 ; i < MAX_ARGC+1;i++)
					memfree(argv[i]);
				memfree(argv);
				continue;
			}
			int foreground = 1 ; 
			if(argv[argc-1][0]=='&'){
				foreground = 0 ; 
			}
			checkModulePtr(argv, argc, foreground);
		} else
			print_f(1, "Comando no valido\n");
	 }

}




void checkModulePtr(char *argv[], int argc,int foreground) {
	for (int i = 0; i < MODULES_SIZE; i++){
		if (!strcmp(argv[0], commandStrings[i])){
			commandFunctions[i](argc, argv,foreground);
			return;
		}
	}; 
	print_f(1, "Comando no valido\n");
}


static int parseArgvPtr(char * buffer , char *argv[]){

	int currentBuffPos = 0; 
	int index=0;
  	int inside = 0 ;
	while( buffer[currentBuffPos]!=0){
		char * aux = (char*)memalloc( (MAX_COMMAND+1) * sizeof(char));
		int current = 0 ; 
	    inside = 0 ; 

    	while(current < MAX_COMMAND && buffer[currentBuffPos]!=0 && buffer[currentBuffPos]!=32 ){
			aux[current++]=buffer[currentBuffPos++];
      		inside = 1; 
		}

	    if(buffer[currentBuffPos]!=0)
  			currentBuffPos++;
    
		if(inside){
    	 	aux[current]=0;
			argv[index++]=aux;
    	}

		if(index == MAX_ARGC + 1) //cantidad de parametros invalida.
			return -1;


	}

	return index;
}





// void checkModule(char argv[MAX_ARGC][MAX_COMMAND], int argc) {
// 	for (int i = 0; i < MODULES_SIZE; i++){
// 		if (!strcmp(argv[0], commandStrings[i])){
// 			commandFunctions[i](argv, argc);
// 			return;
// 		}
// 	}; 
// 	print_f(1, "Comando no valido\n");
// }


// static int parseArgv(char * buffer, char argv[MAX_ARGC][MAX_COMMAND]) { 
// 	int currentArgv = 0, currentBuffPos = 0, currentArgvPos = 0; 
// 	while( buffer[currentBuffPos] != 0) {
// 		// End of the current argv
// 		if (buffer[currentBuffPos] == ' ') {
// 			argv[currentArgv][currentArgvPos] = 0; // cierro el parametro actual 
// 			currentArgvPos = 0; // reseteo el indicador de posicion 
// 			currentBuffPos++;  // Me muevo un lugar en el buffer 
// 			currentArgv ++; // Me muevo un argumento hacia la derecha
// 			if( currentArgv >= MAX_ARGC) return currentArgv; 
// 		}
// 		argv[currentArgv][currentArgvPos++] = buffer[currentBuffPos++]; 
// 	}
// 	argv[currentArgv][currentArgvPos] = 0;

// 	return currentArgv + 1; 
// }


