/*****************************************************************************
 * Mandelbrot on MMB using Fixed Point Math
 * 
 *****************************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    MainDemo.h
 * Processor:       PIC32
 * Compiler:       	MPLAB C32
 * Linker:          MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *****************************************************************************
 *
** Mandelbrot Set Advanced Graphic Demo (fixed point version)
**
** 8/16/06 rev 3.0  Lucio Di Jasio
** 8/08/07 rev 3.1  LDJ PIC32 porting 
** 3/28/09 rev 4.0  LDJ uses fixed point math
** 7/23/09 rev 4.1  LDJ 32-bit math (+16 MAXPOS)
** 8/17/09 rev 5.0  LDJ Testing on MikroE-MMB
**
** HW req.: MikroE MMB board 
** SW req.: 
**
** This demo is an enhanced version of the Mandelbrot demo 
** presented in chapter 13 
** self timing for performance comparisons and 
** offering additional navigation features:
**  - joystick Up, Down, Left, Right     -> Move 
**  - joystick Select                    -> Zoom
**  - joystick Up holding for 1 second   -> Restart
**  - joystick Left holding for 1 second -> Undo
*/

#include <plib.h>
#include <math.h>
#include "MMB.h"
#include "HardwareProfile.h"
#include "Graphics/Graphics.h"
#include "dsplib_def.h"

// Configuration bits 
#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2
#pragma FPBDIV = DIV_1
#pragma config OSCIOFNC = ON, POSCMOD = XT, FSOSCEN = ON, FNOSC = PRIPLL
#pragma config CP = OFF, BWP = OFF, PWP = OFF
//#define FCY     80000000    // system bus freq.
//#define FPB     80000000    // peripheral bus freq.

///////////////////////////////// FONTS ///////////////////////////////////////
extern const FONT_FLASH GOLFontDefault;


#define SIZE  240
#define MAXIT 128
//                    R     G    B
#define COLORX 0b00000000000100010

#define FIX_MAXPOS  +16
#define FIX_SCALE   ((1<<30) / FIX_MAXPOS)
#define FP_TWO      ( 2 * FIX_SCALE)
#define FP_MTWO     (-2 * FIX_SCALE)
#define FP_FOUR     ( 4 * FIX_SCALE)


int32 fix( float f)
{
    return ( floor( ( (double)f * FIX_SCALE) +.5));
} // fix    

float flt( int32 i)
{
    return (float)i / FIX_SCALE;
} // flt        


static __inline int fmul32(int x, int y)
{   
    int64 z = ((int64) x * y);
    return (z >> (30-4));
}


void mandelbrot( int32 xx0,  int32 yy0, int wx, int wy, int32 d)
{
    int32 x, y, x0, y0, x2, y2;

    int i, j, k;

    
    // repeat on each screen pixel
    y0 = yy0;               
    for (i=0; i<wx; i++)
    {
        x0 = xx0;           
        for (j=0; j<wy; j++)
        {        
            // initialization
              x = x0;       
              y = y0;              
              k = 0;
             
             // core iteration
              do {
                x2 = fmul32( x, x);         
                y2 = fmul32( y, y);       
                y  = 2 * fmul32(x, y) + y0;    
                x  = x2 - y2 + x0;                                  
                
                k++;
                if ((x2 + y2) > FP_FOUR)
                    break;
              } while ( k < MAXIT); 
            
            // check if the point belongs to the Mandelbrot set
            //SetColor( ( k & 1) ? WHITE : BLACK);
            SetColor( (k * COLORX ));
            PutPixel( j, wy-i);
            //if ( k == MAXIT) plot( j, i);

            // compute next point x0
            x0 += d;      //fx0 += fd;             
         } // for j
         // compute next y0
         y0 += d;           //fy0 +=fd;

        // check for a break 
       if (MMBReadKey())
        {   
            MMBGetKey();
            return;
        }
     } // for i
 } // mandelbrot


const char logo1[] = "Mandelbrot Set Demo";
const char logo2[] = "v.5.0";
const char logo3[] = "(move joystick to start)";

void StopWatch( void)
{        
    double t;
    char s[32];
    
    t = (double)ReadTimer45() / GetPeripheralClock();
    sprintf( s, "StopWatch = %0.6lf", t);
    SetColor( BLACK);
    Bar(0,0, 320, 24); 
    SetColor( WHITE);
    OutTextXY( 0,0, s);

} // StopWatch


int main( void)
{
    short buffer[160];  // line buffer
    int i;
    
    double x, y, w;     // main screen coordinates
    int wx, wy;         // painting square size
    double ox, oy, ow;  // previous screen coordinates
    int c, oc, k, r;    // temp 
    BOOL recalc;        // flag need to refresh screen
    unsigned t;         // computed time
    char s[128];        // text output buffer


    // initializations
	MMBInit();     // initialize the MMB board
    InitGraph();    // initialize graphics library

    // Show Splash Screen
    { 
        int width, height, x, y;
 
        SetColor(BRIGHTRED);        // set color
        SetFont( (void *)&GOLFontDefault);          // set font
    
        // place logo in the middle of the screen
        // get string width & height
        height = GetTextHeight( (void *)&GOLFontDefault);
        y = (GetMaxY() - 3 * height)/2;
        x = (GetMaxX() - GetTextWidth( (void *)logo1, (void *)&GOLFontDefault))/2;
        OutTextXY(  x, y, (char*)logo1);
        x = (GetMaxX() - GetTextWidth( (void *)logo2, (void *)&GOLFontDefault))/2;
        OutTextXY(  x, y+height, (char*)logo2);
        x = (GetMaxX() - GetTextWidth( (void *)logo3, (void *)&GOLFontDefault))/2;
        OutTextXY(  x, y+2*height, (char*)logo3);
         
        MMBFadeIn(500);
        
        // wait for a key press
        MMBGetKey();
    } // splash screen
    
    
    // initial coordinates lower left corner of the grid 
    x = -2.0;
    y = -1.5;
    // initial grid size
    wx = SIZE;
    wy = SIZE;
    w = 3.0;
    c = 1;
    
	while( 1)
	{    
        // clear screen 
        SetColor( BLACK); ClearDevice();
        
        // update coordinates
        SetColor( WHITE); 
        sprintf( s, "%+.6f", x); 
        OutTextXY( SIZE+8, 0, s);
        sprintf( s, "%+.6f", y); 
        OutTextXY( SIZE+8, 25, s);
        sprintf( s, "%+.6f", w); 
        OutTextXY( SIZE+8, 50, s);
        sprintf( s, "x%d", c); 
        OutTextXY( SIZE+8, 75, s);

        // start timer
        MMBStartStopwatch();
        
        // draw new image
        mandelbrot( fix(x), fix(y), wx, wy, fix(w/SIZE));   
        
        // read timer and print 
        t = MMBReadStopwatch();
        sprintf( s, "%.6lf", (double)t / 1000 );
        SetColor( WHITE);
        OutTextXY( SIZE+8, 100, s);
        
        // wait for a command
        do {
            recalc = TRUE;
            k = MMBGetKey();
            // Undo 
            if (k == 0x80 + JOY_LEFT)
            {
                x = ox; y = oy;
                w = ow; c = oc;
            }          
            else    
            {   // save values for undo
                ox = x; oy = y;       
                ow = w; oc = c;
                    
                switch( k)
                {  // select function   
                case JOY_LEFT:            // first quadrant
                    x -= w/3;
                    break;
                
                case JOY_UP:            // second quadrant
                    y += w/3;
                    break;
                
                case JOY_RIGHT:            // third quadrant
                    x += w/3;
                    break;
                
                case JOY_DOWN:            // fourth quadrant
                    y -= w/3;
                    break;
                        
                // first and second button                        
                case JOY_SELECT:          // zoom in center
                    x += w/4;
                    y += w/4;
                    w /=2;
                    c<<=1;
                    break;
                
                // press and hold first button
                case 0x80 + JOY_UP:          // reset origin
                    x = -2.0;
                    y = -2.0;
                    w = 4.0;
                    c = 1;
                    break;
                    
                default:
                    recalc = FALSE;
                    break;  
                } // switch
            } // else
        } while( !recalc);
    } // main loop
} // main



