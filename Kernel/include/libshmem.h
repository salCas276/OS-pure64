


// Si existe, devuelve el void * y aumenta qProcessesAttached
// Si no existe, asigna memoria dinamica, crea la estructura y devuelve el puntero 
void * createShmem(int id); 

// Decrementa qProcessAttached y si es 0 hace free() de memoria dinamica 
int unlinkShmem(int id); 