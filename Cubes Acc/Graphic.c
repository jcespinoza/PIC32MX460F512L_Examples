/*
** graphic.c
**
** QVGA TFT double buffering
**
** Double buffered Graphic page for PIC32MX
**  8/02/07 v2.2 
**  10/23/07 v3.0 72MHz operation
**  11/20/07 v3.1 removed OC3 interrupt
**  08/05/09 v.4.0  TG port, Output on QVGA display
**  01/04/10 v.5.0  256x200 4-bit color on PIC32MMB
*/

#include <plib.h>
#include <string.h>

#include "graphic.h"
#include "GenericTypeDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/HX8347.h"
#include "font.h"


// memory map
static char V[ VRES*HRES/2]; // image buffer (4-bit color)

volatile char fFrame;
volatile int TimerX;

char gColor;         // active color
char cx, cy;         // cursor position
static WORD LUT[ 16] = {
    //0      1      2     3    4    5    6    7    8       
    BLACK, BRIGHTRED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE,
    // 9      A    B      C    D     E   F
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
    };

// clipping box coordinates
static int clipmx = 0, clipmy = 0, clipx = HRES, clipy = VRES;

#define FRAME_T      ( (FPB / 1000) / 256)     // 25 fps  = 40ms (timer prescaler 1:256)

void copyV( void)
{
    int y;
    char *pV;
       
    // refresh the main screen
    CS_LAT_BIT = 0;
    SetAddress( 0, 0);
    RS_LAT_BIT = 1;             
    pV = &V[0];
    for( y=0; y<VRES*HRES/2; y++, pV++)
    {        
        WriteData( LUT[ (*pV) >> 4]);
        //     PMDIN1 = data;              
        WriteData( LUT[ (*pV) & 0xf]);   
        //    PMDIN1 = data;             
    }    
    CS_LAT_BIT = 1;
    
    // post a message, frame completed and update TimerX
    fFrame = 1;
    TimerX++;
    
} // copyV


void initVideo( void)
{
    gClearScreen();
    InitGraph();

    // define HRES x VRES window
    SetReg(0x04,(HRES-1)>>8);  
    SetReg(0x05,(HRES-1));
    SetReg(0x08,0x00);
    SetReg(0x09,(VRES-1));

    copyV();
    BacklightOn();
    
    // 4. Timer3 on, prescaler 1:8, internal clock, period 
    //OpenTimer3(  T3_ON | T3_PS_1_256 | T3_SOURCE_INT, FRAME_T-1);


    // 9. Enable Timer3 Interrupts
    // set the priority level 6
    //mT3SetIntPriority( 6);
    //mT3IntEnable( 1);

} // initVideo


void gClearScreen( void)
{ // fill with zeros the Active Video array
    memset( V, 0, VRES * HRES/2);

    // reset text cursor position
    cx = cy = 0;

} //clearScreen


void gPlotV( int x, int y) 
{ 
    char *p;
    
    // clipped negative
    if ( (x < clipmx) || (y < clipmy))
        return;
    if ((x < clipx) && (y < clipy) )
//        uPlotV( x, y);
    {
        y = VRES-1-y;
        p = &V[ HRES/2 * y + (x>>1)];
        if (x&1)
            *p = (*p & 0xf0) | gColor;
        else
            *p = (*p & 0x0f) | (gColor << 4);
            
    } //uPlot    
} // plot


#define abs( a)     (((a)> 0) ? (a) : -(a))


void gLine(int x0, int y0, int x1, int y1)
{   
    // clipped generic line
    BOOL steep;
    int t ;
    int deltax, deltay, error;
    int x, y;
    int ystep;
    int _clipx, _clipy, _clipmx, _clipmy;
    
    // advanced clipping 

    // 1. remove extreme cases when no part of the line is visible
    if (( x0 < clipmx) && ( x1 < clipmx))      
        return;         // entire line outside left edge
    if (( y0 < clipmy) && ( y1 < clipmy))       
        return;         // entire line below bottom
    if (( x0 >= clipx) && ( x1 >= clipx))
        return;         // entire line outside right edge
    if (( y0 >= clipy) && ( y1 >= clipy))
        return;         // entire line above the top
         
     
    // 2. reduction to shallow lines (swap x and y)
    steep = ( abs(y1 - y0) > abs(x1 - x0));
    _clipx = clipx;
    _clipy = clipy;
    _clipmx = clipmx;
    _clipmy = clipmy;
    
    if ( steep )
    { // swap x and y
         t = x0; x0 = y0; y0 = t;
         t = x1; x1 = y1; y1 = t;
         _clipx = clipy; _clipy = clipx;
         _clipmx = clipmy; _clipmy = clipmx;
    }

    // 3. reduction to left to right drawing (swap ends)
    if (x0 > x1) 
    {  // swap ends
         t = x0; x0 = x1; x1 = t;
         t = y0; y0 = y1; y1 = t;
    }

    // 4. get the deltas
    deltax = x1 - x0;
    deltay = y1 - y0;

    // 5. check intersection with the y axis (only x0 could)
    if ( x0 < _clipmx)
    {
        x0 = _clipmx;                       // clip left edge
        // intercept y axis (x=_climx)
        y0=(y1*deltax-deltay*x1+deltay*_clipmx)/deltax;    
    }   

    // 4. check intersection with right edge (only x1 could)
    if (x1>_clipx)			
    {                                      // clip right edge
         x1 = _clipx;
         // intercept right edge (x=clipx)
         y1=(y1*deltax-deltay*x1+deltay*_clipx)/deltax;
    }

    // 5. depending on line inclination  
    if (y0 < y1)       // rising
    {
         ystep = 1; 
         // 5.1 check y0 against bottom and y1 against top
         if (y0 < _clipmy)
         {
             y0 = _clipmy;
             // intercept x axis (y= clipmy)
             x0=(deltay*x1-deltax*y1+deltax* _clipmy)/deltay;
         }    
         if (y1 > _clipy)
         {
             y1 = _clipy;
             // intercept top (y=clipy)
             x1=(deltay*x0-deltax*y1+deltax*_clipy)/deltay;
         }    
    }    
    else               // falling or flat
    {
         ystep = -1;
         // 5.2 check y1 against bottom and y0 against top
         if (y1 < _clipmy)
         {
             y1 = _clipmy;
             // intercept x axis (y= clipmy)
             x1=(deltay*x0-deltax*y1+deltax* _clipmy)/deltay;
         }    
         if (y0 > _clipy)
         {
             y0 = _clipy;
             // intercept top ( y=_clipy)
             x0=(deltay*x1-deltax*y1+deltax* _clipy)/deltay;
         }    
    }

    // 6. init the drawing loop
    deltay = abs( deltay);
    error = 0;
    y = y0;

    // 7. the drawing loop (left to right) 
    for (x = x0; x < x1; x++)
    {
        if ( steep) gPlotV( y, x); 
        else gPlotV( x, y);
         
        error += deltay;
         
         if ( (error<<1) >= deltax)
         {
             y += ystep;
             error -= deltax;
         } // if 
    } // for
} // Clip Line 


void putcV( int a)
{
    // monospaced 8x8 grid character print
    int i, j, c;
    char  *p;
    const unsigned char *pf;
    
    // 0. check for backspace
    if ( a == 0x8) 
    {
        if ( cx > 0)
            cx--;
        return;
    }     

    // 1. check if char in range
    if ( a < F_OFFS)         
        return;
    if ( a >= F_OFFS+F_SIZE)   
        return;
     
    // 2. check page boundaries and wrap or scroll as necessary
    if ( cx >= HRES/8)          // wrap around x
    {
        cx = 0;
        cy++;   
    } 
    
    if ( cy >= VRES/8)          //  scroll up y
    {
        char *pd = V;
        char *ps = pd + (HRES)*8;
        for( i=0; i<(HRES/2)*(VRES-8); i++)
            *pd++ = *ps++;
        for( i=0; i<(HRES/2)*8; i++)
            *pd++ = 0;
        // keep cursor within boundary
        cy = VRES/8-1;
    }    
    // 3. set pointer to word in the video map
    // set pointer to first row of the character in font array
    pf = &Font8x8[ (a-F_OFFS) * 8];
    
    // 4. copy one by one each line of the character on screen
    for ( i = 0; i<8; i++)
    {   
        p = &V[ cy * 8 * HRES/2 + i * HRES/2 + cx*4];
        c = *pf++;
        // two pixel at a time
        for (j = 0; j< 4; j++)
        {
            if (c & 0x80)
                *p = (*p & 0x0f) | (gColor << 4);   // overimposed character

            if (c & 0x40)
                *p = (*p & 0xf0) | (gColor & 0xf);   // overimposed character
            p++;
            c<<=2;
        }
    } // for
    
    // 5. advance cursor position
    cx++;
} // putcV


void putsV( char *s)
{
    while (*s)
        putcV( *s++);
    // advance to next line
    cx=0; cy++;
} // putsV


