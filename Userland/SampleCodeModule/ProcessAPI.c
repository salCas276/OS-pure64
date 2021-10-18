#include <ProcessAPI.h>

uint64_t createProcessAsm(uint64_t function);

uint64_t createProcessUserland(uint64_t function){
    return createProcessAsm(function); 
}