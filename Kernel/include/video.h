#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <stdint.h>

#define VBEModeInfoBlockAddress 0x0000000000005C00	

// https://wiki.osdev.org/VESA_Video_Modes
typedef struct vbe_mode_info {
    uint16_t attributes;         
    uint8_t window_a, window_b;            
    uint16_t granularity;        
    uint16_t window_size;
    uint16_t segment_a, segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch; // // number of bytes per horizontal line

    uint16_t width, height;
    uint8_t w_char, y_char;

    uint8_t planes;
    uint8_t bpp;    // bits per pixel in this mode             
    uint8_t banks;

    uint8_t memory_model;
    uint8_t bank_size;           
    uint8_t image_pages;
    uint8_t reserved0;
 
    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t reserved_mask, reserved_position;
    uint8_t direct_color_attributes;
 
    uint32_t framebuffer;         // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed)) vbe_mode_info;


typedef struct prompt_info{
    unsigned char x, y; // Posición de un cursor. NO PIXEL!
    uint8_t * baseX, baseY; // Comienzo de la ventana
    uint16_t windowWidth, windowHeight; // De cada ventana
} prompt_info;

typedef const struct Color{
    uint8_t r, g, b;
} Color;

extern Color PURPLE;
extern Color WHITE;
extern Color BLACK;
extern Color RED;

// ESCRIBIR EN PANTALLA
void fillScreen(Color * color);
void drawShellBorder(Color * color);
void drawChar(prompt_info * p, char c, Color * fontColor, Color * backgroundColor);
void newLine(prompt_info * p, Color * backgronudColor);
void clearWindow(prompt_info * p, Color * backgroundColor);
void eraseChar(prompt_info * p, Color * backgroundColor);

// FUNCIONES AUXILIARES
void init_screen(void);
uint16_t getScreenWidth();
uint16_t getScreenHeight();

#endif