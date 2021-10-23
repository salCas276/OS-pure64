#include "include/fileSystem.h"

inode* inodeTable[MAX_FILES];
openedFile* openedFileTable[MAX_OPEN_FILES];

int createFile(char* name, int fileType){
    int candidate = -1;
    
    for(int i=0; i<MAX_FILES; i++){
        if(!inodeTable[i])
            candidate = i;
        else if(inodeTable[i]->name == name) 1; //El archivo ya fue creado //TODO Mirar como comparar los strings
    }

    if(candidate==-1)
        return -2; //No hay espacio para crear un nuevo archivo

    inodeTable[candidate] = malloc(sizeof(inode));

    inodeTable[candidate]->name = name; //TODO mirar si la asignacion se esta haciendo bien
    inodeTable[candidate]->block = malloc(BLOCK_SIZE);
    inodeTable[candidate]->openCount = 0;
    inodeTable[candidate]->writeOpenCount = 0;
    inodeTable[candidate]->fileType = fileType;
    for(int i=0; i<2; i++) inodeTable[candidate]->indexes[i] = -1;

    return 0;
}


int openFile(inode* inode, int inodeIndex, int mode){

    for(int j=0; j<MAX_OPEN_FILES; j++){
        if(!openedFileTable[j]){

            openedFileTable[j] = malloc(sizeof(openedFile));

            if(inode->indexes[mode] == -1)
                inode->indexes[mode] = 0;
                    
            openedFileTable[j]->mode = mode;
            openedFileTable[j]->inode = inode;
            openedFileTable[j]->inodeIndex = inodeIndex;

            if(mode == 1 || mode==2)
                inode->writeOpenCount++;
            inode->openCount++;
                    
            return j; //Devuelvo la posicion de este objeto en la openedFileTable
        }
    }
    return -2; //El inode correspondiente existe pero no hay espacio para abriar un archivo
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

//Devuelve el inode con ese nombre y el indece de este en la tabla de inodes
//En caso de que no existe un inode con ese nombre se devuelve -1
inode* getInode(char* name, int* inodeIndex){
    for(int i=0; i<MAX_FILES; i++){
        if(inodeTable[i]->name == name){ //TODO mirar como compara bien los strings
            *inodeIndex = i;
            return inodeTable[i];
        }
    }
    *inodeIndex = -1;
    return (inode*) -1; 
}