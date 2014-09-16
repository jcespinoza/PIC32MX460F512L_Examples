/******************************************************************************

  USB Host Mass Storage Device Driver Header File

This is the Mass Storage Class driver header file for a USB Host device.  This
file should be included with USBHost.h to provide the USB hardware interface.
It must be included after the application-specific USBConfig.h file and the
USB Host header file USBHost.h, as definitions in those files are required for
proper compilation.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************

* FileName:        USBHostMSD.h
* Dependencies:    None
* Processor:       PIC24/dsPIC30/dsPIC33/PIC32MX
* Compiler:        C30 v2.01/C32 v0.00.18
* Company:         Microchip Technology, Inc.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the �Company�) for its PICmicro� Microcontroller is intended and
supplied to you, the Company�s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

Author          Date    Comments
--------------------------------------------------------------------------------
KO          15-Oct-2007 First release

*******************************************************************************/
//DOM-IGNORE-END

//DOM-IGNORE-BEGIN
#ifndef _USBHOSTMSD_H_
#define _USBHOSTMSD_H_
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// Section: MSD Class Error Codes
// *****************************************************************************

#define MSD_COMMAND_PASSED                  0x00    // Transfer was successful. Returned in dCSWStatus.
#define MSD_COMMAND_FAILED                  0x01    // Transfer failed. Returned in dCSWStatus.
#define MSD_PHASE_ERROR                     0x02    // Transfer phase error. Returned in dCSWStatus.

#define USB_MSD_ERROR                       USB_ERROR_CLASS_DEFINED             // Error code offset.

#define USB_MSD_COMMAND_PASSED              USB_SUCCESS                         // Command was successful.
#define USB_MSD_COMMAND_FAILED              (USB_MSD_ERROR | MSD_COMMAND_FAILED)// Command failed at the device.
#define USB_MSD_PHASE_ERROR                 (USB_MSD_ERROR | MSD_PHASE_ERROR)   // Command had a phase error at the device.
#define USB_MSD_OUT_OF_MEMORY               (USB_MSD_ERROR | 0x03)              // No dynamic memory is available.
#define USB_MSD_CBW_ERROR                   (USB_MSD_ERROR | 0x04)              // The CBW was not transferred successfully.
#define USB_MSD_CSW_ERROR                   (USB_MSD_ERROR | 0x05)              // The CSW was not transferred successfully.
#define USB_MSD_DEVICE_NOT_FOUND            (USB_MSD_ERROR | 0x06)              // Device with the specified address is not available.
#define USB_MSD_DEVICE_BUSY                 (USB_MSD_ERROR | 0x07)              // A transfer is currently in progress.
#define USB_MSD_INVALID_LUN                 (USB_MSD_ERROR | 0x08)              // Invalid LUN specified.
#define USB_MSD_MEDIA_INTERFACE_ERROR       (USB_MSD_ERROR | 0x09)              // The media interface layer cannot support the device.
#define USB_MSD_RESET_ERROR                 (USB_MSD_ERROR | 0x0A)              // An error occurred while resetting the device.
#define USB_MSD_ILLEGAL_REQUEST             (USB_MSD_ERROR | 0x0B)              // Cannot perform requested operation.

// *****************************************************************************
// Section: Additional return values for USBHostMSDDeviceStatus (see USBHostDeviceStatus also)
// *****************************************************************************

#define USB_MSD_DEVICE_DETACHED             0x50    // Device is detached.
#define USB_MSD_INITIALIZING                0x51    // Device is initializing.
#define USB_MSD_NORMAL_RUNNING              0x52    // Device is running and available for data transfers.
#define USB_MSD_RESETTING_DEVICE            0x53    // Device is being reset.
#define USB_MSD_ERROR_STATE                 0x55    // Device is holding due to a MSD error.

// *****************************************************************************
// Section: Interface and Protocol Constants
// *****************************************************************************

#define DEVICE_CLASS_MASS_STORAGE           0x08    // Class code for Mass Storage.

#define DEVICE_SUBCLASS_RBC                 0x01    // SubClass code for Reduced Block Commands (not supported).
#define DEVICE_SUBCLASS_CD_DVD              0x02    // SubClass code for a CD/DVD drive (not supported).
#define DEVICE_SUBCLASS_TAPE_DRIVE          0x03    // SubClass code for a tape drive (not supported).
#define DEVICE_SUBCLASS_FLOPPY_INTERFACE    0x04    // SubClass code for a floppy disk interface (not supported).
#define DEVICE_SUBCLASS_REMOVABLE           0x05    // SubClass code for removable media (not supported).
#define DEVICE_SUBCLASS_SCSI                0x06    // SubClass code for a SCSI interface device (supported).

#define DEVICE_INTERFACE_PROTOCOL_BULK_ONLY 0x50    // Protocol code for Bulk-only mass storage.


// *****************************************************************************
// Section: MSD Event Definition
// *****************************************************************************

// If the application has not defined an offset for MSD events, set it to 0.
#ifndef EVENT_MSD_OFFSET
    #define EVENT_MSD_OFFSET    0
#endif

#define EVENT_MSD_NONE      EVENT_MSD_BASE + EVENT_MSD_OFFSET + 0   // No event occured (NULL event)
#define EVENT_MSD_TRANSFER  EVENT_MSD_BASE + EVENT_MSD_OFFSET + 1   // A MSD transfer has completed
#define EVENT_MSD_RESET     EVENT_MSD_BASE + EVENT_MSD_OFFSET + 2   // MSD reset complete
#define EVENT_MSD_MAX_LUN   EVENT_MSD_BASE + EVENT_MSD_OFFSET + 3   // Set maximum LUN for the device

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes and Macro Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BYTE USBHostMSDDeviceStatus( BYTE deviceAddress )

Preconditions:  None

Overview:       This function determines the status of a mass storage device.

Input:          BYTE deviceAddress - address of device to query

Output:         None - None

Return:         USB_MSD_DEVICE_NOT_FOUND -  Illegal device address, or the
                                            device is not an MSD
                USB_MSD_INITIALIZING     -  MSD is attached and in the
                                            process of initializing
                USB_MSD_NORMAL_RUNNING   -  MSD is in normal running mode
                USB_MSD_RESETTING_DEVICE -  MSD is resetting
                USB_MSD_DEVICE_DETACHED  -  MSD detached.  Should not occur
                USB_MSD_ERROR_STATE      -  MSD is holding due to an error.  No
                                            communication is allowed.

                Other - Return codes from USBHostDeviceStatus() will also be returned
                if the device is in the process of enumerating.

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDDeviceStatus( BYTE deviceAddress );

/*******************************************************************************
Function:       BYTE USBHostMSDInit( void )

Preconditions:  None

Overview:       This function initializes the Mass Storage class driver.  It
                should only be called once.

Input:          None - None

Output:         None - None

Return:         USB_SUCCESS - Operation successful

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDInit( void );

/*******************************************************************************
Function:       BYTE USBHostMSDRead( BYTE deviceAddress, BYTE deviceLUN, BYTE *commandBlock,
                            BYTE commandBlockLength, BYTE *data, DWORD dataLength );

Preconditions:  None

Overview:       This function starts a mass storage read, utilizing the function
                USBHostMSDTransfer();

Input:          BYTE deviceAddress      - Device address
                BYTE deviceLUN          - Device LUN to access
                BYTE *commandBlock      - Pointer to the command block for the CBW
                BYTE commandBlockLength - Length of the command block
                BYTE *data              - Pointer to the data buffer
                DWORD dataLength        - Byte size of the data buffer

Output:         None - None

Return:         USB_SUCCESS                 - Request started successfully
                USB_MSD_DEVICE_NOT_FOUND    - No device with specified address
                USB_DEVICE_BUSY             - Device not in proper state for
                                              performing a transfer
                USB_MSD_INVALID_LUN         - Specified LUN does not exist

Remarks:        None
*******************************************************************************/
#define USBHostMSDRead( deviceAddress,deviceLUN,commandBlock,commandBlockLength,data,dataLength ) \
        USBHostMSDTransfer( deviceAddress, deviceLUN, 1, commandBlock, commandBlockLength, data, dataLength )

/*******************************************************************************
Function:       BYTE USBHostMSDResetDevice( BYTE deviceAddress )

Preconditions:  None

Overview:       This function starts a bulk-only mass storage reset.  A reset
                can be issued only if the device is attached and not being
                initialized.

Input:          BYTE deviceAddress - Device address

Output:         None - None

Return:         USB_SUCCESS                 - Reset started
                USB_MSD_DEVICE_NOT_FOUND    - No device with specified address
                USB_MSD_ILLEGAL_REQUEST     - Device is in an illegal state
                                              for reset

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDResetDevice( BYTE deviceAddress );

/*******************************************************************************
Function:       void USBHostMSDTasks( void )

Preconditions:  USBHostMSDInit() has been called.

Overview:       This function performs the maintenance tasks required by the
                mass storage class.  It should be called on a regular basis by
                the application.

Input:          None - None

Output:         None - None

Return:         None - None

Remarks:        None
*******************************************************************************/
void    USBHostMSDTasks( void );

/*******************************************************************************
Function:       void USBHostMSDTerminateTransfer( BYTE deviceAddress )

Preconditions:  None

Overview:       This function terminates a mass storage transfer.  After executing
                this function, the application may have to reset the device.

Input:          BYTE deviceAddress  - Device address

Output:         None - None

Return:         None - None

Remarks:        None
*******************************************************************************/
void    USBHostMSDTerminateTransfer( BYTE deviceAddress );

/*******************************************************************************
Function:       BYTE USBHostMSDTransfer( BYTE deviceAddress, BYTE deviceLUN,
                        BYTE direction, BYTE *commandBlock, BYTE commandBlockLength,
                        BYTE *data, DWORD dataLength )

Preconditions:  None

Overview:       This function starts a mass storage transfer.  Usually, users
                will probably use a read/write wrapper to access this function.

Input:          BYTE deviceAddress      - Device address
                BYTE deviceLUN          - Device LUN to access
                BYTE direction          - 1=read, 0=write
                BYTE *commandBlock      - Pointer to the command block for the CBW
                BYTE commandBlockLength - Length of the command block
                BYTE *data              - Pointer to the data buffer
                DWORD dataLength        - Byte size of the data buffer

Output:         None - None

Return:         USB_SUCCESS                 - Request started successfully
                USB_MSD_DEVICE_NOT_FOUND    - No device with specified address
                USB_DEVICE_BUSY             - Device not in proper state for
                                              performing a transfer
                USB_MSD_INVALID_LUN         - Specified LUN does not exist

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDTransfer( BYTE deviceAddress, BYTE deviceLUN, BYTE direction, BYTE *commandBlock,
                            BYTE commandBlockLength, BYTE *data, DWORD dataLength );

/*******************************************************************************
Function:       BOOL USBHostMSDTransferIsComplete( BYTE deviceAddress,
                        BYTE *errorCode, DWORD *byteCount )

Preconditions:  None

Overview:       This function indicates whether or not the last transfer is complete.
                If the functions returns TRUE, the returned byte count and error
                code are valid. Since only one transfer can be performed at once
                and only one endpoint can be used, we only need to know the
                device address.

Input:          BYTE deviceAddress  - Device address

Output:         BYTE *errorCode     - Error code from last transfer
                DWORD *byteCount    - Number of bytes transferred

Return:         TRUE    - Transfer is complete, errorCode is valid
                FALSE   - Transfer is not complete, errorCode is not valid

Remarks:        None
*******************************************************************************/
BOOL    USBHostMSDTransferIsComplete( BYTE deviceAddress, BYTE *errorCode, DWORD *byteCount );

/*******************************************************************************
Function:       BYTE USBHostMSDWrite( BYTE deviceAddress, BYTE deviceLUN, BYTE *commandBlock,
                            BYTE commandBlockLength, BYTE *data, DWORD dataLength );

Preconditions:  None

Overview:       This function starts a mass storage write, utilizing the function
                USBHostMSDTransfer();

Input:          BYTE deviceAddress      - Device address
                BYTE deviceLUN          - Device LUN to access
                BYTE *commandBlock      - Pointer to the command block for the CBW
                BYTE commandBlockLength - Length of the command block
                BYTE *data              - Pointer to the data buffer
                DWORD dataLength        - Byte size of the data buffer

Output:         None - None

Return:         USB_SUCCESS                 - Request started successfully
                USB_MSD_DEVICE_NOT_FOUND    - No device with specified address
                USB_DEVICE_BUSY             - Device not in proper state for
                                              performing a transfer
                USB_MSD_INVALID_LUN         - Specified LUN does not exist

Remarks:        None
*******************************************************************************/
#define USBHostMSDWrite( deviceAddress,deviceLUN,commandBlock,commandBlockLength,data,dataLength ) \
        USBHostMSDTransfer( deviceAddress, deviceLUN, 0, commandBlock, commandBlockLength, data, dataLength )


#endif
