#include "include/fileSystem.h"
#include "include/string.h"
#include "include/naiveConsole.h"
#include "include/libfifo.h"
#include "include/libconsfile.h"
#include "include/libregfile.h"
#include "include/libkbfile.h"
#include "include/semaphore.h"
#include "include/process.h"

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
    strcpy(inodeTable[candidate]->name, name);



    switch (fileType)
    {
    case 0:
        return createKeyboard(inodeTable[candidate]);
    case 1:
        return createConsole(inodeTable[candidate]);
    case 2:
        return createRegular(inodeTable[candidate]);
    case 3:
        return createFifo(inodeTable[candidate]);
    }
    return -1;
}


int openFileFromInode(int pid, inode* inode, int inodeIndex, int mode){

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
            openedFileTable[j]->countFd = 1;

            if(mode == 1 || mode == 2)
                inode->writeOpenCount++;
            inode->openCount++;

            int virtualFd = getMinFdByPid(pid);
            getProcessByPid(pid)->processFileDescriptors[virtualFd] = j;                    

            return virtualFd; //Devuelvo la posicion de este objeto en la openedFileTable
        }
    }
    return -2; //El inode correspondiente existe pero no hay espacio para abriar un archivo
}

int openFile(int pid, char* name, int mode){
    int inodeIndex;
	inode* openedInode = getInode((char*) name, &inodeIndex);

	if(openedInode == (inode*)inodeIndex)
        return -1;

	switch(openedInode->fileType){
        case 0:
            return openKeyboard(pid, openedInode, inodeIndex, mode);
        case 1:
            return openConsole(pid, openedInode, inodeIndex, mode);
		case 2:
			return openRegular(pid, openedInode, inodeIndex, mode);
		case 3:
			return openFifo(pid, openedInode, inodeIndex, mode);
	}
	return -1;
}

int closeFile(int pid, int virtualFd){
    int fd = getProcessByPid(pid)->processFileDescriptors[virtualFd];

    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    inode* auxInode = openedFileTable[fd]->inode;
    int auxInodeIndex = openedFileTable[fd]->inodeIndex;

    getProcessByPid(pid)->processFileDescriptors[virtualFd] = -1;

    openedFileTable[fd]->countFd--;

    if(!openedFileTable[fd]->countFd){

        auxInode->openCount--;

        if(openedFileTable[fd]->mode == 1 || openedFileTable[fd]->mode == 2) 
            auxInode->writeOpenCount--;

        free(openedFileTable[fd]);
        openedFileTable[fd] = (openedFile*) 0;
    }

    //TODO mirar que cierre fd's y no aperturas

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
    if(onDeleteInode->block)
        free(onDeleteInode->block);
    free(onDeleteInode);
    inodeTable[onDeleteInodeIndex] = (inode*) 0;
    return 0;
}

int readFile(int pid, int virtualFd, char* buf, int count){
    int fd = getProcessByPid(pid)->processFileDescriptors[virtualFd];


    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 1) //La apertura no permite esta operacion
        return -1;

    inode* readInode = openedFileTable[fd]->inode;

    switch (readInode->fileType)
    {
    case 0:
        return readKeyboard(readInode, buf, count);
    case 1:
        return -1;
    case 2:
        return readRegular(readInode, buf, count); 
    case 3:
        return readFifo(readInode, buf, count);
    }
    return -1;
}

int writeFile(int pid, int virtualFd, char* buf, int count){
    int fd = getProcessByPid(pid)->processFileDescriptors[virtualFd];

    if(fd == -1 || fd >= MAX_PFD) //El fd no apunta a una apertura existente
        return -1;

    if(openedFileTable[fd]->mode == 0) //La apertura no permite esta operacion
        return -1;

    inode* writtenInode = openedFileTable[fd]->inode;

    switch (writtenInode->fileType)
    {
    case 0:
        return -1;
    case 1:
        return writeConsole(writtenInode, buf, count);
    case 2:
        return writeRegular(writtenInode, buf, count); 
    case 3:
        return writeFifo(writtenInode, buf, count);
    }
    return -1;
}

int dupp(int pid, int oldVirtualFd){
    processControlBlock* targetProcess = getProcessByPid(pid);
    if(targetProcess->processFileDescriptors[oldVirtualFd] == -1)
        return -1;

    int newVirtualFd = getMinFdByPid(pid);

    if(newVirtualFd == -1)
        return -1;

    openedFileTable[targetProcess->processFileDescriptors[oldVirtualFd]]->countFd++;
    targetProcess->processFileDescriptors[newVirtualFd] = targetProcess->processFileDescriptors[oldVirtualFd];
    return newVirtualFd;
}

int dupp2(int pid, int oldVirtualFd, int newVirtualFd){
    processControlBlock* targetProcess = getProcessByPid(pid);

    if(targetProcess->processFileDescriptors[oldVirtualFd] == -1)
        return -1;

    if(newVirtualFd > MAX_PFD)
        return -1;

    closeFile(pid, newVirtualFd);

    openedFileTable[targetProcess->processFileDescriptors[oldVirtualFd]]->countFd++;
    targetProcess->processFileDescriptors[newVirtualFd] = targetProcess->processFileDescriptors[oldVirtualFd];
    return newVirtualFd;
}

int getPidsBlocked(inode* targetInode, int* pidsBuf){
    
    int counter = 0;
    if(targetInode->rPassword != -1){
        counter += getBlockedPidsByPass(targetInode->rPassword, pidsBuf+counter);
    }
    pidsBuf[counter++] = -1; //Marco separeciones entre tipos de pids

    if(targetInode->wPassword != -1){
        counter += getBlockedPidsByPass(targetInode->wPassword, pidsBuf+counter);
    }
    pidsBuf[counter++] = -1;

    if(targetInode->rSemId[0]){
        counter += getSemBlokcedPids(targetInode->rSemId, pidsBuf+counter);
    }
    pidsBuf[counter++] = -1;

    if(targetInode->wSemId[0]){
        counter += getSemBlokcedPids(targetInode->wSemId, pidsBuf+counter);
    }
    pidsBuf[counter++] = -1;

    return counter;
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

int addFd(int realFd){
    if(realFd < 0)
        return -1;
    openedFileTable[realFd]->countFd++;
    return 0;
}

int getTypeInodes(inode** inodeBuf, int fileType){
    int counter = 0;
    for(int i = 0; i < MAX_FILES; i++){
        if(inodeTable[i]){
            if(inodeTable[i]->fileType == fileType){
                inodeBuf[counter++] = inodeTable[i];
            }
        }
    }
    return counter;
}