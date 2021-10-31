#include "MemoryManager.h"


#define SIZE 1048576
#define LEAF_BLOCK 64
#define DIM 2*(SIZE/LEAF_BLOCK)-1



typedef struct header{
  void * ptr; //puntero a la zona de memoria 
  int free ; //disponible o no 
  int bytes; //cantidad de bytes del bloque
  int index; //indice en el arreglo 
} header;


header  * headers = (header*) 0xC00000; 

char ** pointersDelivered= (char**) 0xF00000; 

int initialize=0;


static void markSubTree(header *  node , int value );
static void initArray(header * node, void * ptr,int index,int dim);
static void * findNode(header * node , int size);
static void coalescing(header * node);


void * malloc(size_t size){

  if(!initialize){
    initArray(&headers[0],(void*) MEMBASE,0,SIZE);
    initialize=1;
  }

  return findNode(&headers[0],size);

}


void free(void * ptr){
  if(!ptr) return ; 

  int i;
  for( i = 0 ; i < DIM ; i++){
    if(pointersDelivered[i] == (char*) ptr){
      pointersDelivered[i]=(void*)0;
      break;
    }
  }
  if(i == DIM) return ; 

  
  markSubTree(&headers[i],1);
  coalescing(&headers[i]);

}


void markSubTree(header *  node , int value ){
  node->free=value;
  if(node->bytes > LEAF_BLOCK){ //tiene hijos.
    markSubTree(&headers[2*node->index+1],value);
    markSubTree(&headers[2*node->index+2],value);
  }
}

static void initArray(header * node, void * ptr,int index,int dim){

  node->ptr=ptr;
  node->free=1;
  node->bytes=dim;
  node->index=index;

  if(node->bytes > LEAF_BLOCK){
    initArray(&headers[(index*2)+1],(char*)ptr,(index*2)+1,dim/2);
    initArray(&headers[(index*2)+2],(char*)ptr+(dim/2),(index*2)+2,dim/2);
  }
   
}

void * findNode(header * node , int size){

  if( node->bytes < size || (size > (node->bytes/2) && !node->free) ) 
    return (void*)0;
  
  
  //tengo que retornar este bloque 
    
  if( node->free && (node->bytes==LEAF_BLOCK || size > (node->bytes/2) ) ){
    markSubTree(node,0);
    pointersDelivered[node->index]=(char*)node->ptr;
    return node->ptr;
  }


  if( node->bytes==LEAF_BLOCK )
    return (void*)0;


  void * leftSon = findNode(&headers[(node->index*2+1)],size);

  if(leftSon!=(void*)0){ 
    node->free=0;
    return leftSon;
  }else{ 
    void * rightSon = findNode(&headers[2*(node->index+1)],size);
    if(rightSon!=(void*)0){
      node->free=0;
      return rightSon;
    }
  }

  return (void*)0;  

}




void coalescing(header * node){
  
  //si no tengo hijos o si mis dos hijos estan free, me marco como free  
  if( node->bytes==LEAF_BLOCK || (headers[node->index*2+1].free && headers[node->index*2+2].free ) )
    node->free = 1 ; 
  else return;

  if(!node->index) //root node
    return ;

  if( ( (node->index-1) / 2 ) >= 0)
    coalescing(&headers[((node->index-1)/2)]); //intento liberar a mi nodo padre


}



