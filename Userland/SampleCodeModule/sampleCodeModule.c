/* sampleCodeModule.c */
#include <stdint.h>
#include <lib.h>
#include <utils.h>
#include <exceptions.h>
#include "include/processApi.h"
#include <string.h>
#include "./include/lib.h"
#include <stdio.h>
//#include <stdlib.h>
#include <stdarg.h>
#include "include/testSem.h"

#define MODULES_SIZE 30

typedef void (*commandType)(int argc, char * argv[],int foreground);
void piping(char * argv1[], int argc1, char * argv2[], int argc2);

static char * commandStrings[MODULES_SIZE] = {
	"help",
	"echo",
	"ps", 
	"nice", 
	"kill",
	"sem",

	"mkfifo",
	"mkreg",
	"unlink",
	"open",
	"close",
	"write",
	"read",
	"dup",
	"dup2",
	"printFileContent",
	"printFileInfo",
	"printFdTable",
	"pipe",

	"loop",
	"testProcess",
	"testSync",
	"testNoSync",
	"testMM",
	"testPriority",
	"cat",
	"wc", 
	"filter",
	"mem" 
};
static commandType commandFunctions[MODULES_SIZE] = {
	help,
	echo, 
	printProcessesData,
	nicecmd, 
	killcmd,
	sem,

	api_createFifo,
	api_createReg,
	api_unlink,
	api_open,
	api_close,
	api_write,
	api_read,
	api_dup,
	api_dup2,
	api_printFileContent,
	api_printFileInfo,
	api_printFdTableByPid,
	api_printFifosData,

	loop_wrapper,
	test_processes_wrapper,
	test_sync_wrapper,
	test_no_sync_wrapper,
	test_mm_wrapper,
	test_prio_wrapper,
	cat_wrapper, 
	wc_wrapper, 
	filter_wrapper, 
	printMemState, 
};

// void checkModule(char argv[MAX_ARGC][MAX_COMMAND], int argc); 
// static int parseArgv(char * buffer, char argv[MAX_ARGC][MAX_COMMAND]); 
static int parseArgvPtr(char * buffer , char *argv[MAX_ARGC+1], int * isPiped);
commandType checkModulePtr(char *argv[], int isPiped);//, int argc,int foreground);


int main() {
	 char buffer[MAX_COMMAND + 1];

	// char argv[MAX_ARGC][MAX_COMMAND]; 
	//int32_t counter = 0;

	// Queremos que nuestra shell tenga prioridad maxima 
	// nice(0, -5); 
	// nice(0, -20); 


	print_f(1, "Estamos en userland.\n");

	//test_sync_wrapper(0,0,1);
	// test_no_sync_wrapper(0,0,1);

	//help(argv, 0);





	
	// int i = 0; 
	// char * allocs[32] = {0};
	// while ( (allocs[i] = buddymalloc(32768-25)) != NULL) i ++; 


	// for (int i=0; i<32; i++)
	// 	print_f(1, "Alloc #%d 0x%x\n", i, allocs[i]); 


	// for (int i=0; i<32; i++) {
	// 	buddyfree(allocs[i]); 
	// }

	// print_f(2, "------------\n"); 

	// int j = 0; 
	// char * allocs2[32] = {0}; 
	// while ( (allocs2[j] = buddymalloc( (32768-25)*2)) != NULL) j++; 

	// for (int i=0; i<32; i++)
	// 	print_f(1, "Alloc 0x%x\n", allocs2[i]); 



	// print_f(2, "OK!\n"); 

	while(1) {
		print_f(2, "\n>> ");
		
		/*
		char* aux[] = {"loop"};
		loop_wrapper(1, aux, 0);
		write(-1, 1, "\n", 1);
		char* aux[] = {"loop"};
		loop_wrapper(1, aux, 0);
		char* aux2[] = {"pipe"};
		api_printFifosData(1, aux2);
		openSemaphore("pepe", 1);	
		openSemaphore("b", 1);	
		openSemaphore("a", 1);	
		openSemaphore("d", 1);	
		openSemaphore("t", 1);	
		openSemaphore("3", 1);	
		openSemaphore("z", 1);	
		openSemaphore("w", 1);	
		openSemaphore("y", 1);	
		char* aux2[] = {"kill", "-k=0", "1"};
		killcmd(3, aux2);
		loop_wrapper(1, aux, 0);
		loop_wrapper(1, aux, 0);
		loop_wrapper(1, aux, 0);
		loop_wrapper(1, aux, 0);
		loop_wrapper(1, aux, 0);
		loop_wrapper(1, aux, 0);
		char* aux1[] = {"loop", "r"};
		api_printFileInfo(2, aux1);
		open(-1, "r", 0);
		char* aux2[] = {"sem"};
		sem(1, aux2, 1);
		sem(1, aux2, 1);
		*/
		int64_t ans = read(-1, 0, buffer, -1);
		if (ans != -1 && buffer[0]!=0) {
			char ** argvLeft = (char**) memalloc( (MAX_ARGC+3) * sizeof(char*)); 
			char ** argvRight = (char**) memalloc( (MAX_ARGC+3) * sizeof(char*));	
	
		
			if(argvLeft == 0 || argvRight == 0) {
				print_f(1,"El sistema no tiene memoria disponible");
				break;
			}
			int isPiped = 0; 
			int argcLeft = parseArgvPtr(buffer, argvLeft, &isPiped); 
			int argcRight = 0;
			if (isPiped) { 
				isPiped += 2; 
				argcRight = parseArgvPtr(buffer, argvRight, &isPiped); 
			}
		

			if(argcLeft < 0 || argcRight < 0 ){
				print_f(1,"El sistema fallo parseando su comando : a)Cantidad de parametros invalida b)El sistema no tiene memoria disponible\n");
				for(int i = 0 ; i < MAX_ARGC+1;i++) {
					memfree(argvRight[i]); // Todo corregir esto 
					memfree(argvLeft[i]); 
				}
				memfree(argvRight);
				memfree(argvLeft);
				continue;
			}
			int foreground = ! (argvLeft[argcLeft-1][0]=='&'); 


			if (isPiped){ 
				piping(argvLeft, argcLeft, argvRight, argcRight);	
			} else {
				commandType cmd = checkModulePtr(argvLeft, 0);
				if ( cmd != 0 ) cmd(argcLeft, argvLeft, foreground) ; 
			} ;


		} else
			print_f(1, "Comando no valido\n");

	 }

}

void pipeWrapper(_ARGUMENTS, int foreground) {
	int isWriting = argv[argc-1]; 
	char * fifoName = argv[argc-2]; 
	commandType cmd = checkModulePtr(argv, 1);
	
	if(cmd)
		cmd(argc-2, argv, foreground); 

	exitUserland();
}


void piping(char * argv1[], int argc1, char * argv2[], int argc2) {
	static int id = 0;
	char * fifoName = memalloc(sizeof(char)*3); 
	fifoName[0] = 'F';
	fifoName[1] = id+'!'; 
	fifoName[2] = 0; 

	argv1[argc1++] = fifoName; 
	argv1[argc1++] = 1; 

	argv2[argc2++] = fifoName; 
	argv2[argc2++] = 0; 

	if(!checkModulePtr(argv1, 1) || !checkModulePtr(argv2, 1)){
		memfree(fifoName);
		return;
	}


	createFifo(fifoName); 



	int oldFdW = open(-1, fifoName, 1);
	if(oldFdW == -1)
		return;

	int auxW = dup(-1, 1);
	
	dup2(-1, oldFdW, 1);
	close(-1, oldFdW);

	createProcessUserland((uint64_t) &pipeWrapper, argc1, argv1, 1);

	dup2(-1, auxW, 1);
	close(-1, auxW);

	int oldFdR = open(-1, fifoName, 0);
	if(oldFdR == -1)
		return;
	
	int auxR = dup(-1, 0);

	dup2(-1, oldFdR, 0);
	close(-1, oldFdR);

	createProcessUserland((uint64_t) &pipeWrapper, argc2, argv2, 1); 

	dup2(-1, auxR, 0);
	close(-1, auxR);


	
	waitSon();
	waitSon();

	unlink(fifoName);

	memfree(fifoName);

}


commandType checkModulePtr(char *argv[], int isPiped) {
	for (int i = 0; i < MODULES_SIZE; i++){
		if (!strcmp(argv[0], commandStrings[i])){
			return commandFunctions[i];
		}
	}; 
	print_f(1, "Comando no valido\n");
	return 0; 
}
// void checkModulePtr(char *argv[], int argc,int foreground) {
// 	for (int i = 0; i < MODULES_SIZE; i++){
// 		if (!strcmp(argv[0], commandStrings[i])){
// 			commandFunctions[i](argc, argv,foreground);
// 			return;
// 		}
// 	}; 
// 	print_f(1, "Comando no valido\n");
// }


static int parseArgvPtr(char * buffer , char *argv[], int * isPiped){

	int currentBuffPos = *isPiped; 
	int index=0;
  	int inside = 0 ;
	while( buffer[currentBuffPos]!=0 && buffer[currentBuffPos] != '|'){
		char * aux = (char*)memalloc( (MAX_COMMAND+1) * sizeof(char));
		if(aux == 0)
			return -1;

		int current = 0 ; 
	    inside = 0 ; 

    	while(current < MAX_COMMAND && buffer[currentBuffPos]!=0 && buffer[currentBuffPos]!=' ' ){
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
	if (buffer[currentBuffPos] == '|') *isPiped = currentBuffPos; 
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


