
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
}inode;

//Representa la apertura de un archivo preexistente
typedef struct openedFile{
    int mode;
    inode* inode;
    int inodeIndex; //TODO Estaria bueno solo quedarnos con el indice, el cual necesito siquiero elimiar el inode
}openedFile;


int createFile(char* name);

int openFile(char* name, int mode);

int closeFile(int fd);

int readFile(int fd, char* buf, int count);

int writeFile(int fd, char* buf, int count);