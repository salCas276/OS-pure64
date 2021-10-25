#include <syscalls.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <lib.h>
#include <keyboard.h>
#include <video.h>
#include <memoryManager.h>
#include <Process.h>
#include <semaphore.h>

#define SYSCALL_BLOCK_PASSWORD 0 

//#include <process.h>

typedef struct dateType {
	uint8_t year, month, day;
	uint8_t hour, minute, second;
} dateType;

uint64_t sys_write(uint8_t fd, char * buffer, uint64_t count);
int64_t sys_read(void);
uint64_t sys_date(dateType * pDate);
uint64_t sys_mem(uint64_t rdi, uint64_t rsi, uint8_t rdx);
uint64_t sys_malloc(uint64_t size);
uint64_t sys_free(uint64_t pv); 
uint64_t sysOpenSemaphore(uint64_t rdi , uint64_t rsi );
uint64_t sysWaitSemaphore(uint64_t rdi );
uint64_t sysPostSemaphore(uint64_t rdi );
uint64_t sysCloseSemaphore(uint64_t rdi );
uint64_t sys_kill(uint64_t code, uint64_t pid); 
void sysExit();
void sysWait();
void printSemaphore();

// TODO: Usar un arreglo y no switch case
uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx,int foreground) {
	switch(rcx) {
		case 1: return sys_write(rdi, (char*)rsi, rdx);
		case 2: return sys_read();
		case 3: return sys_date((dateType *)rdi);
		case 4: return sys_mem(rdi, rsi, rdx);
		case 5 : return createProcess(rdi,rsi,(char**)rdx,foreground);
		case 6 : return getProcessesData(rdi);
		case 7 : return getCurrentPid();
		case 8: return sys_malloc(rdi); 
		case 9: return sys_free(rdi); 
		case 16: return sysOpenSemaphore(rdi,rsi);
		case 13 : return sysWaitSemaphore(rdi);
		case 14 : return sysPostSemaphore(rdi);
		case 15 : return sysCloseSemaphore(rdi);
		case 17 :  sysExit(); break;
		case 18 : sysWait();break;
		case 19 : printSemaphore();break;

		case 10: return changeNicenessBy(rdi, rsi); 
		case 11: return sys_kill(rdi, rsi);
		case 12: return renounce();break;  
	}
	return 0;
}

void sysWait(){
	return wait();
}


void sysExit(){
	return exit();
}

uint64_t sys_write(uint8_t fd, char * buffer, uint64_t count) {
	if (buffer == 0 || count <= 0)
		return -1;
		
	if (fd > 2)
		return -1;

	Color * fontColor = (fd == STD_ERR) ? &RED : &WHITE;
    
	for (int i = 0; i < count && buffer[i]; i++)
		ncPrintCharAtt(buffer[i], fontColor, &BLACK);
	
	return count;
}

int64_t sys_read(void) {
  	return getChar();
}

uint8_t BCDToDec(uint8_t bcd) {
	return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

uint64_t sys_date(dateType * pDate){
	if (pDate == 0)
		return 0;
	pDate->year = BCDToDec(getYear());
	pDate->month = BCDToDec(getMonth());
	pDate->day = BCDToDec(getDay());
	pDate->hour = BCDToDec(getHour());
	pDate->minute = BCDToDec(getMinute());
	pDate->second = BCDToDec(getSecond());
	return 1;
}

uint64_t sys_mem(uint64_t rdi, uint64_t rsi, uint8_t rdx){
	uint8_t * src = (uint8_t *)rdi;
	uint8_t * dst = (uint8_t *)rsi;
	
	// qemu tiene 64GB mapeados en memoria, asi que en el emulador
	// incluso con sólo 512MB de memoria
	// Podés acceder a todas las direcciones hasta 0x1000000000 - 1
	if (src >= (uint8_t *) 0x1000000000 || src - 1 + rdx >= (uint8_t *)0x1000000000)
		return 1;

	uint8_t i;
	for (i = 0; i < rdx; i++) 
		dst[i] = src[i];
	
	return i;
}

uint64_t sys_malloc(uint64_t size) {
	return (uint64_t)malloc((unsigned int) size); 
}

uint64_t sys_free(uint64_t pv) {
	free((void *) pv); 
	return 0; 
}



uint64_t sysOpenSemaphore(uint64_t rdi , uint64_t rsi){
	return semOpen( (char*) rdi , rsi);
}

uint64_t sysWaitSemaphore(uint64_t rdi ){
	return semWait( (char*) rdi);
 
}

uint64_t sysPostSemaphore(uint64_t rdi ){
	return semPost( (char*) rdi);
 
}

uint64_t sysCloseSemaphore(uint64_t rdi ){
	return semClose( (char*) rdi);
}
 
uint64_t sys_kill(uint64_t code, uint64_t pid) {
	switch(code) {
		case 0: return deleteProcess(pid); 
		case 1: return blockProcess(pid, SYSCALL_BLOCK_PASSWORD); 
		case 2: return unblockProcess(pid, SYSCALL_BLOCK_PASSWORD); 
	}
	return -1; 
}