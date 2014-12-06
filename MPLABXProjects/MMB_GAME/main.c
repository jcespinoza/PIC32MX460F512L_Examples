#include <plib.h>
#include "MMB.h"
#include "LCDTerminal.h"
#include "Graphics/Graphics.h"
#include "HardwareProfile.h"

// Configuration bits 
#pragma config POSCMOD = XT, FNOSC = PRIPLL, FSOSCEN = ON
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_1, FPBDIV = DIV_1
#pragma config OSCIOFNC = ON, CP = OFF, BWP = OFF, PWP = OFF

struct Sprite {
    int x, y; //Position
    int width, height; //Dimensions
    BITMAP_FLASH *image;
};

// fonts
extern const FONT_FLASH TerminalFont;

/* Image bitmaps 4bpp */
extern const unsigned char pacman_up[];
extern const unsigned char pacman_down[];
extern const unsigned char pacman_left[];
extern const unsigned char pacman_right[];
extern const unsigned char fruit1[];
extern const unsigned char fruit2[];
extern const unsigned char fruit3[];
extern const unsigned char key[];

BITMAP_FLASH pacman_up_bmp = {FLASH, pacman_up};
BITMAP_FLASH pacman_down_bmp = {FLASH, pacman_down};
BITMAP_FLASH pacman_left_bmp = {FLASH, pacman_left};
BITMAP_FLASH pacman_right_bmp = {FLASH, pacman_right};

BITMAP_FLASH fruit1_bmp = {FLASH, fruit1};
BITMAP_FLASH fruit2_bmp = {FLASH, fruit2};
BITMAP_FLASH fruit3_bmp = {FLASH, fruit3};
BITMAP_FLASH key_bmp = {FLASH, key};

/* End of bitmaps declarations */

//Global variables
volatile int count = 0;
volatile int doRefresh = 0;
struct Sprite pacman;
struct Sprite food[4];

//This routine gets executed every 40ms
void __ISR(_TIMER_1_VECTOR, IPL3) Timer1_ISR(void)
{
    int key;

    mT1ClearIntFlag();
    count++;

    key = MMBReadKey();

    switch (key) {
        case JOY_RIGHT: break;
        case JOY_LEFT: break;
        case JOY_UP: break;
        case JOY_DOWN: break;
        case JOY_SELECT: LD3 = !LD3; break;
    }
    
    if (count == 25) { //1 second delay, blink LD0
        LD0 = !LD0;
        count = 0;
    }
}

void StartTimer1()
{
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_3 | T1_INT_SUB_PRIOR_1);
    OpenTimer1(T1_ON | T1_IDLE_CON | T1_PS_1_256 | T1_SOURCE_INT, 12500);
}

void inline getBitmapDimensions(BITMAP_FLASH *bmp, int *w, int *h) {
    const FLASH_WORD *bmpData = (FLASH_WORD *)bmp->address;

    *w = bmpData[2];
    *h = bmpData[1];
}

void InitSprites() {

    //Packman
    pacman.x = 0;
    pacman.y = 0;
    getBitmapDimensions(&pacman_right_bmp, &pacman.width, &pacman.height);
    pacman.image = &pacman_right_bmp;

    //Food
    food[0].x = 60;
    food[0].y = 60;
    getBitmapDimensions(&fruit1_bmp, &food[0].width, &food[0].height);
    food[0].image = &fruit1_bmp;

    food[1].x = 260;
    food[1].y = 60;
    getBitmapDimensions(&fruit2_bmp, &food[1].width, &food[1].height);
    food[1].image = &fruit2_bmp;

    food[2].x = 260;
    food[2].y = 180;
    getBitmapDimensions(&fruit3_bmp, &food[2].width, &food[2].height);
    food[2].image = &fruit3_bmp;

    food[3].x = 60;
    food[3].y = 180;
    getBitmapDimensions(&key_bmp, &food[3].width, &food[3].height);
    food[3].image = &key_bmp;
}

int main(void)
{
    int i, x, y;
    
    MMBInit(); // Initialize the MikroE MMB board
    LCDInit();
    InitSprites();
    
    PutImage(0, 0, &pacman_left_bmp, 1);
    PutImage(pacman.width*1 + 2*1, 0, &pacman_right_bmp, 1);
    PutImage(pacman.width*2 + 2*2, 0, &pacman_down_bmp, 1);
    PutImage(pacman.width*3 + 2*3, 0, &pacman_up_bmp, 1);

    x = 0;
    y = pacman.height + 2;
    for (i=0; i<4; i++) {
        PutImage(x, y, food[i].image, 1);
        x += food[i].width + 2;
    }
    
    LCDSetXY(1, 4);
    SetColor(BRIGHTRED);
    LCDPutString("Welcome to PACMAN GAME");

    SetColor(WHITE);
    LCDSetXY(1, 7);
    LCDPutString("(move joystick to start)");

    MMBGetKey();
    
    StartTimer1(); //Start Timer1

    doRefresh = 1;
    while (1) {
        while (doRefresh == 0) {}

        doRefresh = 0;

        //TODO: Repaint screen
    }

}




