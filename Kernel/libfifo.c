#include "include/libfifo.h"

//Uso esta funcion para agregarle restricciones propias de un fifo a la apertura de un archivo
int openFifo(inode* inode, int inodeIndex, int mode){
    openFile(inode, inodeIndex, mode);
    int flag = 0;
    if(inode->writeOpenCount == 0){ //No hay escritores
        switch(mode){
            case 0: //Llega un lector
                flag = -1; //Bloqueo al lector 
                break;
            case 1: //Llega un escritor
                flag = 1; //Desbloque a todos los lectores
                break;
        }
    }
    else if(inode->writeOpenCount == inode->openCount){ //No hay lectores
        switch(mode){
            case 0: //Llega un lector
                flag = 2; //Desbloqueo a todos los escritores
                break;
            case 1: //Llega un escritor
                flag = -2; //Bloqueo al escritor
                break;
        }
    }
    return 0; //En este caso no hubo ni que bloquear ni desbloquear a nadie
}