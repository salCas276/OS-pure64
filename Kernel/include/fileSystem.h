
#define MAX_FILES 40
#define RD_ONLY 0
#define WRT_ONLY 1
#define RD_WRT 2
#define MAX_OPEN_FILES 40
#define BLOCK_SIZE 256
#define EOF -1

//Representa un archivo creado
typedef struct inode{
    char* name; //Se usa un nombre para poder abrir el archivo desde diferentes procesos
    char* block; //Esta es la memoria almacenada por el archivo
    int indexes[2]; //Indices de lectura (0) y escritura (1), son unicos para todo el archivo
    int openCount, writeOpenCount; //Contador de aperturas y aperturas para escribir
    int fileType; //0 -> File, 1 -> Fifo
    int forUnlink; //Flag que me indica si se le hizo un unlink
}inode;

//Representa la apertura de un archivo preexistente
typedef struct openedFile{
    int mode;
    inode* inode;
    int inodeIndex; 
}openedFile;

int createFile(char* name, int fileType);

int openFile(inode* inode, int inodeIndex, int mode);

int closeFile(int fd);

int unlinkFile(char* name);

int readFile(int fd, char* buf, int count);

int writeFile(int fd, char* buf, int count);

inode* getInode(char* name, int* inodeIndex);