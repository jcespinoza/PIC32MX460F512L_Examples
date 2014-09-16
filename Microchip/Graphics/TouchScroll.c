/*****************************************************************************
 *  Module for Microchip Graphics Library 
 *  GOL Layer 
 *  Picture
 *****************************************************************************
 * FileName:        TouchScroll.c
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
 * Lucio Di Jasio     07/29/09	Version 1.0 
 *****************************************************************************/

#include "Graphics/Graphics.h"

#ifdef USE_TOUCHSCROLL

#define DRAG_MIN    25

/*********************************************************************
* Function: TOUCHSCROLL *TSCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*                              SHORT bottom, WORD state, 
*                              GOL_SCHEME *pScheme)
*
* Overview: creates the touchscroll control
*
********************************************************************/
TOUCHSCROLL *TSCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
			       WORD state, GOL_SCHEME *pScheme)
{
	TOUCHSCROLL *pTS = NULL;
	
	pTS = (TOUCHSCROLL *)malloc(sizeof(TOUCHSCROLL));
	if (pTS == NULL)
		return pTS;

	pTS->hdr.ID      	= ID;
	pTS->hdr.pNxtObj 	= NULL;
	pTS->hdr.type    	= OBJ_TOUCHSCROLL;
	pTS->hdr.left    	= left;
	pTS->hdr.top     	= top;
	pTS->hdr.right   	= right;
	pTS->hdr.bottom  	= bottom;
	pTS->hdr.state   	= state;

	// Set the style scheme to be used
	if (pScheme == NULL)
		pTS->hdr.pGolScheme = _pDefaultGolScheme; 
	else 	
		pTS->hdr.pGolScheme = (GOL_SCHEME *)pScheme; 	

    GOLAddObject((OBJ_HEADER*) pTS);
	
	return pTS;
}

/*********************************************************************
* Function: WORD TSTranslateMsg( TOUCHSCROLL *pTS, GOL_MSG *pMsg)
*
* Overview: translates the GOL message for the TouchScroll control
*
********************************************************************/
WORD TSTranslateMsg(TOUCHSCROLL*pTS, GOL_MSG *pMsg)
{
static int prevX = -1;
static int prevY = -1;
int dx, dy, x, y;

	// Evaluate if the message is for the picture
    // Check if disabled first
	if ( GetState(pTS,TS_DISABLED) )
		return OBJ_MSG_INVALID;

#ifdef USE_K16
    if (pMsg->type == TYPE_K16){
        if (pMsg->uiEvent == EVENT_PRESS)
            return TS_MSG_K16;
    }        
#endif     
   
#ifdef USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN){
    	// Check if it falls in the picture area
		if( (pTS->hdr.left     < pMsg->param1) &&
	  	    (pTS->hdr.right    > pMsg->param1) &&
	   	    (pTS->hdr.top      < pMsg->param2) &&
	   	    (pTS->hdr.bottom   > pMsg->param2) ) 
    	{
        	x = pMsg->param1;
        	y = pMsg->param2;
        	
            switch( pMsg->uiEvent)
            {               
            case EVENT_PRESS:
                // memorize touch down position
                prevX = x;
                prevY = y;    
                return OBJ_MSG_INVALID;
                break;
                
            case EVENT_RELEASE:
                dx = x-prevX;
                dy = y-prevY;
                
                // little movements are returned as SELECT
                if ((abs(dx) < DRAG_MIN) && ( abs(dy) < DRAG_MIN)) 
                {
                    // identify areas of the screen
                    if ( x < (pTS->hdr.left + (pTS->hdr.right-pTS->hdr.left)/3))
                        pMsg->param1 = 1;
                    else if ( x > (pTS->hdr.left + 2 * (pTS->hdr.right-pTS->hdr.left)/3))
                        pMsg->param1 = 3;
                    else if ( y < (pTS->hdr.top + (pTS->hdr.bottom-pTS->hdr.top)/3))
                        pMsg->param1 = 2;
                    else if ( y > (pTS->hdr.top + 2* (pTS->hdr.bottom-pTS->hdr.top)/3))
                        pMsg->param1 = 4;
                    else pMsg->param1 = 0;      // center 1/3
                    return TS_MSG_TOUCHED;
                }
                
                // return the deltas in param1 and param2
                pMsg->param1 = dx;
                pMsg->param2 = dy;
                
                // discriminate the direction of movement
                if (abs(dx) > abs(dy))
                {
                // if moving horizontally
                    return (dx>0)? TS_MSG_MOVE_RIGHT : TS_MSG_MOVE_LEFT;
                }
                else
                {
                // moving vertically
                    return (dy>0)? TS_MSG_MOVE_DOWN : TS_MSG_MOVE_UP;
                }
                break;

            } // switch
        } // if in area
    } // if touchscreen
#endif

    return OBJ_MSG_INVALID;	
}

/*********************************************************************
* Function: WORD TSDraw( TOUCHSCROLL *pTS)
*
* Output: returns the status of the drawing
*		  0 - not completed
*         1 - done
*
* Overview: draws picture
*
********************************************************************/
WORD TSDraw( TOUCHSCROLL *pTS)
{
typedef enum {
	REMOVE,
	DRAW_IMAGE,
} TS_DRAW_STATES;

static TS_DRAW_STATES state = REMOVE;

    if(IsDeviceBusy())
        return 0;

    switch(state){

        case REMOVE:
            if(GetState(pTS,TS_HIDE)){
                if(IsDeviceBusy())
                    return 0;
                SetColor(pTS->hdr.pGolScheme->CommonBkColor);
//                Bar(pTS->hdr.left,pTS->hdr.top,pTS->hdr.right,pTS->hdr.bottom);
                return 1;
            }
            state = DRAW_IMAGE;

        case DRAW_IMAGE:
//            if(pTS->pBitmap != NULL)
            {
                if(IsDeviceBusy())
                    return 0;
                // paint the image background RED
                SetColor( RED);
//                Bar(pTS->hdr.left,pTS->hdr.top,pTS->hdr.right,pTS->hdr.bottom);
            }
            SetColor(pTS->hdr.pGolScheme->CommonBkColor);
            state = REMOVE;
    } // switch
    return 1;
}

#endif // USE_TOUCHSCROLL

