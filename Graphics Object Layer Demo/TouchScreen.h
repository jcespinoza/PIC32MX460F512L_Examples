/*****************************************************************************
 * Simple 4 wire touch screen driver
 *****************************************************************************
 * FileName:        TouchScreen.h
 * Dependencies:    Graphics.h
 * Processor:       PIC24F, PIC24H, dsPIC, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok		01/08/07	...
 * Anton Alkhimenok		06/06/07	Basic calibration and GOL messaging are added
 * Anton Alkhimenok     01/14/08    Graphics PICtail Version 2 support is added
 * Anton Alkhimenok     02/05/08    Portrait orientation is added
 * Sean Justice         02/07/08    PIC32 support is added
 * Anton Alkhimenok     05/27/08    More robust algorithm
 * Anton Alkhimenok     01/07/09    Graphics PICtail Version 3 support is added
 * Lucio Di Jasio       12/01/09    MikroE-MMB support Added
 *****************************************************************************/
#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#include "Graphics/Graphics.h"
#include "MainDemo.h"

#if (GRAPHICS_PICTAIL_VERSION == 1)

//#define SWAP_X_AND_Y
#define FLIP_X
//#define FLIP_Y

#elif (GRAPHICS_PICTAIL_VERSION == 2)

//#define SWAP_X_AND_Y
//#define FLIP_X
#define FLIP_Y

#elif (GRAPHICS_PICTAIL_VERSION == 3)
//#define SWAP_X_AND_Y
//#define FLIP_X
//#define FLIP_Y

#elif (GRAPHICS_PICTAIL_VERSION == 4)   // MIKROE_MMB
//#define SWAP_X_AND_Y
#define FLIP_X
//#define FLIP_Y

#endif

// Default calibration values
#define YMINCAL         0x74
#define YMAXCAL         0x39f//0x284
#define XMINCAL         0x2f
#define XMAXCAL         0x39f

// Max/Min ADC values for each direction
extern volatile WORD _calXMin;
extern volatile WORD _calXMax;
extern volatile WORD _calYMin;
extern volatile WORD _calYMax;

#if (GRAPHICS_PICTAIL_VERSION < 3)
// Addresses for calibration and version values in EEPROM on Explorer 16
#define ADDRESS_VERSION  (unsigned)0x7FFE
#define ADDRESS_XMIN     (unsigned)0x7FFC
#define ADDRESS_XMAX     (unsigned)0x7FFA
#define ADDRESS_YMIN     (unsigned)0x7FF8
#define ADDRESS_YMAX     (unsigned)0x7FF6
#else
// Addresses for calibration and version values in SPI Flash on Graphics PICtail 3
#define ADDRESS_VERSION  (unsigned long)0xFFFFFFFE
#define ADDRESS_XMIN     (unsigned long)0xFFFFFFFC
#define ADDRESS_XMAX     (unsigned long)0xFFFFFFFA
#define ADDRESS_YMIN     (unsigned long)0xFFFFFFF8
#define ADDRESS_YMAX     (unsigned long)0xFFFFFFF6
#endif

// Current ADC values for X and Y channels and potentiometer R6
extern volatile SHORT adcX;
extern volatile SHORT adcY;
extern volatile SHORT adcPot;

/*********************************************************************
* Function: void TouchInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: sets ADC
*
* Note: none
*
********************************************************************/
void TouchInit(void);

/*********************************************************************
* Function: SHORT TouchGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: x coordinate
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetX();

/*********************************************************************
* Function: SHORT TouchGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: y coordinate
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
SHORT TouchGetY();

/*********************************************************************
* Function: void TouchGetMsg(GOL_MSG* pMsg)
*
* PreCondition: none
*
* Input: pointer to the message structure to be populated
*
* Output: none
*
* Side Effects: none
*
* Overview: populates GOL message structure
*
* Note: none
*
********************************************************************/
void TouchGetMsg(GOL_MSG* pMsg);

/*********************************************************************
* Function: void TouchCalibration()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Runs the calibration routine. 
*
* Note: none
*
********************************************************************/
void TouchCalibration();

/*********************************************************************
* Function: void TouchStoreCalibration(void)
*
* PreCondition: EEPROMInit() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: stores calibration parameters into EEPROM
*
* Note: none
*
********************************************************************/
void TouchStoreCalibration(void);

/*********************************************************************
* Function: void TouchLoadCalibration(void)
*
* PreCondition: EEPROMInit() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: loads calibration parameters from EEPROM
*
* Note: none
*
********************************************************************/
void TouchLoadCalibration(void);

/*********************************************************************
* Macros: ADCGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for x direction if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
#define ADCGetX() adcX

/*********************************************************************
* Macros: ADCGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for y direction if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
#define ADCGetY() adcY

/*********************************************************************
* Macros: ADCGetPot()
*
* PreCondition: none
*
* Input: none
*
* Output: ADC result
*
* Side Effects: none
*
* Overview: returns ADC value for potentiometer
*
* Note: none
*
********************************************************************/
#define ADCGetPot() adcPot


#endif
