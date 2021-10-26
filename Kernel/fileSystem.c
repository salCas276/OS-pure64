#include "include/fileSystem.h"
#include <roundRobin.h>
#include "include/string.h"
#include "memoryManager.h"
#include "include/naiveConsole.h"
#include "include/libfifo.h"

inode* inodeTable[MAX_FILES];
openedFile* openedFileTable[MAX_OPEN_FILES];

static int freeInode(inode* onDeleteInode, int onDeleteInodeIndex);

int createFile(char* name, int fileType){
    int candidate = -1;
    
    for(int i=0; i<MAX_FILES; i++){
        if(!inodeTable[i])
            candidate = candidate == -1 ? i : candidate;
        else if(strcmp(inodeTable[i]->name, name) == 0) return 1; //El archivo ya fue creado
    }

    if(candidate==-1)
        return -2; //No hay espacio para crear un nuevo archivo

    inodeTable[candidate] = malloc(sizeof(inode));

    strcpy(name, inodeTable[candidate]->name);
    inodeTable[candidate]->block = malloc(BLOCK_SIZE);
    inodeTable[candidate]->openCount = 0;
    inodeTable[candidate]->writeOpenCount = 0;
    inodeTable[candidate]->fileType = fileType;
    inodeTable[candidate]->forUnlink = 0;
    for(int i=0; i<2; i++) inodeTable[candidate]->indexes[i] = -1;

    return 0;
}


int openFileFromInode(inode* inode, int inodeIndex, int mode){

    for(int j=0; j<MAX_OPEN_FILES; j++){
        if(!openedFileTable[j]){

            openedFileTable[j] = malloc(sizeof(openedFile));


            if(inode->indexes[0] == -1 && (mode == 0 || mode == 2))
                inode->indexes[0] = 0;
            
            if(inode->indexes[1] == -1 && (mode == 1 || mode == 2))
                inode->indexes[1] = 0;
                    
            openedFileTable[j]->mode = mode;
            openedFileTable[j]->inode = inode;
            openedFileTable[j]->inodeIndex = inodeIndex;

            if(mode == 1 || mode == 2)
                inode->writeOpenCount++;
            inode->openCount++;

            int virtualFd = getCurrentMinFd();
            getCurrentTask()->processFileDescriptors[virtualFd] = j;                    

            return virtualFd; //Devuelvo la posicion de este objeto en la openedFileTable
        }
    }
    return -2; //El inode correspondiente existe pero no hay espacio para abriar un archivo
}

int openFile(char* name, int mode){
    int inodeIndex;
	inode* openedInode = getInode((char*) name, &inodeIndex);
	if(openedInode == (inode*)inodeIndex)
        return -1;
	switch(openedInode->fileType){
		case 0: //File
			return openFileFromInode(openedInode, inodeIndex, mode);
		case 1: //Fifo (named pipe)
			if(mode > 1)
				return -1;
			return openFifoFromInode(openedInode, inodeIndex, mode);
	}
	return 0;
}

int closeFile(int virtualFd){
    int fd = getCurrentTask()->processFileDescriptors[virtualFd];

    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    inode* auxInode = openedFileTable[fd]->inode;
    int auxInodeIndex = openedFileTable[fd]->inodeIndex;

    auxInode->openCount--;

    if(openedFileTable[fd]->mode == 1 || openedFileTable[fd]->mode == 2) 
        auxInode->writeOpenCount--;

    free(openedFileTable[fd]);
    openedFileTable[fd] = (openedFile*) 0;

    for(int i=0; i<MAX_PFD; i++){
        if(getCurrentTask()->processFileDescriptors[i] == fd)
            getCurrentTask()->processFileDescriptors[i] = -1;
    }

    //Miro si se cerraron todos las aperturas del archivo y si se habia hecho un llamado a unlink, en tal caso lo elimino de la lista de inodes
    if(auxInode->openCount == 0 && auxInode->forUnlink == 1)
        return freeInode(auxInode, auxInodeIndex);
    
    //No hace falta validar si estoy cerrando el ultimo escritor para agregar un EOF ya que este ultimo se deduce de writerCount == 0 y wIndex != -1

    return 0;
}

int unlinkFile(char* name){
    int targetInodeIndex;
    inode* targetInode = getInode(name, &targetInodeIndex);

    if(targetInodeIndex == -1){
        return -1;
    }

    if(targetInode->openCount == 0){
        return freeInode(targetInode, targetInodeIndex);
    }

    targetInode->forUnlink = 1;
    return 0;
}

static int freeInode(inode* onDeleteInode, int onDeleteInodeIndex){
    free(onDeleteInode->block);
    free(onDeleteInode);
    inodeTable[onDeleteInodeIndex] = (inode*) 0;
    return 0;
}

int readFile(int virtualFd, char* buf, int count){
    int fd = getCurrentTask()->processFileDescriptors[virtualFd];

    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 1) //La apertura no permite esta operacion
        return -1;

    inode* targetInode = openedFileTable[fd]->inode;
    int i;

    for(i=0; i<count; i++){
        if((targetInode->indexes[0]+i)%BLOCK_SIZE == targetInode->indexes[1]){
            if(targetInode->writeOpenCount == 0){
                targetInode->indexes[0] = targetInode->indexes[0]+i;
                return 0; //Llegue al EOF
            }
        }

        buf[i]=targetInode->block[(targetInode->indexes[0]+i)%BLOCK_SIZE];
    }
    targetInode->indexes[0] = targetInode->indexes[0]+i;
    return i;
}

int writeFile(int virtualFd, char* buf, int count){
    int fd = getCurrentTask()->processFileDescriptors[virtualFd];

    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 0) //La apertura no permite esta operacion
        return -1;

    inode* targetInode = openedFileTable[fd]->inode;
    int i;

    for(i=0; i<count; i++){
        targetInode->block[(targetInode->indexes[1]+i)%BLOCK_SIZE] = buf[i];

        if(fd < 3)
            ncPrintCharAtt(buf[i], fd == 1 ? &WHITE : &RED, &BLACK);
    }
    targetInode->indexes[1] = targetInode->indexes[1]+i;
    return i;
}

int dup(int oldVirtualFd){
    processControlBlock* currentProcess = getCurrentTask();
    if(currentProcess->processFileDescriptors[oldVirtualFd] == -1)
        return -1;

    int newVirtualFd = getCurrentMinFd();

    if(newVirtualFd == -1)
        return -1;

    currentProcess->processFileDescriptors[newVirtualFd] = currentProcess->processFileDescriptors[oldVirtualFd];
    return newVirtualFd;
}

int dup2(int oldVirtualFd, int newVirtualFd){
    processControlBlock* currentProcess = getCurrentTask();

    if(currentProcess->processFileDescriptors[oldVirtualFd] == -1)
        return -1;

    if(newVirtualFd > MAX_PFD)
        return -1;

    currentProcess->processFileDescriptors[newVirtualFd] = currentProcess->processFileDescriptors[oldVirtualFd];
    return newVirtualFd;
}

//Devuelve el inode con ese nombre y el indece de este en la tabla de inodes
//En caso de que no existe un inode con ese nombre se devuelve -1
inode* getInode(char* name, int* inodeIndex){
    for(int i=0; i<MAX_FILES; i++){
        if(inodeTable[i]){
            if(strcmp(inodeTable[i]->name, name) == 0){
                *inodeIndex = i;
                return inodeTable[i];
            }
        }
    }
    *inodeIndex = -1;
    return (inode*) -1; 
}
