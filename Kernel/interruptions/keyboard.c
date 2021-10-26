#include <keyboard.h>
#include <lib.h>
#include <naiveConsole.h>
#include <RoundRobin.h>

#define LEFT_SHIFT  0x2A
#define LEFT_SHIFT_FLAG 0b00000001
#define RIGHT_SHIFT 0x36
#define RIGHT_SHIFT_FLAG 0b00000010
#define LEFT_ALT 0x38

#define KEYBOARD_PASSWORD 1 

#define BUFFER_SIZE 128

uint8_t flags = 0;

static char buffer[BUFFER_SIZE];
uint64_t tail = 0, head = 0; // head escribe, tail lee
// {0, 0, 0, 0, 0, 0, 0, 0, 0}
//  t                       h
//              h  t


// https://stanislavs.org/helppc/make_codes.html
unsigned char lowerScancodeToAscii[128] = {

      0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-', '=', 
   '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']', 
   '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',    
      0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0, '*',    
      0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
      0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,    0,   0, 
      0,    0,   0,   0,   0,   0    

};

unsigned char upperScancodeToAscii[128] = {

      0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 
   '\b', 1, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 
   '\n',    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',    
      0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',
      0, ' ',    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
      0,   0,    0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0, 
      0,   0,    0,   0,   0,   0    
      
};

static void appendBuffer(char c) {
  buffer[head++] = c;

  if (head == BUFFER_SIZE)
    head = 0;

  if (head == tail) { // Si me quedo sin espacio, borro el caracter mas antiguo
    tail++;
    if (tail == BUFFER_SIZE)
      tail = 0;
  }

}

void keyboard_handler() {
    uint8_t scancode = getKey();
    // ncPrintHex(scancode);
    if (scancode < 0x80) { // Make/Pressed
      if (scancode == LEFT_SHIFT) {
        flags |= LEFT_SHIFT_FLAG;
      } else if (scancode == RIGHT_SHIFT) {
        flags |= RIGHT_SHIFT_FLAG;
      } else {
        if ((flags & LEFT_SHIFT_FLAG) || (flags & RIGHT_SHIFT_FLAG))
          //ncPrintChar(upperScancodeToAscii[scancode]); 
          appendBuffer(upperScancodeToAscii[scancode]);
        else
          //ncPrintChar(lowerScancodeToAscii[scancode]); 
          appendBuffer(lowerScancodeToAscii[scancode]);
      }
    } else { // Break/Released
      scancode -= 0x80;
      if (scancode == LEFT_SHIFT)
        flags &= ~LEFT_SHIFT_FLAG;
      else if (scancode == RIGHT_SHIFT) 
        flags &= ~RIGHT_SHIFT_FLAG;
    }

    popAndUnblock(KEYBOARD_PASSWORD);
}

int64_t getChar(void) {
  
  char ans = 0 ;
  while(1){
    if(head != tail ){
      ans = buffer[tail++];
      if (tail == BUFFER_SIZE)
        tail = 0;
    }
    else if(head == tail || ans == 0){
      blockProcess(getCurrentPid(),KEYBOARD_PASSWORD);
      continue;
    }

    break;
  }
  return ans;   
  
}