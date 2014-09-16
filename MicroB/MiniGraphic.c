/*
** MiniGraphic.c
**
*/

#include <plib.h>
#include "MiniGraphic.h"
#include "font.h"

#define BOOT __attribute__ ((section ("extra_boot_mem")))

short _color;           // active color
short _back;            // background color
char _cx, _cy;          // cursor position

void  BOOT DelayMs(int time)
{
    int i;
    while(time--)
    {
        WriteCoreTimer( 0); 
        // core timer is at 1/2 system clock
        while( ReadCoreTimer() < (GetSystemClock() / 2000));
    }
} // DelayMs


void  BOOT SetReg(BYTE index, BYTE value)
{
    CS_LAT_BIT = 0;

    SetIndex(index);
    WriteCommand(value);

    CS_LAT_BIT = 1;
}


//void window( int x0, int y0, int x1, int y1)
//{
//    _left = x0;
//    _top = y0;
//    
//    // select the graphic controller window
//    SetReg(0x03, _left);
//    SetReg(0x04, x1 >> 8 );  
//    SetReg(0x05, x1);
//
//    SetReg(0x07, _top);
//    SetReg(0x08, 0);
//    SetReg(0x09, y1 );
//} // Window
//

void BOOT clear( void)
{ 
    int y;
    
    // fill the entire screen with back color
    CS_LAT_BIT = 0;

    SetAddress( 0, 0);
    RS_LAT_BIT = 1;             
    for( y=0; y<HRES*VRES; y++)
    {
        WriteData( _back);
    }
    CS_LAT_BIT = 1;
    
    // reset text cursor position
    _cx = _cy = 0;

} // clear




//void plot( int x, int y) 
//{ 
//    if (( x >=HRES)||(y>=VRES))
//        return; 
//
//    // send pixel to screen
//    CS_LAT_BIT = 0;
//    SetAddress( x, y);
//    RS_LAT_BIT = 1;             
//        WriteData( _color);
//    CS_LAT_BIT = 1;
//} // plot
//
//
//void line( int x0, int y0, int x1, int y1)
//{ // only for horizontal or vertical lines
//
//    if ( y0 == y1 )     // horizontal
//    {
//        if (x0 < x1) 
//            bar( x0, y0, x1, y0+1);
//        else
//            bar( x1, y0, x0, y0+1);
//    }
//    else // assume x0 == x1
//    {
//        if (y0 < y1)
//            bar( x0, y0, x0+1, y1);
//        else
//            bar( x0, y1, x0+1, y0);
//    }                 
//} // line 
//
    
void BOOT putV( int a)
{
    // monospaced 8x8 grid character print
    int i, j, c;
    char  *p;
    const unsigned char *pf;
    
    // 0. check for backspace
    if ( a == 0x8) 
    {
        if ( _cx > 0)
            _cx--;
        return;
    }     

    // 1. check if char in range
    if ( a < F_OFFS)         
        return;
    if ( a >= F_OFFS+F_SIZE)   
        return;
     
    // 2. check page boundaries and wrap or scroll as necessary
    if ( _cx >= HRES/8)          // wrap around x
    {
        _cx = 0;
        _cy++;   
    } 
    
    if ( _cy >= VRES/8)          //  clip
        return;     

    // 3. set pointer to word in the video map
    // set pointer to first row of the character in font array
    pf = &Font8x8[ (a-F_OFFS) * 8];
    
    // 4. copy one by one each line of the character on screen
    CS_LAT_BIT = 0;
    for ( i = 0; i<8; i++)
    {   
        SetAddress( (_cx*8), (_cy*8+i));
        RS_LAT_BIT = 1;             
        c = *pf++;
        // one pixel at a time
        for (j = 0; j< 8; j++)
        {
            if (c & 0x80)
            {
                WriteData( _color);
            }    
            else 
            {
                WriteData( _back);
            }    
            c<<=1;
        } // for j
    } // for i
    CS_LAT_BIT = 1;
    
    // 5. advance cursor position
    _cx++;
} // putV



void BOOT putcV( char A)
{
    int tab, i;
    
    switch( A)
    {
      case 0x08:    // backspace
        if ( _cx>0)
        {
            _cx--;
            putV(' ');
            _cx--;
        }    
        break;
        
      case '\t':    // move to next tab position
        tab = (_cx/8 + 1) * 8;
        // add spaces until the next tab stop
        for( i=_cx; i<tab; i++)
            putV(' ');
        break;

      case '\n':    // New Line
        _cy++;
        // break;   // continue into Home
        
      case '\r':    // Home
        _cx = 0;
        break;

      default:      // print-able char
        putV( A);
        break;
    } // switch    
} // putcV


void BOOT putsV( char *s)
{
    while (*s)
        putcV( *s++);
    // advance to next line
    //_cx=0; _cy++;
} // putsV


/*
**
**	Buttons read and debounce
*/

int BOOT MMBReadKey( void)
{   // returns 0..F if keys pressed, 0 = none
    int c = 0;

    if ( !_RA10) // Select
        c |= 16;
    if ( !BTN_A) 
        c |= 8;
    if ( !BTN_B) 
        c |= 4;
    if ( !BTN_C) 
        c |= 2;
    if ( !BTN_D) 
        c |= 1;

    return c;
} // readKEY


int BOOT MMBGetKey( void)
{   // wait for a key pressed and debounce
    int i=0, r=0, j=0;
    int c;

    // 1. wait for a key pressed for at least .1sec
    do{
        DelayMs( 10);
        if ( (c = MMBReadKey()))
        {
            if ( c>r)       // if more than one button pressed
                r = c;      // take the new code
            i++;    
        }
        else 
            i=0;
    } while ( i<10);

    // 2. wait for key released for at least .1 sec
    i =0;
    do {
        DelayMs( 10);   
        if ( (c = MMBReadKey()))
        {
            if (c>r)        // if more then one button pressed 
                r = c;      // take the new code
            i=0;            
            j++;            // keep counting 
        }
        else 
            i++;
    } while ( i<10);        
    
    // 3. check if a button was pushed longer than 500ms
    if ( j>50)              
        r+=0x80;            // add a flag in bit 7 of the code

    // 4. return code
    return r;
} // getKEY


//const char hexdig[] = "0123456789abcdef";
//
//// convert an integer into a (0x0000) ascii string
//void strhex( char *s, int x)
//{
//    int i;
//    *s++ = ' ';        // space 
//    for( i=0; i<4; i++, x<<=4)
//        *s++ = hexdig[ (x>>12)&0xf];
//    *s = '\0';          // close the string
//} // strhex    


//-------------------------------------------------------------
// main display initialization sequence
// customized for MultiInno MB283 QT-2 (HX8347-D controller)
//
void BOOT initV( void){
    int i;
    
    // Hold in reset
    RST_LAT_BIT = 0;   
    // Set reset pin as output
    RST_TRIS_BIT = 0;

    // Enable data access
    RS_LAT_BIT = 1;
    // Set RS pin as output
    RS_TRIS_BIT = 0;

    // Disable LCD 
    CS_LAT_BIT = 1;
    // Set LCD CS pin as output
    CS_TRIS_BIT = 0;

    // PMP setup 
    PMMODE = 0;
    PMAEN = 0;
    PMCON = 0;
    PMMODEbits.MODE  = 2;  // Master 2
    PMMODEbits.WAITB  = 0;
    PMMODEbits.WAITM  = 3;
    PMMODEbits.WAITE  = 0;

    PMMODEbits.MODE16 = 1;  // 16 bit mode

    PMCONbits.CSF    = 0;
    PMCONbits.PTRDEN = 1;    
    PMCONbits.PTWREN = 1;
    PMCONbits.PMPEN  = 1; 

    // Release from reset
    DelayMs(100);
    RST_LAT_BIT = 1;   
    DelayMs(150);

    // Driving ability setting
	SetReg(0xEA,0x00);   // PTBA[15:8]
	SetReg(0xEB,0x20);   // PTBA[7:0]
	SetReg(0xEC,0x0C);   // STBA[15:8]
	SetReg(0xED,0xC4);   // STBA[7:0]
	SetReg(0xE8,0x40);   // OPON[7:0]
	SetReg(0xE9,0x38);   // OPON1[7:0]
	SetReg(0xF1,0x01);   // OTPS1B
	SetReg(0xF2,0x10);   // GEN
	SetReg(0x27,0xA3);   // 
    
    // Gamma 2.8 setting 
	SetReg(0x40,0x00);   // 
	SetReg(0x41,0x00);   // 
	SetReg(0x42,0x01);   // 
	SetReg(0x43,0x13);   // 
	SetReg(0x44,0x10);   // 
	SetReg(0x45,0x26);   // 
	SetReg(0x46,0x08);   // 
	SetReg(0x47,0x51);   // 
	SetReg(0x48,0x02);   // 
	SetReg(0x49,0x12);   // 
	SetReg(0x4A,0x18);   // 
	SetReg(0x4B,0x19);   // 
	SetReg(0x4C,0x14);   // 
	
	SetReg(0x50,0x19);   // 
	SetReg(0x51,0x2F);   // 
	SetReg(0x52,0x2C);   // 
	SetReg(0x53,0x3E);   // 
	SetReg(0x54,0x3F);   //          
	SetReg(0x55,0x3F);   // 
	SetReg(0x56,0x2E);   // 
	SetReg(0x57,0x77);   // 
	SetReg(0x58,0x0B);   // 
	SetReg(0x59,0x06);   //          
	SetReg(0x5A,0x07);   // 
	SetReg(0x5B,0x0D);   // 
	SetReg(0x5C,0x1D);   // 
	SetReg(0x5D,0xCC);   // 

// set display window
    SetReg(0x04,0x01);
    SetReg(0x05,0x3F);
    SetReg(0x08,0x00);
    SetReg(0x09,0xEF);

//#if (DISP_ORIENTATION == 0)
//	SetReg(0x16,0x08);   // MY=0, MX=0, MV=0, BGR=1
//#else
	SetReg(0x16,0x68);   // MY=0, MX=1, MV=1, BGR=1
//#endif


    // Power Voltage Setting
	SetReg(0x1B,0x1B);   // VRH = 4.65
	SetReg(0x1A,0x01);   // BT
	SetReg(0x24,0x2F);   // VMH
	SetReg(0x25,0x57);   // VML
	
	// Vcom offset
	SetReg(0x23,0x8D);   // FLICKER ADJUST

    // Power ON Setting
	SetReg(0x18,0x36);   // 
	SetReg(0x19,0x01);   //  
	SetReg(0x01,0x00);   // 
	SetReg(0x1F,0x88);   // 	
	DelayMs(5); 
	SetReg(0x1F,0x80);   // 	
	DelayMs(5); 
	SetReg(0x1F,0x90);   // 	
	DelayMs(5); 
	SetReg(0x1F,0xD0);   // 	
	DelayMs(5); 

    // 65K Color Selection
	SetReg(0x17,0x05);   // 	
    
    // Set Panel
  	SetReg(0x36,0x00);   // 	

    // Display ON Setting
	SetReg(0x28,0x38);   // 	
	DelayMs( 40); 
	SetReg(0x28,0x3C);   // 	

    // pick default colors 
    _back = BLACK;
    _color = WHITE;
    
    // clear the entire screen (and set window)
    clear();  
    
} // initV
