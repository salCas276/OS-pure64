#include <syscalls.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <lib.h>
#include <keyboard.h>
#include <video.h>
#include <memoryManager.h>
#include <process.h>
#include "../include/fileSystem.h"
#include "../include/libfifo.h"

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
uint64_t sys_kill(uint64_t code, uint64_t pid); 
uint64_t sys_createFile(uint64_t name);
uint64_t sys_createFifo(uint64_t name);
uint64_t sys_open(uint64_t name, uint64_t mode);
uint64_t sys_getFileContent(uint64_t name, uint64_t buf);
uint64_t sys_getFileInfo(uint64_t name, uint64_t buf);
uint64_t sys_close(uint64_t fd);
uint64_t sys_unlink(uint64_t name);
uint64_t sys_dup(uint64_t oldVirtualFd, uint64_t buf, int* count);
uint64_t sys_dup2(uint64_t oldVirtualFd, uint64_t newVirtualFd, uint64_t buf, int* count);


// TODO: Usar un arreglo y no switch case
uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx) {
	switch(rcx) {
		case 1: return sys_write(rdi, (char*)rsi, rdx);
		case 2: return sys_read();
		case 3: return sys_date((dateType *)rdi);
		case 4: return sys_mem(rdi, rsi, rdx);
		case 5 : return createProcess(rdi);
		case 6 : return getProcessesData(rdi);
		case 7 : return getCurrentPid();
		case 8: return sys_malloc(rdi); 
		case 9: return sys_free(rdi); 
		case 10: return changeNicenessBy(rdi, rsi); 
		case 11: return sys_kill(rdi, rsi);
		case 12: return renounce();  
		case 20: return sys_createFile(rdi);
		case 21: return sys_createFifo(rdi);
		case 22: return sys_open(rdi, rsi);
		case 23: return sys_close(rdi);
		case 24: return sys_unlink(rdi);
		case 25: return sys_getFileContent(rdi, rsi);
		case 26: return sys_getFileInfo(rdi, rsi);
		case 27: return sys_dup(rdi, rsi, (int*) rdx);
		case 28: return sys_dup2(rdi, rsi, rdx, (int*) rcx);
	}
	return 0;
}

uint64_t sys_write(uint8_t fd, char * buffer, uint64_t count) {
	if (buffer == 0 || count <= 0)
		return -1;
		
	/*
	if (fd > 2)
		return -1;

	Color * fontColor = (fd == STD_ERR) ? &RED : &WHITE;
    
	for (int i = 0; i < count && buffer[i]; i++)
		ncPrintCharAtt(buffer[i], fontColor, &BLACK);
	
	return count;
	*/
	return writeFile(fd, buffer, count);
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

uint64_t sys_kill(uint64_t code, uint64_t pid) {
	switch(code) {
		case 0: return killProcess(pid); 
		case 1: return blockProcess(pid, 0); 
		case 2: return unblockProcess(pid, 0); 
	}
	return -1; 
}

uint64_t sys_createFile(uint64_t name){
	return createFile((char*) name, 0);
}

uint64_t sys_createFifo(uint64_t name){
	return createFile((char*) name, 1);
}

uint64_t sys_open(uint64_t name, uint64_t mode){
	return openFile((char*) name, (int) mode);
}

uint64_t sys_getFileContent(uint64_t name, uint64_t buf){
	int inodeIndex;
	char * auxBuf = (char*) buf;
	inode* targetInode = getInode((char*)name, &inodeIndex);
	if(targetInode == (inode*)-1)
		return -1;
	int i;
	for(i=0; i<=targetInode->indexes[1]; i++)
		auxBuf[i] = targetInode->block[targetInode->indexes[0]+i];
	return i;	
}

uint64_t sys_getFileInfo(uint64_t name, uint64_t inodeBuf){
	int inodeIndex;
	inode* targetInode = getInode((char*)name, &inodeIndex);
	if(inodeIndex == -1){
		return -1;
	}
	fileInfo* auxInodeBuf = (fileInfo*) inodeBuf;
	auxInodeBuf->fileType = targetInode->fileType;
	auxInodeBuf->forUnlink = targetInode->forUnlink;
	auxInodeBuf->indexes[0] = targetInode->indexes[0];
	auxInodeBuf->indexes[1] = targetInode->indexes[1];
	auxInodeBuf->openCount = targetInode->openCount;
	auxInodeBuf->writeOpenCount = targetInode->writeOpenCount;
	return 0;
}

uint64_t sys_close(uint64_t fd){
	return closeFile((int) fd);
}

uint64_t sys_unlink(uint64_t name){
	return unlinkFile((char*) name);
}

//TODO sacar los prints del testeo en los dups
uint64_t sys_dup(uint64_t oldVirtualFd, uint64_t buf, int* count){
	int* fdBuf = (char*) buf;
	int newVirtualFd = dup((int) oldVirtualFd);
	int* currentFdVirtualTable = getCurrentTask()->processFileDescriptors;
	int i;
	for(i=0; i<MAX_PFD; i++)
		fdBuf[i] = currentFdVirtualTable[i];
	*count = i;
	return newVirtualFd;
}

uint64_t sys_dup2(uint64_t oldVirtualFd, uint64_t newVirtualFd, uint64_t buf, int* count){
	int* fdBuf = (char*) buf;
	int ret = dup2((int) oldVirtualFd, (int) newVirtualFd);
	int* currentFdVirtualTable = getCurrentTask()->processFileDescriptors;
	int i;
	for(i=0; i<MAX_PFD; i++)
		fdBuf[i] = currentFdVirtualTable[i];
	*count = i;
	return ret;
}