/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  GOL Layer 
 *  Picture control
 *****************************************************************************
 * FileName:        TouchScroll.h
 * Dependencies:    None 
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
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
 * Lucio Di Jasio       07/29/09    
 *****************************************************************************/

#ifndef _TOUCHSCROLL_H
#define _TOUCHSCROLL_H

#include <Graphics/GOL.h>

/*********************************************************************
* Object States Definition: 
*********************************************************************/
#define TS_DISABLED       0x0002  // Bit to indicate Picture is in a disabled state.
#define TS_FRAME  	      0x0004  // Bit to indicate Picture has a frame.
#define TS_HIDE           0x8000  // Bit to indicate Picture must be hidden. 
#define TS_DRAW           0x4000  // Bit to indicate Picture will be redrawn.

/*****************************************************************************
 * Overview: The structure contains data for picture control
 *****************************************************************************/
typedef struct {
	OBJ_HEADER      hdr;			// Generic header for all Objects (see OBJ_HEADER).
} TOUCHSCROLL;


/*********************************************************************
* Function: TOUCHSCROLL  *TSCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*                                SHORT bottom, WORD state, GOL_SCHEME *pScheme)
*
* Overview: This function creates a TOUCHSCROLL object with the parameters given. 
*			It automatically attaches the new object into a global linked list of 
*			objects and returns the address of the object.
*
* PreCondition: none
*
* Input: ID - Unique user defined ID for the object instance.
*        left - Left most position of the Object.
* 		 top - Top most position of the Object. 
*		 right - Right most position of the Object.
*		 bottom - Bottom most position of the object.
*        state - Sets the initial state of the object.
*        pScheme - Pointer to the style scheme
*
* Output: Returns the pointer to the object created
*
* Side Effects: none
*
********************************************************************/
TOUCHSCROLL  *TSCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
			           WORD state, GOL_SCHEME *pScheme);

/*********************************************************************
* Function: WORD TSTranslateMsg(TOUCHSCROLL *pPict, GOL_MSG *pMsg)
*
* Overview: This function evaluates the message from a user if the 
*			message will affect the object or not. The table below 
*			enumerates the translated messages for each event 
*			accepted by the TOUCHSCROLL Object.
*
*	<TABLE>
*    	Translated Message   Input Source  Events         							Description
*     	##################   ############  ######         							###########
*		PICT_MSG_SELECTED    Touch Screen  EVENT_PRESS, EVENT_RELEASE, EVENT_MOVE 	If events occurs and the x,y position falls in the area of the picture.
*		OBJ_MSG_INVALID		 Any		   Any										If the message did not affect the object.
*	</TABLE>
*
* PreCondition: none
*
* Input: pPict - The pointer to the object where the message will be
*				 evaluated to check if the message will affect the object.
*        pMsg  - Pointer to the message struct containing the message from 
*        		 the user interface.
*
* Output: Returns the translated message depending on the received GOL message:
*		  - PICT_MSG_SELECTED – Picture is touched.
*         - OBJ_MSG_INVALID – Picture is not affected
*
* Side Effects: none
*
********************************************************************/
WORD  TSTranslateMsg(TOUCHSCROLL *pTS, GOL_MSG *pMsg);

/*********************************************************************
* Function: WORD TSDraw( TOUCHSCROLL *pTS)
*
* Overview: This function renders the object on the screen using 
* 			the current parameter settings. Location of the object is 
*			determined by the left, top, right and bottom parameters. 
*			The colors used are dependent on the state of the object. 
*			
*			When rendering objects of the same type, each object 
*			must be rendered completely before the rendering of the 
*			next object is started. This is to avoid incomplete 
*			object rendering.
*
* PreCondition: Object must be created before this function is called.
*
* Input: pTS - Pointer to the object to be rendered.
*        
* Output: Returns the status of the drawing 
*		  - 1 - If the rendering was completed and 
*		  - 0 - If the rendering is not yet finished. 
*		  Next call to the function will resume the 
*		  rendering on the pending drawing state.
*
* Side Effects: none
*
********************************************************************/
WORD TSDraw(TOUCHSCROLL *pTS);

#endif // _TOUCHSCROLL_H
