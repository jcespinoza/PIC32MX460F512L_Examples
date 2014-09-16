/*******************************************************************************

  USB Host Mass Storage Device SCSI Interface Driver Header File

This is the header file for a USB Host device that is using a SCSI interface to
the Mass Storage Class.  This file should be included with USBHost.h and
USBHostMSD.h to provide the USB MSD interface.  It must be included after the
application-specific USBConfig.h file, the USB Host header file USBHost.h, and
the USB Host MSD header file USBHostMSD.h, as definitions in those files are
required for proper compilation.

The functions in this file are designed to interface the Microchip Memory Disk
Drive File System library (see Application Note AN1045) to the USB Host Mass
Storage Class, allowing a PIC application to utilize mass storage devices such
as USB flash drives.  For ease of integration, this file contains macros to
allow the File System code to reference the functions in this file.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************

* FileName:        USBHostMSDSCSI.h
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

*******************************************************************************/

//DOM-IGNORE-BEGIN
#ifndef __USBHOSTMSDSCSI_H__
#define __USBHOSTMSDSCSI_H__
//DOM-IGNORE-END

#include "USBConfig.h"
#include "FSConfig.h"
//#include "USBHost.h"
//#include "USBHostMSD.h"

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// SCSI Error Codes
// *****************************************************************************

#define USB_SCSI_ERROR_DISK_MOUNT   0xF0        // Unable to initiate proper communications with the attached device.
#define USB_SCSI_ERROR_SECTOR_0     0xF1        // Writes to sector 0 are not allowed.
#define USB_SCSI_ERROR_SECTOR_SIZE  0xF2        // Only devices with a sector size of 512 are supported.

extern DISK    gDiskData;                       // Global disk information.
extern BYTE    gFATBuffer[MEDIA_SECTOR_SIZE];   // Global buffer for one File Allocation Table sector.


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BYTE USBHostMSDSCSIInit( void )

Preconditions:  None

Overview:       This function loads the master boot record and extracts the required
                information.

Input:          None - None

Output:         None - None

Return:         CE_GOOD                     - Boot sector loaded
                USB_MSD_DEVICE_NOT_FOUND    - No device attached.
                USB_SCSI_ERROR_SECTOR_SIZE  - Sector size must be 512.
                USB_SCSI_ERROR_DISK_MOUNT   - Cannot communicate with attached device.
                CE_BAD_SECTOR_READ          - A bad read occured of a sector
                CE_NOT_FORMATTED            - The disk is of an unsupported format
                CE_CARDFAT32                - FAT 32 - card not supported

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDSCSIInit( void );

/*******************************************************************************
Function:       BYTE USBHostMSDSCSIMediaDetect( void )

Preconditions:  None

Overview:       This function determines if a mass storage device is attached
                and ready to use.

Input:          None - None

Output:         None - None

Return:         TRUE:   MSD present and ready
                FALSE:  MSD not present or not ready

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDSCSIMediaDetect( void );

/*******************************************************************************
Function:       BOOL USBHostMSDSCSIMediaReset( void  )

Preconditions:  None

Overview:       This function resets the media.  It is called if an operation
                returns an error.  Or the application can call it.

Input:          None - None

Output:         None - None

Return:         USB_SUCCESS                 - Reset successful
                USB_MSD_DEVICE_NOT_FOUND    - No device with specified address
                USB_ILLEGAL_REQUEST         - Device is in an illegal USB state
                                              for reset

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDSCSIMediaReset( void  );

/*******************************************************************************
Function:       BYTE USBHostMSDSCSISectorRead( DWORD sectorAddress, BYTE *dataBuffer)

Preconditions:  None

Overview:       This function uses the SCSI command READ10 to read a  512-byte
                sector.  The data is stored in the application buffer.

Input:          DWORD   sectorAddress   - address of sector to read
                BYTE    *dataBuffer     - buffer to store data

Output:         None - None

Return:         USB_SUCCESS - read performed successfully
                Other       - Error codes from USBHostMSDRead() and
                                USBHostMSDTransferIsComplete()

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDSCSISectorRead( DWORD sectorAddress, BYTE *dataBuffer );

/*******************************************************************************
Function:       BYTE USBHostMSDSCSISectorWrite( DWORD sectorAddress, BYTE *dataBuffer, BYTE allowWriteToZero )

Preconditions:  None

Overview:       This function uses the SCSI command WRITE10 to write a 512-byte
                sector.  The data is read from the application buffer.

Input:          DWORD   sectorAddress   - address of sector to write
                BYTE    *dataBuffer     - buffer with application data
                BYTE    allowWriteToZero- If a write to sector 0 is allowed.

Output:         None - None

Return:         USB_SUCCESS - Write performed successfully
                Other       - Error codes from USBHostMSDWrite() and
                                USBHostMSDTransferIsComplete()

Remarks:        None
*******************************************************************************/
BYTE    USBHostMSDSCSISectorWrite( DWORD sectorAddress, BYTE *dataBuffer, BYTE allowWriteToZero);

/*******************************************************************************
Function:       BYTE USBHostMSDSCSIWriteProtectState( void )

Preconditions:  None

Overview:       This function returns the write protect status of the device.

Input:          None - None

Output:         None - None

Return:         0 - not write protected

Remarks:        This function is not yet implemented.
*******************************************************************************/
BYTE    USBHostMSDSCSIWriteProtectState( void );


// *****************************************************************************
// *****************************************************************************
// Compatibility Definitions
// *****************************************************************************
// *****************************************************************************

#define InitIO              USBHostMSDSCSIInit              // Used to access USBHostMSDSCSIInit(), for compatibility with the File System code.
#define MediaDetect         USBHostMSDSCSIMediaDetect       // Used to access USBHostMSDSCSIMediaDetect(), for compatibility with the File System code.
#define SectorRead          USBHostMSDSCSISectorRead        // Used to access USBHostMSDSCSISectorRead(), for compatibility with the File System code.
#define SectorWrite         USBHostMSDSCSISectorWrite       // Used to access USBHostMSDSCSISectorWrite(), for compatibility with the File System code.
#define WriteProtectState   USBHostMSDSCSIWriteProtectState // Used to access USBHostMSDSCSIWriteProtectState(), for compatibility with the File System code.

#endif
