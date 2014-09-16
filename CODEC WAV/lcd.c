/******************************************************************************

    LCD interface file (2x16 emulation on MMB QVGA display)

Description:
    This file contains basic LCD output routines for MikroE MMB

Summary:
    This file contains basic LCD output routines for MikroE MMB

Remarks:
    None

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       lcd.c
* Dependencies:    None
* Processor:       PIC32MX4
* Compiler:        C32 v1.04 or later
* Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

Author          Date    Comments
--------------------------------------------------------------------------------
ADG         14-Apr-2008 First release
LDJ         18-Aug-2009 Modified for MikroE MMB 
*******************************************************************************/


#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "lcd.h"
#include "Graphics/Graphics.h"

extern const FONT_FLASH Font25;

// font size is based on default font 
#define FONT_W  16
#define FONT_H  25
static int _cx, _cy;    // cursor position


void LCDInit(void)
{
    InitGraph();    // initialize graphics library
    SetColor( BLACK); 
    ClearDevice();
    _cx = 0; 
    _cy = 0;        
    SetColor( GREEN); 
    SetFont( (void *)&GOLFontDefault);          // set font

}



void LCDHome(void)
{   // set cursor to home position
    _cx = 0; 
    _cy = 0;
}


void LCDL1Home(void)
{
    _cx = 0;
    _cy = 0;
}

void LCDL2Home(void)
{
    _cx = 0;
    _cy = 1;
}


void LCDClear(void)
{
    SetColor( BLACK); 
    ClearDevice();
    SetColor( GREEN); 
    _cx = 0; 
    _cy = 0;
}


void LCDPutChar(char A)
{
    char s[2] = "";
    s[0] = A;
    OutTextXY( FONT_W * _cx++, FONT_H*_cy, s);
}


void LCDPut( char a)
{
    switch ( a)
    {
      case '\b':
        LCDShiftCursorLeft();
        SetColor( BLACK);
        Bar( _cx * FONT_W, _cy * FONT_H, (_cx+1) * FONT_W, (_cy+1) * FONT_H);
        SetColor( GREEN);
        break;
      case '\n': 
        LCDShiftCursorDown();
        break;
      case '\r':
        _cx = 0;
        break;
      case '\t':
        _cx = ((_cx/8)+1)*8;
        break;
      default:
        LCDPutChar( a);
        break;
    } // switch    
} // LCDPut
   

void LCDPuts( char *s)
{
    while (*s)
        LCDPut(*s++);
}

void LCDShiftCursorLeft(void)
{
    if (_cx>0) _cx--;
}


void LCDShiftCursorRight(void)
{
    if ( ( _cx+2) * FONT_W < GetMaxX()) _cx++;
}

void LCDShiftCursorUp(void)
{
   if ( _cy>0) _cy--;
}

void LCDShiftCursorDown(void)
{
    if ( (_cy+2) * FONT_H < GetMaxY()) _cy++;
    _cx = 0;
}

void Wait(unsigned int B)
{
}


