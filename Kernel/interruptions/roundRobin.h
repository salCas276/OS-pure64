 #ifndef _ROUNDROBIN_H_
#define _ROUNROBIN_H_



#include <stdint.h>
#include "../include/video.h"

#define WORSTPRIORITY 40
#define MAXBLOCKTYPES 40
#define MAX_PFD 40
#define _ARGUMENTS  int argc,char **argv

typedef struct processControlBlock {
    uint8_t pid;
    int parentPid;
    int quantityWaiting;
    uint64_t taskRSP;
    uint64_t functionAddress; //no deberia ser necesario
    uint64_t baseRSP; //no deberia ser necesario 
    char priority; // between 0 and WORSTPRIORITY. 0 = HIGHPRIORITY 
    char currentPushes; 
    prompt_info prompt;
    struct processControlBlock * tail; 
    int processFileDescriptors[MAX_PFD];
} processControlBlock;

typedef struct processDescriptor{
    uint8_t pid;
} processDescriptor;

void addProcess(processControlBlock * process);

//void removeProcess(uint64_t pid);

void nextTask();

processControlBlock * getCurrentTask();

void setCurrentRSP(uint64_t rsp) ;

uint64_t getCurrentRSP();

prompt_info * getCurrentPrompt();

int getCurrentPid();

int getCurrentMinFd();

int changeNicenessBy(uint64_t pid, uint64_t deltaNice); 

int killProcess(int pid);

int blockProcess(int pid, int password);

int blockMyself(int password);

int unblockProcess(int pid, int password);

int renounce(void); 

void popAndUnblock(int password);

void printBlockedBy(int password);

int getAvailablePassword();

#endif
