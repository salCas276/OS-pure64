
#ifndef _LISTFREEMEMORYMANAGER_H_
#define _LISTFREEMEMORYMANAGER_H_


#include <stdint.h>

typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	unsigned int xBlockSize;					        	/*<< The size of the free block. */
} BlockLink_t;



#define portBYTE_ALIGNMENT			8  
#define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )
#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#define heapMINIMUM_BLOCK_SIZE	( ( unsigned int ) ( heapSTRUCT_SIZE * 2 ) )
#define configTOTAL_HEAP_SIZE 1048576
typedef  unsigned long  portPOINTERSIZETYPE;


//test
void printBlocks();

void * malloc( unsigned int xWantedSize );

void free( void *pv );

void InitializeMM(void * StartPointer);


#endif
