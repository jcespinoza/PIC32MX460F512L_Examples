/*
**  MiniGraphic.h
** 
*/

#include "HardwareProfile.h"

#define HRES     320    // desired vertical resolution
#define VRES     240    // desired horizontal resolution pixel

//------------------------------------------------------------
// predefined RGB 565 color palette
//
#define RGB565CONVERT(red, green, blue) (WORD) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

    #define BLACK           RGB565CONVERT(0, 0, 0)
    #define BRIGHTBLUE      RGB565CONVERT(0, 0, 255)
    #define BRIGHTGREEN     RGB565CONVERT(0, 255, 0)
    #define BRIGHTCYAN      RGB565CONVERT(0, 255, 255)
    #define BRIGHTRED       RGB565CONVERT(255, 0, 0)
    #define BRIGHTMAGENTA   RGB565CONVERT(255, 0, 255)
    #define BRIGHTYELLOW    RGB565CONVERT(255, 255, 0)
    #define BLUE            RGB565CONVERT(0, 0, 128)
    #define GREEN           RGB565CONVERT(0, 128, 0)
    #define CYAN            RGB565CONVERT(0, 128, 128)
    #define RED             RGB565CONVERT(128, 0, 0)
    #define MAGENTA         RGB565CONVERT(128, 0, 128)
    #define BROWN           RGB565CONVERT(255, 128, 0)
    #define LIGHTGRAY       RGB565CONVERT(128, 128, 128)
    #define DARKGRAY        RGB565CONVERT(64, 64, 64)
    #define LIGHTBLUE       RGB565CONVERT(128, 128, 255)
    #define LIGHTGREEN      RGB565CONVERT(128, 255, 128)
    #define LIGHTCYAN       RGB565CONVERT(128, 255, 255)
    #define LIGHTRED        RGB565CONVERT(255, 128, 128)
    #define LIGHTMAGENTA    RGB565CONVERT(255, 128, 255)
    #define YELLOW          RGB565CONVERT(255, 255, 128)
    #define WHITE           RGB565CONVERT(255, 255, 255)
    #define GRAY0           RGB565CONVERT(224, 224, 224)
    #define GRAY1           RGB565CONVERT(192, 192, 192)
    #define GRAY2           RGB565CONVERT(160, 160, 160)
    #define GRAY3           RGB565CONVERT(128, 128, 128)
    #define GRAY4           RGB565CONVERT(96, 96, 96)
    #define GRAY5           RGB565CONVERT(64, 64, 64)
    #define GRAY6           RGB565CONVERT(32, 32, 32)

// members
extern short  _color, _back;
extern char _cx, _cy;
extern const char Font8x8[];

 
#define SetColor(x)     _color = (x)
#define GetColor()      _color
#define SetBackColor(x) _back = (x)
#define GetBackColor()  _back

//
// graphics primitives prototypes
//
void initV( void);
void clearV( void);
void plot( int x, int y); 
void line( int x0, int y0, int x1, int y1);
void bar( int, int, int, int);


//
// text on video prototypes
//
#define Home()      { cx=0; cy=0;}
#define AT( x, y)   { _cx = (x); _cy = (y);}
    
void putcV( char a);
void putsV( char *s);

// Key input
int MMBReadKey( void);
int MMBGetKey( void);


// Driver definitions
#define PMPWaitBusy()   while(PMMODEbits.BUSY);
    
#define SetIndex(index) \
    RS_LAT_BIT = 0;         \
    PMDIN = index;         \
    PMPWaitBusy();

#define WriteCommand(cmd) \
    RS_LAT_BIT = 1;           \
    PMDIN = cmd;             \
    PMPWaitBusy();

#define WriteData(data) \
    RS_LAT_BIT = 1;             \
    PMDIN = data;              \
    PMPWaitBusy();

#define SetAddress(x, y)                  \
    SetIndex(0x02);                           \
    WriteCommand(((WORD_VAL) (WORD) x).v[1]); \
    SetIndex(0x03);                           \
    WriteCommand(((WORD_VAL) (WORD) x).v[0]); \
    SetIndex(0x06);                           \
    WriteCommand(((WORD_VAL) (WORD) y).v[1]); \
    SetIndex(0x07);                           \
    WriteCommand(((WORD_VAL) (WORD) y).v[0]); \
    SetIndex(0x22);
