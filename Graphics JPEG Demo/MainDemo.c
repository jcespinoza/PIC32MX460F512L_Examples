/******************************************************************************
*
* This demo shows how to display JPEG files from internal and external memories.
*
******************************************************************************
* The "Font and Bitmap Converter" should be used to add JPEG files as C arrays
* for the internal flash and as Intel HEX file for the external memory. All C files
* generated by this utility must be added in your project. Intel HEX file must be
* programmed into the external memory. For Graphics PICtail 2 and Graphics PICtail 3
* "Graphics PICtail Board Memory Programmer" project can be used to program the HEX file
* into flash memory on these PICtails.
*
* The header file for the external memory access driver must be added into JPEGImage.h.
* In the external memory access driver the data array reading function must be implemented.
* The function must be specified in JPEGImage.h file (see the header file for details).
*
* JPEG displaying example:
*
* extern BITMAP_FLASH     jpegFileInInternalFlash;
* extern BITMAP_EXTERNAL  jpegFileInExternalFlash;

* int main(){
*   ExternalMemoryDriverInit(); // initialize driver for external memory used to store JPEG files
*   GraphInit();                // initialize graphics library. GOLInit() can be used instead
*   JPEGInit();                 // initialize JPEG decoder
*   JPEGPutImage(10,10,&jpegFileInInternalFlash); // display picture from internal memory
*   JPEGPutImage(20,13,&jpegFileInExternalFlash); // display picture from external memory
* }
*
* JPEGImage.h/JPEGImage.c module depends on files from the Graphics Library.
*
* To add the JPEGImage.h/JPEGImage.c module to the Graphics Library project the following
* files should be included:
*   > ImageDecoder.c - Image Decoders library file
*   > JpegDecoder.c - Image Decoders library file
*   > jidctint.c - Image Decoders library file
*   > JPEGImage.c - contains JPEGInit() and JPEGPutImage(...) implementations
*   > ImageDecoderConfig.h - must be in the project folder
*   > JPEGImage.h - must be in the project folder
*   > C files generated by the utility with JPEG images located in internal memory
*   > C reference files generated by the utility with references to JPEG images located
*     in external memory (these files are created for each HEX file)
*
*******************************************************************************
* FileName:        MainDemo.c
* Dependencies:    Image decoding library, Graphics Library, SST25 and SST39 flash drivers
* Processor:       PIC24F, PIC24H, dsPIC, PIC32
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology, Inc.

 * Software License Agreement
 *
 * Copyright � 2009 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY
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

Author                 Date           Comments
--------------------------------------------------------------------------------
Anton Alkhimenok       05-May-2009
Lucio Di Jasio         08-Dec-2009  Adapted to MikroE MX4 MMB
*******************************************************************************/

#include "MMB.h"
#include "JPEGImage.h"

// Configuration bits
#pragma config POSCMOD = XT, FNOSC = PRIPLL, FSOSCEN = ON
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_8
#pragma config FWDTEN = OFF, FCKSM = CSECME, OSCIOFNC = ON, CP = OFF, BWP = OFF, PWP = OFF

/////////////////////////////////////////////////////////////////////////////
// JPEG Files
/////////////////////////////////////////////////////////////////////////////
extern BITMAP_FLASH Logo;
extern BITMAP_EXTERNAL Scene05;
extern BITMAP_EXTERNAL Scene04;
extern BITMAP_EXTERNAL Scene03;
extern BITMAP_EXTERNAL Scene02;
extern BITMAP_EXTERNAL Scene01;

#define DELAY_MS 500
#define DELAYW_MS 3000
#define DELAY2_MS 250

int main(void)
{
    MMBInit();
    SFLASHInit();
    InitGraph(); // Initialize graphics
    JPEGInit();  // Initialize JPEG

    
//    SetFont((void*)&GOLFontDefault);

    while(1)
    {
        // FROM INTERNAL FLASH
//        JPEGPutImage(0,0,&Logo); 
//        MMBFadeIn( DELAY2_MS);
//        SetColor(WHITE);
////        OutTextXY(0,0,"INTERNAL FLASH");
//        DelayMs(DELAYW_MS);
//        MMBFadeOut( DELAY_MS);
        
        // FROM EXTERNAL MEMORY
        JPEGPutImage(0,0,&Scene01);
        MMBFadeIn( DELAY2_MS);
        SetColor(WHITE);
//        OutTextXY(0,0,"EXTERNAL MEMORY");
        DelayMs(DELAYW_MS);
        MMBFadeOut( DELAY_MS);

        JPEGPutImage(0,0,&Scene02);
        MMBFadeIn( DELAY2_MS);
        SetColor(WHITE);
//        OutTextXY(0,0,"EXTERNAL MEMORY");
        DelayMs(DELAYW_MS);
        MMBFadeOut( DELAY_MS);

        JPEGPutImage(0,0,&Scene03);
        MMBFadeIn( DELAY2_MS);
        SetColor(WHITE);
//        OutTextXY(0,0,"EXTERNAL MEMORY");
        DelayMs(DELAYW_MS);
        MMBFadeOut( DELAY_MS);

        JPEGPutImage(0,0,&Scene04);
        MMBFadeIn( DELAY2_MS);
        SetColor(WHITE);
//        OutTextXY(0,0,"EXTERNAL MEMORY");
        DelayMs(DELAYW_MS);
        MMBFadeOut( DELAY_MS);
        
        JPEGPutImage(0,0,&Scene05);
        MMBFadeIn( DELAY2_MS);
        SetColor(WHITE);
//        OutTextXY(0,0,"EXTERNAL MEMORY");
        DelayMs(DELAYW_MS);
        MMBFadeOut( DELAY_MS);
    }

    return 0;
}
