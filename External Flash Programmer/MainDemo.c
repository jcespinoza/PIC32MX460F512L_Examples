 /*****************************************************************************
 * EXTERNAL FLASH MEMORY PROGRAMMER
 * This program receives Intel HEX file from UART and programs
 * flash memory installed on MikroE MMB board.
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok		10/17/07	...
 * Anton Alkhimenok		02/05/08	PIC32 support is added
 * Jayanth Murthy       06/25/09    dsPIC & PIC24H support 
 * Lucio Di Jasio       12/08/09    added MikroE-MMB display support 
 *****************************************************************************/
#include "MMB.h"
#include "SerialFLASH.h"
#include "SimpleUART.h"
#include "LCDTerminal.h"
#include "Graphics/Graphics.h"

// Configuration bits
//#pragma config OSCIOFNC = ON, POSCMOD = XT, FSOSCEN = ON, FNOSC = PRIPLL
//#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_18, FPLLODIV = DIV_1
//#pragma config FPBDIV = DIV_8
//#pragma config FWDTEN = OFF, FCKSM = CSECME
//#pragma config CP = OFF, BWP = OFF, PWP = OFF

#pragma config POSCMOD = XT, FNOSC = PRIPLL, FSOSCEN = ON
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_1, FPBDIV = DIV_1
#pragma config OSCIOFNC = ON, CP = OFF, BWP = OFF, PWP = OFF

// Local functions prototypes
BYTE ConvertRecord2Bin(void);
BYTE ProcessRecord(void);

// Macros to interface with memory

#define ChipErase() SFLASHChipErase();
#define WriteArray(address, pdata, len) SFLASHWriteArray(address, pdata, len)


// Acknowledgement
#define ACK     'Y'
#define NAK     'N'

// Current memory address
DWORD_VAL       address;

// Buffer for incoming data
BYTE            buffer[1024];

// Main state mashine 
#define START   0
#define DATA    1
BYTE    state;

const char motion[] = "\\|/-";

///////////////////////////////////// MAIN ////////////////////////////////////
int main(void){
BYTE* pointer;
BYTE  byte;
char c, i = 0;

    MMBInit();              // initialize PIC32 and MMB peripherals
    MMBUARTInit(115200ul);     // init UART 115200 baud
    SFLASHInit();           // init Serial Flash interface (SPI2)
    LCDInit();              // intialize the terminal emulation
    
    state = START;
    address.Val = 0;
    
    
    // create a splash screen
    BacklightOff();          // turn backlight off
    SetColor( BRIGHTRED);
    LCDCenterString( -2, "External FLASH Programmer");
    LCDCenterString( -1, "for");
    LCDCenterString(  0, "MikroE PIC32MX4 MMB");
    
    SetColor( WHITE);
    LCDCenterString( +2, "Use joystick to select:");
    LCDCenterString( +3, "Down - Erase, Right - Skip");
    MMBFadeIn(500);    // 1 sec fade in
    
    // wait for user input
    c = MMBGetKey();
    MMBFadeOut(1000);
    LCDClear();
    if ( c == JOY_DOWN) 
    { // bulk erase chip
        SetColor( BRIGHTRED);
        LCDCenterString( 0, "Erasing Flash Memory...");
        MMBFadeIn(500);    // 1/2 sec fade in
        SFLASHChipErase();     
        MMBFadeOut(500);    
        LCDClear();
    }    

    LCDPutString( "Ready to receive...");
    BacklightOn();          // turn backlight on
    
    while(1){
        byte = UARTWaitChar();
    
        switch(state){
    
            case START:
                // start of record is detected
                if(byte == ':')
                {
                    pointer = buffer;
                    state = DATA;
    
                }
    /*
                if(byte == '\r')
                {
                    UARTPutChar(NAK);
                }
    */
                break;
    
            case DATA:
                // end of record
                if(byte == '\r')
                {
                    // end of string
                    *pointer++ = 0;
                    *pointer++ = 0;
    
                    state = START;
    
                    if( !ConvertRecord2Bin() )
                    {
                        // checksum is wrong
                        UARTPutChar(NAK);
                        break;
                    }
    
                    if( !ProcessRecord() )
                    {
                        // programming error
                        UARTPutChar(NAK);
                        UARTPutChar('P');
                        break;
                    }
                    // record is processed successfully
                    UARTPutChar(ACK);
                    if (i%4 == 0)
                    {   // every four lines update screen 
                        LCDPutChar( motion[i>>2]);
                        LCDPut('\b');           // step back
                    }    
                    i = (i+1) % 16;  
                    break;
                }
                *pointer++ = byte;
                break;
    
            default:
                break;
    
        }// end of switch

    }// end of while

    return 0;
}

BYTE ConvertRecord2Bin(void)
{
BYTE* pIn;
BYTE* pOut;
WORD  checksum = 0;
BYTE  byte;

    pIn = pOut = buffer;

    while(*pIn){
        // Get first nibble
        byte = Char2Hex(*pIn++);
        byte <<= 4;
        // Get second nibble
        byte |= Char2Hex(*pIn++);
        checksum += byte;
        *pOut++ = byte;
    }

    checksum &= 0x00ff;

    // last byte was a transmitted checksum, compare with calculated
    if(checksum)
        return 0; // error
    
    return 1; // good
}


typedef struct _RECORD_HEADER_{
BYTE reclen;
BYTE msbOffset;
BYTE lsbOffset;
BYTE rectype;
} RECORD_HEADER;

#define REC_DATA        0
#define REC_EOF         1
#define REC_UPPERADDR   4
#define REC_STARTADDR   5

BYTE ProcessRecord(void)
{
BYTE*           pData;
RECORD_HEADER*  pRecord;


    pRecord = (RECORD_HEADER*) buffer;
    pData   = buffer + sizeof(RECORD_HEADER);

    switch(pRecord->rectype){

        // data record
        case REC_DATA:
            // set lower 16 bit of the linear address
            address.v[1] = pRecord->msbOffset;
            address.v[0] = pRecord->lsbOffset;
            return WriteArray(address.Val, pData, pRecord->reclen);
            
        // end of file record
        case REC_EOF:
            return 1;

        // extended linear address record
        case REC_UPPERADDR:
            // set upper 16 bit of the linear address
            address.v[3] = *pData++;
            address.v[2] = *pData;
            return 1;

        // start linear address record
        case REC_STARTADDR:
            // set full address
            address.v[3] = *pData++;
            address.v[2] = *pData++;
            address.v[1] = *pData++;
            address.v[0] = *pData;
            return 1;
        
        default:
            return 1;

    }// end of switch

    return 0;
}
