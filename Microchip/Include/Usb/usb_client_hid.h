/******************************************************************************

  USB Host Mass Storage Device Driver Header File

This is the HID Class driver header file for a USB Host device.  This
file should be included with USBHost.h to provide the USB hardware interface.
It must be included after the application-specific USBConfig.h file and the
USB Host header file USBHost.h, as definitions in those files are required for
proper compilation.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************

* FileName:        usb_client_hid.h
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
ADG                     First release

*******************************************************************************/
//DOM-IGNORE-END

//DOM-IGNORE-BEGIN
#ifndef _USB_CLIENT_HID_H_
#define _USB_CLIENT_HID_H_
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// HID Class Error Codes
// *****************************************************************************

#define HID_COMMAND_PASSED				 0x00
#define HID_COMMAND_FAILED				 0x01
#define HID_PHASE_ERROR					 0x02

#define USB_HID_CLASS_ERROR              USB_ERROR_CLASS_DEFINED

#define USB_HID_COMMAND_PASSED           USB_SUCCESS
#define USB_HID_COMMAND_FAILED          (USB_HID_CLASS_ERROR | HID_COMMAND_FAILED)
#define USB_HID_PHASE_ERROR             (USB_HID_CLASS_ERROR | HID_PHASE_ERROR)
#define USB_HID_DEVICE_NOT_FOUND        (USB_HID_CLASS_ERROR | 0x03)
#define USB_HID_DEVICE_BUSY             (USB_HID_CLASS_ERROR | 0x04)
#define USB_HID_NO_REPORT_DESCRIPTOR    (USB_HID_CLASS_ERROR | 0x05)
#define USB_HID_INTERFACE_ERROR         (USB_HID_CLASS_ERROR | 0x06) // The interface layer cannot support the device.
#define USB_HID_REPORT_DESCRIPTOR_BAD   (USB_HID_CLASS_ERROR | 0x05)
#define USB_HID_RESET_ERROR             (USB_HID_CLASS_ERROR | 0x0A) // An error occurred while resetting the device.
#define USB_HID_ILLEGAL_REQUEST         (USB_HID_CLASS_ERROR | 0x0B) // Cannot perform requested operation.


/* TODO: Error Codes for Report Descriptor TBD - Amar */

// *****************************************************************************
// Section: Interface and Protocol Constants
// *****************************************************************************

#define DEVICE_CLASS_HID             0x03 /* HID Interface Class Code */

#define DSC_HID                      0x21 /* HID Descriptor Code */
#define DSC_RPT                      0x2200 /* Report Descriptor Code */
#define DSC_PHY                      0x23 /* Pysical Descriptor Code */

#define DEVICE_SUBCLASS_NONE         0x00 
#define DEVICE_SUBCLASS_BOOT         0x01 /* HID BOOT Interface SubClass Code */

#define DEVICE_PROTOCOL_NONE         0x00 
#define DEVICE_PROTOCOL_KEYBOARD     0x01 /* Subclass Code for Keyboard */
#define DEVICE_PROTOCOL_MOUSE        0x02 /* Subclass Code for Mouse */


// *****************************************************************************
// Additional return values for USBHostHIDDeviceStatus (see USBHostDeviceStatus also)
// *****************************************************************************

#define USB_HID_DEVICE_DETACHED             0x50
#define USB_HID_INITIALIZING                0x51
#define USB_PROCESSING_REPORT_DESCRIPTOR    0x52
#define USB_HID_NORMAL_RUNNING              0x53
#define USB_HID_DEVICE_HOLDING              0x54
#define USB_HID_RESETTING_DEVICE            0x55

// *****************************************************************************
// Section: HID Event Definition
// *****************************************************************************

// If the application has not defined an offset for HID events, set it to 0.
#ifndef EVENT_HID_OFFSET
    #define EVENT_HID_OFFSET    0
#endif

#define EVENT_HID_NONE        EVENT_HID_BASE + EVENT_HID_OFFSET + 0   // No event occured (NULL event)
#define EVENT_HID_TRANSFER    EVENT_HID_BASE + EVENT_HID_OFFSET + 1   // A HID transfer has completed
#define EVENT_HID_READ_DONE   EVENT_HID_BASE + EVENT_HID_OFFSET + 3   // A HID Read transfer has completed
#define EVENT_HID_WRITE_DONE  EVENT_HID_BASE + EVENT_HID_OFFSET + 4   // A HID Write transfer has completed
#define EVENT_HID_RESET       EVENT_HID_BASE + EVENT_HID_OFFSET + 5   // HID reset complete

// *****************************************************************************
// Section: Function Prototypes 
// *****************************************************************************

/*******************************************************************************
Function:       BYTE    USBHostHIDDeviceStatus( BYTE deviceAddress )

Preconditions:  None

Overview:       This function determines the status of a HID device.

Input:          BYTE deviceAddress - address of device to query

Output:         None - None

Return Values:  USB_HID_DEVICE_NOT_FOUND           -  Illegal device address, or the
                                                      device is not an HID
                USB_HID_INITIALIZING               -  HID is attached and in the
                                                      process of initializing
                USB_PROCESSING_REPORT_DESCRIPTOR   -  HID device is detected and report 
                                                      descriptor is being parsed
                USB_HID_NORMAL_RUNNING             -  HID Device is running normal, 
                                                      ready to send and receive reports 
                USB_HID_DEVICE_HOLDING             - 
                USB_HID_DEVICE_DETACHED            -  HID detached.

Remarks:        None
*******************************************************************************/
BYTE    USBHostHIDDeviceStatus( BYTE deviceAddress );

/*******************************************************************************
Function:       void USBHostHIDInitialize( void )

Precondition:   None

Overview:       This function initializes the HID class driver.  It
                should only be called once.

Input:          None

Output:         None

Return Values:  None

Remarks:        The presence of a return code is for similarity with other
                USB initialization functions and future expansion.
*******************************************************************************/
void  USBHostHIDInitialize( void );


/*******************************************************************************
Function:       BYTE USBHostHIDRead( BYTE deviceAddress,BYTE reportid, BYTE size, BYTE *data)

Preconditions:  None

Overview:       This function starts a Get report transfer reuest from the device,
                utilizing the function USBHostHIDTransfer();

Input:          BYTE deviceAddress      - Device address
                BYTE reportid           - Report ID of the requested report
                BYTE size               - Byte size of the data buffer
                BYTE *data              - Pointer to the data buffer

Output:         None - None

Return Values:  USB_SUCCESS                 - Request started successfully
                USB_HID_DEVICE_NOT_FOUND    - No device with specified address
                USB_HID_DEVICE_BUSY         - Device not in proper state for
                                              performing a transfer
Remarks:        None
*******************************************************************************/
#define USBHostHIDRead( deviceAddress,reportid,size,data)      USBHostHIDTransfer( deviceAddress,1,reportid,size,data)



//BYTE    USBHostHIDResetDevice( BYTE deviceAddress ); //To Be Done

/*******************************************************************************
Function:       void USBHostHIDTasks( void )

Precondition:   USBHostHIDInitialize() has been called.

Overview:       This function performs the maintenance tasks required by the
                HID class.  It should be called on a regular basis by
                the application.

Input:          None

Output:         None

Return:         None

Remarks:        None
Tested:
*******************************************************************************/
void    USBHostHIDTasks( void );

/*******************************************************************************
Function:       USBHostHIDTransfer( BYTE deviceAddress, BYTE direction, BYTE reportid, BYTE size, BYTE *data)

Preconditions:  None

Overview:       This function starts a HID transfer.  Usually, users
                will probably use a read/write wrapper to access this function.

Input:          BYTE deviceAddress      - Device address
                BYTE direction          - 1=read, 0=write
                BYTE reportid           - Report ID of the requested report
                BYTE size               - Byte size of the data buffer
                BYTE *data              - Pointer to the data buffer

Output:         None - None

Return Values:  USB_SUCCESS                 - Request started successfully
                USB_HID_DEVICE_NOT_FOUND    - No device with specified address
                USB_HID_DEVICE_BUSY         - Device not in proper state for
                                              performing a transfer
Remarks:        None
*******************************************************************************/
BYTE USBHostHIDTransfer( BYTE deviceAddress, BYTE direction, BYTE reportid, BYTE size, BYTE *data);

/*******************************************************************************
Function:       BOOL USBHostHIDTransferIsComplete( BYTE deviceAddress,
                        BYTE *errorCode, DWORD *byteCount )

Precondition:   None

Overview:       This function indicates whether or not the last transfer is complete.
                If the functions returns TRUE, the returned byte count and error
                code are valid. Since only one transfer can be performed at once
                and only one endpoint can be used, we only need to know the
                device address.

Input:          BYTE deviceAddress  - Device address

Output:         BYTE *errorCode     - Error code from last transfer
                DWORD *byteCount    - Number of bytes transferred

Return Values:  TRUE    - Transfer is complete, errorCode is valid
                FALSE   - Transfer is not complete, errorCode is not valid

Tested:
*******************************************************************************/
BOOL    USBHostHIDTransferIsComplete( BYTE deviceAddress, BYTE *errorCode, BYTE *byteCount );

/*******************************************************************************
Function:       BYTE USBHostHIDWrite( BYTE deviceAddress,BYTE reportid, BYTE size, BYTE *data)

Preconditions:  None

Overview:       This function starts a Set report transfer request to the device,
                utilizing the function USBHostHIDTransfer();

Input:          BYTE deviceAddress      - Device address
                BYTE reportid           - Report ID of the report to be sent
                BYTE size               - Byte size of the data buffer
                BYTE *data              - Pointer to the data buffer

Output:         None - None

Return Values:  USB_SUCCESS                 - Request started successfully
                USB_HID_DEVICE_NOT_FOUND    - No device with specified address
                USB_HID_DEVICE_BUSY         - Device not in proper state for
                                              performing a transfer
Remarks:        None
*******************************************************************************/
#define USBHostHIDWrite( address,reportid,size,data)     USBHostHIDTransfer( address,0,reportid,size,data)

// *****************************************************************************
// *****************************************************************************
// USB Host Callback Function Prototypes
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BOOL USBHostHIDClientInitialize( BYTE address, WORD flags )

Precondition:   None

Overview:       This function is called by USBHost.c when the USB device is
                being enumerated.  For a HID device we need to look into HID descriptor,
                endpoint descriptor and validate the configutaion

Input:          BYTE address        - Address of the new device
                WORD flags          - Initialization flags

Output:         None - None

Return Values:  TRUE   - We can support the device.
                FALSE  - We cannot support the device, invalid configuration.

Notes:          None

Tested:
*******************************************************************************/
BOOL USBHostHIDClientInitialize( BYTE address, WORD flags );

/*******************************************************************************
Function:       BOOL USBHostHIDClientEventHandler( BYTE address, USB_EVENT event,
                        void *data, WORD size )

Precondition:   The device has been initialized.

Overview:       This function is called when various events occur in the
                USB Host layer.

Input:          BYTE address    - Address of the device
                USB_EVENT event - Event that has occurred
                void *data      - Pointer to data pertinent to the event
                WORD size       - Size of the data

Output:         None - None

Return:         TRUE   - Event was handled
                FALSE  - Event was not handled

Notes:          None

Tested:
*******************************************************************************/
BOOL USBHostHIDClientEventHandler( BYTE address, USB_EVENT event, void *data, WORD size );


#endif
