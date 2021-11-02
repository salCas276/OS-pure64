// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*Fully based on 
https://github.com/Infineon/freertos/blob/master/Source/portable/MemMang/heap_2.c
*/
#include "../include/memoryManager.h"

static void *ptr = (void *)0x900000; 


typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock; /*<< The next free block in the list. */
	unsigned int xBlockSize;			  /*<< The size of the free block. */
} BlockLink_t;

#define portBYTE_ALIGNMENT 8
#define configADJUSTED_HEAP_SIZE (configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT)
#define portBYTE_ALIGNMENT_MASK (0x0007)
#define heapMINIMUM_BLOCK_SIZE ((unsigned int)(heapSTRUCT_SIZE * 2))
#define configTOTAL_HEAP_SIZE 1048576
typedef unsigned long portPOINTERSIZETYPE;


static int deliveredMemory = 0 ; 

static void prvHeapInit(void);

static const unsigned long heapSTRUCT_SIZE = ((sizeof(BlockLink_t) + (portBYTE_ALIGNMENT - 1)) & ~portBYTE_ALIGNMENT_MASK);

static BlockLink_t xStart, xEnd;

void prvInsertBlockIntoFreeList(BlockLink_t * pxBlockToInsert)                                                                                
	{                                                                                                                              
		BlockLink_t *pxIterator;                                                                                                   
		unsigned int xBlockSize;                                                                                                   
                                                                                                                                   
		xBlockSize = pxBlockToInsert->xBlockSize;                                                                                  
                                                                                                                                   
		/* Iterate through the list until a block is found that has a larger size */                                               
		/* than the block we are inserting. */                                                                                     
		for (pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize; pxIterator = pxIterator->pxNextFreeBlock) 
		{                                                                                                                          
			/* There is nothing to do here - just iterate to the correct position. */   										   				
			if(pxIterator -> pxNextFreeBlock == &xEnd )	         														       		
				break;         																									   		
				         																										   		
				         																										   		
		}                                                                                                                          
                                                                                                                                   
		/* Update the list to include the block being inserted in the correct */                                                   
		/* position. */                                                                                                            
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;                                                            
		pxIterator->pxNextFreeBlock = pxBlockToInsert;                                                                             
	}

void *malloc(size_t xWantedSize)
{
	BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
	static int xHeapHasBeenInitialised = 0;
	void *pvReturn = (void *)0;

	if (!xHeapHasBeenInitialised)
	{
		prvHeapInit();
		xHeapHasBeenInitialised = 1;
	}

	if (xWantedSize > 0)
	{
		xWantedSize += heapSTRUCT_SIZE; //adding the block before.

		/* Ensure that blocks are always aligned to the required number of bytes. */
		if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0)
		{
			/* Byte alignment required. */
			xWantedSize += (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK));
		}
	}

	if ((xWantedSize > 0) && (xWantedSize < configADJUSTED_HEAP_SIZE))
	{
		pxPreviousBlock = &xStart;
		pxBlock = xStart.pxNextFreeBlock;

		while ((pxBlock->xBlockSize < xWantedSize) && (pxBlock->pxNextFreeBlock != ((void *)0)))
		{
			pxPreviousBlock = pxBlock;
			pxBlock = pxBlock->pxNextFreeBlock;
		}

		if (pxBlock != &xEnd)
		{
			/* Return the memory space - jumping over the BlockLink_t structure
				at its start. */
			pvReturn = (void *)(((char *)pxPreviousBlock->pxNextFreeBlock) + heapSTRUCT_SIZE);

			/* This block is being returned for use so must be taken out of the
				list of free blocks. */
			pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

			/* If the block is larger than required it can be split into two. */
			if ((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE)
			{
				/* This block is to be split into two.  Create a new block
					following the number of bytes requested. The void cast is
					used to prevent byte alignment warnings from the compiler. */
				pxNewBlockLink = (void *)(((char *)pxBlock) + xWantedSize);

				/* Calculate the sizes of two blocks split from the single
					block. */
				pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize; //xWantedSize contains space for the struct.
				pxBlock->xBlockSize = xWantedSize;

				/* Insert the new block into the list of free blocks. */
				prvInsertBlockIntoFreeList((pxNewBlockLink));
			}
		}
	}

	if(pvReturn){
		deliveredMemory+=xWantedSize;
	}
	return pvReturn;
}

void free(void *ptr)
{
	char *BlockStartPtr = (char *)ptr;
	BlockLink_t *NewFreeBlock;

	if (ptr != (void *)0)
	{
		/* The memory being freed will have an BlockLink_t structure immediately
		before it. */
		BlockStartPtr -= heapSTRUCT_SIZE;

		/* This unexpected casting is to keep some compilers from issuing
		byte alignment warnings. */
		NewFreeBlock = (BlockLink_t *)BlockStartPtr;
		deliveredMemory-=NewFreeBlock->xBlockSize;

		/* Add this block to the list of free blocks. */
		prvInsertBlockIntoFreeList((NewFreeBlock));
		
	}
}

static void prvHeapInit(void)
{
	BlockLink_t *pxFirstFreeBlock;
	unsigned int *alignedptr = (unsigned int *)(((unsigned long)(char *)(ptr) + portBYTE_ALIGNMENT) & (~((unsigned long)portBYTE_ALIGNMENT_MASK)));

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.*/
	xStart.pxNextFreeBlock = (void *)alignedptr;
	xStart.xBlockSize = (unsigned int)0;

	/* xEnd is used to mark the end of the list of free blocks. */
	xEnd.xBlockSize = configADJUSTED_HEAP_SIZE;
	xEnd.pxNextFreeBlock = (void *)0;

	/* To start with there is a single free block that is sized to take up the
	entire heap space. */
	pxFirstFreeBlock = (void *)alignedptr;
	pxFirstFreeBlock->xBlockSize = configADJUSTED_HEAP_SIZE;
	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;
}

void getMemState(memstateType * state) {
  state->totalMemory = configTOTAL_HEAP_SIZE; 
  state->free = configTOTAL_HEAP_SIZE - deliveredMemory; 
  state->occupied = deliveredMemory; 
}
