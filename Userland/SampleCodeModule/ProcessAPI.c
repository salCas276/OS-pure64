#include <ProcessAPI.h>

uint64_t createProcessAsm(uint64_t function);
uint64_t niceAsm(uint64_t pid, uint64_t deltaNice); 


uint64_t createProcessUserland(uint64_t function){
    return createProcessAsm(function); 
}

uint64_t nice(int pid, int deltaNice) {
    return niceAsm(pid, deltaNice); 
}
