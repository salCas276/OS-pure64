// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <video.h>
#include <font.h>
#include <lib.h>

vbe_mode_info * vbeInfo = (vbe_mode_info *) 0x5C00;

Color PURPLE = {.r = 0x88, .g = 0x00, .b = 0xFF};
Color WHITE = {.r = 0xFF, .g = 0xFF, .b = 0xFF};
Color BLACK = {.r = 0x00, .g = 0x00, .b = 0x00};
Color RED = {.r = 0xFF, .g = 0x00, .b = 0x00};

void init_screen(void){
    vbeInfo = (vbe_mode_info *) VBEModeInfoBlockAddress;
}

uint16_t getScreenWidth() {
    return vbeInfo->width;
}

uint16_t getScreenHeight() {
    return vbeInfo->height;
}

int getXPixel(prompt_info * p){
    return p->x * font.Width;
}


int getYPixel(prompt_info * p){
    return p->y * font.Height;
}

static void setPixel(int x, int y, Color * color) {
    uint8_t * screen = (uint8_t *) ((uint64_t) vbeInfo->framebuffer + x * vbeInfo->bpp / 8 + (int) y * vbeInfo->pitch);
    screen[0] = color->b;
    screen[1] = color->g;
    screen[2] = color->r;
}

void drawShellBorder(Color * color) {
    for (int j = 0; j < vbeInfo->height; j++)
        for (int i = -2; i < 3; i++)
            setPixel(vbeInfo->width / 2 + i, j, color);
}


static void scrollUp(prompt_info * p, Color * backgroundColor) {
    for (int dy = 0; dy < p->windowHeight - font.Height; dy++){
        int x = (long )p->baseX;
        int yTo = p->baseY + dy;
        int yFrom = p->baseY + dy + font.Height;
        uint8_t * to = (uint8_t *) ((uint64_t) vbeInfo->framebuffer + x * vbeInfo->bpp / 8 + (long) yTo * vbeInfo->pitch);
        uint8_t * from = (uint8_t *) ((uint64_t) vbeInfo->framebuffer + x * vbeInfo->bpp / 8 + (long) yFrom * vbeInfo->pitch);
        memcpy(to, from, p->windowWidth * vbeInfo->bpp / 8);
    }
    
    for (int y = p->windowHeight - font.Height; y < p->windowHeight; y++)
        for (int x = (long)p->baseX; x < (long)p->baseX + p->windowWidth; x++) 
            setPixel(x, y, backgroundColor);
}

void clearWindow(prompt_info * p, Color * backgroundColor) {
    for (int x = 0; x < (long)p->windowWidth; x++)
        for (int y = 0; y < p->y * font.Height; y++)
            setPixel((long)p->baseX + x, p->baseY + y, backgroundColor);

    p->y = 0;
    p->x = 0;
}

static char isPromptOutOfWindowWidth(prompt_info * p) {
    return getXPixel(p) + font.Width - 1 >= p->windowWidth;
}

static char isPromptOutOfWindowHeight(prompt_info * p) {
    return getYPixel(p) + font.Height - 1 >= p->windowHeight;
}

void drawChar(prompt_info * p, char c, Color * fontColor, Color * backgroundColor) {
    if (isPromptOutOfWindowWidth(p)) {
        p->x = 0;
        p->y++;
    }

    if (isPromptOutOfWindowHeight(p)) {
        scrollUp(p, backgroundColor);
        p->y--;
    }

    
    uint8_t * bitMapCharacter = bitMap(c);
    int baseXPixel = (long)(p->x * font.Width + p->baseX);
    int baseYPixel = p->y * font.Height + p->baseY;
    for (int i = 0; i < font.Height; i++) {
        for (int j = 0; j < font.Width; j++) {
            if (bitMapCharacter[i] & 1 << (font.Width - j - 1)) // pregunta si el bit numero i esta prendido
                setPixel(baseXPixel + j, baseYPixel + i, fontColor);
            else
                setPixel(baseXPixel + j, baseYPixel + i, backgroundColor);
        }
    }
    p->x++;
}

void newLine(prompt_info * p, Color * backgroundColor) {
    p->x = 0;
    p->y++;
    if (isPromptOutOfWindowHeight(p)) {
        scrollUp(p, backgroundColor);
        p->y--;
    }
}

void eraseChar(prompt_info * p, Color * backgroundColor) {
    if (p->x == 0 && p->y == 0)
        return;

    if (p->x == 0) {
        p->x = p->windowWidth / font.Width - 1;
        p->y--;
    } else 
        p->x--;
    
    int baseXPixel = (long)p->baseX + getXPixel(p); // Coordenadas absolutas
    int baseYPixel = p->baseY + getYPixel(p);
    for (int y = baseYPixel; y < baseYPixel + font.Height; y++)
        for (int x = baseXPixel; x < baseXPixel + font.Width; x++)
            setPixel(x, y, backgroundColor);
}