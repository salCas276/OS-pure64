
//Un fifo es basicamente un buffer circular representado por un nombre que puede ser leido y escrito por diferentes procesos no relacionados
//La lecutra y la escritura son sincronizadas a traves de un sistema de bloqueo

//Los fifo's creados se encuentran en una table de "inodes", cada fifo se caracteriza por un nombre (name)
//Estos "inodes" contienen informacion sobre los indices de escritura y lectura, la direccion en la que se encuentra el pipe, los procesos bloqueados
//Para listar lop fifo's uno puede recorrer esta tabla 
//En caso de que ya existe un fifo con este nombre returno 1, en caso de error -1 y en caso de que todo salga bien 0
int creat(char* name);

//Este habilita para lectura o escritura (con mode) un fifo previamente creado
//Si abro un fifo para escirtura me bloquea hasta que haya uno abierto para lectura?
//Me retorna un fd que referencia especificamente a esta apertura, los fd pares son de lectura y los impares de escritura (fd de stdin, stdout y stderr?)
//Aunque se puedan hacer muchas aperturas solo hay un indice de lectura y uno de escirtura
//En caso de error retorna -1
int open(char* name, int mode);

//Indico la cantidad de bytes a leer (count) de un file descriptor particular (fd) y que me deje la informacion en un buffer (buf)
//Si count es -1 leo todo lo que haya para leer
//Si llego hasta el indice de escritura y quiero seguir leyendo me bloqueo, si llego a un EOF corto y retorno 0
//Retorno la cantidad de caracteres leidos si todo va bien, 0 si leo el EOF y -1 en caso de error
int readFifo(int fd, char* buf, int count);

//Los parametros son los mismos que los de readFifo
//Los valores de retorno son los mismos que los de readFifo
//La escritura es atomica, si hay un proceso escribiendo y otro quiere escribir tambien este ultimo se bloquea
//Si doy toda la vuelt al buffer y llego hasta el indice de lectura me bloqueo hasta que se libere espacio para escribir
int writeFifo(int fd, char* buf, int count);

//El close se hace sobre un fd, esto libera al fd
//Una vez que todos los escritores hicieron close se agrega un EOF
//Una vez que todos los procesos hicieron close se libera el fifo
//Si todo va bien se retorna 0 y en caso de error -1
int close(int fd);
