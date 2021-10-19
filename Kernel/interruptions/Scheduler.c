#include <scheduler.h>
#include "../include/memoryManager.h"


//call when scheduler executes. Selects next Process.
void timer_handler(void){
    nextTask();
}










