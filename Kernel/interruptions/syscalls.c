// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <semaphore.h>
#include "../include/libshmem.h"

#define SYSCALL_BLOCK_PASSWORD 0 

//#include <process.h>

typedef struct dateType {
	uint8_t year, month, day;
	uint8_t hour, minute, second;
} dateType;

uint64_t sys_writeFifo(uint64_t fd, uint64_t buf, uint64_t count);
uint64_t sys_readFifo(uint64_t fd, uint64_t buf, uint64_t count);
uint64_t sys_write(int pid, int fd, char * buffer, uint64_t count);
int64_t sys_read(int pid, uint8_t fd, char * buffer, uint64_t count);
uint64_t sys_date(dateType * pDate);
uint64_t sys_mem(uint64_t rdi, uint64_t rsi, uint8_t rdx);
uint64_t sys_malloc(uint64_t size);
uint64_t sys_free(uint64_t pv); 
uint64_t sysOpenSemaphore(uint64_t rdi , uint64_t rsi );
uint64_t sysWaitSemaphore(uint64_t rdi );
uint64_t sysPostSemaphore(uint64_t rdi );
uint64_t sysCloseSemaphore(uint64_t rdi );
int sys_kill(uint64_t code, uint64_t pid); 
uint64_t sys_createReg(uint64_t name);
uint64_t sys_createFifo(uint64_t name);
uint64_t sys_open(int pid, uint64_t name, uint64_t mode);
uint64_t sys_getFileContent(uint64_t name, uint64_t buf);
uint64_t sys_getFileInfo(uint64_t name, uint64_t buf);
uint64_t sys_close(int pid, uint64_t fd);
uint64_t sys_unlink(uint64_t name);
uint64_t sys_dup(int pid, uint64_t oldVirtualFd);
uint64_t sys_dup2(int pid, uint64_t oldVirtualFd, uint64_t newVirtualFd);
uint64_t sys_getFdTableByPid(int pid, int* fdsBuf);
uint64_t sys_getFifosData(fifoData* fifosBuf);
void sysExit();
void sysWait();
void printSemaphore();


int syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, int r8) {
	switch(rcx) {
		case 1: return sys_write(rdi, (int)rsi, (char*)rdx, (uint64_t)r8);
		case 2: return sys_read((int)rdi, (uint8_t)rsi, (char*)rdx, (uint64_t) r8);
		case 3: return sys_date((dateType *)rdi);
		case 4: return sys_mem(rdi, rsi, rdx);
		case 5 : return createProcess(rdi,rsi,(char**)rdx, r8);
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
		case 19 : return getSemaphoreData(rdi);
		case 10: return changeNicenessBy(rdi, rsi); 
		case 11: return sys_kill(rdi, rsi);
		case 12: return renounce();break;  

		case 20: return sys_createReg(rdi);
		case 21: return sys_createFifo(rdi);
		case 22: return sys_open(rdi, rsi, rdx);
		case 23: return sys_close(rdi, rsi);
		case 24: return sys_unlink(rdi);
		case 25: return sys_getFileContent(rdi, rsi);
		case 26: return sys_getFileInfo(rdi, rsi);
		case 27: return sys_dup(rdi, rsi);
		case 28: return sys_dup2(rdi, rsi, rdx);
		case 29: return sys_getFdTableByPid(rdi, (int*) rsi);
		case 30: return sys_getFifosData((fifoData*)rdi);

		case 70:  getMemState((memstateType *)rdi); break; 
		case 71: return createShmem((int)rdi); 
		case 72: return unlinkShmem((int)rdi); 
	}
	return 0;
}


void sysWait(){
	wait();
}


void sysExit(){
	exit();
}

uint64_t sys_write(int pid, int fd, char * buffer, uint64_t count) {
	if (buffer == 0 || count <= 0)
		return -1;
	
	return writeFile(pid, (int) fd, (char*) buffer, (int) count);
}

int64_t sys_read(int pid, uint8_t fd, char * buffer, uint64_t count) {
  	return readFile(pid, fd, buffer, count);
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
	// if (src >= (uint8_t *) 0x1000000000 || src - 1 + rdx >= (uint8_t *)0x1000000000)
	// 	return 1;

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
 
int sys_kill(uint64_t code, uint64_t pid) {
	switch(code) {
		case 0: return deleteProcess(pid); 
		case 1: return blockProcess(pid, SYSCALL_BLOCK_PASSWORD); 
		case 2: return unblockProcess(pid, SYSCALL_BLOCK_PASSWORD); 
	}
	return -1; 
}

uint64_t sys_createReg(uint64_t name){
	return createFile((char*) name, 2);
}

uint64_t sys_createFifo(uint64_t name){
	return createFile((char*) name, 3);
}

uint64_t sys_open(int pid, uint64_t name, uint64_t mode){
	return openFile(pid, (char*) name, (int) mode);
}

uint64_t sys_getFileContent(uint64_t name, uint64_t buf){
	int inodeIndex;
	char * auxBuf = (char*) buf;

	inode* targetInode = getInode((char*)name, &inodeIndex);

	if(targetInode == (inode*)-1)
		return -1;
	int i;

	for(i=0; i<256; i++)
		auxBuf[i] = targetInode->block[i];
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

uint64_t sys_close(int pid, uint64_t fd){
	return closeFile(pid, (int) fd);
}

uint64_t sys_unlink(uint64_t name){
	return unlinkFile((char*) name);
}

//TODO sacar los prints del testeo en los dups
uint64_t sys_dup(int pid, uint64_t oldVirtualFd){
	int newVirtualFd = dupp(pid, (int) oldVirtualFd);
	// int* fdVirtualTable = getProcessByPid(pid)->processFileDescriptors;
	return newVirtualFd;
}

uint64_t sys_dup2(int pid, uint64_t oldVirtualFd, uint64_t newVirtualFd){
	int ret = dupp2(pid, (int) oldVirtualFd, (int) newVirtualFd);
	//int* fdVirtualTable = getProcessByPid(pid)->processFileDescriptors;
	return ret;
}

uint64_t sys_getFdTableByPid(int pid, int* fdsBuf){
	if(pid < -1)
		return -1;
	int* fdTable = getProcessByPid(pid)->processFileDescriptors;	
	int i;
	for(i=0; i < MAX_PFD; i++)
		fdsBuf[i] = fdTable[i];
	return i;
}

uint64_t sys_getFifosData(fifoData* fifosBuf){
	return loadFifosData(fifosBuf);
}