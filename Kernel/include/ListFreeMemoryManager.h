

#include <stdint.h>

typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	unsigned int xBlockSize;					        	/*<< The size of the free block. */
} BlockLink_t;


#define heapMINIMUM_BLOCK_SIZE	 ( heapSTRUCT_SIZE * 2 ) 

#define configADJUSTED_HEAP_SIZE 1048576 //1MB

//test
void printBlocks();

void * memalloc( unsigned int xWantedSize );

void vPortFree( void *pv );

void InitializeMM(void * StartPointer);