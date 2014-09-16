/******************************************************************************
* This module allows displaying JPEG files from internal and external memories
*******************************************************************************
* FileName:        JPEGImage.h
* Dependencies:    Image decoding library, Graphics library
* Processor:       PIC24F, PIC24H, dsPIC, PIC32
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology, Inc.

 * Software License Agreement
 *
 * Copyright © 2009 Microchip Technology Inc.  All rights reserved.
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

Author                 Date           Comments
--------------------------------------------------------------------------------
Pradeep Budagutta    25-Jun-2008    First release
Anton Alkhimenok     05-May-2009    Modified to support JPEG files in external memory
Lucio Di Jasio       08-Dec-2009    Adapted to MikroE-MMB
*******************************************************************************/
#ifndef _JPEGIMAGE_H
#define _JPEGIMAGE_H
 
#include "Graphics/Graphics.h"
#include "Image Decoders/ImageDecoder.h"


// The module uses an external memory driver to get an JPEG image from the external memory.
// The header file for the driver must be provided HERE:
#include "SerialFLASH.h"  // contains driver for Serial Flash with JPEG files

// One function must be implemented in the driver:
/************************************************************************
* Function: void FunctionToReadArrayFromMemory(DWORD address, BYTE* pData, nCount)                                                                     
*
* Overview: this function reads data into buffer specified
*                                                                       
* Input: memory address, pointer to the data buffer, data count
*                                                                       
************************************************************************/
// Definition for the function implemented in the external memory driver must be HERE:
#define ReadArray(address,pData,nCount) SFLASHReadArray(address,pData,nCount)

#define USE_JPEG_FLASH     // uncomment this line if JPEG images are in internal flash
#define USE_JPEG_EXTERNAL  // uncomment this line if JPEG images are in external memory

// JPEG file image structure for external and interanal memories
// See Primitive.h for BITMAP_FLASH and BITMAP_EXTERNAL info
#define JPEG_FLASH      BITMAP_FLASH
#define JPEG_EXTERNAL   BITMAP_EXTERNAL

// Valid values of the first field for JPEG_FLASH and JPEG_EXTERNAL structures
#define FILE_JPEG_FLASH      2   // the JPEG file is located in internal flash
#define FILE_JPEG_EXTERNAL   3   // the JPEG file is located in external memory

// Structure with data access functions pointers 
extern  IMG_FILE_SYSTEM_API _jpegFileApi;

/*******************************************************************************
Function:       void* JPEGfopen(void* jpegImage)

Precondition:   None

Overview:       This function sets up the variables to emulate a file
                using constant data in internal flash or external memory

Input:          pointer to JPEG file image

Output:         pointer to JPEG file image
*******************************************************************************/
void* JPEGfopen(void* jpegImage);

/*******************************************************************************
Function:       void JPEGInit()

Precondition:   None

Overview:       This function initializes JPEG decoder

Input:          None

Output:         None
*******************************************************************************/
void JPEGInit();

/*******************************************************************************
Macro:          JPEGPutImage(left,top,pImage)

Precondition:   None

Overview:       This macro puts the picture from the JPEG file image on the screen

Input:          left,top corner of the picture on the display, pImage - pointer to JPEG file image

Output:         None
*******************************************************************************/
#define JPEGPutImage(left,top,pImage) ImageDecode(JPEGfopen((void*)pImage), IMG_JPEG, left, top, (GetMaxX()+1), (GetMaxY()+1), 0, &_jpegFileApi, NULL);

#endif // _JPEGIMAGE
