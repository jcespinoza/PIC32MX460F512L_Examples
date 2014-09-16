/*
**  graphic.h
** 
**  QVGA double buffered 
**
**  11/20/07 v 4.0 LDJ output on QVGA
**  01/04/09 v.5.0 LDJ output on 256*200 on PIC32MMB
*/

#define VRES     200    // desired vertical resolution
#define HRES     256    // desired horizontal resolution pixel
#define FPB 80000000L    // peripheral bus frequency


void initVideo( void);

void gClearScreen( void);
void copyV( void);

void gPlotV( int x, int y); 

void gLineV( int x0, int y0, int x1, int y1);

#define setColor(x)     gColor = (x)
#define getColor        (gColor)
#define getFrame()      (fFrame)
#define clrFrame()      fFrame = 0
#define setTimerX(x)    TimerX = (x)
#define getTimerX()     TimerX

extern char cx, cy;
extern char gColor;
extern volatile char    fFrame;
extern volatile int     TimerX;

//
// text on video 
//

#define Home()      { cx=0; cy=0;}
#define Clrscr()    { clearScreen(); Home();}
#define AT( x, y)   { cx = (x); cy = (y);}
    
void putcV( int a);

void putsV( char *s);


