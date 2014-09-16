/******************************************************************************

    USB Keyboard Host Application Demo

Description:
    This file contains the basic USB keyboard application. Purpose of the demo
    is to demonstrate the capability of HID host . Any Low speed/Full Speed
    USB keyboard can be connected to the PICtail USB adapter along with 
    Explorer 16 demo board. This file schedules the HID ransfers, and interprets
    the report received from the keyboard. Key strokes are decoded to ascii 
    values and the same can be displayed either on hyperterminal or on the LCD
    display mounted on the Explorer 16 board. Since the purpose is to 
    demonstrate HID host all the keys have not been decoded. However demo gives
    a fair idea and user should be able to incorporate necessary changes for
    the required application. All the alphabets, numeric characters, special
    characters, ESC , Shift, CapsLK and space bar keys have been implemented.

Summary:
 This file contains the basic USB keyboard application.

Remarks:
    This demo requires Explorer 16 board and the USB PICtail plus connector.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       keyboard_demo.c
* Dependencies:    None
* Processor:       PIC32MX460
* Compiler:        C32 v1.05 or later
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
ADG             14-Apr-2008 First release
Lucio Di Jasio  08-Dec-2009 Adapted to MikroE-MMB
*******************************************************************************/
//DOM-IGNORE-END
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "usb_config.h"
#include "LCDTerminal.h"
#include "USB\usb.h"
#include "USB\usb_host_hid_parser.h"
#include "USB\usb_host_hid.h"
#include "graphics\graphics.h"
#include "MMB.h"

//#define DEBUG_MODE

// redirect error messages to screen 
#ifndef DEBUG_MODE
    #define UART2PrintString    LCDPutString
#endif
// *****************************************************************************
// *****************************************************************************
// Constants
// *****************************************************************************
// *****************************************************************************

// We are taking Timer 3  to schedule input report transfers

// NOTE - The datasheet doesn't state this, but the timer does get reset to 0
// after a period register match.  So we don't have to worry about resetting
// the timer manually.

#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000

#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
#define TIMER_INTERRUPT_PRIORITY    0x0001


// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

// Configuration Bit settings
//      Primary Oscillator:             HS
//      Internal USB 3.3v Regulator:    Disabled
//      IOLOCK:                         Set Once
//      Primary Oscillator Output:      Digital I/O
//      Clock Switching and Monitor:    Both disabled
//      Oscillator:                     Primary with PLL
//      USB 96MHz PLL Prescale:         Divide by 2
//      Internal/External Switch Over:  Enabled
//      WDT Postscaler:                 1:32768
//      WDT Prescaler:                  1:128
//      WDT Window:                     Non-window Mode
//      Comm Channel:                   EMUC2/EMUD2
//      Clip on Emulation Mode:         Reset into Operation Mode
//      Write Protect:                  Disabled
//      Code Protect:                   Disabled
//      JTAG Port Enable:               Disabled

    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    #pragma config DEBUG    = ON            // Background Debugger Enable


// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

typedef enum _APP_STATE
{
    DEVICE_NOT_CONNECTED,
    DEVICE_CONNECTED, /* Device Enumerated  - Report Descriptor Parsed */
    READY_TO_TX_RX_REPORT,
    GET_INPUT_REPORT, /* perform operation on received report */
    INPUT_REPORT_PENDING,
    SEND_OUTPUT_REPORT, /* Not needed in case of mouse */
    OUTPUT_REPORT_PENDING,
    ERROR_REPORTED 
} APP_STATE;

typedef struct _HID_REPORT_BUFFER
{
    WORD  Report_ID;
    WORD  ReportSize;
    BYTE* ReportData;
    WORD  ReportPollRate;
}   HID_REPORT_BUFFER;

typedef struct _HID_LED_REPORT_BUFFER
{
    BYTE  NUM_LOCK      : 1;
    BYTE  CAPS_LOCK     : 1;
    BYTE  SCROLL_LOCK   : 1;
    BYTE  UNUSED        : 5;
}   HID_LED_REPORT_BUFFER;



// *****************************************************************************
// *****************************************************************************
// Internal Function Prototypes
// *****************************************************************************
// *****************************************************************************
BYTE App_HID2ASCII(BYTE a); //convert USB HID code (buffer[2 to 7]) to ASCII code
void AppInitialize(void);
BOOL AppGetParsedReportDetails(void);
void App_Detect_Device(void);
void App_ProcessInputReport(void);
void App_PrepareOutputReport(void);
void InitializeTimer(void);
void App_Clear_Data_Buffer(void);
BOOL App_CompareKeyPressedPrevBuf(BYTE data);
void App_CopyToShadowBuffer(void);
BOOL USB_HID_DataCollectionHandler(void);

void LCDDisplayString(BYTE* data, BYTE lineNum);
void LCD_Display_Routine(BYTE data, BYTE HIDData);

// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************
#define MAX_ALLOWED_CURRENT             (500)         // Maximum power we can supply in mA
#define MINIMUM_POLL_INTERVAL           (0x0A)        // Minimum Polling rate for HID reports is 10ms

#define USAGE_PAGE_LEDS                 (0x08)

#define USAGE_PAGE_KEY_CODES            (0x07)
#define USAGE_MIN_MODIFIER_KEY          (0xE0)
#define USAGE_MAX_MODIFIER_KEY          (0xE7)

#define USAGE_MIN_NORMAL_KEY            (0x00)
#define USAGE_MAX_NORMAL_KEY            (0xFF)

/* Array index for modifier keys */
#define MODIFIER_LEFT_CONTROL           (0)
#define MODIFIER_LEFT_SHIFT             (1)
#define MODIFIER_LEFT_ALT               (2)
#define MODIFIER_LEFT_GUI               (3)
#define MODIFIER_RIGHT_CONTROL          (4)
#define MODIFIER_RIGHT_SHIFT            (5)
#define MODIFIER_RIGHT_ALT              (6)
#define MODIFIER_RIGHT_GUI              (7)

#define HID_CAPS_LOCK_VAL               (0x39)
#define HID_NUM_LOCK_VAL                (0x53)

#define MAX_ERROR_COUNTER               (10)


#define LCD_LINE_ONE                    (1)
#define LCD_LINE_TWO                    (2)

//******************************************************************************
//  macros to identify special charaters(other than Digits and Alphabets)
//******************************************************************************
#define Symbol_Exclamation              (0x1E)
#define Symbol_AT                       (0x1F)
#define Symbol_Pound                    (0x20)
#define Symbol_Dollar                   (0x21)
#define Symbol_Percentage               (0x22)
#define Symbol_Cap                      (0x23)
#define Symbol_AND                      (0x24)
#define Symbol_Star                     (0x25)
#define Symbol_NormalBracketOpen        (0x26)
#define Symbol_NormalBracketClose       (0x27)

#define Symbol_Return                   (0x28)
#define Symbol_Escape                   (0x29)
#define Symbol_Backspace                (0x2A)
#define Symbol_Tab                      (0x2B)
#define Symbol_Space                    (0x2C)
#define Symbol_HyphenUnderscore         (0x2D)
#define Symbol_EqualAdd                 (0x2E)
#define Symbol_BracketOpen              (0x2F)
#define Symbol_BracketClose             (0x30)
#define Symbol_BackslashOR              (0x31)
#define Symbol_SemiColon                (0x33)
#define Symbol_InvertedComma            (0x34)
#define Symbol_Tilde                    (0x35)
#define Symbol_CommaLessThan            (0x36)
#define Symbol_PeriodGreaterThan        (0x37)
#define Symbol_FrontSlashQuestion       (0x38)

// *****************************************************************************
// *****************************************************************************
// Global Variables
// *****************************************************************************
// *****************************************************************************

APP_STATE App_State_Keyboard = DEVICE_NOT_CONNECTED;

HID_DATA_DETAILS Appl_LED_Indicator;


HID_DATA_DETAILS Appl_ModifierKeysDetails;
HID_DATA_DETAILS Appl_NormalKeysDetails;

HID_USER_DATA_SIZE Appl_BufferModifierKeys[8];
HID_USER_DATA_SIZE Appl_BufferNormalKeys[6];
HID_USER_DATA_SIZE Appl_ShadowBuffer1[6];

HID_REPORT_BUFFER     Appl_raw_report_buffer;
HID_LED_REPORT_BUFFER Appl_led_report_buffer;

BYTE ErrorDriver;
BYTE ErrorCounter;
BYTE NumOfBytesRcvd;

BOOL ReportBufferUpdated;
BOOL LED_Key_Pressed = FALSE;
BOOL DisplayConnectOnce = FALSE;
BOOL DisplayDeatachOnce = FALSE;
BYTE CAPS_Lock_Pressed = 0;
BYTE NUM_Lock_Pressed = 0;
BYTE HeldKeyCount = 0;
BYTE HeldKey;

BYTE currCharPos;
BYTE FirstKeyPressed ;
//******************************************************************************
//******************************************************************************
// USB Support Functions
//******************************************************************************
//******************************************************************************

BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    switch( event )
    {
        case EVENT_VBUS_REQUEST_POWER:
            // The data pointer points to a byte that represents the amount of power
            // requested in mA, divided by two.  If the device wants too much power,
            // we reject it.
            if (((USB_VBUS_POWER_EVENT_DATA*)data)->current <= (MAX_ALLOWED_CURRENT / 2))
            {
                return TRUE;
            }
            else
            {
                UART2PrintString( "\r\n***** USB Error - device requires too much current *****\r\n" );
            }
            break;

        case EVENT_VBUS_RELEASE_POWER:
            // Turn off Vbus power.
            // The PIC24F with the Explorer 16 cannot turn off Vbus through software.
            return TRUE;
            break;

        case EVENT_HUB_ATTACH:
            UART2PrintString( "\r\n***** USB Error - hubs are not supported *****\r\n" );
            return TRUE;
            break;

        case EVENT_UNSUPPORTED_DEVICE:
            UART2PrintString( "\r\n***** USB Error - device is not supported *****\r\n" );
            return TRUE;
            break;

        case EVENT_CANNOT_ENUMERATE:
            UART2PrintString( "\r\n***** USB Error - cannot enumerate device *****\r\n" );
            return TRUE;
            break;

        case EVENT_CLIENT_INIT_ERROR:
            UART2PrintString( "\r\n***** USB Error - client driver initialization error *****\r\n" );
            return TRUE;
            break;

        case EVENT_OUT_OF_MEMORY:
            UART2PrintString( "\r\n***** USB Error - out of heap memory *****\r\n" );
            return TRUE;
            break;

        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
            UART2PrintString( "\r\n***** USB Error - unspecified *****\r\n" );
            return TRUE;
            break;

		case EVENT_HID_RPT_DESC_PARSED:
			 #ifdef APPL_COLLECT_PARSED_DATA
			     return(APPL_COLLECT_PARSED_DATA());
		     #else
				 return TRUE;
			 #endif
			break;

        default:
            break;
    }
    return FALSE;
}

//******************************************************************************
//******************************************************************************
// Main
//******************************************************************************
//******************************************************************************

int main (void)
{
    BYTE i;
 
    MMBInit();      // init MIkroE-MMB peripherals
    LCDInit();      // init LCD Terminal emulation

    #ifdef DEBUG_MODE
        UARTInit(115200ul);    // init serial port 
        UART2PrintString( "\r\n\r\n***** MikroElektronika MMB demo " );
        UART2PrintString( "USB Keyboard Host Demo " );
        UART2PrintString( " *****\r\n\r\n" );
    #endif

// Show Splash Screen
    {     
        LCDCenterString( -1, "Keyboard USB Host Demo");
        LCDCenterString( 0, "MikroE MMB");
        LCDCenterString( 2, "move joystick to start");
        MMBFadeIn( 500);
        
        // wait for a key press
        MMBGetKey();
        LCDClear();
    } // splash screen
        
        // Initialize USB layers
        USBInitialize(0);

        while(1)
        {
            USBTasks();
            App_Detect_Device();
            switch(App_State_Keyboard)
            {
                case DEVICE_NOT_CONNECTED:
                             USBTasks();
                             if(DisplayDeatachOnce == FALSE)
                             {
                                LCDClear();
                                LCDL1Home();
                                LCDDisplayString((BYTE*)"Device Detached ", LCD_LINE_ONE);
                                DisplayDeatachOnce = TRUE;
                             }
                             if(USBHostHID_ApiDeviceDetect()) /* True if report descriptor is parsed with no error */
                             {
                                App_State_Keyboard = DEVICE_CONNECTED;
                                DisplayConnectOnce = FALSE;
                             }
                    break;
                case DEVICE_CONNECTED:
                           App_State_Keyboard = READY_TO_TX_RX_REPORT;
                             if(DisplayConnectOnce == FALSE)
                             {
                                LCDClear();
                                LCDL1Home();
                                LCDDisplayString((BYTE*)"Keyboard Attached!", LCD_LINE_ONE);
                                DisplayConnectOnce = TRUE;
                                DisplayDeatachOnce = FALSE;
                             }
                            InitializeTimer(); // start 10ms timer to schedule input reports

                    break;
                case READY_TO_TX_RX_REPORT:
                             if(!USBHostHID_ApiDeviceDetect())
                             {
                                App_State_Keyboard = DEVICE_NOT_CONNECTED;
//                                DisplayOnce = FALSE;
                             }
                    break;
                case GET_INPUT_REPORT:
                            if(USBHostHID_ApiGetReport(Appl_raw_report_buffer.Report_ID,Appl_ModifierKeysDetails.interfaceNum,
                                                        Appl_raw_report_buffer.ReportSize, Appl_raw_report_buffer.ReportData))
                            {
                               /* Host may be busy/error -- keep trying */
                            }
                            else
                            {
                                App_State_Keyboard = INPUT_REPORT_PENDING;
                            }
                            USBTasks();
                    break;
                case INPUT_REPORT_PENDING:
                           if(USBHostHID_ApiTransferIsComplete(&ErrorDriver,&NumOfBytesRcvd))
                            {
                                if(ErrorDriver ||(NumOfBytesRcvd !=     Appl_raw_report_buffer.ReportSize ))
                                {
                                  ErrorCounter++ ; 
                                  if(MAX_ERROR_COUNTER <= ErrorDriver)
                                     App_State_Keyboard = ERROR_REPORTED;
                                  else
                                     App_State_Keyboard = READY_TO_TX_RX_REPORT;
                                }
                                else
                                {
                                  ErrorCounter = 0; 
                                  ReportBufferUpdated = TRUE;
                                  App_State_Keyboard = READY_TO_TX_RX_REPORT;

                                  if(DisplayConnectOnce == TRUE)
                                    {
                                        for(i=0;i<Appl_raw_report_buffer.ReportSize;i++)
                                        {
                                            if(Appl_raw_report_buffer.ReportData[i] != 0)
                                            {
                                                LCDClear();
                                                LCDL1Home();
                                                DisplayConnectOnce = FALSE;
                                            }
                                        }
                                    }

                                  App_ProcessInputReport();
                                  App_PrepareOutputReport();
                                }
                            }
                    break;

                case SEND_OUTPUT_REPORT: /* Will be done while implementing Keyboard */
                            if(USBHostHID_ApiSendReport(Appl_LED_Indicator.reportID,Appl_LED_Indicator.interfaceNum, Appl_LED_Indicator.reportLength,
                               (BYTE*)&Appl_led_report_buffer))
                            {
                               /* Host may be busy/error -- keep trying */
                            }
                            else
                            {
                                App_State_Keyboard = OUTPUT_REPORT_PENDING;
                            }
                            USBTasks();
                             
                    break;
                 case OUTPUT_REPORT_PENDING:
                            if(USBHostHID_ApiTransferIsComplete(&ErrorDriver,&NumOfBytesRcvd))
                            {
                                if(ErrorDriver)
                                {
                                  ErrorCounter++ ; 
                                  if(MAX_ERROR_COUNTER <= ErrorDriver)
                                     App_State_Keyboard = ERROR_REPORTED;

//                                App_State_Keyboard = READY_TO_TX_RX_REPORT;
                                }
                                else
                                {
                                  ErrorCounter = 0; 
                                  App_State_Keyboard = READY_TO_TX_RX_REPORT;
                                }
                            }
                    break;

               case ERROR_REPORTED:
                    break;
                default:
                    break;

            }
        }
}

/****************************************************************************
  Function:
    void App_PrepareOutputReport(void)

  Description:
    This function schedules output report if any LED indicator key is pressed.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_PrepareOutputReport(void)
{
//    if((READY_TO_TX_RX_REPORT == App_State_Keyboard) && (ReportBufferUpdated == TRUE))
    if(ReportBufferUpdated == TRUE)
    {
        ReportBufferUpdated = FALSE;
        if(LED_Key_Pressed)
        {
            App_State_Keyboard = SEND_OUTPUT_REPORT;
            LED_Key_Pressed = FALSE;
        }
     }
}

/****************************************************************************
  Function:
    void App_ProcessInputReport(void)

  Description:
    This function processes input report received from HID device.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_ProcessInputReport(void)
{
    BYTE  i;
    BYTE  data;
   /* process input report received from device */
    USBHostHID_ApiImportData(Appl_raw_report_buffer.ReportData, Appl_raw_report_buffer.ReportSize
                          ,Appl_BufferModifierKeys, &Appl_ModifierKeysDetails);
    USBHostHID_ApiImportData(Appl_raw_report_buffer.ReportData, Appl_raw_report_buffer.ReportSize
                          ,Appl_BufferNormalKeys, &Appl_NormalKeysDetails);

    #ifdef DEBUG_MODE
    UART2PrintString( "\n\rHID: Raw Report  " );
    for(i=0;i<(Appl_raw_report_buffer.ReportSize);i++)
    {
    UART2PutHex( Appl_raw_report_buffer.ReportData[i]);
    UART2PrintString( "-" );
    }
    #endif

    for(i=0;i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0]));i++)
    {
        if(Appl_BufferNormalKeys[i] != 0)
        {
            if(Appl_BufferNormalKeys[i] == HID_CAPS_LOCK_VAL)
            {
               CAPS_Lock_Pressed = !CAPS_Lock_Pressed;
               LED_Key_Pressed = TRUE;
               Appl_led_report_buffer.CAPS_LOCK = CAPS_Lock_Pressed;
            }else if(Appl_BufferNormalKeys[i] == HID_NUM_LOCK_VAL)
            {
                NUM_Lock_Pressed = !NUM_Lock_Pressed;
                LED_Key_Pressed = TRUE;
                Appl_led_report_buffer.NUM_LOCK = NUM_Lock_Pressed;
            }else
            {
            /* check if key press was present in previous report */
            // if key press was pressed in previous report neglect it ???? 
               if(!App_CompareKeyPressedPrevBuf(Appl_BufferNormalKeys[i]))
                {
                    data = App_HID2ASCII(Appl_BufferNormalKeys[i]);  // convert data to ascii
                    #ifdef DEBUG_MODE
                        UART2PutChar(data);
                    #endif
                    LCD_Display_Routine(data,Appl_BufferNormalKeys[i] );
                }
            }
        }
        else
        {
            if(i==0)
            {
                HeldKeyCount = 0;
            }
            else
            {
                if(Appl_BufferNormalKeys[i-1] == HeldKey)
                {
                    if(HeldKeyCount < 3)
                    {
                        HeldKeyCount++;
                    }
                    else
                    {
                        data = App_HID2ASCII(HeldKey);  // convert data to ascii
                        #ifdef DEBUG_MODE
                            UART2PutChar(data);
                        #endif
                        LCD_Display_Routine(data,HeldKey );
                    }
                }
                else
                {
                    HeldKeyCount = 0;
                    HeldKey = Appl_BufferNormalKeys[i-1];
                }
            }
            break;
        }
      }
      App_CopyToShadowBuffer();
      App_Clear_Data_Buffer();
}

/****************************************************************************
  Function:
    void App_CopyToShadowBuffer(void)

  Description:
    This function updates the shadow buffers with previous reports.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_CopyToShadowBuffer(void)
{
    BYTE i;

    for(i=0;i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0]));i++)
    {
        Appl_ShadowBuffer1[i] = Appl_BufferNormalKeys[i];
    }
}

/****************************************************************************
  Function:
    BOOL App_CompareKeyPressedPrevBuf(BYTE data)

  Description:
    This function compares if the data byte received in report was sent in 
    previous report. This is to avoid duplication incase user key in strokes
    at fast rate.

  Precondition:
    None

  Parameters:
    BYTE data         -   data byte that needs to be compared with previous
                          report

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
BOOL App_CompareKeyPressedPrevBuf(BYTE data)
{
    BYTE i;

    for(i=0;i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0]));i++)
    {
        if(data == Appl_ShadowBuffer1[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}


/****************************************************************************
  Function:
    void App_Detect_Device(void)

  Description:
    This function monitors the status of device connected/disconnected

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_Detect_Device(void)
{
  if(!USBHostHID_ApiDeviceDetect())
  {
     App_State_Keyboard = DEVICE_NOT_CONNECTED;
  }
}

/****************************************************************************
  Function:
    void App_Clear_Data_Buffer(void)

  Description:
    This function clears the content of report buffer after reading

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
***************************************************************************/
void App_Clear_Data_Buffer(void)
{
    BYTE i;

    for(i=0;i<(sizeof(Appl_BufferNormalKeys)/sizeof(Appl_BufferNormalKeys[0]));i++)
    {
        Appl_BufferNormalKeys[i] = 0;
    }

    for(i=0;i<Appl_raw_report_buffer.ReportSize;i++)
    {
        Appl_raw_report_buffer.ReportData[i] = 0;
    }
}

/****************************************************************************
  Function:
    BYTE App_HID2ASCII(BYTE a)
  Description:
    This function converts the HID code of the key pressed to coressponding
    ASCII value. For Key strokes like Esc, Enter, Tab etc it returns 0.

  Precondition:
    None

  Parameters:
    BYTE a          -   HID code for the key pressed

  Return Values:
    BYTE            -   ASCII code for the key pressed

  Remarks:
    None
***************************************************************************/
BYTE App_HID2ASCII(BYTE a) //convert USB HID code (buffer[2 to 7]) to ASCII code
{
   BYTE AsciiVal;
   BYTE ShiftkeyStatus = 0;
   if((Appl_BufferModifierKeys[MODIFIER_LEFT_SHIFT] == 1)||(Appl_BufferModifierKeys[MODIFIER_RIGHT_SHIFT] == 1))
    {
        ShiftkeyStatus = 1;
    }
    
   if(a>=0x1E && a<=0x27) 
    {
       if(ShiftkeyStatus)
        {
            switch(a)
            {
                case Symbol_Exclamation: AsciiVal = 0x21;           
                    break;                                          
                case Symbol_AT: AsciiVal = 0x40;            
                    break;                                          
                case Symbol_Pound: AsciiVal = 0x23;         
                    break;                                  
                case Symbol_Dollar: AsciiVal = 0x24;        
                    break;                                  
                case Symbol_Percentage: AsciiVal = 0x25;    
                    break;                                  
                case Symbol_Cap: AsciiVal = 0x5E;   
                    break;                                  
                case Symbol_AND: AsciiVal = 0x26;           
                    break;                                  
                case Symbol_Star: AsciiVal = 0x2A;          
                    break;                                  
                case Symbol_NormalBracketOpen: AsciiVal = 0x28;     
                    break;                                          
                case Symbol_NormalBracketClose: AsciiVal = 0x29;    
                    break;
                default:
                    break;
            }
       
            return(AsciiVal);
        }
       else
       {
            if(a==0x27)
            {
                return(0x30);
            }
            else
            {
                return(a+0x13);
            }
       } 
    }

   if((a>=0x59 && a<=0x61)&&(NUM_Lock_Pressed == 1))
    {
       return(a-0x28);
    }

   if((a==0x62) &&(NUM_Lock_Pressed == 1)) return(0x30);


   if(a>=0x04 && a<=0x1D)
    {
      if(((CAPS_Lock_Pressed == 1)&&((Appl_BufferModifierKeys[MODIFIER_LEFT_SHIFT] == 0)&&
                                (Appl_BufferModifierKeys[MODIFIER_RIGHT_SHIFT] == 0)))
          ||((CAPS_Lock_Pressed == 0)&&((Appl_BufferModifierKeys[MODIFIER_LEFT_SHIFT] == 1) ||
                                        (Appl_BufferModifierKeys[MODIFIER_RIGHT_SHIFT] == 1))))
           return(a+0x3d); /* return capital */
      else
           return(a+0x5d); /* return small case */
    }


   if(a>=0x2D && a<=0x38)
    {
        switch(a)
        {
            case Symbol_HyphenUnderscore:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x2D;
                else
                    AsciiVal = 0x5F;
                break;                      
            case Symbol_EqualAdd:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x3D;
                else
                    AsciiVal = 0x2B;
                break;                                      
            case Symbol_BracketOpen:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x5B;
                else
                    AsciiVal = 0x7B;        
                break;                                      
            case Symbol_BracketClose:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x5D;
                else
                    AsciiVal = 0x7D;        
                break;                                      
            case Symbol_BackslashOR:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x5C;
                else
                    AsciiVal = 0x7C;        
                break;                                      
            case Symbol_SemiColon:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x3B;
                else
                    AsciiVal = 0x3A;        
                break;                                      
            case Symbol_InvertedComma:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x27;
                else
                    AsciiVal = 0x22;
                break;                                      
            case Symbol_Tilde:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x60;
                else
                    AsciiVal = 0x7E;            
                break;                                      
            case Symbol_CommaLessThan:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x2C;
                else
                    AsciiVal = 0x3C;        
                break;                                      
            case Symbol_PeriodGreaterThan:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x2E;
                else
                    AsciiVal = 0x3E;    
                break;                                      
            case Symbol_FrontSlashQuestion:
                if(!ShiftkeyStatus)
                    AsciiVal = 0x2F;
                else
                    AsciiVal = 0x3F;
                break;                                      
            default:
                break;
        }
        return(AsciiVal);
    }   
   
   return(0);
}


/****************************************************************************
  Function:
    void LCD_Display_Routine(BYTE data , BYTE HIDData)
  Description:
    This function displays the key strokes on the LCD  mounted on MikroE-MMB
    demo board. 

  Precondition:
    None

  Parameters:
    BYTE data       -   ASCII code for the key pressed
    BYTE HIDData    -   HID code for the key pressed, this is needed to take
                        action for keys like Esc, Enter, Tab etc.

  Return Values:
    None

  Remarks:
***************************************************************************/

void LCD_Display_Routine(BYTE data , BYTE HIDData)
{
    BYTE LineNum;
//    BYTE CharPos;
    
    LineNum = ((currCharPos & 0x30) >> 4);
//    CharPos = currCharPos & 0x0F;

   if((HIDData>=0x1E && HIDData<=0x27) || (HIDData>=0x04 && HIDData<=0x1D) ||
            (HIDData>=0x2D && HIDData<=0x38) || ((HIDData>=0x59 && HIDData<=0x62)&&(NUM_Lock_Pressed == 1)))
    {
        LCDPut(data);
//        currCharPos++;
    }
    else if(HIDData == 0x29)    // escape key pressed
    {
        LCDClear();
//        LCDL1Home();
//        currCharPos = 0;
    }
    else if (HIDData == 0x2C) // space pressed
    {
        LCDPut(0x20);    // space ascii value 0x20
   //     currCharPos++;
    }
    else if (HIDData == Symbol_Backspace) // back space pressed
    {
  //      if(currCharPos != 0)
  //      {
  //          LCDShiftCursorLeft();
            LCDPut(0x20);    // space ascii value 0x20
  //          LCDShiftCursorLeft();
  //          currCharPos--;
  //       }
    }
    else if((HIDData>=0x4F && HIDData<=0x52) ||
            (( HIDData==0x5C || HIDData==0x5E || HIDData==0x5A || HIDData==0x60  )
               && (NUM_Lock_Pressed == 0)))
    {
        switch(HIDData)
        {
            case 0x4F :   // Right Arrow
            case 0x5E :
                LCDShiftCursorRight();
//                currCharPos++;
                break;
            case 0x50 :   // Left Arrow
            case 0x5C :
                LCDShiftCursorLeft();
//                currCharPos--;
                break;
            case 0x52 :   // Up Arrow
            case 0x60 :
//                if(LineNum == 1)
                {
                    LCDShiftCursorUp();
//                    currCharPos = currCharPos - 16;
//                    LineNum = 0;
                }
                break;
            case 0x51 :   // Down Arrow
            case 0x5A :
//                if(LineNum == 0)
                {
                    LCDShiftCursorDown();
//                    currCharPos = currCharPos + 16;
//                    LineNum = 1;
                }
                break;
            default :
                break;
        }

    }
    
  //  LineNum = ((currCharPos & 0x30) >> 4);
  //  CharPos = currCharPos & 0x0F;

   // if((LineNum == 1) && (CharPos == 0x0))
  //  {
  //      LCDL2Home();
  //      currCharPos = 0x10;
  //  }else if((LineNum == 2) && (CharPos == 0x0))
  //  {
  //      LCDClear();
  //      LCDL1Home();
  //      currCharPos = 0;
  //  }

  // if(currCharPos > 0x20)
  //  {
  //      LCDClear();
  //      LCDL1Home();
  //      currCharPos = 0;
  //  }
   
}

/****************************************************************************
  Function:
    void LCDDisplayString(BYTE* data, BYTE lineNum)
  Description:
    This function displays the string on the LCD 

  Precondition:
    None

  Parameters:
    BYTE* data      -   Array of characters to be displayed on the LCD
    BYTE lineNum    -   LCD_LINE_ONE : To display on Line one to the LCD
                        LCD_LINE_TWO : To display on Line two to the LCD

  Return Values:
    None

  Remarks:
***************************************************************************/
void LCDDisplayString(BYTE* data, BYTE lineNum)
{
   BYTE index = 0;
   if(lineNum == 1)
    {
//      LCDClear();
      LCDL1Home();
      while((*data != '\0') || (index < 16))
      {
         LCDPut(*data );
         index++;
         data++;
      }
    }
    if(lineNum == 2)
    {
//      LCDClear();
      LCDL2Home();
      while((*data != '\0') || (index < 16))
      {
         LCDPut(*data );
         index++;
         data++;
      }
    }
}


/****************************************************************************
  Function:
    BOOL USB_HID_DataCollectionHandler(void)
  Description:
    This function is invoked by HID client , purpose is to collect the 
    details extracted from the report descriptor. HID client will store
    information extracted from the report descriptor in data structures.
    Application needs to create object for each report type it needs to 
    extract.
    For ex: HID_DATA_DETAILS Appl_ModifierKeysDetails;
    HID_DATA_DETAILS is defined in file usb_host_hid_appl_interface.h
    Each member of the structure must be initialized inside this function.
    Application interface layer provides functions :
    USBHostHID_ApiFindBit()
    USBHostHID_ApiFindValue()
    These functions can be used to fill in the details as shown in the demo
    code.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE    - If the report details are collected successfully.
    FALSE   - If the application does not find the the supported format.

  Remarks:
    This Function name should be entered in the USB configuration tool
    in the field "Parsed Data Collection handler".
    If the application does not define this function , then HID cient 
    assumes that Application is aware of report format of the attached
    device.
***************************************************************************/
BOOL USB_HID_DataCollectionHandler(void)
{
  BYTE NumOfReportItem = 0;
  BYTE i;
  USB_HID_ITEM_LIST* pitemListPtrs;
  USB_HID_DEVICE_RPT_INFO* pDeviceRptinfo;
  HID_REPORTITEM *reportItem;
  HID_USAGEITEM *hidUsageItem;
  BYTE usageIndex;
  BYTE reportIndex;
  BOOL foundLEDIndicator = FALSE;
  BOOL foundModifierKey = FALSE;
  BOOL foundNormalKey = FALSE;



  pDeviceRptinfo = USBHostHID_GetCurrentReportInfo(); // Get current Report Info pointer
  pitemListPtrs = USBHostHID_GetItemListPointers();   // Get pointer to list of item pointers

  BOOL status = FALSE;
   /* Find Report Item Index for Modifier Keys */
   /* Once report Item is located , extract information from data structures provided by the parser */
   NumOfReportItem = pDeviceRptinfo->reportItems;
   for(i=0;i<NumOfReportItem;i++)
    {
       reportItem = &pitemListPtrs->reportItemList[i];
       if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == HIDData_Variable)&&
           (reportItem->globals.usagePage==USAGE_PAGE_KEY_CODES))
        {
           /* We now know report item points to modifier keys */
           /* Now make sure usage Min & Max are as per application */
            usageIndex = reportItem->firstUsageItem;
            hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];
            if((hidUsageItem->usageMinimum == USAGE_MIN_MODIFIER_KEY)
                &&(hidUsageItem->usageMaximum == USAGE_MAX_MODIFIER_KEY)) //else application cannot suuport
            {
               reportIndex = reportItem->globals.reportIndex;
               Appl_ModifierKeysDetails.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
               Appl_ModifierKeysDetails.reportID = (BYTE)reportItem->globals.reportID;
               Appl_ModifierKeysDetails.bitOffset = (BYTE)reportItem->startBit;
               Appl_ModifierKeysDetails.bitLength = (BYTE)reportItem->globals.reportsize;
               Appl_ModifierKeysDetails.count=(BYTE)reportItem->globals.reportCount;
               Appl_ModifierKeysDetails.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
               foundModifierKey = TRUE;
            }

        }
        else if((reportItem->reportType==hidReportInput) && (reportItem->dataModes == HIDData_Array)&&
           (reportItem->globals.usagePage==USAGE_PAGE_KEY_CODES))
        {
           /* We now know report item points to modifier keys */
           /* Now make sure usage Min & Max are as per application */
            usageIndex = reportItem->firstUsageItem;
            hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];
            if((hidUsageItem->usageMinimum == USAGE_MIN_NORMAL_KEY)
                &&(hidUsageItem->usageMaximum <= USAGE_MAX_NORMAL_KEY)) //else application cannot suuport
            {
               reportIndex = reportItem->globals.reportIndex;
               Appl_NormalKeysDetails.reportLength = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
               Appl_NormalKeysDetails.reportID = (BYTE)reportItem->globals.reportID;
               Appl_NormalKeysDetails.bitOffset = (BYTE)reportItem->startBit;
               Appl_NormalKeysDetails.bitLength = (BYTE)reportItem->globals.reportsize;
               Appl_NormalKeysDetails.count=(BYTE)reportItem->globals.reportCount;
               Appl_NormalKeysDetails.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
               foundNormalKey = TRUE;
            }

        }
        else if((reportItem->reportType==hidReportOutput) &&
                (reportItem->globals.usagePage==USAGE_PAGE_LEDS))
        {
            usageIndex = reportItem->firstUsageItem;
            hidUsageItem = &pitemListPtrs->usageItemList[usageIndex];

            reportIndex = reportItem->globals.reportIndex;
            Appl_LED_Indicator.reportLength = (pitemListPtrs->reportList[reportIndex].outputBits + 7)/8;
            Appl_LED_Indicator.reportID = (BYTE)reportItem->globals.reportID;
            Appl_LED_Indicator.bitOffset = (BYTE)reportItem->startBit;
            Appl_LED_Indicator.bitLength = (BYTE)reportItem->globals.reportsize;
            Appl_LED_Indicator.count=(BYTE)reportItem->globals.reportCount;
            Appl_LED_Indicator.interfaceNum= USBHostHID_ApiGetCurrentInterfaceNum();
            foundLEDIndicator = TRUE;
        }

    }


   if(pDeviceRptinfo->reports == 1)
    {
        Appl_raw_report_buffer.Report_ID = 0;
        Appl_raw_report_buffer.ReportSize = (pitemListPtrs->reportList[reportIndex].inputBits + 7)/8;
        Appl_raw_report_buffer.ReportData = (BYTE*)malloc(Appl_raw_report_buffer.ReportSize);
        Appl_raw_report_buffer.ReportPollRate = pDeviceRptinfo->reportPollingRate;
        if((foundNormalKey == TRUE)&&(foundModifierKey == TRUE))
        status = TRUE;
    }

    return(status);
}

/****************************************************************************
  Function:
     void InitializeTimer( void )

  Description:
    This function initializes the tick timer.  It configures and starts the
    timer, and enables the timer interrupt.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void InitializeTimer( void )
{
    WORD timerPeriod;

    #if defined(__C30__)
    IPC2bits.T3IP = TIMER_INTERRUPT_PRIORITY;
    IFS0bits.T3IF = 0;

    TMR3 = 0;
    timerPeriod = TIMER_PERIOD*(Appl_raw_report_buffer.ReportPollRate/MINIMUM_POLL_INTERVAL);
        // adjust the timer presaclar if poll rate is too high
        // 20000 counts correspond to 10ms , so current pre sacaler setting will 
        // allow maximum 30ms poll interval
    PR3 = TIMER_PERIOD;
    T3CON = TIMER_ON | STOP_TIMER_IN_IDLE_MODE | TIMER_SOURCE_INTERNAL |
            GATED_TIME_DISABLED | TIMER_16BIT_MODE | TIMER_PRESCALER;

    IEC0bits.T3IE = 1;

    #elif defined(__PIC32MX__)
    //TODO - PIC32 support

    T4CON = 0x0; //Stop and Init Timer
    T4CON = 0x0060;
    //prescaler=1:64,
    //internal clock
    TMR4 = 0; //Clear timer register
    PR4 = 0x7FFF; //Load period register

    IPC4SET = 0x00000004; // Set priority level=1 and
    IPC4SET = 0x00000001; // Set subpriority level=1
    // Could have also done this in single
    // operation by assigning IPC5SET = 0x00000005

//    IFS0CLR = 0x00010000; // Clear the Timer5 interrupt status flag
//    IEC0SET = 0x00010000; // Enable Timer5 interrupts
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;

    T4CONSET = 0x8000;//Start Timer

    #else
        #error No timer configuration.
    #endif

    return;
}

/****************************************************************************
  Function:
    void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)

  Description:
    Timer ISR, used to update application state. If no transfers are pending
    new input request is scheduled.
  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  ***************************************************************************/

#if defined(__C30__)
void __attribute__((__interrupt__, auto_psv)) _T3Interrupt( void )
{
    if (IFS0bits.T3IF)
    {
        IFS0bits.T3IF   = 0;
        if(READY_TO_TX_RX_REPORT == App_State_Keyboard)
        {
            App_State_Keyboard = GET_INPUT_REPORT; // If no report is pending schedule new request
        }
    }
}
#elif defined(__PIC32MX__)
#pragma interrupt _T4Interrupt ipl4 vector 16
void _T4Interrupt( void )
{
    if (IFS0bits.T4IF)
    {
        IFS0bits.T4IF   = 0;
        if(READY_TO_TX_RX_REPORT == App_State_Keyboard)
        {
            App_State_Keyboard = GET_INPUT_REPORT; // If no report is pending schedule new request
        }
    }
}
#else
    #error No timer interrupt
#endif
