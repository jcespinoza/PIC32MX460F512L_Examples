/*
**  MicroB.c    
**
**  Author: Lucio Di Jasio <lucio@dijasio.com>
**  HW Req: PIC32MX4 MMB MikroE
**  SW Req: 
**  Ref:    www.exploringpic32.com
** 
**  v 0.1  3/24/10  LDJ
*/


// configuration bit settings
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1
#pragma config FWDTEN=OFF, CP=OFF, BWP=OFF

#include <plib.h>
#include <string.h>
#include "MiniGraphic.h"
#include "HardwareProfile.h"
#include "boot_config.h"
#include "boot_load.h"
#include "SDMMC.h"
#include "fileio.h"


#define BootApplication()       (((int(*)(void))(APPLICATION_ADDRESS))())
//#define BootApplication()       LD0 = 0; while(1);

#define N_FILES     25


// Buffer for reading image file data
BYTE            ReadBuffer[BL_READ_BUFFER_SIZE];

// Stores data and base address of data for use by the programming routine
FLASH_BLOCK     FlashBlock;


char filename[ 16];


void exit( int e)
{ 
    while(1);
}
//    
// redefine the exception handler for debugging purposes
void _general_exception_handler( unsigned c, unsigned s)
{
    while (1);
} // exception handler
//

void MMBInit( void)
{
    // 1. disable the JTAG port  
    mJTAGPortEnable( 0);
    
    // 2. Config system for max performance
    //SYSTEMConfig( GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
   
    // 3. allow vectored interrupts
    //INTEnableSystemMultiVectoredInt();   // Interrupt vectoring
    
    // 4. enable joystick input
	AD1PCFG = 0xFFFF;   // all PORTB inputs digital
    
    // 5. init LEDs 	
	LD0 = LED_OFF;  // turn all LED off
	LD1 = LED_OFF;  
	LD2 = LED_OFF;
	LD3 = LED_OFF; 
	LD0_TRIS = 0;     // make LED outputs
	LD1_TRIS = 0;     // make LED outputs
	LD2_TRIS = 0;     // make LED outputs
	LD3_TRIS = 0;     // make LED outputs
	
    // 6. Display backlight off 
    BL_TRIS_BIT = 0;  // OUTPUT     
    BacklightOn();

} // init MikroEMMB


//---------------------------------------------------------------------------
void Status( char *m)
{
    SetBackColor( BLACK);
    clear();

    SetColor( RED);
    // print centered error message
    AT( (40-strlen( m)-7)/2, 15); putsV("E: "); putsV( m);    
    MMBGetKey();            // wait for acknowledge
    clear();
    
} // Status


/****************************************************************************
  Function:
    BOOL LoadFile (  char *file_name )

  Description:
    Accesses the boot image file, reads it over the transport, and programs
    it to Flash.

  Precondition:
    The boot medium and file system must have been initialized and
    maintained as needed.

  Parameters:
    file_name - Pointer to a null-terminated character string giving the 
                name of the application's image file.

  Returns:
    TRUE      - If a valid application image was read from the thumb drive
                and programmed into Flash at the APPLICATION_ADDRESS.
    FALSE     - If the application image was not valid or if unable to 
                program a valid application to Flash for any reason.

  Remarks:
    This routine calls the loader layer to translate and program the boot
    image file.
    
    This routine can be modified to account for differences in how the medium
    and file format must be processed.
  ***************************************************************************/

BOOL LoadFile (  char *file_name )
{
    MFILE           *fp;             // File pointer
    size_t          nBuffer;        // Number of bytes in the read buffer
    size_t          iBuffer;        // Index into the read buffer
    unsigned int    nRemaining;     // Number of bytes remaining to decode
    unsigned int    Result;         // Result code from "GetFlashBlock" operation

    // Attempt to open the file
    if ( (fp=fopenM( file_name, "r" )) == NULL )
    {
        LD0 = 0;
        return FALSE;
    }
    else
    {
        Loader_Initialize(&FlashBlock);

        // Read the file and program it to Flash
        iBuffer     = 0;
        nRemaining  = 0;
        nBuffer     = BL_READ_BUFFER_SIZE;
        while ( (nBuffer=freadM( &ReadBuffer[iBuffer],  nBuffer, fp )) != 0 )
        {
            // Decode the raw data and program as many Flash Blocks as we can.
            // Note:  Loader_GetFlashBlock updates nRemaining.
            iBuffer		= 0;
            nRemaining += nBuffer;
            nBuffer     = nRemaining;
            while ( (Result=Loader_GetFlashBlock(&FlashBlock, &nRemaining, &ReadBuffer[iBuffer])) == LOADER_BLOCK_READY )
            {
                // The Flash block is ready to program.
                if (!Loader_ProgramFlashBlock(&FlashBlock))
                {
                    // Error reported by the loader layer, close and fail.
                    //FSfclose( fp );
                    LD1 = 0;
                    return FALSE;
                }

                // Update the ReadBuffer index.
                iBuffer = nBuffer - nRemaining;
            }

            // Check "GetFlashBlock" result
            if (Result == LOADER_EOF)
            {
                // EOF record found!  Program the last block and close the file.
                Loader_ProgramFlashBlock(&FlashBlock);
                //fcloseM( fp );
                LD2 = 0;
                return TRUE;
            }
            if (Result > LOADER_NEED_DATA)
            {
                // Error reported by the loader layer, close and fail.
                //FSfclose( fp );
                LD3 = 0;
                return FALSE;
            }
            
            // Calculate the index and amount for the next read
            iBuffer = nBuffer - nRemaining;
            if (nRemaining > 0)
            {
                // Copy the remaining data to the top of the buffer and 
                // adjust the read size and starting position
                memcpy(ReadBuffer, &ReadBuffer[iBuffer], nRemaining);
                nBuffer = BL_READ_BUFFER_SIZE - nRemaining;
                iBuffer = nRemaining;
            }
            else
            {
                // Read a full buffer next time.
                nBuffer = BL_READ_BUFFER_SIZE;
                iBuffer = 0;
            }
        }

        //FSfclose( fp );
    }

    return TRUE;

} // LoadFile



//------------------------------------------------------------------- 
int Menu ( char *list, int items, int width)
{
    int i, j, c, n;
    
    n = 0; 
    while ( 1)
    {
        // paint menu
        for(i=0; i< items; i++)
        {
            AT( 15, ( 30-items)/2 + i);     // center the menu
            if (i != n)        
            {
                SetBackColor( BLUE);
                SetColor( YELLOW);
            }    
            else 
            {
                SetBackColor( YELLOW);
                SetColor( BLUE);
            }    
            putcV(' ');
            for(j=0; j<width; j++)
                putcV( list[i * 8 + j]);
            putcV(' ');
        }
        
        c = MMBGetKey();            
        switch( c)
        {
          case JOY_SELECT:
          case JOY_RIGHT:
            return n+1;
            break;
            
          case JOY_UP:
            if (n > 0)
                n--;
            break;
            
          case JOY_DOWN:
            if ( n< items-1)
                n++;
            break;
            
          case JOY_LEFT:
            return 0;      // none selected
            break;
            
          default:
            break;
        } // switch    
    }     
    
} // Menu
    
   
//-------------------------------------------------------------------
void SelectImage( char *fn)
{
    char list[ N_FILES * 8];
    int n;
    
    while( 1)
    {
        while( !mount())
            DelayMs( 100);

        // search for .hex files
        n = listTYPE( list, N_FILES, "HEX");
        if ( n>0)
        {
            // found at least one file
            n = Menu( list, n, 8); 
        }
        
        if ( n == 0)
        {
            //report error and allow to swap card
            unmount();    
            Status( "INSERT/CHANGE CARD");
            //MMBGetKey();
        }        
        else 
        {
             //form the chosen filename
             memcpy( fn, &list[(n-1)*8],8);
             fn +=7;
             while( *fn == ' ')         // remove trailing spaces
                fn--;
             memcpy( fn+1, ".HEX", 5);
             return;
        }    
    }// while         
    
} // select image    
   
   
//-------------------------------------------------------------------     
int main( void)
{
    // hardware initialization 
    MMBInit();          // init processor and MMB peripherals

    // check for the bootstrap button
    if ( MMBReadKey() != JOY_DOWN)
    {
        BootApplication();      // run existing application
    }       
       
    // splash screen
    initV();            // init the mini-graphics module
    SetColor( BRIGHTRED);  
    AT(15, 2); putsV( "MICROB V0.1");// - Micro Bootloader");
    
    while( MMBReadKey());      // absorb boot button 
    
    // initialize loader
    Loader_Initialize( &FlashBlock);
    
    // select an image
    SelectImage( filename);
    
    // load the application
    if (LoadFile( filename))
    {
        // Launch the application //
        BootApplication();
    }
    
    // Should never get here if a valid application was loaded.
    Status( "FAILED");
    
    // Hang system
    while ( 1);

} // main






