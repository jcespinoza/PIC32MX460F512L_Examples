/******************************************************************************

    USB Host Driver

This file provides the hardware interface for a USB Host application.  To use
this file effectively, one or more class driver files should also be included
in the project.

TODO: Clean up surplus DEBUG_MODE and HOST_TRACE

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* File Name:       USBHost.c
* Dependencies:    None
* Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
* Compiler:        C30 v2.01/C32 v0.00.18
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
KO/BC       15-Oct-2007 First release

*******************************************************************************/
//DOM-IGNORE-END

#include <stdlib.h>
#include <string.h>
#include "GenericTypeDefs.h"
#include "struct_queue.h"
#include "USBConfig.h"
#include "USB\USBHost.h"

//DOM-IGNORE-BEGIN

//#define PULSE_MARKERS
//#define DEBUG_MODE
#ifdef DEBUG_MODE
    #include "uart2.h"
#endif

// *****************************************************************************
// Development test configurations.

// If this is not defined, then we don't get TRNIF on a NAK.  Apparently, we can
// only send one control transaction per frame.  Hope it's one per device...
#define ONE_CONTROL_TRANSACTION_PER_FRAME

//#define __PIC24FJ256GB110_REV_A1__
#define __MIGUEL_PIC24FJ256GB110_REV_A1__

#define ALLOW_MULTIPLE_BULK_TRANSACTIONS_PER_FRAME
//#define ALLOW_MULTIPLE_NAKS_PER_FRAME
//#define USE_MANUAL_DETACH_DETECT

// *****************************************************************************

#if defined __PIC32MX__
    #define USBRST          RESET           // Required for compatibility.
    #define DETACHIE        DATTCHIE        // Required for compatibility.
    #define DETACHIF        DATTCHIF        // Required for compatibility.
#endif


#ifdef PULSE_MARKERS
    #if defined( __PIC32MX__)
        #define PULSE   { LATFSET = 0x04; LATFCLR = 0x04; }
    #else
        #error Define PULSE
    #endif
#else
    #define PULSE
#endif

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants
//
// These constants are internal to the stack.  All constants required by the
// API are in the header file(s).
// *****************************************************************************
// *****************************************************************************
//DOM-IGNORE-BEGIN

// *****************************************************************************
// Section: State Machine Constants
// *****************************************************************************

#define STATE_MASK                                      0x0F00  //
#define SUBSTATE_MASK                                   0x00F0  //
#define SUBSUBSTATE_MASK                                0x000F  //

#define NEXT_STATE                                      0x0100  //
#define NEXT_SUBSTATE                                   0x0010  //
#define NEXT_SUBSUBSTATE                                0x0001  //

#define SUBSUBSTATE_ERROR                               0x000F  //

#define NO_STATE                                        0xFFFF  //

/*
*******************************************************************************
DETACHED state machine values

This state machine handles the condition when no device is attached.
*/

#define STATE_DETACHED                                  0x0000  //
#define SUBSTATE_INITIALIZE                             0x0000  //
#define SUBSTATE_WAIT_FOR_DEVICE                        0x0010  //

/*
*******************************************************************************
ATTACHED state machine values

This state machine gets the device descriptor of the remote device.  We get the
size of the device descriptor, and use that size to get the entire device
descriptor.  Then we check the VID and PID and make sure they appear in the TPL.
*/

#define STATE_ATTACHED                                  0x0100  //

#define SUBSTATE_SETTLE                                 0x0000  //
#define SUBSUBSTATE_START_SETTLING_DELAY                0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SETTLING                   0x0001  //
#define SUBSUBSTATE_SETTLING_DONE                       0x0002  //

#define SUBSTATE_RESET_DEVICE                           0x0010  //
#define SUBSUBSTATE_SET_RESET                           0x0000  //
#define SUBSUBSTATE_RESET_WAIT                          0x0001  //
#define SUBSUBSTATE_RESET_RECOVERY                      0x0002  //
#define SUBSUBSTATE_RECOVERY_WAIT                       0x0003  //
#define SUBSUBSTATE_RESET_COMPLETE                      0x0004  //

#define SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE             0x0020  //
#define SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR_SIZE     0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR_SIZE 0x0001  //
#define SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE_COMPLETE 0x0002  //

#define SUBSTATE_GET_DEVICE_DESCRIPTOR                  0x0030  //
#define SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR          0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR      0x0001  //
#define SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_COMPLETE      0x0002  //

#define SUBSTATE_VALIDATE_VID_PID                       0x0040  //

/*
*******************************************************************************
ADDRESSING state machine values

This state machine sets the address of the remote device.
*/

#define STATE_ADDRESSING                                0x0200  //

#define SUBSTATE_SET_DEVICE_ADDRESS                     0x0000  //
#define SUBSUBSTATE_SEND_SET_DEVICE_ADDRESS             0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SET_DEVICE_ADDRESS         0x0001  //
#define SUBSUBSTATE_SET_DEVICE_ADDRESS_COMPLETE         0x0002  //

/*
*******************************************************************************
CONFIGURING state machine values

This state machine sets the configuration of the remote device, and sets up
internal variables to support the device.
*/
#define STATE_CONFIGURING                               0x0300  //

#define SUBSTATE_INIT_CONFIGURATION                     0x0000  //

#define SUBSTATE_GET_CONFIG_DESCRIPTOR_SIZE             0x0010  //
#define SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR_SIZE     0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR_SIZE 0x0001  //
#define SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_SIZECOMPLETE  0x0002  //

#define SUBSTATE_GET_CONFIG_DESCRIPTOR                  0x0020  //
#define SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR          0x0000  //
#define SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR      0x0001  //
#define SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_COMPLETE      0x0002  //

#define SUBSTATE_SELECT_CONFIGURATION                   0x0030  //
#define SUBSUBSTATE_SELECT_CONFIGURATION                0x0000  //
#define SUBSUBSTATE_SEND_SET_OTG                        0x0001  //
#define SUBSUBSTATE_WAIT_FOR_SET_OTG_DONE               0x0002  //
#define SUBSUBSTATE_SET_OTG_COMPLETE                    0x0003  //

#define SUBSTATE_SET_CONFIGURATION                      0x0040  //
#define SUBSUBSTATE_SEND_SET_CONFIGURATION              0x0000  //
#define SUBSUBSTATE_WAIT_FOR_SET_CONFIGURATION          0x0001  //
#define SUBSUBSTATE_SET_CONFIGURATION_COMPLETE          0x0002  //
#define SUBSUBSTATE_INIT_CLIENT_DRIVERS                 0x0003  //

/*
*******************************************************************************
RUNNING state machine values

*/

#define STATE_RUNNING                                   0x0400  //
#define SUBSTATE_NORMAL_RUN                             0x0000  //
#define SUBSTATE_SUSPEND_AND_RESUME                     0x0010  //
#define SUBSUBSTATE_SUSPEND                             0x0000  //
#define SUBSUBSTATE_RESUME                              0x0001  //
#define SUBSUBSTATE_RESUME_WAIT                         0x0002  //
#define SUBSUBSTATE_RESUME_RECOVERY                     0x0003  //
#define SUBSUBSTATE_RESUME_RECOVERY_WAIT                0x0004  //
#define SUBSUBSTATE_RESUME_COMPLETE                     0x0005  //


/*
*******************************************************************************
HOLDING state machine values

*/

#define STATE_HOLDING                                   0x0500  //
#define SUBSTATE_HOLD_INIT                              0x0000  //
#define SUBSTATE_HOLD                                   0x0001  //


// *****************************************************************************
// Section: Token State Machine Constants
// *****************************************************************************

#define TSTATE_MASK                             0x00F0  //
#define TSUBSTATE_MASK                          0x000F  //

#define TSUBSTATE_ERROR                         0x000F  //

#define TSTATE_IDLE                             0x0000  //

#define TSTATE_CONTROL_NO_DATA                  0x0010  //
#define TSUBSTATE_CONTROL_NO_DATA_SETUP         0x0000  //
#define TSUBSTATE_CONTROL_NO_DATA_ACK           0x0001  //
#define TSUBSTATE_CONTROL_NO_DATA_COMPLETE      0x0002  //

#define TSTATE_CONTROL_READ                     0x0020  //
#define TSUBSTATE_CONTROL_READ_SETUP            0x0000  //
#define TSUBSTATE_CONTROL_READ_DATA             0x0001  //
#define TSUBSTATE_CONTROL_READ_ACK              0x0002  //
#define TSUBSTATE_CONTROL_READ_COMPLETE         0x0003  //

#define TSTATE_CONTROL_WRITE                    0x0030  //
#define TSUBSTATE_CONTROL_WRITE_SETUP           0x0000  //
#define TSUBSTATE_CONTROL_WRITE_DATA            0x0001  //
#define TSUBSTATE_CONTROL_WRITE_ACK             0x0002  //
#define TSUBSTATE_CONTROL_WRITE_COMPLETE        0x0003  //

#define TSTATE_INTERRUPT_READ                   0x0040  //
#define TSUBSTATE_INTERRUPT_READ_DATA           0x0000  //
#define TSUBSTATE_INTERRUPT_READ_COMPLETE       0x0001  //

#define TSTATE_INTERRUPT_WRITE                  0x0050  //
#define TSUBSTATE_INTERRUPT_WRITE_DATA          0x0000  //
#define TSUBSTATE_INTERRUPT_WRITE_COMPLETE      0x0001  //

#define TSTATE_ISOCHRONOUS_READ                 0x0060  //
#define TSUBSTATE_ISOCHRONOUS_READ_DATA         0x0000  //
#define TSUBSTATE_ISOCHRONOUS_READ_COMPLETE     0x0001  //

#define TSTATE_ISOCHRONOUS_WRITE                0x0070  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_DATA        0x0000  //
#define TSUBSTATE_ISOCHRONOUS_WRITE_COMPLETE    0x0001  //

#define TSTATE_BULK_READ                        0x0080  //
#define TSUBSTATE_BULK_READ_DATA                0x0000  //
#define TSUBSTATE_BULK_READ_COMPLETE            0x0001  //

#define TSTATE_BULK_WRITE                       0x0090  //
#define TSUBSTATE_BULK_WRITE_DATA               0x0000  //
#define TSUBSTATE_BULK_WRITE_COMPLETE           0x0001  //

//******************************************************************************
// Section: USB Peripheral Constants
//******************************************************************************

// Section: USB Control Register Constants

// Section: U1PWRC

#define USB_SUSPEND_MODE                    0x02    // U1PWRC - Put the module in suspend mode.
#define USB_NORMAL_OPERATION                0x00    // U1PWRC - Normal USB operation
#define USB_ENABLED                         0x01    // U1PWRC - Enable the USB module.
#define USB_DISABLED                        0x00    // U1PWRC - Disable the USB module.

// Section: U1OTGCON

#define USB_DPLUS_PULLUP_ENABLE             0x80    // U1OTGCON - Enable D+ pull-up
#define USB_DMINUS_PULLUP_ENABLE            0x40    // U1OTGCON - Enable D- pull-up
#define USB_DPLUS_PULLDOWN_ENABLE           0x20    // U1OTGCON - Enable D+ pull-down
#define USB_DMINUS_PULLDOWN_ENABLE          0x10    // U1OTGCON - Enable D- pull-down
#define USB_VBUS_ON                         0x08    // U1OTGCON - Enable Vbus
#define USB_OTG_ENABLE                      0x04    // U1OTGCON - Enable OTG
#define USB_VBUS_CHARGE_ENABLE              0x02    // U1OTGCON - Vbus charge line set to 5V
#define USB_VBUS_DISCHARGE_ENABLE           0x01    // U1OTGCON - Discharge Vbus

// Section: U1OTGIE/U1OTGIR

#define USB_INTERRUPT_IDIF                  0x80    // U1OTGIR - ID state change flag
#define USB_INTERRUPT_T1MSECIF              0x40    // U1OTGIR - 1ms timer interrupt flag
#define USB_INTERRUPT_LSTATEIF              0x20    // U1OTGIR - line state stable flag
#define USB_INTERRUPT_ACTIVIF               0x10    // U1OTGIR - bus activity flag
#define USB_INTERRUPT_SESVDIF               0x08    // U1OTGIR - session valid change flag
#define USB_INTERRUPT_SESENDIF              0x04    // U1OTGIR - B-device Vbus change flag
#define USB_INTERRUPT_VBUSVDIF              0x01    // U1OTGIR - A-device Vbus change flag

// Section: U1CON

#define USB_JSTATE_DETECTED                 0x80    // U1CON - J state !@#$% missing from FRM
#define USB_SE0_DETECTED                    0x40    // U1CON - Single ended 0 detected
#define USB_TOKEN_BUSY                      0x20    // U1CON - Token currently being processed
#define USB_ASSERT_RESET                    0x10    // U1CON - RESET signalling
#define USB_HOST_MODE_ENABLE                0x08    // U1CON - Enable host mode
#define USB_RESUME_ACTIVATED                0x04    // U1CON - RESUME signalling
#define USB_PINGPONG_RESET                  0x02    // U1CON - Reset ping-pong buffer pointer
#define USB_SOF_ENABLE                      0x01    // U1CON - Enable SOF generation
#define USB_SOF_DISABLE                     0x00    // U1CON - Disable SOF generation

// Section: U1CNFG1

#define USB_EYE_PATTERN_TEST                0x80    // U1CFG1 - Enable eye pattern test
#define USB_MONITOR_OE                      0x40    // U1CFG1 - nOE signal active
#define USB_FREEZE_IN_DEBUG_MODE            0x20    // U1CFG1 - Freeze on halt when in debug mode
#define USB_STOP_IN_IDLE_MODE               0x10    // U1CFG1 - Stop module in idle mode
#define USB_SUSPEND                         0x08    // U1CFG1 - !@#$% missing from FRM
#define USB_PING_PONG__ALL_BUT_EP0          0x03    // U1CFG1 - Ping-pong on all endpoints except EP0
#define USB_PING_PONG__FULL_PING_PONG       0x02    // U1CFG1 - Ping-pong on all endpoints
#define USB_PING_PONG__EP0_OUT_ONLY         0x01    // U1CFG1 - Ping-pong on EP 0 out only
#define USB_PING_PONG__NO_PING_PONG         0x00    // U1CFG1 - No ping-pong

// Section: U1CNFG2

#define USB_VBUS_PULLUP_ENABLE              0x01    // U1CNFG2 - Enable Vbus pull-up
#define USB_EXTERNAL_IIC                    0x08    // U1CNFG2 - External module controlled by I2C
#define USB_VBUS_BOOST_DISABLE              0x04    // U1CNFG2 - Disable Vbus boost
#define USB_VBUS_BOOST_ENABLE               0x00    // U1CNFG2 - Enable Vbus boost
#define USB_VBUS_COMPARE_DISABLE            0x02    // U1CNFG2 - Vbus comparator disabled
#define USB_VBUS_COMPARE_ENABLE             0x00    // U1CNFG2 - Vbus comparator enabled
#define USB_ONCHIP_DISABLE                  0x01    // U1CNFG2 - On-chip transceiver disabled
#define USB_ONCHIP_ENABLE                   0x00    // U1CNFG2 - On-chip transceiver enabled

// Section: U1IE/U1IR

#define USB_INTERRUPT_STALL                     0x80    // U1IE - Stall interrupt enable
#define USB_INTERRUPT_ATTACH                    0x40    // U1IE - Attach interrupt enable
#define USB_INTERRUPT_RESUME                    0x20    // U1IE - Resume interrupt enable
#define USB_INTERRUPT_IDLE                      0x10    // U1IE - Idle interrupt enable
#define USB_INTERRUPT_TRANSFER                  0x08    // U1IE - Transfer Done interrupt enable
#define USB_INTERRUPT_SOF                       0x04    // U1IE - Start of Frame Threshold interrupt enable
#define USB_INTERRUPT_ERROR                     0x02    // U1IE - USB Error interrupt enable
#define USB_INTERRUPT_DETACH                    0x01    // U1IE - Detach interrupt enable

//******************************************************************************
// Section: USB Specification Constants
//******************************************************************************

// Section: USB Token Types

#define USB_TOKEN_OUT                           0x01    // U1TOK - OUT token
#define USB_TOKEN_IN                            0x09    // U1TOK - IN token
#define USB_TOKEN_SETUP                         0x0D    // U1TOK - SETUP token


//******************************************************************************
// Section: Other Constants
//******************************************************************************

#define DTS_DATA0                           0       // DTS bit - DATA0 PID
#define DTS_DATA1                           1       // DTS bit - DATA1 PID

#define UEP_DIRECT_LOW_SPEED                0x80    // UEP0 - Direct connect to low speed device enabled
#define UEP_NO_DIRECT_LOW_SPEED             0x00    // UEP0 - Direct connect to low speed device disabled
#define UEP_RETRY_NAKS                      0x40    // UEP0 - No automatic retry of NAK'd transactions
#define UEP_NO_RETRY_NAKS                   0x00    // UEP0 - Automatic retry of NAK'd transactions
#define UEP_NO_SETUP_TRANSFERS              0x10    // UEP0 - SETUP transfers not allowed
#define UEP_ALLOW_SETUP_TRANSFERS           0x00    // UEP0 - SETUP transfers allowed
#define UEP_RX_ENABLE                       0x08    // UEP0 - Endpoint can receive data
#define UEP_RX_DISABLE                      0x00    // UEP0 - Endpoint cannot receive data
#define UEP_TX_ENABLE                       0x04    // UEP0 - Endpoint can transmit data
#define UEP_TX_DISABLE                      0x00    // UEP0 - Endpoint cannot transmit data
#define UEP_HANDSHAKE_ENABLE                0x01    // UEP0 - Endpoint handshake enabled
#define UEP_HANDSHAKE_DISABLE               0x00    // UEP0 - Endpoint handshake disabled (isochronous endpoints)

#define USB_ENDPOINT_CONTROL_BULK           (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE) //
#define USB_ENDPOINT_CONTROL_ISOCHRONOUS    (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE )                       //
#define USB_ENDPOINT_CONTROL_INTERRUPT      (UEP_NO_SETUP_TRANSFERS | UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE) //
#define USB_ENDPOINT_CONTROL_SETUP          (UEP_RX_ENABLE | UEP_TX_ENABLE | UEP_HANDSHAKE_ENABLE)                          //

#define USB_DEVICE_ADDRESS                  0x01    // Default USB device address (single device support)
#define USB_DISABLE_ENDPOINT                0x00    // Value to disable an endpoint.

#define USB_SOF_THRESHOLD_08                0x12    // U1SOF - Threshold for a max packet size of 8
#define USB_SOF_THRESHOLD_16                0x1A    // U1SOF - Threshold for a max packet size of 16
#define USB_SOF_THRESHOLD_32                0x2A    // U1SOF - Threshold for a max packet size of 32
#define USB_SOF_THRESHOLD_64                0x4A    // U1SOF - Threshold for a max packet size of 64

#define USB_1MS_TIMER_FLAG                  0x40
#ifndef USB_INSERT_TIME
    #define USB_INSERT_TIME                 (250+1) // Insertion delay time (spec minimum is 100 ms)
#endif
#define USB_RESET_TIME                      (50+1)  // RESET signaling time - 50ms
#if defined( __C30__ )
    #define USB_RESET_RECOVERY_TIME         (10+1)  // RESET recovery time.
#elif defined( __PIC32MX__ )
    #define USB_RESET_RECOVERY_TIME         (100+1) // RESET recovery time - Changed to 100 ms from 10ms.  Some devices take longer.
#else
    #error Unknown USB_RESET_RECOVERY_TIME
#endif
#define USB_RESUME_TIME                     (20+1)  // RESUME signaling time - 20 ms
#define USB_RESUME_RECOVERY_TIME            (10+1)  // RESUME recovery time - 10 ms

#define USB_HUB_CLASSCODE                   0x09    //  Class code for a hub.

//DOM-IGNORE-END


//******************************************************************************
//******************************************************************************
// Section: Macros
//
// These macros are all internal to the stack.
//******************************************************************************
//******************************************************************************

/*******************************************************************************
Function:       void _USB_InitErrorCounters( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_InitErrorCounters()        { numCommandTries   = USB_NUM_COMMAND_TRIES; }

/*******************************************************************************
Function:       void _USB_SetDATA01( x )

Notes:          The value of x should be DTS_DATA0 or DTS_DATA1.

Tested:

TODO:
*******************************************************************************/
#define _USB_SetDATA01(x)               { pCurrentEndpoint->status.bfNextDATA01 = x; }

/*******************************************************************************
Function:       void _USB_SetErrorCode(x)

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetErrorCode(x)            { usbDeviceInfo.errorCode = x; }

/*******************************************************************************
Function:       void _USB_SetHoldState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetHoldState()             { usbHostState = STATE_HOLDING; }

/*******************************************************************************
Function:       void _USB_SetNextState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetNextState()             { usbHostState = (usbHostState & STATE_MASK) + NEXT_STATE; }

/*******************************************************************************
Function:       void _USB_SetNextSubState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetNextSubState()          { usbHostState = (usbHostState & (STATE_MASK | SUBSTATE_MASK)) + NEXT_SUBSTATE; }

/*******************************************************************************
Function:       void _USB_SetNextSubSubState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetNextSubSubState()       { usbHostState =  usbHostState + NEXT_SUBSUBSTATE; }

/*******************************************************************************
Function:       _USB_SetNextTransferState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetNextTransferState()     { pCurrentEndpoint->transferState ++; }

/*******************************************************************************
Function:       void _USB_SetPreviousSubSubState( void )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetPreviousSubSubState()   { usbHostState =  usbHostState - NEXT_SUBSUBSTATE; }

/*******************************************************************************
Function:       void _USB_SetTransferErrorState( x )

Notes:

Tested:

TODO:
*******************************************************************************/
#define _USB_SetTransferErrorState(x)   { x->transferState = (x->transferState & ~TSTATE_MASK) | TSUBSTATE_ERROR; }

/*******************************************************************************
Function:       void freez(x)

Notes:

Tested:

TODO:
*******************************************************************************/
#define freez(x)                        { free(x); x = NULL; }


//******************************************************************************
//******************************************************************************
// Section: Data Structures
//
// These data structures are all internal to the stack.
//******************************************************************************
//******************************************************************************

// *****************************************************************************
/* USB Bus Information

This structure is used to hold information about the USB bus status.
*/
typedef struct _USB_BUS_INFO
{
    volatile union
    {
        struct
        {
            BYTE        bfControlTransfersDone      : 1;    // All control transfers in the current frame are complete.
            BYTE        bfInterruptTransfersDone    : 1;    // All interrupt transfers in the current frame are complete.
            BYTE        bfIsochronousTransfersDone  : 1;    // All isochronous transfers in the current frame are complete.
            BYTE        bfBulkTransfersDone         : 1;    // All bulk transfers in the current frame are complete.
        };
        WORD            val;                                //
    }                   flags;                              //
    volatile DWORD      dBytesSentInFrame;                  // TODO: The number of bytes sent during the current frame.
    volatile BYTE       lastBulkTransaction;                // The last bulk transaction sent.
} USB_BUS_INFO;


// *****************************************************************************
/* USB Configuration Node

This structure is used to make a linked list of all the configuration
descriptors of an attached device.
*/
typedef struct _USB_CONFIGURATION
{
    BYTE                        *descriptor;    // Complete Configuration Descriptor.
    struct _USB_CONFIGURATION   *next;          // Pointer to next node.
    BYTE                        configNumber;   // Number of this Configuration.
} USB_CONFIGURATION;


// *****************************************************************************
/* USB Device Information

This structure is used to hold all the information about an attached device.
*/
typedef struct _USB_DEVICE_INFO
{
    USB_CONFIGURATION   *currentConfigurationDescriptor;    // Descriptor of the current Configuration.
    BYTE                currentConfiguration;               // Value of current Configuration.
    BYTE                attributesOTG;                      // OTG attributes.
    BYTE                deviceAddressAndSpeed;              // Device address and low/full speed indication.
    BYTE                deviceAddress;                      // Device address.
    BYTE                errorCode;                          // Error code of last operation.
    BYTE                deviceClientDriver;                 // Index of client driver for this device if bfUseDeviceClientDriver=1.
    WORD                currentConfigurationPower;          // Max power in milli-amps.

    volatile union
    {
        struct
        {
            BYTE        bfIsLowSpeed                : 1;    // If the device is low speed (default = 0).
            BYTE        bfSupportsOTG               : 1;    // If the device supports OTG (default = 0).
            BYTE        bfConfiguredOTG             : 1;    // If OTG on the device has been configured (default = 0).
            BYTE        bfAllowHNP                  : 1;    // If Host Negotiation Protocol is allowed (default = 0).
            BYTE        bfPingPongIn                : 1;    // Ping-pong status of IN buffers (default = 0).
            BYTE        bfPingPongOut               : 1;    // Ping-pong status of OUT buffers (default = 0).
            BYTE        bfUseDeviceClientDriver     : 1;    // Indicates driver should use a single client driver (deviceClientDriver)
        };
        WORD            val;
    }                   flags;
} USB_DEVICE_INFO;


// *****************************************************************************
/* Interface Information Structure

This structure contains information about one interface.
*/
typedef struct _USB_INTERFACE_INFO
{
   struct _USB_INTERFACE_INFO *next;        // Pointer to the next node in the list.

   BYTE                interface;           // Interface number
   BYTE                interfaceAltSetting; // Alternate Interface setting
   BYTE                clientDriver;        // Index into client driver table for this Interface

} USB_INTERFACE_INFO;


// *****************************************************************************
/* Endpoint Information Node

This structure contains all the needed information about an endpoint.  Multiple
endpoints form a linked list.
*/
typedef struct _USB_ENDPOINT_INFO
{
    struct _USB_ENDPOINT_INFO   *next;                  // Pointer to the next node in the list.

    volatile union
    {
        struct
        {
            BYTE        bfErrorCount            : 3;    // We need to error at 4. Not used for isochronous.
            BYTE        bfStalled               : 1;    // Received a STALL.  Requires host interaction to clear.
            BYTE        bfError                 : 1;    // Error count excessive. Must be cleared by the application.
            BYTE        bfUserAbort             : 1;    // User terminated transfer.
            BYTE        bfTransferSuccessful    : 1;    // Received an ACK.
            BYTE        bfTransferComplete      : 1;    // Transfer done, status obtained.
            BYTE        bfUseDTS                : 1;    // Use DTS error checking.
            BYTE        bfNextDATA01            : 1;    // The value of DTS for the next transfer.
            BYTE        bfLastTransferNAKd      : 1;    // The last transfer attempted NAK'd.
        };
        WORD            val;
    }                   status;
    WORD                wInterval;                      // Polling interval for interrupt and isochronous endpoints.
    volatile WORD       wIntervalCount;                 // Current interval count.
    WORD                wMaxPacketSize;                 // Endpoint packet size.
    volatile DWORD      dataCountMax;                   // Amount of data to transfer during the transfer.
    WORD                dataCountMaxSETUP;              // Amount of data in the SETUP packet (if applicable).
    volatile DWORD      dataCount;                      // Count of bytes transferred.
    BYTE               *pUserDataSETUP;                 // Pointer to data for the SETUP packet (if applicable).
    BYTE               *pUserData;                      // Pointer to data for the transfer.
    volatile BYTE       transferState;                  // State of endpoint tranfer.
    USB_INTERFACE_INFO *pInterface;                     // Pointer to current interface.
    BYTE                bEndpointAddress;               // Endpoint address
    TRANSFER_ATTRIBUTES bmAttributes;                   // Endpoint attributes, including transfer type.
    volatile BYTE       bErrorCode;                     // If bfError is set, this indicates the reason
    volatile WORD       countNAKs;                      // Count of NAK's of current transaction.

} USB_ENDPOINT_INFO;

// *****************************************************************************
/* Event Data

This structure defines the data associated with any USB events (see USB_EVENT)
that can be generated by the USB ISR (see _USB1Interrupt).  These events and
their associated data are placed in an event queue used to synchronize between
the main host-tasks loop (see USBHostTasks) and the ISR.  This queue is required
only if transfer events are being used.  All other events are send directly to
the client drivers.
*/
#if defined( USB_ENABLE_TRANSFER_EVENT )
    typedef struct
    {
        USB_EVENT               event;          // Event that occured.
        union
        {
            //BYTE                deviceAddress;  // Event: EVENT_DETACH, sent directly
            HOST_TRANSFER_DATA  TransferData;   // Event: EVENT_TRANSFER,
                                                //        EVENT_BUS_ERROR

            // To Do: Add additional items as needed for new events.
        };
    } USB_EVENT_DATA;
#endif

// *****************************************************************************
/* Event Queue

This structure defines the queue of USB events that can be generated by the
ISR that need to be synchronized to the USB event tasks loop (see
USB_EVENT_DATA, above).  See "struct_queue.h" for usage and operations.
*/
#if defined( USB_ENABLE_TRANSFER_EVENT )
    #ifndef USB_EVENT_QUEUE_DEPTH
        #define USB_EVENT_QUEUE_DEPTH   4       // Default depth of 4 events
    #endif

    typedef struct _usb_event_queue
    {
        int             head;
        int             tail;
        int             count;
        USB_EVENT_DATA  buffer[USB_EVENT_QUEUE_DEPTH];

    } USB_EVENT_QUEUE;
#endif


//******************************************************************************
//******************************************************************************
// Section: Local Prototypes
//******************************************************************************
//******************************************************************************

void                _USB_CheckCommandAndEnumerationAttempts( void );
BOOL                _USB_FindClassDriver( BYTE bClass, BYTE bSubClass, BYTE bProtocol, BYTE *pbClientDrv );
BOOL                _USB_FindDeviceLevelClientDriver( void );
USB_INTERFACE_INFO * _USB_FindInterface ( BYTE bInterface, BYTE bAltSetting );
void                _USB_FindNextToken( void );
void                _USB_FreeConfigMemory( void );
void                _USB_FreeMemory( void );
void                _USB_InitControlRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData, WORD controlSize,
                              BYTE *pData, WORD size );
void                _USB_InitControlWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData, WORD controlSize,
                               BYTE *pData, WORD size );
void                _USB_InitRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size );
void                _USB_InitWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size );
void                _USB_NotifyClients( BYTE DevAddress, USB_EVENT event, void *data, unsigned int size );
BOOL                _USB_ParseConfigurationDescriptor( void );
void                _USB_ResetDATA0( BYTE endpoint );
void                _USB_SendToken( BYTE endpoint, BYTE tokenType );
void                _USB_SetBDT( BYTE  direction );
BOOL                _USB_TransferInProgress( void );


//******************************************************************************
//******************************************************************************
// Section: Host Global Variables
//******************************************************************************
//******************************************************************************

#if (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)
    BDT_ENTRY __attribute__ ((aligned(512)))    BDT[2];
    #define BDT_IN                                  (&BDT[0])           // EP0 IN Buffer Descriptor
    #define BDT_OUT                                 (&BDT[1])           // EP0 OUT Buffer Descriptor
#elif (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
    BDT_ENTRY __attribute__ ((aligned(512)))    BDT[3];
    #define BDT_IN                                  (&BDT[0])           // EP0 IN Buffer Descriptor
    #define BDT_OUT                                 (&BDT[1])           // EP0 OUT Even Buffer Descriptor
    #define BDT_OUT_ODD                             (&BDT[2])           // EP0 OUT Odd Buffer Descriptor
#elif (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
    BDT_ENTRY __attribute__ ((aligned(512)))    BDT[4];
    #define BDT_IN                                  (&BDT[0])           // EP0 IN Even Buffer Descriptor
    #define BDT_IN_ODD                              (&BDT[1])           // EP0 IN Odd Buffer Descriptor
    #define BDT_OUT                                 (&BDT[2])           // EP0 OUT Even Buffer Descriptor
    #define BDT_OUT_ODD                             (&BDT[3])           // EP0 OUT Odd Buffer Descriptor
#endif

BYTE                        countConfigurations;                        // Count the Configuration Descriptors read during enumeration.
BYTE                        numCommandTries;                            // The number of times the current command has been tried.
BYTE                        numEnumerationTries;                        // The number of times enumeration has been attempted on the attached device.
volatile WORD               numTimerInterrupts;                         // The number of milliseconds elapsed during the current waiting period.
volatile USB_ENDPOINT_INFO *pCurrentEndpoint;                           // Pointer to the endpoint currently performing a transfer.
USB_CONFIGURATION          *pConfigurationDescriptorList       = NULL;  // Pointer to the list of Donfiguration Descriptors of the attached device.
BYTE                       *pCurrentConfigurationDescriptor    = NULL;  // Pointer to the current configuration descriptor of the attached device.
BYTE                       *pDeviceDescriptor                  = NULL;  // Pointer to the Device Descriptor of the attached device.
USB_ENDPOINT_INFO          *pEndpointList                      = NULL;  // List of endpoints on the attached device.
BYTE                       *pEP0Data                           = NULL;  // A data buffer for use by EP0.
USB_INTERFACE_INFO         *pInterfaceList                     = NULL;  // List of interfaces on the attached device.
USB_BUS_INFO                usbBusInfo;                                 // Information about the USB bus.
USB_DEVICE_INFO             usbDeviceInfo;                              // A collection of information about the attached device.
#if defined( USB_ENABLE_TRANSFER_EVENT )
    USB_EVENT_QUEUE         usbEventQueue;                              // Queue of USB events used to synchronize ISR to main tasks loop.
#endif
volatile WORD               usbHostState;                               // State machine state of the attached device.
volatile WORD               usbOverrideHostState;                       // Next state machine state, when set by interrupt processing.

#ifdef ENABLE_STATE_TRACE   // Debug trace support
    WORD prevHostState;
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Application Callable Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BYTE USBHostClearEndpointErrors( BYTE deviceAddress, BYTE endpoint )

Notes:          None

Tested:
*******************************************************************************/

BYTE USBHostClearEndpointErrors( BYTE deviceAddress, BYTE endpoint )
{
    USB_ENDPOINT_INFO *ep;

    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (ep->bEndpointAddress == endpoint)
        {
            ep->status.bfStalled    = 0;
            ep->status.bfError      = 0;

            return USB_SUCCESS;
        }
        ep = ep->next;
    }
    return USB_ENDPOINT_NOT_FOUND;
}


/*******************************************************************************
Function:       BYTE USBHostDeviceRequest( BYTE deviceAddress, BYTE bmRequestType,
                    BYTE bRequest, WORD wValue, WORD wIndex, WORD wLength,
                    BYTE *data, BYTE dataDirection )

Notes:          DTS reset is done before the command is issued.

Tested:
*******************************************************************************/

BYTE USBHostDeviceRequest( BYTE deviceAddress, BYTE bmRequestType, BYTE bRequest,
            WORD wValue, WORD wIndex, WORD wLength, BYTE *data, BYTE dataDirection )
{
    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    // If we are not in a normal user running state, we cannot do this.
    if ((usbHostState & STATE_MASK) != STATE_RUNNING)
    {
        return USB_INVALID_STATE;
    }

    // Make sure no other reads or writes on EP0 are in progress.
    if (!pEndpointList->status.bfTransferComplete)
    {
        return USB_ENDPOINT_BUSY;
    }

    // We can't do a SET CONFIGURATION here.  Must use USBHostSetDeviceConfiguration().
    if (bRequest == USB_REQUEST_SET_CONFIGURATION)
    {
        return USB_ILLEGAL_REQUEST;
    }

    // If the user is doing a SET INTERFACE, we must reset DATA0 for all endpoints.
    if (bRequest == USB_REQUEST_SET_INTERFACE)
    {
        _USB_ResetDATA0( 0 );
    }

    // If the user is doing a CLEAR FEATURE(ENDPOINT_HALT), we must reset DATA0 for that endpoint.
    if ((bRequest == USB_REQUEST_CLEAR_FEATURE) && (wValue == USB_FEATURE_ENDPOINT_HALT))
    {
        _USB_ResetDATA0( (BYTE)wIndex );
    }

    // Set up the control packet.
    pEP0Data[0] = bmRequestType;
    pEP0Data[1] = bRequest;
    pEP0Data[2] = wValue & 0xFF;
    pEP0Data[3] = (wValue >> 8) & 0xFF;
    pEP0Data[4] = wIndex & 0xFF;
    pEP0Data[5] = (wIndex >> 8) & 0xFF;
    pEP0Data[6] = wLength & 0xFF;
    pEP0Data[7] = (wLength >> 8) & 0xFF;

    if (dataDirection == USB_DEVICE_REQUEST_SET)
    {
        // We are doing a SET command that requires data be sent.
        _USB_InitControlWrite( pEndpointList, pEP0Data,8, data, wLength );
    }
    else
    {
        // We are doing a GET request.
        _USB_InitControlRead( pEndpointList, pEP0Data, 8, data, wLength );
    }

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       BYTE USBHostDeviceStatus( BYTE deviceAddress )

Notes:          None

Tested:

TODO:           This will change when multiple devices are supported.  For right
                now, just give the status of the single attached device.
*******************************************************************************/

BYTE USBHostDeviceStatus( BYTE deviceAddress )
{
    if ((usbHostState & STATE_MASK) == STATE_DETACHED)
    {
        return USB_DEVICE_DETACHED;
    }

    if ((usbHostState & STATE_MASK) == STATE_RUNNING)
    {
        return USB_DEVICE_ATTACHED;
    }

    if ((usbHostState & STATE_MASK) == STATE_HOLDING)
    {
        return usbDeviceInfo.errorCode;
    }

    return USB_DEVICE_ENUMERATING;
}

/*******************************************************************************
Function:       void USBHostInit( void )

Notes:          An entry is created in the endpoint list for EP0.

                For PIC24, the BDT address is a two-byte value.  For PIC32MX,
                it is a four-byte value.

Tested:
*******************************************************************************/

BYTE USBHostInit( void )
{
    // Allocate space for Endpoint 0.  We will initialize it in the state machine,
    // so we can reinitialize when another device connects.
    if ((pEndpointList = (USB_ENDPOINT_INFO*)malloc( sizeof(USB_ENDPOINT_INFO) )) == NULL)
    {
        #ifdef DEBUG_MODE
            UART2PrintString( "HOST: Cannot allocate for endpoint list.\r\n" );
        #endif
        return USB_MEMORY_ALLOCATION_ERROR;
    }
    pEndpointList->next = NULL;

    // Initialize other variables.
    pCurrentEndpoint                    = pEndpointList;
    usbHostState                        = STATE_DETACHED;
    usbOverrideHostState                = NO_STATE;
    usbDeviceInfo.deviceAddressAndSpeed = 0;
    usbDeviceInfo.deviceAddress         = 0;

    // Initialize event queue
    #if defined( USB_ENABLE_TRANSFER_EVENT )
        StructQueueInit(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
    #endif

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       BYTE USBHostRead( BYTE deviceAddress, BYTE endpoint, BYTE *pData,
                        DWORD size )

Notes:          None

Tested:
*******************************************************************************/

BYTE USBHostRead( BYTE deviceAddress, BYTE endpoint, BYTE *pData, DWORD size )
{
    USB_ENDPOINT_INFO *ep;

    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    // If we are not in a normal user running state, we cannot do this.
    if ((usbHostState & STATE_MASK) != STATE_RUNNING)
    {
        return USB_INVALID_STATE;
    }

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (ep->bEndpointAddress == endpoint)
        {
            if (ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_CONTROL)
            {
                // Must not be a control endpoint.
                return USB_ENDPOINT_ILLEGAL_TYPE;
            }

            if (!(ep->bEndpointAddress & 0x80))
            {
                // Trying to do an IN with an OUT endpoint.
                return USB_ENDPOINT_ILLEGAL_DIRECTION;
            }

            if (ep->status.bfStalled)
            {
                // The endpoint is stalled.  It must be restarted before a write
                // can be performed.
                return USB_ENDPOINT_STALLED;
            }

            if (ep->status.bfError)
            {
                // The endpoint has errored.  The error must be cleared before a
                // write can be performed.
                return USB_ENDPOINT_ERROR;
            }

            if (!ep->status.bfTransferComplete)
            {
                // We are already processing a request for this endpoint.
                return USB_ENDPOINT_BUSY;
            }

            _USB_InitRead( ep, pData, size );

            return USB_SUCCESS;
        }
        ep = ep->next;
    }
    return USB_ENDPOINT_NOT_FOUND;   // Endpoint not found
}

/*******************************************************************************
Function:       BYTE USBHostResetDevice( BYTE deviceAddress )

Notes:          In order to do a full clean-up, the state is set back to
                STATE_DETACHED rather than a reset state.  The ATTACH interrupt
                will automatically be triggered when the module is re-enabled,
                and the proper reset will be performed.

Tested:

TODO:           This function will have to be modified when multiple devices
                are supported.
*******************************************************************************/

BYTE USBHostResetDevice( BYTE deviceAddress )
{
    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    if ((usbHostState & STATE_MASK) == STATE_DETACHED)
    {
        return USB_ILLEGAL_REQUEST;
    }

    usbHostState = STATE_DETACHED;

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       BYTE USBHostResumeDevice( BYTE deviceAddress )

Notes:          None

Tested:

TODO: This function will change when hubs and multiple devices
                are supported.
*******************************************************************************/

BYTE USBHostResumeDevice( BYTE deviceAddress )
{
    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    if (usbHostState != (STATE_RUNNING | SUBSTATE_SUSPEND_AND_RESUME | SUBSUBSTATE_SUSPEND))
    {
        return USB_ILLEGAL_REQUEST;
    }

    // Advance the state machine to issue resume signalling.
    _USB_SetNextSubSubState();

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       BYTE USBHostSetDeviceConfiguration( BYTE deviceAddress,
                    BYTE configuration )

Notes:          None

Tested:
*******************************************************************************/

BYTE USBHostSetDeviceConfiguration( BYTE deviceAddress, BYTE configuration )
{
    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    // If we are not in a normal user running state, we cannot do this.
    if ((usbHostState & STATE_MASK) != STATE_RUNNING)
    {
        return USB_INVALID_STATE;
    }

    // Make sure no other reads or writes are in progress.
    if (_USB_TransferInProgress())
    {
        return USB_BUSY;
    }

    // Set the new device configuration.
    usbDeviceInfo.currentConfiguration = configuration;

    // Set the state back to configure the device.  This will destroy the
    // endpoint list and terminate any current transactions.  We already have
    // the configuration, so we can jump into the Select Configuration state.
    // If the configuration value is invalid, the state machine will error and
    // put the device into a holding state.
    usbHostState = STATE_CONFIGURING | SUBSTATE_SELECT_CONFIGURATION;

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       void USBHostShutdown( void )

Notes:          None

Tested:
*******************************************************************************/

void USBHostShutdown( void )
{
    U1PWRC = USB_NORMAL_OPERATION | USB_DISABLED;
    USB_HOST_APP_EVENT_HANDLER(USB_ROOT_HUB, EVENT_VBUS_SES_END, NULL, 0);

    _USB_FreeMemory();
}


/*******************************************************************************
Function:       BYTE USBHostSuspendDevice( BYTE deviceAddress )

Notes:          None

Tested:

TODO:           This function will need to be reworked when hubs and
                multiple devices are supported.
*******************************************************************************/

BYTE USBHostSuspendDevice( BYTE deviceAddress )
{
    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    if (usbHostState != (STATE_RUNNING | SUBSTATE_NORMAL_RUN))
    {
        return USB_ILLEGAL_REQUEST;
    }

    // Turn off SOF's, so the bus is idle.
    U1CONbits.SOFEN = 0;

    // Put the state machine in suspend mode.
    usbHostState = STATE_RUNNING | SUBSTATE_SUSPEND_AND_RESUME | SUBSUBSTATE_SUSPEND;

    return USB_SUCCESS;
}

/*******************************************************************************
Function:       void USBHostTasks( void )

Notes:          None

Tested:
*******************************************************************************/

void USBHostTasks( void )
{
    USB_CONFIGURATION  *pCurrentConfigurationNode;
    USB_INTERFACE_INFO *pCurrentInterface;
    BYTE               *pTemp;
    BYTE                temp;

    #ifdef DEBUG_MODE
//        UART2PutChar('<');
//        UART2PutHex( usbHostState>>8 );
//        UART2PutHex( usbHostState & 0xff );
//        UART2PutChar('-');
//        UART2PutHex( pCurrentEndpoint->transferState );
//        UART2PutChar('>');
    #endif

    // The PIC32MX detach interrupt is not reliable.  If we are not in one of
    // the detached states, we'll do a check here to see if we've detached.
    // If the ATTACH bit is 0, we have detached.
    #ifdef __PIC32MX__
        #ifdef USE_MANUAL_DETACH_DETECT
            if (((usbHostState & STATE_MASK) != STATE_DETACHED) && !U1IRbits.ATTACHIF)
            {
                #ifdef DEBUG_MODE
                    UART2PutChar( '>' );
                    UART2PutChar( ']' );
                #endif
                usbHostState = STATE_DETACHED;
            }
        #endif
    #endif

    // Send any queued events to the client and application layers.
    // To Do: Include other events as needed.
    #if defined ( USB_ENABLE_TRANSFER_EVENT )
    {
        USB_EVENT_DATA *item;
        #if defined( __C30__ )
            WORD        interrupt_mask;
        #elif defined( __PIC32MX__ )
            UINT32      interrupt_mask;
        #else
            #error Cannot save interrupt status
        #endif

        while (StructQueueIsNotEmpty(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
        {
            item = StructQueuePeekTail(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);

            switch(item->event)
            {
//                case EVENT_DETACH:
//                    // This event is sent directly to the client drivers, rather than being queued.
//                    _USB_NotifyClients( usbDeviceInfo.deviceAddress, item->event, &item->deviceAddress, sizeof(BYTE) );
//                    break;
                case EVENT_TRANSFER:
                case EVENT_BUS_ERROR:
                    _USB_NotifyClients( usbDeviceInfo.deviceAddress, item->event, &item->TransferData, sizeof(HOST_TRANSFER_DATA) );
                    break;
            }

            // Guard against USB interrupts
            interrupt_mask = U1IE;
            U1IE = 0;

            StructQueueRemove(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);

            // Re-enable USB interrupts
            U1IE = interrupt_mask;
        }
    }
    #endif

    // See if we got an interrupt to change our state.
    if (usbOverrideHostState != NO_STATE)
    {
        #ifdef DEBUG_MODE
            UART2PutChar('>');
        #endif
        usbHostState = usbOverrideHostState;
        usbOverrideHostState = NO_STATE;
    }

    switch (usbHostState & STATE_MASK)
    {
        case STATE_DETACHED:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_INITIALIZE:
                    // We got here either from initialization or from the user
                    // unplugging the device at any point in time.

                    // Notify the higher layers of the detach
                    if (usbDeviceInfo.deviceAddress)
                    {
                        _USB_NotifyClients(usbDeviceInfo.deviceAddress, EVENT_DETACH, &usbDeviceInfo.deviceAddress, sizeof(BYTE) );
                    }

                    // Turn off the module and free up memory.
                    USBHostShutdown();

                    #ifdef DEBUG_MODE
                        UART2PrintString( "HOST: Initializing DETACHED state.\r\n" );
                    #endif

                    // Initialize Endpoint 0 attributes.
                    pEndpointList->next                         = NULL;
                    pEndpointList->status.val                   = 0x00;
                    pEndpointList->status.bfUseDTS              = 1;
                    pEndpointList->status.bfTransferComplete    = 1;    // Initialize to success to allow preprocessing loops.
                    pEndpointList->wMaxPacketSize               = 64;
                    pEndpointList->dataCount                    = 0;    // Initialize to 0 since we set bfTransferComplete.
                    pEndpointList->bEndpointAddress             = 0;
                    pEndpointList->transferState                = TSTATE_IDLE;
                    pEndpointList->bmAttributes.bfTransferType  = USB_TRANSFER_TYPE_CONTROL;

                    // Initialize any device specific information.
                    numEnumerationTries                 = USB_NUM_ENUMERATION_TRIES;
                    usbDeviceInfo.currentConfiguration  = 0; // Will be overwritten by config process or the user later
                    usbDeviceInfo.attributesOTG         = 0;
                    usbDeviceInfo.deviceAddressAndSpeed = 0;
                    usbDeviceInfo.flags.val             = 0;

                    // Set up the hardware.
                    U1IE                = 0;        // Clear and turn off interrupts.
                    U1IR                = 0xFF;
                    U1OTGIE             = 0;
                    U1OTGIR             = 0xFF;
                    U1EIE               = 0;
                    U1EIR               = 0xFF;
                    U1IEbits.ATTACHIE   = 1;        // Enable the ATTACH interrupt.

                    // Initialize the Buffer Descriptor Table pointer.
                    #if defined(__C30__)
                       U1BDTP1 = (WORD)(&BDT) >> 8;
                    #elif defined(__PIC32MX__)
                       U1BDTP1 = ((DWORD)KVA_TO_PA(&BDT) & 0x0000FF00) >> 8;
                       U1BDTP2 = ((DWORD)KVA_TO_PA(&BDT) & 0x00FF0000) >> 16;
                       U1BDTP3 = ((DWORD)KVA_TO_PA(&BDT) & 0xFF000000) >> 24;
                    #else
                        #error Cannot set up the Buffer Descriptor Table pointer.
                    #endif

                    // Configure the module
                    U1CON               = USB_HOST_MODE_ENABLE | USB_SOF_DISABLE;                       // Turn of SOF's to cut down noise
                    U1CON               = USB_HOST_MODE_ENABLE | USB_PINGPONG_RESET | USB_SOF_DISABLE;  // Reset the ping-pong buffers
                    U1CON               = USB_HOST_MODE_ENABLE | USB_SOF_DISABLE;                       // Release the ping-pong buffers
                    U1OTGCON            = USB_DPLUS_PULLDOWN_ENABLE | USB_DMINUS_PULLDOWN_ENABLE | USB_VBUS_ON | USB_OTG_ENABLE; // Pull down D+ and D-
                    U1CNFG1             = USB_STOP_IN_IDLE_MODE | USB_PING_PONG_MODE;
                    #if defined(__C30__)
                        U1CNFG2         = USB_VBUS_BOOST_ENABLE | USB_VBUS_COMPARE_ENABLE | USB_ONCHIP_ENABLE;
                    #endif
                    U1ADDR              = 0;                        // Set default address and LSPDEN to 0
                    U1EP0bits.LSPD      = 0;
                    U1SOF               = USB_SOF_THRESHOLD_64;     // Maximum EP0 packet size

                    // Power on the module
                    U1PWRC              = USB_NORMAL_OPERATION | USB_ENABLED;
                    USB_HOST_APP_EVENT_HANDLER(USB_ROOT_HUB, EVENT_VBUS_SES_VALID, NULL, 0);

                    // Set the next substate.  We do this before we enable
                    // interrupts in case the interrupt changes states.
                    _USB_SetNextSubState();

                    #if defined( __C30__ )
                        IFS5            &= 0xFFBF;
                        IPC21           &= 0xF0FF;
                        IPC21           |= 0x0600;
                        IEC5            |= 0x0040;
                    #elif defined( __PIC32MX__ )
                        // Enable the USB interrupt.
                        IFS1CLR         = 0x02000000;
                        IPC11CLR        = 0x0000FF00;
                        IPC11SET        = 0x00001000;
                        IEC1SET         = 0x02000000;
                    #else
                        #error Cannot enable USB interrupt.
                    #endif
                    break;

                case SUBSTATE_WAIT_FOR_DEVICE:
                    // Wait here for the ATTACH interrupt.
                    break;
            }
            break;

        case STATE_ATTACHED:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_SETTLE:
                    // Wait 100ms for the insertion process to complete and power
                    // at the device to be stable.
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_START_SETTLING_DELAY:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Starting settling delay.\r\n" );
                            #endif

                            // Clear and turn on the DETACH interrupt.
                            U1IR                    = USB_INTERRUPT_DETACH;   // The interrupt is cleared by writing a '1' to the flag.
                            U1IEbits.DETACHIE       = 1;

                            // Configure and turn on the settling timer - 100ms.
                            numTimerInterrupts      = USB_INSERT_TIME;
                            U1OTGIR                 = USB_INTERRUPT_T1MSECIF; // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIEbits.T1MSECIE    = 1;
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_SETTLING:
                            // Wait for the timer to finish in the background.
                            break;

                        case SUBSUBSTATE_SETTLING_DONE:
                            _USB_SetNextSubState();
                            break;

                        default:
                            // We shouldn't get here.
                            break;
                    }
                    break;

                case SUBSTATE_RESET_DEVICE:
                    // Reset the device.  We have to do the reset timing ourselves.
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SET_RESET:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Resetting the device.\r\n" );
                            #endif

                            // Prepare a data buffer for us to use.  We'll make it 8 bytes for now,
                            // which is the minimum wMaxPacketSize for EP0.
                            if (pEP0Data != NULL)
                            {
                                freez( pEP0Data );
                            }
                            if ((pEP0Data = (BYTE *)malloc( 8 )) == NULL)
                            {
                                #ifdef DEBUG_MODE
                                    UART2PrintString( "HOST: Error alloc-ing pEP0Data\r\n" );
                                #endif
                                _USB_SetErrorCode( USB_HOLDING_OUT_OF_MEMORY );
                                _USB_SetHoldState();
                                break;
                            }

                            // Initialize the USB Device information
                            usbDeviceInfo.currentConfiguration      = 0;
                            usbDeviceInfo.attributesOTG             = 0;
                            usbDeviceInfo.flags.val                 = 0;

                            _USB_InitErrorCounters();

                            // Disable all EP's except EP0.
                            U1EP0  = USB_ENDPOINT_CONTROL_SETUP;
                            U1EP1  = USB_DISABLE_ENDPOINT;
                            U1EP2  = USB_DISABLE_ENDPOINT;
                            U1EP3  = USB_DISABLE_ENDPOINT;
                            U1EP4  = USB_DISABLE_ENDPOINT;
                            U1EP5  = USB_DISABLE_ENDPOINT;
                            U1EP6  = USB_DISABLE_ENDPOINT;
                            U1EP7  = USB_DISABLE_ENDPOINT;
                            U1EP8  = USB_DISABLE_ENDPOINT;
                            U1EP9  = USB_DISABLE_ENDPOINT;
                            U1EP10 = USB_DISABLE_ENDPOINT;
                            U1EP11 = USB_DISABLE_ENDPOINT;
                            U1EP12 = USB_DISABLE_ENDPOINT;
                            U1EP13 = USB_DISABLE_ENDPOINT;
                            U1EP14 = USB_DISABLE_ENDPOINT;
                            U1EP15 = USB_DISABLE_ENDPOINT;

                            // See if the device is low speed.
                            if (!U1CONbits.JSTATE)
                            {
                                #ifdef DEBUG_MODE
                                    UART2PrintString( "HOST: Low Speed!\r\n" );
                                #endif
                                usbDeviceInfo.flags.bfIsLowSpeed    = 1;
                                usbDeviceInfo.deviceAddressAndSpeed = 0x80;
                                U1ADDR                              = 0x80;
                                U1EP0bits.LSPD                      = 1;
                            }

                            // Reset all ping-pong buffers if they are being used.
                            U1CONbits.PPBRST                    = 1;
                            U1CONbits.PPBRST                    = 0;
                            usbDeviceInfo.flags.bfPingPongIn    = 0;
                            usbDeviceInfo.flags.bfPingPongOut   = 0;

                            // Assert reset for 10ms.  Start a timer countdown.
                            U1CONbits.USBRST                    = 1;
                            numTimerInterrupts                  = USB_RESET_TIME;
                            //U1OTGIRbits.T1MSECIF                = 1;       // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIR                             = USB_INTERRUPT_T1MSECIF; // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIEbits.T1MSECIE                = 1;

                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_RESET_WAIT:
                            // Wait for the timer to finish in the background.
                            break;

                        case SUBSUBSTATE_RESET_RECOVERY:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Reset complete.\r\n" );
                            #endif

                            // Deassert reset.
                            U1CONbits.USBRST        = 0;

                            // Start sending SOF's.
                            U1CONbits.SOFEN         = 1;

                            // Wait for the reset recovery time.
                            numTimerInterrupts      = USB_RESET_RECOVERY_TIME;
                            //U1OTGIRbits.T1MSECIF    = 1;       // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIR                 = USB_INTERRUPT_T1MSECIF; // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIEbits.T1MSECIE    = 1;

                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_RECOVERY_WAIT:
                            // Wait for the timer to finish in the background.
                            break;

                        case SUBSUBSTATE_RESET_COMPLETE:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Reset complete.\r\n" );
                            #endif

                            // Enable USB interrupts
                            U1IE                    = USB_INTERRUPT_TRANSFER | USB_INTERRUPT_SOF | USB_INTERRUPT_ERROR | USB_INTERRUPT_DETACH;
                            U1EIE                   = 0xFF;

                            _USB_SetNextSubState();
                            break;

                        default:
                            // We shouldn't get here.
                            break;
                    }
                    break;

                case SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE:
                    // Send the GET DEVICE DESCRIPTOR command to get just the size
                    // of the descriptor and the max packet size, so we can allocate
                    // a large enough buffer for getting the whole thing and enough
                    // buffer space for each piece.
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR_SIZE:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Getting Device Descriptor size.\r\n" );
                            #endif

                            // Set up and send GET DEVICE DESCRIPTOR
                            if (pDeviceDescriptor != NULL)
                            {
                                freez( pDeviceDescriptor );
                            }

                            pEP0Data[0] = USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_GET_DESCRIPTOR;
                            pEP0Data[2] = 0; // Index
                            pEP0Data[3] = USB_DESCRIPTOR_DEVICE; // Type
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = 8;
                            pEP0Data[7] = 0;

                            _USB_InitControlRead( pEndpointList, pEP0Data, 8, pEP0Data, 8 );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR_SIZE:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    // See if a hub is attached.  Hubs are not supported.
                                    if (pEP0Data[4] == USB_HUB_CLASSCODE)   // bDeviceClass
                                    {
                                        _USB_SetErrorCode( USB_HOLDING_UNSUPPORTED_HUB );
                                        _USB_NotifyClients(usbDeviceInfo.deviceAddress, EVENT_HUB_ATTACH, &usbDeviceInfo.deviceAddress, sizeof(BYTE) );
                                        _USB_SetHoldState();
                                    }
                                    else
                                    {
                                        _USB_SetNextSubSubState();
                                    }
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE_COMPLETE:
                            // Allocate a buffer for the entire Device Descriptor
                            if ((pDeviceDescriptor = (BYTE *)malloc( *pEP0Data )) == NULL)
                            {
                                // We cannot continue.  Freeze until the device is removed.
                                _USB_SetErrorCode( USB_HOLDING_OUT_OF_MEMORY );
                                _USB_SetHoldState();
                                break;
                            }
                            // Save the descriptor size in the descriptor (bLength)
                            *pDeviceDescriptor = *pEP0Data;

                            // Set the EP0 packet size.
                            pEndpointList->wMaxPacketSize = ((USB_DEVICE_DESCRIPTOR *)pEP0Data)->bMaxPacketSize0;

                            // Make our pEP0Data buffer the size of the max packet.
                            freez( pEP0Data );
                            if ((pEP0Data = (BYTE *)malloc( pEndpointList->wMaxPacketSize )) == NULL)
                            {
                                // We cannot continue.  Freeze until the device is removed.
                                #ifdef DEBUG_MODE
                                    UART2PrintString( "HOST: Error re-alloc-ing pEP0Data\r\n" );
                                #endif
                                _USB_SetErrorCode( USB_HOLDING_OUT_OF_MEMORY );
                                _USB_SetHoldState();
                                break;
                            }

                            // Clean up and advance to the next substate.
                            _USB_InitErrorCounters();
                            _USB_SetNextSubState();
                            break;

                        default:
                            break;
                    }
                    break;

                case SUBSTATE_GET_DEVICE_DESCRIPTOR:
                    // Send the GET DEVICE DESCRIPTOR command and receive the response
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_GET_DEVICE_DESCRIPTOR:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Getting device descriptor.\r\n" );
                            #endif

                            // If we are currently sending a token, we cannot do anything.
                            if (U1CONbits.TOKBUSY)
                                break;

                            // Set up and send GET DEVICE DESCRIPTOR
                            pEP0Data[0] = USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_GET_DESCRIPTOR;
                            pEP0Data[2] = 0; // Index
                            pEP0Data[3] = USB_DESCRIPTOR_DEVICE; // Type
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = *pDeviceDescriptor;
                            pEP0Data[7] = 0;
                            _USB_InitControlRead( pEndpointList, pEP0Data, 8, pDeviceDescriptor, *pDeviceDescriptor  );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_GET_DEVICE_DESCRIPTOR:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_GET_DEVICE_DESCRIPTOR_COMPLETE:
                            // Clean up and advance to the next substate.
                            _USB_InitErrorCounters();
                            _USB_SetNextSubState();
                            break;

                        default:
                            break;
                    }
                    break;

                case SUBSTATE_VALIDATE_VID_PID:
                    #ifdef DEBUG_MODE
                        UART2PrintString( "HOST: Validating VID and PID.\r\n" );
                    #endif

                    // Search the TPL for the device's VID & PID.  If a client driver is
                    // available for the over-all device, use it.  Otherwise, we'll search
                    // again later for an appropriate class driver.
                    _USB_FindDeviceLevelClientDriver();

                    // Advance to the next state to assign an address to the device.
                    //
                    // Note: We assign an address to all devices and hold later if
                    // we can't find a supported configuration.
                    _USB_SetNextState();
                    break;
            }
            break;

        case STATE_ADDRESSING:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_SET_DEVICE_ADDRESS:
                    // Send the SET ADDRESS command.  We can't set the device address
                    // in hardware until the entire transaction is complete.
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_SET_DEVICE_ADDRESS:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Setting device address.\r\n" );
                            #endif

                            // Select an address for the device.  Store it so we can access it again
                            // easily.  We'll put the low speed indicator on later.
                            // This has been broken out so when we allow multiple devices, we have
                            // a single interface point to allocate a new address.
                            usbDeviceInfo.deviceAddress = USB_DEVICE_ADDRESS;

                            // Set up and send SET ADDRESS
                            pEP0Data[0] = USB_SETUP_HOST_TO_DEVICE | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_SET_ADDRESS;
                            pEP0Data[2] = usbDeviceInfo.deviceAddress;
                            pEP0Data[3] = 0;
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = 0;
                            pEP0Data[7] = 0;
                            _USB_InitControlWrite( pEndpointList, pEP0Data, 8, NULL, 0 );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_SET_DEVICE_ADDRESS:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_SET_DEVICE_ADDRESS_COMPLETE:
                            // Set the device's address here.
                            usbDeviceInfo.deviceAddressAndSpeed = (usbDeviceInfo.flags.bfIsLowSpeed << 7) | usbDeviceInfo.deviceAddress;

                            // Clean up and advance to the next state.
                            _USB_InitErrorCounters();
                            _USB_SetNextState();
                            break;

                        default:
                            break;
                    }
                    break;
            }
            break;

        case STATE_CONFIGURING:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_INIT_CONFIGURATION:
                    // Delete the old list of configuration descriptors and
                    // initialize the counter.  We will request the descriptors
                    // from highest to lowest so the lowest will be first in
                    // the list.
                    countConfigurations = ((USB_DEVICE_DESCRIPTOR *)pDeviceDescriptor)->bNumConfigurations;
                    while (pConfigurationDescriptorList != NULL)
                    {
                        pTemp = (BYTE *)pConfigurationDescriptorList->next;
                        free( pConfigurationDescriptorList->descriptor );
                        free( pConfigurationDescriptorList );
                        pConfigurationDescriptorList = (USB_CONFIGURATION *)pTemp;
                    }
                    _USB_SetNextSubState();
                    break;

                case SUBSTATE_GET_CONFIG_DESCRIPTOR_SIZE:
                    // Get the size of the Configuration Descriptor for the current configuration
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR_SIZE:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Getting Config Descriptor size.\r\n" );
                            #endif

                            // Set up and send GET CONFIGURATION (n) DESCRIPTOR with a length of 8
                            pEP0Data[0] = USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_GET_DESCRIPTOR;
                            pEP0Data[2] = countConfigurations-1;    // USB 2.0 - range is 0 - count-1
                            pEP0Data[3] = USB_DESCRIPTOR_CONFIGURATION;
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = 8;
                            pEP0Data[7] = 0;
                            _USB_InitControlRead( pEndpointList, pEP0Data, 8, pEP0Data, 8 );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR_SIZE:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_SIZECOMPLETE:
                            // Allocate a buffer for an entry in the configuration descriptor list.
                            if ((pTemp = (BYTE *)malloc( sizeof (USB_CONFIGURATION) )) == NULL)
                            {
                                // We cannot continue.  Freeze until the device is removed.
                                _USB_SetErrorCode( USB_HOLDING_OUT_OF_MEMORY );
                                _USB_SetHoldState();
                                break;
                            }
                            // Allocate a buffer for the entire Configuration Descriptor
                            if ((((USB_CONFIGURATION *)pTemp)->descriptor = (BYTE *)malloc( ((WORD)pEP0Data[3] << 8) + (WORD)pEP0Data[2] )) == NULL)
                            {
                                // Not enough memory for the descriptor!
                                freez( pTemp );

                                // We cannot continue.  Freeze until the device is removed.
                                _USB_SetErrorCode( USB_HOLDING_OUT_OF_MEMORY );
                                _USB_SetHoldState();
                                break;
                            }
                            // Save wTotalLength
                            ((USB_CONFIGURATION_DESCRIPTOR *)((USB_CONFIGURATION *)pTemp)->descriptor)->wTotalLength =
                                    ((WORD)pEP0Data[3] << 8) + (WORD)pEP0Data[2];
                            // Put the new node at the front of the list.
                            ((USB_CONFIGURATION *)pTemp)->next = pConfigurationDescriptorList;
                            pConfigurationDescriptorList = (USB_CONFIGURATION *)pTemp;
                            // Save the configuration descriptor pointer and number
                            pCurrentConfigurationDescriptor            = ((USB_CONFIGURATION *)pTemp)->descriptor;
                            ((USB_CONFIGURATION *)pTemp)->configNumber = countConfigurations;
                            // Clean up and advance to the next state.
                            _USB_InitErrorCounters();
                            _USB_SetNextSubState();
                            break;

                        default:
                            break;
                    }
                    break;

                case SUBSTATE_GET_CONFIG_DESCRIPTOR:
                    // Get the entire Configuration Descriptor for this configuration
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_GET_CONFIG_DESCRIPTOR:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Getting Config Descriptor.\r\n" );
                            #endif

                            // Set up and send GET CONFIGURATION (n) DESCRIPTOR.
                            pEP0Data[0] = USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_GET_DESCRIPTOR;
                            pEP0Data[2] = countConfigurations-1;
                            pEP0Data[3] = USB_DESCRIPTOR_CONFIGURATION;
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = pConfigurationDescriptorList->descriptor[2];    // wTotalLength
                            pEP0Data[7] = pConfigurationDescriptorList->descriptor[3];
                            _USB_InitControlRead( pEndpointList, pEP0Data, 8, pConfigurationDescriptorList->descriptor,
                                    ((USB_CONFIGURATION_DESCRIPTOR *)pConfigurationDescriptorList->descriptor)->wTotalLength );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_GET_CONFIG_DESCRIPTOR:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_GET_CONFIG_DESCRIPTOR_COMPLETE:
                            // Clean up and advance to the next state.  Keep the data for later use.
                            _USB_InitErrorCounters();
                            countConfigurations --;
                            if (countConfigurations)
                            {
                                // There are more descriptors that we need to get.
                                usbHostState = STATE_CONFIGURING | SUBSTATE_GET_CONFIG_DESCRIPTOR_SIZE;
                            }
                            else
                            {
                                // Start configuring the device.
                                _USB_SetNextSubState();
                              }
                            break;

                        default:
                            break;
                    }
                    break;

                case SUBSTATE_SELECT_CONFIGURATION:
                    // Set the OTG configuration of the device
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SELECT_CONFIGURATION:
                            // Free the old configuration (if any)
                            _USB_FreeConfigMemory();
                            // If the configuration wasn't selected based on the VID & PID
                            if (usbDeviceInfo.currentConfiguration == 0)
                            {
                                // Search for a supported class-specific configuration.
                                pCurrentConfigurationNode = pConfigurationDescriptorList;
                                while (pCurrentConfigurationNode)
                                {
                                    pCurrentConfigurationDescriptor = pCurrentConfigurationNode->descriptor;
                                    if (_USB_ParseConfigurationDescriptor())
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // Free the memory allocated and
                                        // advance to  next configuration
                                        _USB_FreeConfigMemory();
                                        pCurrentConfigurationNode = pCurrentConfigurationNode->next;
                                    }
                                }
                            }
                            else
                            {
                                // Configuration selected by VID & PID, initialize data structures
                                pCurrentConfigurationNode = pConfigurationDescriptorList;
                                while (pCurrentConfigurationNode && pCurrentConfigurationNode->configNumber != usbDeviceInfo.currentConfiguration)
                                {
                                    pCurrentConfigurationNode = pCurrentConfigurationNode->next;
                                }
                                pCurrentConfigurationDescriptor = pCurrentConfigurationNode->descriptor;
                                if (!_USB_ParseConfigurationDescriptor())
                                {
                                    // Free the memory allocated, config attempt failed.
                                    _USB_FreeConfigMemory();
                                    pCurrentConfigurationNode = NULL;
                                }
                            }
                            // Did we fail to configure?
                            if (pCurrentConfigurationNode == NULL)
                            {
                                // Failed to find a supported configuration.
                                _USB_SetErrorCode( USB_HOLDING_UNSUPPORTED_DEVICE );
                                _USB_SetHoldState();
                            }
                            else
                            {
                                _USB_SetNextSubSubState();
                            }
                            break;

                        case SUBSUBSTATE_SEND_SET_OTG:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Determine OTG capability.\r\n" );
                            #endif

                            // If the device does not support OTG, or
                            // if the device has already been configured, bail.
                            // Otherwise, send SET FEATURE to configure it.
                            if (!usbDeviceInfo.flags.bfConfiguredOTG)
                            {
                                #ifdef DEBUG_MODE
                                    UART2PrintString( "HOST: ...OTG needs configuring.\r\n" );
                                #endif
                                usbDeviceInfo.flags.bfConfiguredOTG = 1;

                                // Send SET FEATURE
                                pEP0Data[0] = USB_SETUP_HOST_TO_DEVICE | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                                pEP0Data[1] = USB_REQUEST_SET_FEATURE;
                                if (usbDeviceInfo.flags.bfAllowHNP) // Needs to be set by the user
                                {
                                    pEP0Data[2] = OTG_FEATURE_B_HNP_ENABLE;
                                }
                                else
                                {
                                    pEP0Data[2] = OTG_FEATURE_A_HNP_SUPPORT;
                                }
                                pEP0Data[3] = 0;
                                pEP0Data[4] = 0;
                                pEP0Data[5] = 0;
                                pEP0Data[6] = 0;
                                pEP0Data[7] = 0;
                                _USB_InitControlWrite( pEndpointList, pEP0Data, 8, NULL, 0 );
                                _USB_SetNextSubSubState();
                            }
                            else
                            {
                                #ifdef DEBUG_MODE
                                    UART2PrintString( "HOST: ...No OTG.\r\n" );
                                #endif
                                _USB_SetNextSubState();
                            }
                            break;

                        case SUBSUBSTATE_WAIT_FOR_SET_OTG_DONE:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_SET_OTG_COMPLETE:
                            // Clean up and advance to the next state.
                            _USB_InitErrorCounters();
                            _USB_SetNextSubState();
                            break;

                        default:
                            break;
                    }
                    break;

                case SUBSTATE_SET_CONFIGURATION:
                    // Set the configuration to the one specified for this device
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SEND_SET_CONFIGURATION:
                            #ifdef DEBUG_MODE
                                UART2PrintString( "HOST: Set configuration.\r\n" );
                            #endif

                            // Set up and send SET CONFIGURATION.
                            pEP0Data[0] = USB_SETUP_HOST_TO_DEVICE | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE;
                            pEP0Data[1] = USB_REQUEST_SET_CONFIGURATION;
                            pEP0Data[2] = usbDeviceInfo.currentConfiguration;
                            pEP0Data[3] = 0;
                            pEP0Data[4] = 0;
                            pEP0Data[5] = 0;
                            pEP0Data[6] = 0;
                            pEP0Data[7] = 0;
                            _USB_InitControlWrite( pEndpointList, pEP0Data, 8, NULL, 0 );
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_WAIT_FOR_SET_CONFIGURATION:
                            if (pEndpointList->status.bfTransferComplete)
                            {
                                if (pEndpointList->status.bfTransferSuccessful)
                                {
                                    _USB_SetNextSubSubState();
                                }
                                else
                                {
                                    // We are here because of either a STALL or a NAK.  See if
                                    // we have retries left to try the command again or try to
                                    // enumerate again.
                                    _USB_CheckCommandAndEnumerationAttempts();
                                }
                            }
                            break;

                        case SUBSUBSTATE_SET_CONFIGURATION_COMPLETE:
                            // Clean up and advance to the next state.
                            _USB_InitErrorCounters();
                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_INIT_CLIENT_DRIVERS:
                            _USB_SetNextState();
                            // Initialize client driver(s) for this configuration.
                            if (usbDeviceInfo.flags.bfUseDeviceClientDriver)
                            {
                                temp = usbDeviceInfo.deviceClientDriver;
                                if (!usbClientDrvTable[temp].Initialize(usbDeviceInfo.deviceAddress, usbClientDrvTable[temp].flags))
                                {
                                    _USB_SetErrorCode( USB_HOLDING_CLIENT_INIT_ERROR );
                                    _USB_SetHoldState();
                                }
                            }
                            else
                            {
                                pCurrentInterface = pInterfaceList;
                                while (pCurrentInterface)
                                {
                                    temp = pCurrentInterface->clientDriver;
                                    if (!usbClientDrvTable[temp].Initialize(usbDeviceInfo.deviceAddress, usbClientDrvTable[temp].flags))
                                    {
                                        _USB_SetErrorCode( USB_HOLDING_CLIENT_INIT_ERROR );
                                        _USB_SetHoldState();
                                    }
                                    pCurrentInterface = pCurrentInterface->next;
                                }
                            }
                            break;

                        default:
                            break;
                    }
                    break;
            }
            break;

        case STATE_RUNNING:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_NORMAL_RUN:
                    break;

                case SUBSTATE_SUSPEND_AND_RESUME:
                    switch (usbHostState & SUBSUBSTATE_MASK)
                    {
                        case SUBSUBSTATE_SUSPEND:
                            // The IDLE state has already been set.  We need to wait here
                            // until the application decides to RESUME.
                            break;

                        case SUBSUBSTATE_RESUME:
                            // Issue a RESUME.
                            U1CONbits.RESUME = 1;

                            // Wait for the RESUME time.
                            numTimerInterrupts      = USB_RESUME_TIME;
                            //U1OTGIRbits.T1MSECIF    = 1;       // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIR                 = USB_INTERRUPT_T1MSECIF; // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIEbits.T1MSECIE    = 1;

                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_RESUME_WAIT:
                            // Wait here until the timer expires.
                            break;

                        case SUBSUBSTATE_RESUME_RECOVERY:
                            // Turn off RESUME.
                            U1CONbits.RESUME        = 0;

                            // Start sending SOF's, so the device doesn't go back into the SUSPEND state.
                            U1CONbits.SOFEN         = 1;

                            // Wait for the RESUME recovery time.
                            numTimerInterrupts      = USB_RESUME_RECOVERY_TIME;
                            //U1OTGIRbits.T1MSECIF    = 1;       // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIR                 = USB_INTERRUPT_T1MSECIF; // The interrupt is cleared by writing a '1' to the flag.
                            U1OTGIEbits.T1MSECIE    = 1;

                            _USB_SetNextSubSubState();
                            break;

                        case SUBSUBSTATE_RESUME_RECOVERY_WAIT:
                            // Wait here until the timer expires.
                            break;

                        case SUBSUBSTATE_RESUME_COMPLETE:
                            // Go back to normal running.
                            usbHostState = STATE_RUNNING | SUBSTATE_NORMAL_RUN;
                            break;
                    }
            }
            break;

        case STATE_HOLDING:
            switch (usbHostState & SUBSTATE_MASK)
            {
                case SUBSTATE_HOLD_INIT:
                    // We're here because we cannot communicate with the current device
                    // that is plugged in.  Turn off SOF's and all interrupts except
                    // the DETACH interrupt.
                    #ifdef DEBUG_MODE
                        UART2PrintString( "HOST: Holding.\r\n" );
                    #endif
                    U1CON               = USB_HOST_MODE_ENABLE | USB_SOF_DISABLE;                       // Turn of SOF's to cut down noise
                    U1IE                = 0;
                    U1IR                = 0xFF;
                    U1OTGIE             = 0;
                    U1OTGIR             = 0xFF;
                    U1EIE               = 0;
                    U1EIR               = 0xFF;
                    U1IEbits.DETACHIE   = 1;

                    _USB_SetNextSubState();
                    break;

                case SUBSTATE_HOLD:
                    // Hold here until a DETACH interrupt frees us.
                    break;

                default:
                    break;
            }
            break;
    }

      HOST_TRACE_STATE(HOST_LAYER,usbHostState);
}

/*******************************************************************************
Function:       void USBHostTerminateTransfer( BYTE deviceAddress, BYTE endpoint )

Notes:          None

Tested:
*******************************************************************************/

void USBHostTerminateTransfer( BYTE deviceAddress, BYTE endpoint )
{
    USB_ENDPOINT_INFO *ep;

    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return; // USB_UNKNOWN_DEVICE;
    }

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (ep->bEndpointAddress == endpoint)
        {
            ep->status.bfUserAbort          = 1;
            ep->status.bfTransferComplete   = 1;
            return;
        }
        ep = ep->next;
    }

    return;
}

/*******************************************************************************
Function:       BOOL USBHostTransferIsComplete( BYTE deviceAddress, BYTE endpoint,
                        BYTE *errorCode, DWORD *byteCount )

Notes:          bfTransferComplete, the status flags, and byte count can be
                changed in an interrupt service routine.  Therefore, we'll
                grab it first, save it locally, and then determine the rest of
                the information.  It is better to say that the transfer is not
                yet complete, since the caller will simply try again.

Tested:
*******************************************************************************/

BOOL USBHostTransferIsComplete( BYTE deviceAddress, BYTE endpoint, BYTE *errorCode,
            DWORD *byteCount )
{
    USB_ENDPOINT_INFO   *ep;
    BYTE                transferComplete;

    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return TRUE;
    }

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (ep->bEndpointAddress == endpoint)
        {
            // Save off the Transfer Complete status.  That way, we won't
            // load up bad values and then say the transfer is complete.
            transferComplete = ep->status.bfTransferComplete;

            // Set up error code.  This is only valid if the transfer is complete.
            if (ep->status.bfTransferSuccessful)
            {
                *errorCode = USB_SUCCESS;
                *byteCount = ep->dataCount;
            }
            else if (ep->status.bfStalled)
            {
                *errorCode = USB_ENDPOINT_STALLED;
            }
            else if (ep->status.bfError)
            {
                *errorCode = ep->bErrorCode;
            }
            else
            {
                *errorCode = USB_ENDPOINT_UNRESOLVED_STATE;
            }

            return transferComplete;
        }
        ep = ep->next;
    }

    // The endpoint was not found.  Return TRUE so we can return a valid error code.
    *errorCode = USB_ENDPOINT_NOT_FOUND;
    return TRUE;
}

/*******************************************************************************
Function:       BYTE  USBHostVbusEvent( USB_EVENT vbusEvent, BYTE deviceNumber,
                                        BYTE portNumber)

Notes:          None

Tested:
*******************************************************************************/
BYTE  USBHostVbusEvent(USB_EVENT vbusEvent, BYTE deviceNumber, BYTE portNumber)
{
    if (vbusEvent    == EVENT_VBUS_OVERCURRENT &&
        deviceNumber == USB_ROOT_HUB           &&
        portNumber   == 0                        )
    {
        USBHostShutdown();
        return USB_SUCCESS;
    }

    return USB_ILLEGAL_REQUEST;
}


/*******************************************************************************
Function:       BYTE USBHostWrite( BYTE deviceAddress, BYTE endpoint, BYTE *data,
                        DWORD size )

Notes:          None

Tested:
*******************************************************************************/

BYTE USBHostWrite( BYTE deviceAddress, BYTE endpoint, BYTE *data, DWORD size )
{
    USB_ENDPOINT_INFO *ep;

    // Find the required device
    if (deviceAddress != usbDeviceInfo.deviceAddress)
    {
        return USB_UNKNOWN_DEVICE;
    }

    // If we are not in a normal user running state, we cannot do this.
    if ((usbHostState & STATE_MASK) != STATE_RUNNING)
    {
        return USB_INVALID_STATE;
    }

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (ep->bEndpointAddress == endpoint)
        {
            if (ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_CONTROL)
            {
                // Must not be a control endpoint.
                return USB_ENDPOINT_ILLEGAL_TYPE;
            }

            if (ep->bEndpointAddress & 0x80)
            {
                // Trying to do an OUT with an IN endpoint.
                return USB_ENDPOINT_ILLEGAL_DIRECTION;
            }

            if (ep->status.bfStalled)
            {
                // The endpoint is stalled.  It must be restarted before a write
                // can be performed.
                return USB_ENDPOINT_STALLED;
            }

            if (ep->status.bfError)
            {
                // The endpoint has errored.  The error must be cleared before a
                // write can be performed.
                return USB_ENDPOINT_ERROR;
            }

            if (!ep->status.bfTransferComplete)
            {
                // We are already processing a request for this endpoint.
                return USB_ENDPOINT_BUSY;
            }

            _USB_InitWrite( ep, data, size );

            return USB_SUCCESS;
        }
        ep = ep->next;
    }
    return USB_ENDPOINT_NOT_FOUND;   // Endpoint not found
}


// *****************************************************************************
// *****************************************************************************
// Section: Internal Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BOOL _USB_FindDeviceLevelClientDriver( void )

Precondition:   usbHostState == STATE_ATTACHED|SUBSTATE_VALIDATE_VID_PID
                usbTPL must be define by the application.

Overview:       This function searches the TPL to try to find a device-level
                client driver.

Input:          None - None

Output:         None - None

Return:         TRUE    - Client driver found
                FALSE   - Client driver not found

Notes:          If successful, it preserves the client's index from the client
                driver table and sets flags indicating that the device should
                use a single client driver.

Tested:
*******************************************************************************/

BOOL _USB_FindDeviceLevelClientDriver( void )
{
    WORD                   i;
    USB_DEVICE_DESCRIPTOR *pDesc = (USB_DEVICE_DESCRIPTOR *)pDeviceDescriptor;

    // Scan TPL
    i = 0;
    usbDeviceInfo.flags.bfUseDeviceClientDriver = 0;
    while (i < NUM_TPL_ENTRIES)
    {
        if (usbTPL[i].flags.bfIsClassDriver)
        {
            // Check for a device-class client driver
            if ((usbTPL[i].device.bClass    == pDesc->bDeviceClass   ) &&
                (usbTPL[i].device.bSubClass == pDesc->bDeviceSubClass) &&
                (usbTPL[i].device.bProtocol == pDesc->bDeviceProtocol)   )
            {
                usbDeviceInfo.flags.bfUseDeviceClientDriver = 1;
            }
        }
        else
        {
            // Check for a device-specific client driver by VID & PID
            if ((usbTPL[i].device.idVendor  == pDesc->idVendor ) &&
                (usbTPL[i].device.idProduct == pDesc->idProduct)   )
            {
                usbDeviceInfo.flags.bfUseDeviceClientDriver = 1;
            }
        }

        if (usbDeviceInfo.flags.bfUseDeviceClientDriver)
        {
            // Save client driver info
            usbDeviceInfo.deviceClientDriver = usbTPL[i].ClientDriver;

            // Select configuration if it is given in the TPL
            if (usbTPL[i].flags.bfSetConfiguration)
            {
                usbDeviceInfo.currentConfiguration = usbTPL[i].bConfiguration;
            }

            return TRUE;
        }

        i++;
    }

    return FALSE;
}


/*******************************************************************************
Function:       _USB_FindClassDriver

Precondition:   usbTPL must be define by the application.

Overview:       This routine scans the TPL table looking for the entry with
                the given class, subclass, and protocol values.

Input:          bClass      - The class of the desired entry
                bSubClass   - The subclass of the desired entry
                bProtocol   - The protocol of the desired entry

Output:         pbClientDrv - Index to the client driver in the client driver
                              table.

Return:         TRUE    - A class driver was found.
                FALSE   - A class driver was not found.

Notes:

Tested:
*******************************************************************************/

BOOL _USB_FindClassDriver( BYTE bClass, BYTE bSubClass, BYTE bProtocol, BYTE *pbClientDrv )
{
    int i;

    i = 0;
    while (i < NUM_TPL_ENTRIES)
    {
        if ((usbTPL[i].flags.bfIsClassDriver == 1        ) &&
            (usbTPL[i].device.bClass         == bClass   ) &&
            (usbTPL[i].device.bSubClass      == bSubClass) &&
            (usbTPL[i].device.bProtocol      == bProtocol)   )
        {
            *pbClientDrv = usbTPL[i].ClientDriver;
            #ifdef DEBUG_MODE
                UART2PrintString( "HOST: Client driver found.\r\n" );
            #endif
            return TRUE;
        }
        i++;
    }

    #ifdef DEBUG_MODE
        UART2PrintString( "HOST: Client driver NOT found.\r\n" );
    #endif
    return FALSE;

} // _USB_FindClassDriver


/*******************************************************************************
Function:       _USB_FindInterface()

Precondition:   The

Overview:       This routine scans the interface linked list and returns a
                pointer to the node identified by the interface and alt setting.

Input:          bInterface  - Interface number
                bAltSetting - Interface alternate setting number

Return:         USB_INTERFACE_INFO *  - Pointer to the interface linked list node.

Notes:          None

Tested:
*******************************************************************************/

USB_INTERFACE_INFO * _USB_FindInterface ( BYTE bInterface, BYTE bAltSetting )
{
    USB_INTERFACE_INFO *pCurIntf = pInterfaceList;

    while (pCurIntf)
    {
        if (pCurIntf->interface           == bInterface &&
            pCurIntf->interfaceAltSetting == bAltSetting  )
        {
            return pCurIntf;
        }
    }

    return NULL;

} // _USB_FindInterface


/*******************************************************************************
Function:       _USB_NotifyClients()

Precondition:   None

Overview:       This routine notifies all active client drivers for the given
                device of the given event.

Input:          BYTE address        - Address of the device generating the event
                USB_EVENT event     - Event ID
                void *data          - Pointer to event data
                unsigned int size   - Size of data pointed to by data

Output:         None - None

Return:         None - None

Notes:          None

Tested:
*******************************************************************************/

void _USB_NotifyClients( BYTE address, USB_EVENT event, void *data, unsigned int size )
{
    // To Do: Update this for multiple devices.

    if (usbDeviceInfo.flags.bfUseDeviceClientDriver)
    {
        usbClientDrvTable[usbDeviceInfo.deviceClientDriver].EventHandler(address, event, data, size);
    }
    else
    {
        USB_INTERFACE_INFO *pInterface = pInterfaceList;

        while (pInterface != NULL)
        {
            usbClientDrvTable[pInterface->clientDriver].EventHandler(address, event, data, size);
            pInterface = pInterface->next;
        }
    }
} // _USB_NotifyClients


/*******************************************************************************
Function:       void _USB_CheckCommandAndEnumerationAttempts( void )

Precondition:   usbHostState != STATE_RUNNING

Overview:       This function is called when we've received a STALL or a NAK
                when trying to enumerate.  We allow so many attempts at each
                command, and so many attempts at enumeration.  If the command
                fails and there are more command attempts, we try the command
                again.  If the command fails and there are more enumeration
                attempts, we reset and try to enumerate again.  Otherwise, we
                go to the holding state.

Input:          None - None

Output:         None - None

Return:         None - None

Notes:          None

Tested:
*******************************************************************************/

void _USB_CheckCommandAndEnumerationAttempts( void )
{
    #ifdef DEBUG_MODE
        UART2PutChar( '=' );
    #endif

    // Clear the error and stall flags.  A stall here does not require
    // host intervention to clear.
    pCurrentEndpoint->status.bfError    = 0;
    pCurrentEndpoint->status.bfStalled  = 0;

    numCommandTries --;
    if (numCommandTries != 0)
    {
        // We still have retries left on this command.  Try again.
        usbHostState &= ~SUBSUBSTATE_MASK;
    }
    else
    {
        // This command has timed out.
        // We are enumerating.  See if we can try to enumerate again.
        numEnumerationTries --;
        if (numEnumerationTries != 0)
        {
            // We still have retries left to try to enumerate.  Reset and try again.
            usbHostState = STATE_ATTACHED | SUBSTATE_RESET_DEVICE;
        }
        else
        {
            // Give up.  The device is not responding properly.
            _USB_SetErrorCode( USB_CANNOT_ENUMERATE );
            _USB_SetHoldState();
        }
    }
}


/*******************************************************************************
Function:       void _USB_FindNextToken( void )

Precondition:   None

Overview:       This function determines the next token to send of all current
                pending transfers.

Input:          None - None

Output:         None - None

Return:         TRUE    - A token was sent
                FALSE   - No token was found to send, so the routine can be
                            called again.

Notes:          This routine is only called from an interrupt handler, either
                SOFIF or TRNIF.

                The TOKBUSY bit will read 1 as soon as the U1TOK register is
                written.  So we do not have to worry about the case where this
                routine is called twice during the SOF threshold time.

Tested:
*******************************************************************************/

void _USB_FindNextToken( void )
{
    USB_ENDPOINT_INFO   *ep;
    BYTE                countBulkTransactions;
    BOOL                illegalState = FALSE;

    // If the device is suspended or resuming, do not send any tokens.  We will
    // send the next token on an SOF interrupt after the resume recovery time
    // has expired.
    if ((usbHostState & (SUBSTATE_MASK | SUBSUBSTATE_MASK)) == (STATE_RUNNING | SUBSTATE_SUSPEND_AND_RESUME))
    {
        return;
    }

    // If we are currently sending a token, we cannot do anything.  We will come
    // back in here when we get either the Token Done or the Start of Frame interrupt.
    if (U1CONbits.TOKBUSY)
    {
        return;
    }

    // We will handle control transfers first.  We only allow one control
    // transfer per frame.
    if (!usbBusInfo.flags.bfControlTransfersDone)
    {
        // Look for any control transfers.
        ep = pEndpointList;
        while (ep != NULL)
        {
            if ((ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_CONTROL) &&
                !ep->status.bfTransferComplete)
            {
                // This endpoint is ready for servicing.
                pCurrentEndpoint = ep;
                switch (ep->transferState & TSTATE_MASK)
                {
                    case TSTATE_CONTROL_NO_DATA:
                        switch (ep->transferState & TSUBSTATE_MASK)
                        {
                            case TSUBSTATE_CONTROL_NO_DATA_SETUP:
                                _USB_SetDATA01( DTS_DATA0 );
                                _USB_SetBDT( USB_TOKEN_SETUP );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_SETUP );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_NO_DATA_ACK:
                                pCurrentEndpoint->dataCountMax = pCurrentEndpoint->dataCount;
                                _USB_SetDATA01( DTS_DATA1 );
                                _USB_SetBDT( USB_TOKEN_IN );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_NO_DATA_COMPLETE:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_TRANSFER;
                                        data->TransferData.dataCount        = ep->dataCount;
                                        data->TransferData.pUserData        = ep->pUserData;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = USB_SUCCESS;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                        break;

                            case TSUBSTATE_ERROR:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_BUS_ERROR;
                                        data->TransferData.dataCount        = 0;
                                        data->TransferData.pUserData        = NULL;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = ep->bErrorCode;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }
                        break;

                    case TSTATE_CONTROL_READ:
                        switch (ep->transferState & TSUBSTATE_MASK)
                        {
                            case TSUBSTATE_CONTROL_READ_SETUP:
                                _USB_SetDATA01( DTS_DATA0 );
                                _USB_SetBDT( USB_TOKEN_SETUP );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_SETUP );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_READ_DATA:
                                _USB_SetBDT( USB_TOKEN_IN );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_READ_ACK:
                                pCurrentEndpoint->dataCountMax = pCurrentEndpoint->dataCount;
                                _USB_SetDATA01( DTS_DATA1 );
                                _USB_SetBDT( USB_TOKEN_OUT );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_OUT );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_READ_COMPLETE:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_TRANSFER;
                                        data->TransferData.dataCount        = ep->dataCount;
                                        data->TransferData.pUserData        = ep->pUserData;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = USB_SUCCESS;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                                break;

                            case TSUBSTATE_ERROR:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_BUS_ERROR;
                                        data->TransferData.dataCount        = 0;
                                        data->TransferData.pUserData        = NULL;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = ep->bErrorCode;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }
                        break;

                    case TSTATE_CONTROL_WRITE:
                        switch (ep->transferState & TSUBSTATE_MASK)
                        {
                            case TSUBSTATE_CONTROL_WRITE_SETUP:
                                _USB_SetDATA01( DTS_DATA0 );
                                _USB_SetBDT( USB_TOKEN_SETUP );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_SETUP );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_WRITE_DATA:
                                _USB_SetBDT( USB_TOKEN_OUT );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_OUT );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_WRITE_ACK:
                                pCurrentEndpoint->dataCountMax = pCurrentEndpoint->dataCount;
                                _USB_SetDATA01( DTS_DATA1 );
                                _USB_SetBDT( USB_TOKEN_IN );
                                _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                #ifdef ONE_CONTROL_TRANSACTION_PER_FRAME
                                    usbBusInfo.flags.bfControlTransfersDone = 1; // Only one control transfer per frame.
                                #endif
                                return;
                                break;

                            case TSUBSTATE_CONTROL_WRITE_COMPLETE:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_TRANSFER;
                                        data->TransferData.dataCount        = ep->dataCount;
                                        data->TransferData.pUserData        = ep->pUserData;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = USB_SUCCESS;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                                break;

                            case TSUBSTATE_ERROR:
                                ep->transferState               = TSTATE_IDLE;
                                ep->status.bfTransferComplete   = 1;
                                #if defined( USB_ENABLE_TRANSFER_EVENT )
                                    if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                    {
                                        USB_EVENT_DATA *data;

                                        data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                        data->event = EVENT_BUS_ERROR;
                                        data->TransferData.dataCount        = 0;
                                        data->TransferData.pUserData        = NULL;
                                        data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                        data->TransferData.bErrorCode       = ep->bErrorCode;
                                        data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                    }
                                    else
                                    {
                                        ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                    }
                                #endif
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }
                        break;

                    default:
                        illegalState = TRUE;
                }

                if (illegalState)
                {
                    // We should never use this, but in case we do, put the endpoint
                    // in a recoverable state.
                    ep->transferState               = TSTATE_IDLE;
                    ep->status.bfTransferComplete   = 1;
                }
            }
            HOST_TRACE_STATE(HOST_LAYER,usbHostState);
            ep = ep->next;
        }

        // If we've gone through all the endpoints, we do not have any more control transfers.
        usbBusInfo.flags.bfControlTransfersDone = 1;
    }

    #ifdef USB_SUPPORT_ISOCHRONOUS_TRANSFERS
        // Next, we will handle isochronous transfers.  We must be careful with
        // these.  The maximum packet size for an isochronous transfer is 1023
        // bytes, so we cannot use the threshold register (U1SOF) to ensure that
        // we do not write too many tokens during a frame.  Instead, we must count
        // the number of bytes we are sending and stop sending isochronous
        // transfers when we reach that limit.

        // TODO: Implement scheduling by using usbBusInfo.dBytesSentInFrame

        if (!usbBusInfo.flags.bfIsochronousTransfersDone)
        {
            // Look for any isochronous operations.
            ep = pEndpointList;
            while (ep != NULL)
            {
                if (ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS)
                {
                    if (ep->status.bfTransferComplete)
                    {
                        // The endpoint doesn't need servicing.  If the interval count
                        // has reached 0 and the user has not initiated another transaction,
                        // reset the interval count for the next interval.
                        if (ep->wIntervalCount == 0)
                        {
                            // Reset the interval count for the next packet.
                            ep->wIntervalCount = ep->wInterval;
                        }
                    }
                    else
                    {
                        // This endpoint might be ready for servicing.
                        pCurrentEndpoint = ep;

                        switch (ep->transferState & TSTATE_MASK)
                        {
                            case TSTATE_ISOCHRONOUS_READ:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_ISOCHRONOUS_READ_DATA:
                                        if (ep->wIntervalCount == 0)
                                        {
                                            // Reset the interval count for the next packet.
                                            ep->wIntervalCount = ep->wInterval;

                                            _USB_SetDATA01( DTS_DATA0 );    // Always DATA0 for isochronous
                                            _USB_SetBDT( USB_TOKEN_IN );
                                            _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                            return;
                                        }
                                        break;

                                    case TSUBSTATE_ISOCHRONOUS_READ_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            case TSTATE_ISOCHRONOUS_WRITE:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_ISOCHRONOUS_WRITE_DATA:
                                        if (ep->wIntervalCount == 0)
                                        {
                                            // Reset the interval count for the next packet.
                                            ep->wIntervalCount = ep->wInterval;

                                            _USB_SetDATA01( DTS_DATA0 );    // Always DATA0 for isochronous
                                            _USB_SetBDT( USB_TOKEN_OUT );
                                            _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_OUT );
                                            return;
                                        }
                                        break;

                                    case TSUBSTATE_ISOCHRONOUS_WRITE_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }

                        if (illegalState)
                        {
                            // We should never use this, but in case we do, put the endpoint
                            // in a recoverable state.
                            ep->transferState               = TSTATE_IDLE;
                            ep->status.bfTransferComplete   = 1;
                        }
                    }
                }
                HOST_TRACE_STATE(HOST_LAYER,usbHostState);
                ep = ep->next;
            }

            // If we've gone through all the endpoints, we do not have any more isochronous transfers.
            usbBusInfo.flags.bfIsochronousTransfersDone = 1;
        }
    #endif

    #ifdef USB_SUPPORT_INTERRUPT_TRANSFERS
        if (!usbBusInfo.flags.bfInterruptTransfersDone)
        {
            // Look for any interrupt operations.
            ep = pEndpointList;
            while (ep != NULL)
            {
                if (ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT)
                {
                    if (ep->status.bfTransferComplete)
                    {
                        // The endpoint doesn't need servicing.  If the interval count
                        // has reached 0 and the user has not initiated another transaction,
                        // reset the interval count for the next interval.
                        if (ep->wIntervalCount == 0)
                        {
                            // Reset the interval count for the next packet.
                            ep->wIntervalCount = ep->wInterval;
                        }
                    }
                    else
                    {
                        // This endpoint might be ready for servicing.
                        pCurrentEndpoint = ep;

                        switch (ep->transferState & TSTATE_MASK)
                        {
                            case TSTATE_INTERRUPT_READ:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_INTERRUPT_READ_DATA:
                                        if (ep->wIntervalCount == 0)
                                        {
                                            // Reset the interval count for the next packet.
                                            ep->wIntervalCount = ep->wInterval;

                                            _USB_SetBDT( USB_TOKEN_IN );
                                            _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                            return;
                                        }
                                        break;

                                    case TSUBSTATE_INTERRUPT_READ_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            case TSTATE_INTERRUPT_WRITE:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_INTERRUPT_WRITE_DATA:
                                        if (ep->wIntervalCount == 0)
                                        {
                                            // Reset the interval count for the next packet.
                                            ep->wIntervalCount = ep->wInterval;

                                            _USB_SetBDT( USB_TOKEN_OUT );
                                            _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_OUT );
                                            return;
                                        }
                                        break;

                                    case TSUBSTATE_INTERRUPT_WRITE_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }

                        if (illegalState)
                        {
                            // We should never use this, but in case we do, put the endpoint
                            // in a recoverable state.
                            ep->status.bfTransferComplete   = 1;
                            ep->transferState               = TSTATE_IDLE;
                        }
                    }
                }
                HOST_TRACE_STATE(HOST_LAYER,usbHostState);
                ep = ep->next;
            }

            // If we've gone through all the endpoints, we do not have any more interrupt transfers.
            usbBusInfo.flags.bfInterruptTransfersDone = 1;
        }
    #endif

    #ifdef USB_SUPPORT_BULK_TRANSFERS
#ifdef ALLOW_MULTIPLE_BULK_TRANSACTIONS_PER_FRAME
TryBulk:
#endif

        if (!usbBusInfo.flags.bfBulkTransfersDone)
        {
            #ifndef ALLOW_MULTIPLE_BULK_TRANSACTIONS_PER_FRAME
                // Only go through this section once if we are not allowing multiple transactions
                // per frame.
                usbBusInfo.flags.bfBulkTransfersDone = 1;
            #endif

            // Look for any bulk operations.  Try to service all pending requests within the frame.
            ep = pEndpointList;
            countBulkTransactions = 0;

            while (ep != NULL)
            {
#ifdef ALLOW_MULTIPLE_NAKS_PER_FRAME
                if ((ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK) &&
                    !ep->status.bfTransferComplete)
#else
                if ((ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_BULK) &&
                    !ep->status.bfTransferComplete &&
                    !ep->status.bfLastTransferNAKd)
#endif
                {
                    countBulkTransactions ++;
                    if (countBulkTransactions > usbBusInfo.lastBulkTransaction)
                    {
                        // This endpoint is ready for servicing.
                        pCurrentEndpoint = ep;
                        usbBusInfo.lastBulkTransaction = countBulkTransactions;

                        switch (ep->transferState & TSTATE_MASK)
                        {
                            case TSTATE_BULK_READ:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_BULK_READ_DATA:
                                        _USB_SetBDT( USB_TOKEN_IN );
                                        _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_IN );
                                        return;
                                        break;

                                    case TSUBSTATE_BULK_READ_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            case TSTATE_BULK_WRITE:
                                switch (ep->transferState & TSUBSTATE_MASK)
                                {
                                    case TSUBSTATE_BULK_WRITE_DATA:
                                        _USB_SetBDT( USB_TOKEN_OUT );
                                        _USB_SendToken( ep->bEndpointAddress, USB_TOKEN_OUT );
                                        return;
                                        break;

                                    case TSUBSTATE_BULK_WRITE_COMPLETE:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_TRANSFER;
                                                data->TransferData.dataCount        = ep->dataCount;
                                                data->TransferData.pUserData        = ep->pUserData;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = USB_SUCCESS;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    case TSUBSTATE_ERROR:
                                        ep->transferState               = TSTATE_IDLE;
                                        ep->status.bfTransferComplete   = 1;
                                        #if defined( USB_ENABLE_TRANSFER_EVENT )
                                            if (StructQueueIsNotFull(&usbEventQueue, USB_EVENT_QUEUE_DEPTH))
                                            {
                                                USB_EVENT_DATA *data;

                                                data = StructQueueAdd(&usbEventQueue, USB_EVENT_QUEUE_DEPTH);
                                                data->event = EVENT_BUS_ERROR;
                                                data->TransferData.dataCount        = 0;
                                                data->TransferData.pUserData        = NULL;
                                                data->TransferData.bEndpointAddress = ep->bEndpointAddress;
                                                data->TransferData.bErrorCode       = ep->bErrorCode;
                                                data->TransferData.bmAttributes.val = ep->bmAttributes.val;
                                            }
                                            else
                                            {
                                                ep->bmAttributes.val = USB_EVENT_QUEUE_FULL;
                                            }
                                        #endif
                                        break;

                                    default:
                                        illegalState = TRUE;
                                        break;
                                }
                                break;

                            default:
                                illegalState = TRUE;
                                break;
                        }

                        if (illegalState)
                        {
                            // We should never use this, but in case we do, put the endpoint
                            // in a recoverable state.
                            ep->transferState               = TSTATE_IDLE;
                            ep->status.bfTransferComplete   = 1;
                        }
                    }
                }
                HOST_TRACE_STATE(HOST_LAYER,usbHostState);
                ep = ep->next;
            }

            // We've gone through all the bulk transactions, but we have time for more.
            // If we have any bulk transactions, go back to the beginning of the list
            // and start over.
            #ifdef ALLOW_MULTIPLE_BULK_TRANSACTIONS_PER_FRAME
                if (countBulkTransactions)
                {
                    usbBusInfo.lastBulkTransaction = 0;
                    goto TryBulk;

                }
            #endif

            // If we've gone through all the endpoints, we do not have any more bulk transfers.
            usbBusInfo.flags.bfBulkTransfersDone = 1;
        }
    #endif

    return;
}

/*******************************************************************************
Function:       void _USB_FreeConfigMemory( void )

Precondition:   None

Overview:       This function frees the interface and endpoint lists associated
                with a configuration.  (EP 0 block is retained.)

Input:          None - None

Output:         None - None

Return:         None - None

Notes:

Tested:
*******************************************************************************/

void _USB_FreeConfigMemory( void )
{
    BYTE    *pTemp;

    while (pInterfaceList != NULL)
    {
        pTemp = (BYTE *)pInterfaceList->next;
        free(pInterfaceList);
        pInterfaceList = (USB_INTERFACE_INFO *)pTemp;
    }

    if (pEndpointList != NULL) // Should not be null!
    {
        while (pEndpointList->next != NULL) // Leave EP0 intact
        {
            pTemp = (BYTE *)pEndpointList->next->next;
            free( pEndpointList->next );
            pEndpointList->next = (USB_ENDPOINT_INFO *)pTemp;
        }
    }
    pCurrentEndpoint = pEndpointList;

} // _USB_FreeConfigMemory

/*******************************************************************************
Function:       void _USB_FreeMemory( void )

Precondition:   None

Overview:       This function frees all memory that can be freed.  Only the EP0
                information block is retained.

Input:          None - None

Output:         None - None

Return:         None - None

Notes:

Tested:
*******************************************************************************/

void _USB_FreeMemory( void )
{
    BYTE    *pTemp;

    while (pConfigurationDescriptorList != NULL)
    {
        pTemp = (BYTE *)pConfigurationDescriptorList->next;
        free( pConfigurationDescriptorList->descriptor );
        free( pConfigurationDescriptorList );
        pConfigurationDescriptorList = (USB_CONFIGURATION *)pTemp;
    }
    if (pDeviceDescriptor != NULL)
    {
        freez( pDeviceDescriptor );
    }
    if (pEP0Data != NULL)
    {
        freez( pEP0Data );
    }

    _USB_FreeConfigMemory();

}

/*******************************************************************************
Function:       void _USB_InitControlRead( USB_ENDPOINT_INFO *pEndpoint,
                    BYTE *pControlData, WORD controlSize, BYTE *pData, WORD size )

Precondition:   All error checking must be done prior to calling this function.

Overview:       This function sets up the endpoint information for a control
                (SETUP) transfer that will read information.

Input:          USB_ENDPOINT_INFO *pEndpoint    - Points to the desired endpoint
                                                  in the endpoint information list.
                BYTE *pControlData              - Points to the SETUP message.
                WORD controlSize                - Size of the SETUP message.
                BYTE *pData                     - Points to where the read data
                                                  is to be stored.
                WORD size                       - Number of data bytes to read.

Output:         None - None

Return:         None - None

Notes:          Since endpoint servicing is interrupt driven, the
                bfTransferComplete flag must be set last.

Tested:
*******************************************************************************/

void _USB_InitControlRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData, WORD controlSize,
                            BYTE *pData, WORD size )
{
    pEndpoint->status.bfStalled             = 0;
    pEndpoint->status.bfError               = 0;
    pEndpoint->status.bfUserAbort           = 0;
    pEndpoint->status.bfTransferSuccessful  = 0;
    pEndpoint->status.bfErrorCount          = 0;
    pEndpoint->status.bfLastTransferNAKd    = 0;
    pEndpoint->pUserData                    = pData;
    pEndpoint->dataCount                    = 0;
    pEndpoint->dataCountMax                 = size;
    pEndpoint->countNAKs                    = 0;

    pEndpoint->pUserDataSETUP               = pControlData;
    pEndpoint->dataCountMaxSETUP            = controlSize;
    pEndpoint->transferState                = TSTATE_CONTROL_READ;

    // Set the flag last so all the parameters are set for an interrupt.
    pEndpoint->status.bfTransferComplete    = 0;
}

/*******************************************************************************
Function:       void _USB_InitControlWrite( USB_ENDPOINT_INFO *pEndpoint,
                    BYTE *pControlData, WORD controlSize, BYTE *pData, WORD size )

Precondition:   All error checking must be done prior to calling this function.

Overview:       This function sets up the endpoint information for a control
                (SETUP) transfer that will write information.

Input:          USB_ENDPOINT_INFO *pEndpoint    - Points to the desired endpoint
                                                  in the endpoint information list.
                BYTE *pControlData              - Points to the SETUP message.
                WORD controlSize                - Size of the SETUP message.
                BYTE *pData                     - Points to where the write data
                                                  is to be stored.
                WORD size                       - Number of data bytes to write.

Output:         None - None

Return:         None

Notes:          Since endpoint servicing is interrupt driven, the
                bfTransferComplete flag must be set last.

Tested:
*******************************************************************************/

void _USB_InitControlWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pControlData,
                WORD controlSize, BYTE *pData, WORD size )
{
    pEndpoint->status.bfStalled             = 0;
    pEndpoint->status.bfError               = 0;
    pEndpoint->status.bfUserAbort           = 0;
    pEndpoint->status.bfTransferSuccessful  = 0;
    pEndpoint->status.bfErrorCount          = 0;
    pEndpoint->status.bfLastTransferNAKd    = 0;
    pEndpoint->pUserData                    = pData;
    pEndpoint->dataCount                    = 0;
    pEndpoint->dataCountMax                 = size;
    pEndpoint->countNAKs                    = 0;

    pEndpoint->pUserDataSETUP               = pControlData;
    pEndpoint->dataCountMaxSETUP            = controlSize;

    if (size == 0)
    {
        pEndpoint->transferState    = TSTATE_CONTROL_NO_DATA;
    }
    else
    {
        pEndpoint->transferState    = TSTATE_CONTROL_WRITE;
    }

    // Set the flag last so all the parameters are set for an interrupt.
    pEndpoint->status.bfTransferComplete    = 0;
}

/*******************************************************************************
Function:       void _USB_InitRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData,
                        WORD size )

Precondition:   All error checking must be done prior to calling this function.

Overview:       This function sets up the endpoint information for an interrupt,
                isochronous, or bulk read.

Input:          USB_ENDPOINT_INFO *pEndpoint  - Points to the desired endpoint
                                                in the endpoint information list.
                BYTE *pData                   - Points to where the data is to
                                                be stored.
                WORD size                     - Number of data bytes to read.

Output:         None - None

Return:         None - None

Notes:          * Control reads should use the routine _USB_InitControlRead().
                Since endpoint servicing is interrupt driven, the
                bfTransferComplete flag must be set last.

                * For interrupt and isochronous endpoints, we let the interval
                count free run.  The transaction will begin when the interval
                count reaches 0.

Tested:
*******************************************************************************/

void _USB_InitRead( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size )
{
    pEndpoint->status.bfUserAbort           = 0;
    pEndpoint->status.bfTransferSuccessful  = 0;
    pEndpoint->status.bfErrorCount          = 0;
    pEndpoint->status.bfLastTransferNAKd    = 0;
    pEndpoint->pUserData                    = pData;
    pEndpoint->dataCount                    = 0;
    pEndpoint->dataCountMax                 = size;
    pEndpoint->countNAKs                    = 0;

    if (pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT)
    {
        pEndpoint->transferState            = TSTATE_INTERRUPT_READ;
    }
    else if (pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS)
    {
        pEndpoint->transferState            = TSTATE_ISOCHRONOUS_READ;
    }
    else // Bulk
    {
        pEndpoint->transferState            = TSTATE_BULK_READ;
    }

    // Set the flag last so all the parameters are set for an interrupt.
    pEndpoint->status.bfTransferComplete    = 0;
}

/*******************************************************************************
Function:       void _USB_InitWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData,
                            WORD size )

Precondition:   All error checking must be done prior to calling this function.

Overview:       This function sets up the endpoint information for an interrupt,
                isochronous, or bulk  write.

Input:          USB_ENDPOINT_INFO *pEndpoint  - Points to the desired endpoint
                                                in the endpoint information list.
                BYTE *pData                   - Points to where the data to send
                                                is stored.
                WORD size                     - Number of data bytes to write.

Output:         None - None

Return:         None - None

Notes:          * Control writes should use the routine _USB_InitControlWrite().
                Since endpoint servicing is interrupt driven, the
                bfTransferComplete flag must be set last.

                * For interrupt and isochronous endpoints, we let the interval
                count free run.  The transaction will begin when the interval
                count reaches 0.

Tested:
*******************************************************************************/

void _USB_InitWrite( USB_ENDPOINT_INFO *pEndpoint, BYTE *pData, WORD size )
{
    pEndpoint->status.bfUserAbort           = 0;
    pEndpoint->status.bfTransferSuccessful  = 0;
    pEndpoint->status.bfErrorCount          = 0;
    pEndpoint->status.bfLastTransferNAKd    = 0;
    pEndpoint->pUserData                    = pData;
    pEndpoint->dataCount                    = 0;
    pEndpoint->dataCountMax                 = size;
    pEndpoint->countNAKs                    = 0;

    if (pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT)
    {
        pEndpoint->transferState            = TSTATE_INTERRUPT_WRITE;
    }
    else if (pEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS)
    {
        pEndpoint->transferState            = TSTATE_ISOCHRONOUS_WRITE;
    }
    else // Bulk
    {
        pEndpoint->transferState            = TSTATE_BULK_WRITE;
    }

    // Set the flag last so all the parameters are set for an interrupt.
    pEndpoint->status.bfTransferComplete    = 0;
}

/*******************************************************************************
Function:       BOOL _USB_ParseConfigurationDescriptor( void )

Precondition:   pCurrentConfigurationDescriptor points to a valid Configuration
                Descriptor, which contains the endpoint descriptors.  The
                current interface and the current interface settings must be set
                up in usbDeviceInfo.

Overview:       This function parses all the endpoint descriptors for the
                required setting of the required interface and sets up the
                internal endpoint information.

Input:          None - None

Output:         None - None

Return:         TRUE    - Successful
                FALSE   - Configuration not supported*.

Notes:          * This function also automatically resets all endpoints (except
                endpoint 0) to DATA0, so _USB_ResetDATA0 does not have to be
                called.

                * If the configuration is not supported, the caller will need to
                clean up, freeing memory by calling _USB_FreeConfigMemory.

TODO: Add checks for descriptors that are shorter than the expected length?

Tested:         3-Apr-2007
*******************************************************************************/

BOOL _USB_ParseConfigurationDescriptor( void )
{
    BYTE                bAlternateSetting;
    BYTE                bDescriptorType;
    BYTE                bInterfaceNumber;
    BYTE                bLength;
    BYTE                bNumEndpoints;
    BYTE                bNumInterfaces;
    BYTE                bMaxPower;
    BYTE                Class;
    BYTE                SubClass;
    BYTE                Protocol;
    BYTE                ClientDriver;
    WORD                wTotalLength;

    WORD                index;
    BYTE                currentEndpoint;
    BYTE                currentInterface;
    BYTE                currentConfiguration;
    BYTE                currentAlternateSetting;
    USB_ENDPOINT_INFO  *newEndpointInfo;
    USB_INTERFACE_INFO *newInterfaceInfo;
    BYTE               *ptr;

    // Prime the loops.
    index                   = 0;
    ptr                     = pCurrentConfigurationDescriptor;
    currentInterface        = 0;
    currentAlternateSetting = 0;

    // Assume no OTG support (determine otherwise, below).
    usbDeviceInfo.flags.bfSupportsOTG   = 0;
    usbDeviceInfo.flags.bfConfiguredOTG = 1;

    // Load up the values from the Configuration Descriptor
    bLength              = *ptr++;
    bDescriptorType      = *ptr++;
    wTotalLength         = *ptr++;           // In case these are not word aligned
    wTotalLength        += (*ptr++) << 8;
    bNumInterfaces       = *ptr++;
    currentConfiguration = *ptr++;  // bConfigurationValue
                            ptr++;  // iConfiguration
                            ptr++;  // bmAttributes
    bMaxPower            = *ptr;

    // Check Max Power to see if we can support this configuration.
    if (!USB_HOST_APP_EVENT_HANDLER( USB_ROOT_HUB, EVENT_REQUEST_POWER, &bMaxPower, 1 ))
    {
        return FALSE;
    }

    // Skip over the rest of the Configuration Descriptor
    index += bLength;
    ptr    = &pCurrentConfigurationDescriptor[index];

    while (index < wTotalLength)
    {
        // Check the descriptor length and type
        bLength         = *ptr++;
        bDescriptorType = *ptr++;


        // Find the OTG discriptor (if present)
        if (bDescriptorType == USB_DESCRIPTOR_OTG)
        {
            // We found an OTG Descriptor, so the device supports OTG.
            usbDeviceInfo.flags.bfSupportsOTG = 1;
            usbDeviceInfo.attributesOTG       = *ptr;

            // See if we need to send the SET FEATURE command.  If we do,
            // clear the bConfiguredOTG flag.
            if ( (usbDeviceInfo.attributesOTG & OTG_HNP_SUPPORT) && (usbDeviceInfo.flags.bfAllowHNP))
            {
                usbDeviceInfo.flags.bfConfiguredOTG = 0;
            }
        }

        // Find the interface descriptor
        if (bDescriptorType != USB_DESCRIPTOR_INTERFACE)
        {
            // Skip over the rest of the Descriptor
            index += bLength;
            ptr = &pCurrentConfigurationDescriptor[index];
        }
        else
        {
            // Read some data from the interface descriptor
            bInterfaceNumber  = *ptr++;
            bAlternateSetting = *ptr++;
            bNumEndpoints     = *ptr++;
            Class             = *ptr++;
            SubClass          = *ptr++;
            Protocol          = *ptr++;

            // Get client driver index
            if (usbDeviceInfo.flags.bfUseDeviceClientDriver)
            {
                ClientDriver = usbDeviceInfo.deviceClientDriver;
            }
            else
            {
                if (!_USB_FindClassDriver(Class, SubClass, Protocol, &ClientDriver))
                {
                    // Skip to the next interface descriptor
                    currentAlternateSetting++;
                    index += bLength;
                    ptr = &pCurrentConfigurationDescriptor[index];
                    continue;
                }
            }

            // See if this is the interface and setting we want.
            if ((currentInterface        != bInterfaceNumber ) ||
                (currentAlternateSetting != bAlternateSetting)   )
            {
                // Skip over the rest of the Descriptor
                index += bLength;
                ptr = &pCurrentConfigurationDescriptor[index];
            }
            else
            {
                // This is the setting we want.  Create an entry for the new interface.
                if ((newInterfaceInfo = (USB_INTERFACE_INFO *)malloc( sizeof(USB_INTERFACE_INFO) )) == NULL)
                {
                    return FALSE;   // To Do: Handle out of memory error
                }

                // Initialize the interface node
                newInterfaceInfo->interface           = bInterfaceNumber;
                newInterfaceInfo->interfaceAltSetting = bAlternateSetting;
                newInterfaceInfo->clientDriver        = ClientDriver;

                // Insert it into the list.
                newInterfaceInfo->next                = (USB_INTERFACE_INFO *)pInterfaceList;
                pInterfaceList                        = newInterfaceInfo;

                // Advance to the next interface
                currentInterface++;
                currentAlternateSetting = 0;

                //  Configure the endpoints for this interface.
                currentEndpoint = 0;

                // Skip over the rest of the Interface Descriptor
                index += bLength;
                ptr = &pCurrentConfigurationDescriptor[index];

                // Find the Endpoint Descriptors.  There might be Class and Vendor descriptors in here
                while ((index < wTotalLength) && (currentEndpoint < bNumEndpoints))
                {
                    bLength = *ptr++;
                    bDescriptorType = *ptr++;

                    if (bDescriptorType != USB_DESCRIPTOR_ENDPOINT)
                    {
                        // Skip over the rest of the Descriptor
                        index += bLength;
                        ptr = &pCurrentConfigurationDescriptor[index];
                    }
                    else
                    {
                        // Create an entry for the new endpoint.
                        if ((newEndpointInfo = (USB_ENDPOINT_INFO *)malloc( sizeof(USB_ENDPOINT_INFO) )) == NULL)
                        {
                            return FALSE;   // To Do: Handle out of memory error
                        }
                        newEndpointInfo->bEndpointAddress           = *ptr++;
                        newEndpointInfo->bmAttributes.val           = *ptr++;
                        newEndpointInfo->wMaxPacketSize             = *ptr++;
                        newEndpointInfo->wMaxPacketSize            += (*ptr++) << 8;
                        newEndpointInfo->wInterval                  = *ptr++;
                        newEndpointInfo->status.val                 = 0x00;
                        newEndpointInfo->status.bfUseDTS            = 1;
                        newEndpointInfo->status.bfTransferComplete  = 1;  // Initialize to success to allow preprocessing loops.
                        newEndpointInfo->dataCount                  = 0;  // Initialize to 0 since we set bfTransferComplete.
                        newEndpointInfo->transferState              = TSTATE_IDLE;
                        newEndpointInfo->pInterface                 = newInterfaceInfo;

                        // Special setup for isochronous endpoints.
                        if (newEndpointInfo->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS)
                        {
                            // Validate and convert the interval to the number of frames.  The value must
                            // be between 1 and 16, and the frames is 2^(bInterval-1).
                            if (newEndpointInfo->wInterval == 0) newEndpointInfo->wInterval = 1;
                            if (newEndpointInfo->wInterval > 16) newEndpointInfo->wInterval = 16;
                            newEndpointInfo->wInterval = 1 << (newEndpointInfo->wInterval-1);

                            // Disable DTS
                            newEndpointInfo->status.bfUseDTS = 0;
                        }

                        // Initialize interval count
                        newEndpointInfo->wIntervalCount = newEndpointInfo->wInterval;
                        // Put the new endpoint in the list after EP0.
                        newEndpointInfo->next = pEndpointList->next;
                        pEndpointList->next   = newEndpointInfo;

                        // To Do: Check the available bandwidth to make sure that we can support this endpoint.

                        // Get ready for the next endpoint.
                        currentEndpoint++;
                        index += bLength;
                        ptr = &pCurrentConfigurationDescriptor[index];
                    }
                }

                // Ensure that we found all the endpoints for this interface.
                if (currentEndpoint != bNumEndpoints)
                {
                    return FALSE;
                }
            }
        }
    }

    // Ensure that we found all the interfaces in this configuration.
    if (currentInterface != bNumInterfaces)
    {
        return FALSE;
    }

    // Set configuration.
    usbDeviceInfo.currentConfiguration      = currentConfiguration;
    usbDeviceInfo.currentConfigurationPower = bMaxPower;

    // Success!
    return TRUE;
}


/*******************************************************************************
Function:       void _USB_ResetDATA0( BYTE endpoint )

Precondition:   None

Overview:       This function resets DATA0 for the specified endpoint.  If the
                specified endpoint is 0, it resets DATA0 for all endpoints.

Input:          BYTE endpoint   - Endpoint number to reset.

Output:         None - None

Return:         None - None

Notes:          None

Tested:
*******************************************************************************/

void _USB_ResetDATA0( BYTE endpoint )
{
    USB_ENDPOINT_INFO   *ep;

    ep = pEndpointList;
    while (ep != NULL)
    {
        if ((endpoint == 0) || (endpoint == ep->bEndpointAddress))
        {
            ep->status.bfNextDATA01 = 0;
        }
        ep = ep->next;
    }
}

/*******************************************************************************
Function:       void _USB_SendToken( BYTE endpoint, BYTE tokenType )

Precondition:   None

Overview:       This function sets up the endpoint control register and sends
                the token.

Input:          BYTE endpoint   - Endpoint number
                BYTE tokenType  - Token to send

Output:         None - None

Return:         None - None

Notes:          If the device is low speed, the transfer must be set to low
                speed.  If the endpoint is isochronous, handshaking must be
                disabled.

Tested:
*******************************************************************************/

void _USB_SendToken( BYTE endpoint, BYTE tokenType )
{
    BYTE    temp;

    // Disable retries, disable control transfers, enable Rx and Tx and handshaking.
    temp = 0x5D;

    // Enable low speed transfer if the device is low speed.
    if (usbDeviceInfo.flags.bfIsLowSpeed)
    {
        temp |= 0x80;   // Set LSPD
    }

    // Enable control transfers if necessary.
    if (pCurrentEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_CONTROL)
    {
        temp &= 0xEF;   // Clear EPCONDIS
    }

    // Disable handshaking for isochronous endpoints.
    if (pCurrentEndpoint->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS)
    {
        temp &= 0xFE;   // Clear EPHSHK
    }

    U1EP0 = temp;

    #ifdef DEBUG_MODE
        if (U1CONbits.TOKBUSY) UART2PutChar( '+' );
    #endif

    U1ADDR = usbDeviceInfo.deviceAddressAndSpeed;
    U1TOK = (tokenType << 4) | (endpoint & 0x7F);

    #if defined (__MIGUEL_PIC24FJ256GB110_REV_A1__)
        U1CONbits.TOKBUSY = 1;
    #endif

    #ifdef DEBUG_MODE
        //UART2PutChar('(');
        //UART2PutHex(U1ADDR);
        //UART2PutHex(U1EP0);
        //UART2PutHex(U1TOK);
        //UART2PutChar(')');
    #endif
}

/*******************************************************************************
Function:       void _USB_SetBDT( BYTE token )

Precondition:   pCurrentEndpoint must point to the current endpoint being
                serviced.

Overview:       This function sets up the BDT for the transfer.  The function
                handles the different ping-pong modes.

Input:          BYTE token  - Token for the transfer.  That way we can tell
                                which ping-pong buffer and which data pointer
                                to use.  USB_TOKEN_SETUP, USB_TOKEN_IN, or
                                USB_TOKEN_OUT.

Output:         None - None

Return:         None - None

Notes:          None

Tested:
*******************************************************************************/

void _USB_SetBDT( BYTE token )
{
    WORD                currentPacketSize;
    BDT_ENTRY           *pBDT;

    if (token == USB_TOKEN_IN)
    {
        // Find the BDT we need to use.
        #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
            pBDT = BDT_IN;
            if (usbDeviceInfo.flags.bfPingPongIn)
            {
                pBDT = BDT_IN_ODD;
            }
        #else
            pBDT = BDT_IN;
        #endif

        // Set up ping-pong for the next transfer
        #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
            usbDeviceInfo.flags.bfPingPongIn = ~usbDeviceInfo.flags.bfPingPongIn;
        #endif
    }
    else  // USB_TOKEN_OUT or USB_TOKEN_SETUP
    {
        // Find the BDT we need to use.
        #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
            pBDT = BDT_OUT;
            if (usbDeviceInfo.flags.bfPingPongOut)
            {
                pBDT = BDT_OUT_ODD;
            }
        #else
            pBDT = BDT_OUT;
        #endif

        // Set up ping-pong for the next transfer
        #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
            usbDeviceInfo.flags.bfPingPongOut = ~usbDeviceInfo.flags.bfPingPongOut;
        #endif
    }

    // Determine how much data we'll transfer in this packet.
    if (token == USB_TOKEN_SETUP)
    {
        if ((pCurrentEndpoint->dataCountMaxSETUP - pCurrentEndpoint->dataCount) > pCurrentEndpoint->wMaxPacketSize)
        {
            currentPacketSize = pCurrentEndpoint->wMaxPacketSize;
        }
        else
        {
            currentPacketSize = pCurrentEndpoint->dataCountMaxSETUP - pCurrentEndpoint->dataCount;
        }
    }
    else
    {
        if ((pCurrentEndpoint->dataCountMax - pCurrentEndpoint->dataCount) > pCurrentEndpoint->wMaxPacketSize)
        {
            currentPacketSize = pCurrentEndpoint->wMaxPacketSize;
        }
        else
        {
            currentPacketSize = pCurrentEndpoint->dataCountMax - pCurrentEndpoint->dataCount;
        }
    }

    // Load up the BDT address.
    if (token == USB_TOKEN_SETUP)
    {
        #if defined(__C30__)
            pBDT->ADR  = pCurrentEndpoint->pUserDataSETUP;
        #elif defined(__PIC32MX__)
            pBDT->ADR  = (BYTE *)KVA_TO_PA((DWORD)pCurrentEndpoint->pUserDataSETUP);
        #else
            #error Cannot set BDT address.
        #endif
    }
    else
    {
        #if defined(__C30__)
            pBDT->ADR  = (BYTE *)((WORD)pCurrentEndpoint->pUserData + (WORD)pCurrentEndpoint->dataCount);
        #elif defined(__PIC32MX__)
            pBDT->ADR  = (BYTE *)KVA_TO_PA((DWORD)pCurrentEndpoint->pUserData + (DWORD)pCurrentEndpoint->dataCount);
        #else
            #error Cannot set BDT address.
        #endif
    }

    // Load up the BDT status register.
    pBDT->STAT.Val      = 0;
    pBDT->count         = currentPacketSize;
    pBDT->STAT.DTS      = pCurrentEndpoint->status.bfNextDATA01;
    pBDT->STAT.DTSEN    = pCurrentEndpoint->status.bfUseDTS;

    // Transfer the BD to the USB OTG module.
    pBDT->STAT.UOWN     = 1;

    #ifdef DEBUG_MODE
//        UART2PutChar('{');
//        UART2PutHex((pBDT->v[0] >> 24) & 0xff);
//        UART2PutHex((pBDT->v[0] >> 16) & 0xff);
//        UART2PutHex((pBDT->v[0] >> 8) & 0xff);
//        UART2PutHex((pBDT->v[0]) & 0xff);
//        UART2PutChar('-');
//        UART2PutHex((currentPacketSize >> 24) & 0xff);
//        UART2PutHex((pBDT->v[1] >> 16) & 0xff);
//        UART2PutHex((currentPacketSize >> 8) & 0xff);
//        UART2PutHex(currentPacketSize & 0xff);
//        UART2PutChar('}');
    #endif

}

/*******************************************************************************
Function:       BOOL _USB_TransferInProgress( void )

Precondition:   None

Overview:       This function checks to see if any read or write transfers are
                in progress.

Input:          None - None

Output:         None - None

Return:         TRUE    - At least one read or write transfer is occurring.
                FALSE   - No read or write transfers are occurring.

Notes:          None

Tested:
*******************************************************************************/

BOOL _USB_TransferInProgress( void )
{
    USB_ENDPOINT_INFO   *ep;

    ep = pEndpointList;
    while (ep != NULL)
    {
        if (!ep->status.bfTransferComplete)
        {
            return TRUE;
        }
        ep = ep->next;
    }
    return FALSE;
}


// *****************************************************************************
// *****************************************************************************
// Section: Interrupt Handlers
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       void _USB1Interrupt( void )

Precondition:   In TRNIF, pCurrentEndpoint is still pointing to the last
                endpoint to which a token was sent.

Overview:       This is the interrupt service routine for the USB interrupt.
                The following cases are serviced:
                 * Device Attach
                 * Device Detach
                 * One millisecond Timer
                 * Start of Frame
                 * Transfer Done
                 * USB Error

Input:          None - None

Output:         None - None

Return:         None - None

Notes:          None

Tested:
*******************************************************************************/
#define U1STAT_TX_MASK                      0x08    // U1STAT bit mask for Tx/Rx indication
#define U1STAT_ODD_MASK                     0x04    // U1STAT bit mask for even/odd buffer bank

#if defined(__C30__)
void __attribute__((__interrupt__, auto_psv)) _USB1Interrupt( void )
#elif defined(__PIC32MX__)
#pragma interrupt _USB1Interrupt ipl4 vector 45
void _USB1Interrupt( void )
#else
    #error Cannot define timer interrupt vector.
#endif
{
    #if defined( __C30__)
        IFS5 &= 0xFFBF;
    #elif defined( __PIC32MX__)
        IFS1CLR = 0x02000000;
    #else
        #error Cannot clear USB interrupt.
    #endif

    // -------------------------------------------------------------------------
    // One Millisecond Timer ISR

    if (U1OTGIEbits.T1MSECIE && U1OTGIRbits.T1MSECIF)
    {
        // The interrupt is cleared by writing a '1' to it.
        //U1OTGIRbits.T1MSECIF = 1;
        U1OTGIR = USB_INTERRUPT_T1MSECIF;

        #ifdef DEBUG_MODE
            UART2PutChar('~');
        #endif

        numTimerInterrupts--;
        if (numTimerInterrupts == 0)
        {
            // Turn off the timer interrupt.
            U1OTGIEbits.T1MSECIE = 0;

            // Advance to the next state.  We can do this here, because the only time
            // we'll get a time interrupt is while we are in one of the holding states.
            _USB_SetNextSubSubState();
        }
    }

    // -------------------------------------------------------------------------
    // Attach ISR

    // The attach interrupt is level, not edge, triggered.  So make sure we have it enabled.
    if (U1IEbits.ATTACHIE && U1IRbits.ATTACHIF)
    {
        #ifdef DEBUG_MODE
            UART2PutChar( '[' );
        #endif

        // The attach interrupt is level, not edge, triggered.  If we clear it, it just
        // comes right back.  So clear the enable instead
        U1IEbits.ATTACHIE   = 0;
        U1IR                = USB_INTERRUPT_ATTACH;

        if (usbHostState == (STATE_DETACHED | SUBSTATE_WAIT_FOR_DEVICE))
        {
            _USB_SetNextState();
        }
    }

    // -------------------------------------------------------------------------
    // Detach ISR

    if (U1IEbits.DETACHIE && U1IRbits.DETACHIF)
    {
        #ifdef DEBUG_MODE
            UART2PutChar( ']' );
        #endif

        U1IR                    = USB_INTERRUPT_DETACH;
        U1IEbits.DETACHIE       = 0;
        usbOverrideHostState    = STATE_DETACHED;
    }

    // -------------------------------------------------------------------------
    // Transfer Done ISR

    if (U1IEbits.TRNIE && U1IRbits.TRNIF)
    {
        #if defined(__C30__)
            U1STATBITS          copyU1STATbits;
        #elif defined(__PIC32MX__)
            __U1STATbits_t      copyU1STATbits;
        #else
            #error Need structure name for copyU1STATbits.
        #endif
        WORD                    packetSize;
        BDT_ENTRY               *pBDT;

        #ifdef DEBUG_MODE
//            UART2PutChar( '!' );
        #endif

        #if defined(__PIC24FJ256GB110_REV_A1__)
            // The TOKBUSY bit is not being cleared automatically by hardware.  We
            // have to clear it ourselves.  It takes two shots to actually clear it.
            while (U1CONbits.TOKBUSY)
            {
                U1CONbits.TOKBUSY = 0;
            }
        #endif

        copyU1STATbits = U1STATbits;    // Read the status register before clearing the flag.

        U1IR = USB_INTERRUPT_TRANSFER;  // Clear the interrupt by writing a '1' to the flag.

        // In host mode, U1STAT does NOT reflect the endpoint.  It is really the last updated
        // BDT, which, in host mode, is always 0.  To get the endpoint, we either need to look
        // at U1TOK, or trust that pCurrentEndpoint is still accurate.
        if ((pCurrentEndpoint->bEndpointAddress & 0x0F) == (U1TOK & 0x0F))
        {
            if (copyU1STATbits.DIR)     // TX
            {
                // We are processing OUT or SETUP packets.
                // Set up the BDT pointer for the transaction we just received.
                #if (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY) || (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
                    pBDT = BDT_OUT;
                    if (copyU1STATbits.PPBI) // Odd
                    {
                        pBDT = BDT_OUT_ODD;
                    }
                #elif (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG) || (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)
                    pBDT = BDT_OUT;
                #endif
            }
            else
            {
                // We are processing IN packets.
                // Set up the BDT pointer for the transaction we just received.
                #if (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
                    pBDT = BDT_IN;
                    if (copyU1STATbits.PPBI) // Odd
                    {
                        pBDT = BDT_IN_ODD;
                    }
                #else
                    pBDT = BDT_IN;
                #endif
            }

            if (pBDT->STAT.PID == PID_ACK)
            {
                // We will only get this PID from an OUT or SETUP packet.

                // Update the count of bytes tranferred.  (If there was an error, this count will be 0.)
                // The Byte Count is NOT 0 if a NAK occurs.  Therefore, we can only update the
                // count when an ACK, DATA0, or DATA1 is received.
                packetSize                  = pBDT->count;
                pCurrentEndpoint->dataCount += packetSize;

                // Set the NAK retries for the next transaction;
                pCurrentEndpoint->countNAKs = 0;

                // Toggle DTS for the next transfer.
                if (pCurrentEndpoint->status.bfNextDATA01)
                {
                    pCurrentEndpoint->status.bfNextDATA01 = 0;
                }
                else
                {
                    pCurrentEndpoint->status.bfNextDATA01 = 1;
                }

                if ((pCurrentEndpoint->transferState == (TSTATE_CONTROL_NO_DATA | TSUBSTATE_CONTROL_NO_DATA_SETUP)) ||
                    (pCurrentEndpoint->transferState == (TSTATE_CONTROL_READ    | TSUBSTATE_CONTROL_READ_SETUP)) ||
                    (pCurrentEndpoint->transferState == (TSTATE_CONTROL_WRITE   | TSUBSTATE_CONTROL_WRITE_SETUP)))
                {
                    // We are doing SETUP transfers. See if we are done with the SETUP portion.
                    if (pCurrentEndpoint->dataCount >= pCurrentEndpoint->dataCountMaxSETUP)
                    {
                        // We are done with the SETUP.  Reset the byte count and
                        // proceed to the next token.
                        pCurrentEndpoint->dataCount = 0;
                        _USB_SetNextTransferState();
                    }
                }
                else
                {
                    // We are doing OUT transfers.  See if we've written all the data.
                    // We've written all the data when we send a short packet or we have
                    // transferred all the data.
                    if ((packetSize < pCurrentEndpoint->wMaxPacketSize) ||
                        (pCurrentEndpoint->dataCount >= pCurrentEndpoint->dataCountMax))
                    {
                        // We've written all the data. Proceed to the next step.
                        pCurrentEndpoint->status.bfTransferSuccessful = 1;
                        _USB_SetNextTransferState();
                    }
                    else
                    {
                        // We need to process more data.  Keep this endpoint in its current
                        // transfer state.
                    }
                }
            }
            else if ((pBDT->STAT.PID == PID_DATA0) || (pBDT->STAT.PID == PID_DATA1))
            {
                // We will only get these PID's from an IN packet.

                // Update the count of bytes tranferred.  (If there was an error, this count will be 0.)
                // The Byte Count is NOT 0 if a NAK occurs.  Therefore, we can only update the
                // count when an ACK, DATA0, or DATA1 is received.
                packetSize                  = pBDT->count;
                pCurrentEndpoint->dataCount += packetSize;

                // Set the NAK retries for the next transaction;
                pCurrentEndpoint->countNAKs = 0;

                // Toggle DTS for the next transfer.
                if (pCurrentEndpoint->status.bfNextDATA01)
                {
                    pCurrentEndpoint->status.bfNextDATA01 = 0;
                }
                else
                {
                    pCurrentEndpoint->status.bfNextDATA01 = 1;
                }

                // We are doing IN transfers.  See if we've received all the data.
                // We've received all the data when we receive a short packet or we have
                // transferred all the data.
                if ((packetSize < pCurrentEndpoint->wMaxPacketSize) ||
                    (pCurrentEndpoint->dataCount >= pCurrentEndpoint->dataCountMax))
                {
                    // If we've received all the data, stop the transfer.  We've received all the
                    // data when we receive a short or zero-length packet.  If the data length is a
                    // multiple of wMaxPacketSize, we will get a 0-length packet.
                    pCurrentEndpoint->status.bfTransferSuccessful = 1;
                    _USB_SetNextTransferState();
                }
                else
                {
                    // We need to process more data.  Keep this endpoint in its current
                    // transfer state.
                }
            }
            else if (pBDT->STAT.PID == PID_NAK)
            {
                #ifndef ALLOW_MULTIPLE_NAKS_PER_FRAME
                    pCurrentEndpoint->status.bfLastTransferNAKd = 1;
                #endif

                pCurrentEndpoint->countNAKs ++;

                switch( pCurrentEndpoint->bmAttributes.bfTransferType )
                {
                    case USB_TRANSFER_TYPE_BULK:
                        // Bulk IN and OUT transfers are allowed to retry NAK'd
                        // transactions until a timeout.
                        if (pCurrentEndpoint->countNAKs > USB_NUM_BULK_NAKS)
                        {
                            pCurrentEndpoint->bErrorCode = USB_ENDPOINT_NAK_TIMEOUT;
                            _USB_SetTransferErrorState( pCurrentEndpoint );
                        }
                        break;

                    case USB_TRANSFER_TYPE_CONTROL:
                        // Devices should not NAK the SETUP portion.  If they NAK
                        // the DATA portion, they are allowed to retry a fixed
                        // number of times.
                        if (pCurrentEndpoint->countNAKs > USB_NUM_CONTROL_NAKS)
                        {
                            pCurrentEndpoint->bErrorCode = USB_ENDPOINT_NAK_TIMEOUT;
                            _USB_SetTransferErrorState( pCurrentEndpoint );
                        }
                        break;

                    case USB_TRANSFER_TYPE_INTERRUPT:
                        if ((pCurrentEndpoint->bEndpointAddress & 0x80) == 0x00)
                        {
                            // Interrupt OUT transfers are allowed to retry NAK'd
                            // transactions until a timeout.
                            if (pCurrentEndpoint->countNAKs > USB_NUM_INTERRUPT_NAKS)
                            {
                                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_NAK_TIMEOUT;
                                _USB_SetTransferErrorState( pCurrentEndpoint );
                            }
                        }
                        else
                        {
                            // Interrupt IN transfers terminate with no error.
                            pCurrentEndpoint->status.bfTransferSuccessful = 1;
                            _USB_SetNextTransferState();
                        }
                        break;
                    case USB_TRANSFER_TYPE_ISOCHRONOUS:
                        // Isochronous transfers terminate with no error.
                        pCurrentEndpoint->status.bfTransferSuccessful = 1;
                        _USB_SetNextTransferState();
                        break;
                }
            }
            else if (pBDT->STAT.PID == PID_STALL)
            {
                // Device is stalled.  Stop the transfer, and indicate the stall.
                // The application must clear this if not a control endpoint.
                // A stall on a control endpoint does not indicate that the
                // endpoint is halted.
                pCurrentEndpoint->status.bfStalled = 1;
                pCurrentEndpoint->bErrorCode       = USB_ENDPOINT_STALLED;
                _USB_SetTransferErrorState( pCurrentEndpoint );
            }
            else
            {
                // Module-defined PID - Bus Timeout (0x0) or Data Error (0x0F).  Increment the error count.
                pCurrentEndpoint->status.bfErrorCount++;

                if (pCurrentEndpoint->status.bfErrorCount >= 3)
                {
                    // We have too many errors.  Stall the endpoint.
                    pCurrentEndpoint->status.bfStalled  = 1;

                    // Stop the transfer and indicate an error.
                    // The application must clear this.
                    pCurrentEndpoint->status.bfError    = 1;
                    pCurrentEndpoint->bErrorCode        = USB_ENDPOINT_ERROR_ILLEGAL_PID;
                    _USB_SetTransferErrorState( pCurrentEndpoint );

                    // Avoid the error interrupt code, because we are going to
                    // find another token to send.
                    U1EIR = 0xFF;
                    U1IR  = USB_INTERRUPT_ERROR;
                }
                else
                {
                    // Try the transfer again.

                    // TODO ? Set the NAK retries for the next transaction;
//                    pCurrentEndpoint->countNAKs = 0;
                }
            }
        }
        else
        {
            // We have a mismatch between the endpoint we were expecting and the one that we got.
            // The user may be trying to select a new configuration.  Discard the transaction.
        }

        _USB_FindNextToken();
    } // U1IRbits.TRNIF


    // -------------------------------------------------------------------------
    // Start-of-Frame ISR

    if (U1IEbits.SOFIE && U1IRbits.SOFIF)
    {
        USB_ENDPOINT_INFO   *ep;

        #ifdef DEBUG_MODE
//            UART2PutChar( '$' );
        #endif
        U1IR = USB_INTERRUPT_SOF; // Clear the interrupt by writing a '1' to the flag.

        ep = pEndpointList->next;
        while (ep != NULL)
        {
            // Decrement the interval count of all interrupt and isochronous endpoints.
            if ((ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_INTERRUPT) ||
                (ep->bmAttributes.bfTransferType == USB_TRANSFER_TYPE_ISOCHRONOUS))
            {
                if (ep->wIntervalCount != 0)
                {
                    ep->wIntervalCount--;
                }
            }
            #ifndef ALLOW_MULTIPLE_NAKS_PER_FRAME
                ep->status.bfLastTransferNAKd = 0;
            #endif
            ep = ep->next;
        }

        usbBusInfo.flags.bfControlTransfersDone     = 0;
        usbBusInfo.flags.bfInterruptTransfersDone   = 0;
        usbBusInfo.flags.bfIsochronousTransfersDone = 0;
        usbBusInfo.flags.bfBulkTransfersDone        = 0;
        usbBusInfo.dBytesSentInFrame                = 0;
        usbBusInfo.lastBulkTransaction              = 0;

        _USB_FindNextToken();
    }

    // -------------------------------------------------------------------------
    // USB Error ISR

    if (U1IEbits.UERRIE && U1IRbits.UERRIF)
    {
        U1IR = USB_INTERRUPT_ERROR; // Clear the interrupt by writing a '1' to the flag.

        #ifdef DEBUG_MODE
            UART2PutChar('#');
            UART2PutHex( U1EIR );
        #endif

        // Interrupt error.  Increment the error count.
        pCurrentEndpoint->status.bfErrorCount++;

        if (pCurrentEndpoint->status.bfErrorCount >= 3)
        {
            // We have too many errors.  Stall the endpoint.
            pCurrentEndpoint->status.bfStalled  = 1;

            // Check U1EIR for the appropriate error codes to return
            if (U1EIRbits.BTSEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_BIT_STUFF;
            if (U1EIRbits.DMAEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_DMA;
            if (U1EIRbits.BTOEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_TIMEOUT;
            if (U1EIRbits.DFN8EF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_DATA_FIELD;
            if (U1EIRbits.CRC16EF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_CRC16;
            if (U1EIRbits.EOFEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_END_OF_FRAME;
            if (U1EIRbits.PIDEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_PID_CHECK;
            #if defined(__PIC32MX__)
            if (U1EIRbits.BMXEF)
                pCurrentEndpoint->bErrorCode = USB_ENDPOINT_ERROR_BMX;
            #endif

            U1EIR = 0xFF;   // Clear the interrupts by writing '1' to the flags.
            pCurrentEndpoint->status.bfError    = 1;

            _USB_SetTransferErrorState( pCurrentEndpoint );
        }
    }
}



