#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video.h>
#include "./include/memoryManager.h"
#include <process.h>

#include "./include/test_util.h"

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
	//char buffer[10];

	// ncPrint("[x64BareBones]");
	// ncNewline();

	// ncPrint("CPU Vendor:");
	// ncPrint(cpuVendor(buffer));
	// ncNewline();

	// ncPrint("[Loading modules]");
	// ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	// ncPrint("[Done]");
	// ncNewline();
	// ncNewline();

	// ncPrint("[Initializing kernel's binary]");
	// ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);


	char *c = malloc( 20000 * sizeof(char));
	// char *d = malloc( 20000 * sizeof(char));
	// char *e = malloc( 20000 * sizeof(char));
	int summer = 0; 

	for (int i=0; i<20000; i++)
		c[i] = 1; 

	// for (int i=0; i<20000; i++)
	// 	d[i] = 2; 

	// for (int i=0; i<20000; i++)
	// 	e[i] = 3; 

	// summer = 0; 
	// for (int i=0; i<20000; i++) 
	// 	summer += c[i]; 
	
	// if (summer != 20000)
	// 	return (void*) 0; 

	// summer = 0; 
	// for (int i=0; i<20000; i++) 
	// 	summer += d[i]; 

	// if (summer != 40000)
	// 	return (void*) 0; 

	// summer = 0; 
	// for (int i=0; i<20000; i++) 
	// 	summer += e[i]; 

	// if (summer != 60000)
	// 	return (void*) 0; 

		
	// ncPrint("  text: 0x");
	// ncPrintHex((uint64_t)&text);
	// ncNewline();
	// ncPrint("  rodata: 0x");
	// ncPrintHex((uint64_t)&rodata);
	// ncNewline();
	// ncPrint("  data: 0x");
	// ncPrintHex((uint64_t)&data);
	// ncNewline();
	// ncPrint("  bss: 0x");
	// ncPrintHex((uint64_t)&bss);
	// ncNewline();

	// ncPrint("[Done]");
	// ncNewline();
	// ncNewline();

	init_screen();


	return getStackBase();
}



#define MAX_BLOCKS 128
#define MAX_MEMORY 838861 //Should be around 80% of memory managed by the MM

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;


void test_mm(){
//   mm_rq mm_rqs[MAX_BLOCKS];
//   uint8_t rq;
//   uint32_t total;

//   while (1){
//     rq = 0;
//     total = 0;

//     // Request as many blocks as we can
//     while(rq < MAX_BLOCKS && total < MAX_MEMORY){
//       mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
//       mm_rqs[rq].address = malloc(mm_rqs[rq].size); // TODO: Port this call as required
// 	//TODO: check if NULL
//       total += mm_rqs[rq].size;
//       rq++;
//     }

//     // Set
//     uint32_t i;
//     for (i = 0; i < rq; i++)
//       if (mm_rqs[i].address != NULL)
//         memset(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required

//     // Check
//     for (i = 0; i < rq; i++)
//       if (mm_rqs[i].address != NULL)
//         if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) 
//           ncPrint("ERROR!"); // TODO: Port this call as required

//     // Free
//     for (i = 0; i < rq; i++)
//       if (mm_rqs[i].address != NULL)
//         free(mm_rqs[i].address);  // TODO: Port this call as required
//   } 
}





int main() {	

	//illScreen(&PURPLE);
	 drawShellBorder(&WHITE);
	_cli();
	 load_idt();
	// // ncPrint("  IDT loaded");
	// // ncNewline();
	// // ncPrint("[Kernel Main]");
	// // ncNewline();
	// // ncPrint("  Sample code module at 0x");
	// // ncPrintHex((uint64_t)sampleCodeModuleAddress);
	// // ncNewline();
	// // ncPrint("  Calling the sample code module returned: ");
	prompt_info leftPrompt = {	.x = 0,
								.y = 0,
							  	.baseX = 0,
							  	.baseY = 0,
							  	.windowWidth = getScreenWidth()/2 - 4,
							  	.windowHeight = getScreenHeight()};

	// prompt_info rightPrompt = {	.x = 0,
	// 							.y = 0,
	// 							.baseX = (uint8_t * )(long)(getScreenWidth() / 2 + 4),
	// 							.baseY = 0,
	// 							.windowWidth = getScreenWidth()/2 - 4, 
	// 							.windowHeight = getScreenHeight()};

//	uint64_t * bsP2 = malloc(4096 * sizeof(uint64_t));

	//addProcess(0,(uint64_t)test_mm,(uint64_t)&bsP2[4095],rightPrompt);
	//createProcess((uint64_t)print , rightPrompt );
	firstProcess((uint64_t)sampleCodeModuleAddress,leftPrompt);


//	loadTask(1, (uint64_t)test_mm, 0x700000, rightPrompt);


	//initCurrentTask();


	// // ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	// // currentTask = 1;
	// // task1RSP = 0x600000;
	// // create_task(1, 0x600000, sampleCodeModuleAddress);
	// // uint64_t task2RSP = create_task(2, 0x700000, sampleCodeModuleAddress);
	// // setOtherRSP(task2RSP);
	// // init_task(task1RSP);
	

	// ncNewline();
	// ncNewline();

	// ncPrint("  Sample data module at 0x");
	// ncPrintHex((uint64_t)sampleDataModuleAddress);
	
	// ncNewline();
	// ncPrint("  Sample data module contents: ");
	// ncPrint((char*)sampleDataModuleAddress);
	// ncNewline();
	
	// ncPrint("[Finished]");
	// ncNewline();

	

	ncPrint("Pass");

	while(1);

	return 0;
}
