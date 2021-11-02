#include <processApi.h>

uint64_t createProcessAsm(uint64_t function,_ARGUMENTS,int foreground);
uint64_t niceAsm(uint64_t pid, uint64_t deltaNice); 
uint64_t killAsm(uint64_t signal, uint64_t pid); 
uint64_t renounceAsm(void); 
void exitAsm();
void waitAsm();
uint64_t getPidAsm();

int createProcessUserland(uint64_t function,_ARGUMENTS,int foreground){
    return createProcessAsm(function,argc,argv,foreground); 
}

uint64_t nice(int pid, int deltaNice) {
    return niceAsm(pid, deltaNice); 
}

uint64_t kill(int signal, int pid) {
    return killAsm(signal, pid); 
}

uint64_t renounceUserland(void) {
    return renounceAsm(); 
} 

void exitUserland(){
    exitAsm();
}


void waitSon(){
    waitAsm();
}


uint64_t getPid(){
    return getPidAsm();
}
