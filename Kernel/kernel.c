#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video.h>
#include "./include/memoryManager.h"
#include <process.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

void _cli(); //clear interrupts.


typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	init_screen();


	return getStackBase();
}





int main() {	

	 drawShellBorder(&WHITE);
	_cli();
	 load_idt();

	prompt_info shellPrompt = {	.x = 0,
								.y = 0,
							  	.baseX = 0,
							  	.baseY = 0,
							  	.windowWidth = getScreenWidth()/2 - 4,
							  	.windowHeight = getScreenHeight()};

	prompt_info backgroundPrompt = {	.x = 0,
								.y = 0,
								.baseX = (uint8_t * )(long)(getScreenWidth() / 2 + 4),
								.baseY = 0,
								.windowWidth = getScreenWidth()/2 - 4, 
								.windowHeight = getScreenHeight()};

	firstProcess((uint64_t)sampleCodeModuleAddress,shellPrompt,backgroundPrompt);


	ncPrint("El sistema no tiene memoria , no puede arrancar\n");

	while(1);

	return 0;
}
