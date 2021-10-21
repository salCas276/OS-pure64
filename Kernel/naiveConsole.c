#include <naiveConsole.h>
#include <video.h>
#include "./interruptions/roundRobin.h"


static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
//static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;

void ncPrint(const char * string) {
	for (int i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

/**
* Prints a string with the given attribute. 
* @params string: text to print
*		  textColor: color of the text in binary format
*		  backColor: color of the background in binary format
**/
void ncPrintAtt(const char * string, Color * textColor, Color * backColor) {
	for (int i = 0; string[i] != 0; i++)
		ncPrintCharAtt(string[i], textColor, backColor);
}

/**
* Prints a letter with the default black background, white font.
* @params character: character to print
**/
void ncPrintChar(char character) {
	ncPrintCharAtt(character, &WHITE, &BLACK);
}

/**
* Prints a letter with the given attribute.
* It also checks if the pointer is going out of bounds and
* moves everything one line up in that case.
* @params character: letter to print
**/

void ncPrintCharAtt(char character, Color * fontColor, Color * backgroundColor) {
	prompt_info * pPrompt = getCurrentPrompt();
	if(pPrompt == (void*) 0 ) //no tiene una pantalla asignada.
		return;

	if (character == '\n') 
		newLine(pPrompt, backgroundColor);
	else if (character == '\b')
		eraseChar(pPrompt, backgroundColor);
	else
		drawChar(pPrompt, character, fontColor, backgroundColor);
}

// https://stackoverflow.com/questions/28133020/how-to-convert-bcd-to-decimal
void ncPrintBCD(uint64_t value) {
	int decimal = ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
	ncPrintDec(decimal);
}

void ncPrintDec(uint64_t value) {
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value) {
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value) {
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2){
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void ncNewline() {
	prompt_info * pPrompt = getCurrentPrompt();
	newLine(pPrompt, &BLACK);
}