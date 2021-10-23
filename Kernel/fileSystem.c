#include "include/fileSystem.h"

inode* inodeTable[MAX_FILES];
openedFile* openedFileTable[MAX_OPEN_FILES];

int createFile(char* name){
    int candidate = -1;
    
    for(int i=0; i<MAX_FILES; i++){
        if(!inodeTable[i])
            candidate = i;
        else if(inodeTable[i]->name == name) 1; //El archivo ya fue creado //TODO Mirar como comparar los strings
    }

    if(candidate==-1)
        return -2; //No hay espacio para crear un nuevo archivo

    inodeTable[candidate] = malloc(sizeof(inode));

    inodeTable[candidate]->name = name;
    inodeTable[candidate]->block = malloc(BLOCK_SIZE);
    inodeTable[candidate]->openCount = 0;
    inodeTable[candidate]->writeOpenCount = 0;
    for(int i=0; i<2; i++) inodeTable[candidate]->indexes[i] = -1;

    return 0;
}


int openFile(char* name, int mode){

    for(int i=0; i<MAX_FILES; i++){
        if(inodeTable[i]->name == name){ //TODO Mirar como comparar los strings

            for(int j=0; j<MAX_OPEN_FILES; j++){
                if(!openedFileTable[j]){

                    openedFileTable[j] = malloc(sizeof(openedFile));

                    if(inodeTable[i]->indexes[mode] == -1)
                        inodeTable[i]->indexes[mode] = 0;
                    

                    openedFileTable[j]->mode = mode; //TODO Te bloqueo si sos el unico escribiendo?
                    openedFileTable[j]->inode = inodeTable[i];
                    openedFileTable[j]->inodeIndex = i;

                    if(mode == 1)
                        inodeTable[i]->writeOpenCount++;
                    inodeTable[i]->openCount++;
                    

                    return j; //Devuelvo la posicion de este objeto en la openedFileTable
                }
            }

            return -2; //El archivo fue creado pero no hay espacio para abriar un archivo
        }
    }

    return -1; //El archivo buscado no fue creado
}

int closeFile(int fd){
    inode* auxInode = openedFileTable[fd]->inode;
    int auxInodeIndex = openedFileTable[fd]->inodeIndex;

    auxInode->openCount--;

    if(openedFileTable[fd]->mode == 1)
        auxInode->writeOpenCount--;

    free(openedFileTable[fd]);
    openedFileTable[fd] = (openedFile*) 0;

    //Miro si se cerraron todos las aperturas del archivo, en tal caso lo elimino de la lista de inodes
    if(!auxInode->openCount){
        free(auxInode->block);
        free(auxInode);
        inodeTable[auxInodeIndex] = (inode*) 0;
    }
    else if(!auxInode->writeOpenCount) //Miro si este es el ultimo lector abierto, en tal caso agrego un EOF
       auxInode->block[auxInode->indexes[1]] = EOF; //TODO Mirar como manejar el EOF

}

int readFile(int fd, char* buf, int count){

}

int writeFile(int fd, char* buf, int count){

}