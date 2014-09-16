/*****************************************************************************
 * MikroE MMB testing project
 * 
 *****************************************************************************
 * FileName:        Main.c
 * Dependencies:    
 * Processor:       PIC32
 * Compiler:       	MPLAB C32
 * Linker:          MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 *****************************************************************************
 *
** MikroE MMB - Generic Demo template
**
** 8/17/09 rev 1.0  LDJ Testing on MikroE-MMB
** 11/18/09 rev 2.0 LDJ Testing v5
** 12/08/09 rev 2.2 LDJ Update to GFX lib v.2.0
** HW req.: MikroE MMB board 
** SW req.: Graphics lib 2.00 or later
**
*/

#include <plib.h>
#include "MMB.h"
#include "LCDTerminal.h"
#include "Graphics/Graphics.h"
#include "MDD File System/FSIO.h"
#include "SerialFLASH.h"

// Configuration bits 
#pragma config POSCMOD = XT, FNOSC = PRIPLL, FSOSCEN = ON
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20, FPLLODIV = DIV_1, FPBDIV = DIV_1
#pragma config OSCIOFNC = ON, CP = OFF, BWP = OFF, PWP = OFF

// fonts
extern const FONT_FLASH TerminalFont;

int main( void)
{
    char s[128];
    int w;
    float fTemp;

//    SearchRec rec;

    //-------------------------------------------------------------
    // initializations
	MMBInit();          // initialize the MikroE MMB board
    LCDInit();
    
    //-------------------------------------------------------------
    // Show Splash Screen
    SetColor( BRIGHTRED);        
    LCDCenterString( -1, "MikroE PIC32MX4 MMB Test");
    LCDCenterString(  0, "v 1.0");
    SetColor( WHITE);
    LCDCenterString(  2, "(move joystick to start)");  
    MMBFadeIn( 250);    
    
// main loop
	while( 1)
	{      
        // Wait for a key pressed  (first test)  
        MMBGetKey();
        
        // clear screen and start from the top 
        LCDClear();    
        LCDPutString( "LCD Display:\t");
        LCDPutString( "Pass\n");
        
    //-------------------------------------------------------------
        // Temperature sensor read
        LCDPutString( "Temperature:\t");
        fTemp = MMBGetTdegC();  
        sprintf( s, "%0.1f degC\n", fTemp);
        LCDPutString( s);
    
    
    //-------------------------------------------------------------
        // testing the joystick contacts
        LCDPutString( "Joystick:\t?");
        w = 0;
//        while ( w != 0x10)     // test only center position 
//        while ( w != 0x1f )    // test all five positions
//        {   // repeat until req. positions acquired
//            w = MMBGetKey();
//        }  
        LCDPutString( "\bPass\n");
        
    //-------------------------------------------------------------
        // visual check of all four LEDs
        LCDPutString( "LED ON:\t\t?");
        LD0 = LD1 = LD2 = LD3 = LED_ON;
        if ( MMBGetKey() == JOY_RIGHT)  // OK/SKIP
            {
                LCDPutString( "\bPass\n");
                LD0 = LD1 = LD2 = LD3 = LED_OFF;
            }    
        else
            LCDPutString( "\bFail\n");
            // let LED ON 
    //-------------------------------------------------------------
        // Serial EEPROM test
        LCDPutString( "Serial EEPROM:\t?");
        EEPROMInit();
        EEPROMWriteWord( 0x1234, 10 );
        if ( EEPROMReadWord( 10) == 0x1234)
            LCDPutString( "\bPass\n");
        else
            LCDPutString( "\bFail\n");
            
    //-------------------------------------------------------------
        // Serial FLASH test
        LCDPutString( "Serial FLASH:\t?");
    	SFLASHInit();
        if ( SFLASHReadID()== 0x20)
            LCDPutString( "\bPass\n");
        else
            LCDPutString( "\bFail\n");
        
    //-------------------------------------------------------------
        // SD/MMC card reader test (optional, require SD/MMC card)
        LCDPutString( "SD/MMC:\t\t?");
        // wait for a card to be inserted and initialized
        while( !(w = FSInit()))
        {   // or exit if joystick moved
            if ( MMBReadKey())
            {   MMBGetKey();  
                break;
            }    
            DelayMs( 100);
        }
        if ( w)
            LCDPutString( "\bPass\n");
        else
            LCDPutString( "\bSkip\n");
        
    //    { // Directory listing 
    //        if ( !FindFirst( "*.*", ATTR_DIRECTORY | ATTR_ARCHIVE | ATTR_SYSTEM | ATTR_VOLUME, &rec)) 
    //        {  
    //            do{ // print each element found, file or directory
    //                k++;
    //                printf( "%02d %s\t", k, rec.filename);
    //        		if(rec.attributes & ATTR_DIRECTORY)
    //        			puts("<DIR>");
    //        		else
    //        			puts("");    
    //            } while ( !FindNext( &rec));
    //        }
    //        
    //        SetColor( WHITE); 
    //        sprintf( s, "Found %d files", k); 
    //        OutTextXY( 0, 0, s);
    //        k = MMBgetKEY();
    //    }
    
    //-------------------------------------------------------------
        // Serial Port (UART2) test (optional, require test cable)
        LCDPutString( "UART:\t\t?");
        MMBUARTInit( 9600);
        UARTPutChar( '*');
    //    // Clear error flag
    //    if(U2STAbits.OERR)
    //		U2STAbits.OERR = 
        // Wait for one char to arrive
        while(U2STAbits.URXDA == 0) 
        {   // skip if joystick moved
            if ( MMBReadKey())
            {   MMBGetKey();  
                break;
            }    
            DelayMs( 10);
        }
        if ( UARTGetChar() == '*')
            LCDPutString( "\bPass\n");
        else
            LCDPutString( "\bSkip\n");
        
        
        // Accelerometer test (turn the board upside down)
        LCDPutString( "Accelerometer:\t?");
        if ( MMBAccInit())
            LCDPutString( "\bPass\n");
        else
            LCDPutString( "\bFail\n");        


        LCDPutString( "\nRepeat??");
        
    } // main loop
} // main




