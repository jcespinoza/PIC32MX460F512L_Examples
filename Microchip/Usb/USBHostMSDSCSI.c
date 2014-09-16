/******************************************************************************

    USB Host Mass Storage Device SCSI Interface Driver

This file provides the interface between the file system and the USB Host Mass
Storage class.  It translates the file system funtionality requirements to the
appropriate SCSI commands, and sends the SCSI commands via the USB Mass
Storage class.

Currently, the file system layer above this interface layer is limited to one
LUN (Logical Unit Number) on a single mass storage device.  This layer accepts
and stores the max LUN from the USB MSD layer, but all sector reads and writes
are hard-coded to LUN 0, since the layer above does not specify a LUN in the
sector read and write commands.  Also, to interface with the existing file
system code, only one attached device is allowed.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* FileName:        USBHostMSDSCSI.c
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
KO          15-Oct-2007 First release

*******************************************************************************/
//DOM-IGNORE-END

#include <stdlib.h>
#include <string.h>
#include "GenericTypeDefs.h"
#include "system.h"
#include "USBConfig.h"
#include "FSconfig.h"
#include "MDD File System/FSDefs.h"
#include "MDD File System/FSIO.h"
#include "USB\USBHost.h"
#include "USB\USBHostMSD.h"
#include "USB\USBHostMSDSCSI.h"

//#define DEBUG_MODE
#if defined(DEBUG_MODE)
    #include "uart2.h"
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

#define FAT_GOOD_SIGN_0             0x55        // The value in the Signature0 field of a valid partition.
#define FAT_GOOD_SIGN_1             0xAA        // The value in the Signature1 field of a valid partition.
#define FO_MBR                      0L          // Master Boot Record sector LBA
#define FUA_ALLOW_CACHE             0x00        // Force Unit Access, allow cache use
#define RDPROTECT_NORMAL            0x00        // Normal Read Protect behavior.
#define WRPROTECT_NORMAL            0x00        // Normal Write Protect behavior.


//******************************************************************************
//******************************************************************************
// Section: Local Prototypes and Macros
//******************************************************************************
//******************************************************************************

#if !defined( USBTasks )
    #define USBTasks()                  \
        {                               \
            USBHostTasks();             \
            USBHostMSDTasks();          \
        }
#endif

CETYPE  _USBHostMSDSCSI_DiskMount( DISK *dsk );
BYTE    _USBHostMSDSCSI_LoadBootSector( DISK *dsk );
CETYPE  _USBHostMSDSCSI_LoadMasterBootRecord( DISK *dsk );

#if defined( PERFORM_TEST_UNIT_READY )
    BOOL    _USBHostMSDSCSI_TestUnitReady( void );
#endif

#define ReadByte(a,b)   a[b]

DWORD   ReadDWord( BYTE* pBuffer, WORD index );
WORD    ReadWord( BYTE* pBuffer, WORD index );


//******************************************************************************
//******************************************************************************
// Section: SCSI MSD Host Global Variables
//******************************************************************************
//******************************************************************************

//******************************************************************************
// Section: Global Interface Variables
//******************************************************************************

DISK                                gDiskData;                      // Global disk information.
BYTE __attribute__ ((aligned(4)))   gFATBuffer[MEDIA_SECTOR_SIZE];  // Global buffer for one File Allocation Table sector.

//******************************************************************************
// Section: Internal Global Variables
//******************************************************************************

BYTE                                deviceAddress   = 0x00;         // USB address of the attached device.
BYTE __attribute__ ((aligned(4)))   gDataBuffer[MEDIA_SECTOR_SIZE]; // Buffer to hold one sector of data.
BYTE                                maxLUN          = 0x00;         // Maximum Logical Unit Number of the attached device.


// *****************************************************************************
// *****************************************************************************
// Section: MSD Host Stack Callback Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BOOL USBMSDSCSIInitialize( BYTE address, DWORD flags )

Precondition:   None

Overview:       This function is called when a USB Mass Storage device is being
                enumerated.

Input:          BYTE address        Address of the new device
                DWORD flags         Initialization flags

Output:         Return:
                    TRUE:   We can support the device.
                    FALSE:  We cannot support the device.

Notes:          The division of functionality between USBMSDSCSIInitialize() and
                USBHostMSDSCSIInit() is based on the structure of the file
                system implementation layer above this layer.  The file system
                layer is polling based, so the initialization performed in the
                event handler USBMSDSCSIInitialize() is minimal.  The majority
                of the initialization is performed in the polling function,
                USBHostMSDSCSIInit().  If the file system is reworked to utilize
                events, the polling function, USBHostMSDSCSIInit(), can probably
                be eliminated.

Tested:
*******************************************************************************/
BOOL USBMSDSCSIInitialize( BYTE address, DWORD flags )
{
    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Device attached.\r\n" );
    #endif

    if (deviceAddress == 0)
    {
        // Save the address of the new device.
        deviceAddress = address;
        return TRUE;
    }
    else
    {
        // We can currently only handle one device.
        return FALSE;
    }
}

/*******************************************************************************
Function:       BOOL USBMSDSCSIEventHandler( BYTE address, USB_EVENT event,
                        void *data, DWORD size )

Precondition:   The device has been initialized.

Overview:       This function is called when various events occur in the
                USB MSD Host layer.

Input:          BYTE address        Address of the device
                USB_EVENT event     Event that has occurred
                void *data          Pointer to data pertinent to the event
                DWORD size          Size of the data

Output:         Return:
                    TRUE:   Event was handled
                    FALSE:  Event was not handled

Notes:          None

Tested:
*******************************************************************************/

BOOL USBMSDSCSIEventHandler( BYTE address, USB_EVENT event, void *data, DWORD size )
{
    if (deviceAddress == address)
    {
        switch( event )
        {
            case EVENT_MSD_NONE:
            case EVENT_MSD_TRANSFER:                 // A MSD transfer has completed
                return TRUE;
                break;

            case EVENT_HUB_ATTACH:                  // USB hub has been attached
                #ifdef DEBUG_MODE
                    UART2PrintString( "SCSI: Hub use is not supported.\r\n" );
                #endif
                return TRUE;
                break;

            case EVENT_MSD_RESET:
                #ifdef DEBUG_MODE
                    UART2PrintString( "SCSI: MSD Reset performed.\r\n" );
                #endif
                return TRUE;
                break;

            case EVENT_MSD_MAX_LUN:
                #ifdef DEBUG_MODE
                    UART2PrintString( "SCSI: Max LUN set.\r\n" );
                #endif
                maxLUN = *((BYTE *)data);
                return TRUE;
                break;

            case EVENT_DETACH:                      // USB cable has been detached (data: BYTE, address of device)
                #ifdef DEBUG_MODE
                    UART2PrintString( "SCSI: Device detached.\r\n" );
                #endif
                deviceAddress = 0;
                return TRUE;
                break;

            default:
                return FALSE;
                break;
        }
    }
    return FALSE;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Callable Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BYTE USBHostMSDSCSIInit( void )

Notes:          The division of functionality between USBMSDSCSIInitialize() and
                USBHostMSDSCSIInit() is based on the structure of the file
                system implementation layer above this layer.  The file system
                layer is polling based, so the initialization performed in the
                event handler USBMSDSCSIInitialize() is minimal.  The majority
                of the initialization is performed in the polling function,
                USBHostMSDSCSIInit().  If the file system is reworked to utilize
                events, the polling function, USBHostMSDSCSIInit(), can probably
                be eliminated.

Tested:
*******************************************************************************/

BYTE USBHostMSDSCSIInit( void )
{
    USBTasks();

    return (_USBHostMSDSCSI_DiskMount( &gDiskData ) == CE_GOOD);
}


/*******************************************************************************
Function:       BYTE USBHostMSDSCSIMediaDetect( void )

Notes:          Since this will often be called in a loop while waiting for
                a device, we'll make sure the tasks are executed.

Tested:
*******************************************************************************/

BYTE USBHostMSDSCSIMediaDetect( void )
{
    USBTasks();

    if (USBHostMSDDeviceStatus( deviceAddress ) == USB_MSD_NORMAL_RUNNING)
    {
        return TRUE;
    }

    return FALSE;
}


/*******************************************************************************
Function:       BOOL USBHostMSDSCSIMediaReset( void  )

Notes:          None

Tested:
*******************************************************************************/

BYTE USBHostMSDSCSIMediaReset( void  )
{
    DWORD   byteCount;
    BYTE    errorCode;

    errorCode = USBHostMSDResetDevice( deviceAddress );
    if (errorCode)
    {
        return errorCode;
    }

    do
    {
        USBTasks();
        errorCode = USBHostMSDDeviceStatus( deviceAddress );
    } while (errorCode == USB_MSD_RESETTING_DEVICE);


    if (USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
    {
        return errorCode;
    }

    return USB_MSD_RESET_ERROR;
}



/*******************************************************************************
Function:       BYTE USBHostMSDSCSISectorRead( DWORD sectorAddress, BYTE *dataBuffer)

Notes:          To follow convention, this function blocks until the read is
                complete.

Command Block:  The READ10 command block is as follows:

        Byte/Bit    7       6       5       4       3       2       1       0
           0                    Operation Code (0x28)
           1        [    RDPROTECT      ]  DPO     FUA      -     FUA_NV    -
           2        [ (MSB)
           3                        Logical Block Address
           4
           5                                                          (LSB) ]
           6        [         -         ][          Group Number            ]
           7        [ (MSB)         Transfer Length
           8                                                          (LSB) ]
           9        [                    Control                            ]

Tested:
*******************************************************************************/

BYTE USBHostMSDSCSISectorRead( DWORD sectorAddress, BYTE *dataBuffer )
{
    DWORD   byteCount;
    BYTE    commandBlock[10];
    BYTE    errorCode;

    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Reading sector " );
        UART2PutHex(sectorAddress >> 24);
        UART2PutHex(sectorAddress >> 16);
        UART2PutHex(sectorAddress >> 8);
        UART2PutHex(sectorAddress);
        UART2PrintString( " Device " );
        UART2PutHex(deviceAddress);
        UART2PrintString( "\r\n" );
    #endif

    if (deviceAddress == 0)
    {
        return USB_MSD_DEVICE_NOT_FOUND;
    }

    // Fill in the command block with the READ10 parameters.
    commandBlock[0] = 0x28;     // Operation code
    commandBlock[1] = RDPROTECT_NORMAL | FUA_ALLOW_CACHE;
    commandBlock[2] = (BYTE) (sectorAddress >> 24);     // Big endian!
    commandBlock[3] = (BYTE) (sectorAddress >> 16);
    commandBlock[4] = (BYTE) (sectorAddress >> 8);
    commandBlock[5] = (BYTE) (sectorAddress);
    commandBlock[6] = 0x00;     // Group Number
    commandBlock[7] = 0x00;     // Number of blocks - Big endian!
    commandBlock[8] = 0x01;
    commandBlock[9] = 0x00;     // Control

    // TODO Currently using LUN=0.  When the File System supports multiple LUN's, this will change.
    errorCode = USBHostMSDRead( deviceAddress, 0, commandBlock, 10, dataBuffer, MEDIA_SECTOR_SIZE );
    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Read sector init error " );
        UART2PutHex( errorCode );
        UART2PrintString( "\r\n" );
    #endif

    if (!errorCode)
    {
        while (!USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
        {
            // TODO Any other timeout?  Maybe at the host level, using the 1ms timer...
            USBTasks();
        }
    }

    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Read sector error " );
        UART2PutHex( errorCode );
        UART2PrintString( "\r\n" );
    #endif

    if (!errorCode)
    {
        return TRUE;
    }
    else
    {
//        USBHostMSDSCSIMediaReset();
        return FALSE;
    }
}

/*******************************************************************************
Function:       BYTE USBHostMSDSCSISectorWrite( DWORD sectorAddress, BYTE *dataBuffer, BYTE allowWriteToZero )

Notes:          To follow convention, this function blocks until the write is
                complete.

Command Block:  The WRITE10 command block is as follows:

        Byte/Bit    7       6       5       4       3       2       1       0
           0                    Operation Code (0x2A)
           1        [    WRPROTECT      ]  DPO     FUA      -     FUA_NV    -
           2        [ (MSB)
           3                        Logical Block Address
           4
           5                                                          (LSB) ]
           6        [         -         ][          Group Number            ]
           7        [ (MSB)         Transfer Length
           8                                                          (LSB) ]
           9        [                    Control                            ]

Tested:
*******************************************************************************/

BYTE USBHostMSDSCSISectorWrite( DWORD sectorAddress, BYTE *dataBuffer, BYTE allowWriteToZero )
{
    DWORD   byteCount;
    BYTE    commandBlock[10];
    BYTE    errorCode;

    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Writing sector " );
        UART2PutHex(sectorAddress >> 24);
        UART2PutHex(sectorAddress >> 16);
        UART2PutHex(sectorAddress >> 8);
        UART2PutHex(sectorAddress);
        UART2PrintString( " Device " );
        UART2PutHex(deviceAddress);
        UART2PrintString( "\r\n" );
    #endif

    if (deviceAddress == 0)
    {
        return USB_MSD_DEVICE_NOT_FOUND;
    }

    if ((sectorAddress == 0) && (allowWriteToZero == FALSE))
    {
        return USB_SCSI_ERROR_SECTOR_0;
    }

    // Fill in the command block with the WRITE 10 parameters.
    commandBlock[0] = 0x2A;     // Operation code
    commandBlock[1] = WRPROTECT_NORMAL | FUA_ALLOW_CACHE;
    commandBlock[2] = (BYTE) (sectorAddress >> 24);     // Big endian!
    commandBlock[3] = (BYTE) (sectorAddress >> 16);
    commandBlock[4] = (BYTE) (sectorAddress >> 8);
    commandBlock[5] = (BYTE) (sectorAddress);
    commandBlock[6] = 0x00;     // Group Number
    commandBlock[7] = 0x00;     // Number of blocks - Big endian!
    commandBlock[8] = 0x01;
    commandBlock[9] = 0x00;     // Control

    // TODO Currently using LUN=0.  When the File System supports multiple LUN's, this will change.
    errorCode = USBHostMSDWrite( deviceAddress, 0, commandBlock, 10, dataBuffer, MEDIA_SECTOR_SIZE );
    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Write sector init error " );
        UART2PutHex( errorCode );
        UART2PrintString( "\r\n" );
    #endif

    if (!errorCode)
    {
        while (!USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
        {
            // TODO Any other timeout?  Maybe at the host level, using the 1ms timer...
            USBTasks();
        }
    }

    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Write sector error " );
        UART2PutHex( errorCode );
        UART2PrintString( "\r\n" );
    #endif

    if (!errorCode)
    {
        return TRUE;
    }
    else
    {
//        USBHostMSDSCSIMediaReset();
        return FALSE;
    }
}


/*******************************************************************************
Function:       BYTE USBHostMSDSCSIWriteProtectState( void )

Notes:          This must be a function rather than a macro for compatibility.

Tested:
*******************************************************************************/

BYTE    USBHostMSDSCSIWriteProtectState( void )
{
    return 0;
}


// *****************************************************************************
// *****************************************************************************
// Section: Internal Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       CETYPE _USBHostMSDSCSI_DiskMount( DISK *dsk )

Precondition:   None

Overview:       This function "mounts" the disk.  It loads the mastor boot
                record and the boot sector.

Input:          DISK *dsk   - Disk to read

Output:         DISK *dsk   - Updated disk information

Return:         CE_GOOD         - Disk mounted
                CE_INIT_ERROR   - Initialization error has occured

Notes:          This function performs the following SCSI commands:
                    * READ CAPACITY 10
                    * REQUEST SENSE

Command Block:  The READ CAPACITY 10 command block is as follows:

        Byte/Bit    7       6       5       4       3       2       1       0
           0                    Operation Code (0x25)
           1        [                      Reserved                         ]
           2        [ (MSB)
           3                        Logical Block Address
           4
           5                                                          (LSB) ]
           6        [                      Reserved
           7                                                                ]
           8        [                      Reserved                 ] [ PMI ]
           9        [                    Control                            ]

                The REQUEST SENSE command block is as follows:

        Byte/Bit    7       6       5       4       3       2       1       0
           0                    Operation Code (0x02)
           1        [                      Reserved                 ] [ DESC]
           2        [                      Reserved
           3                                                                ]
           4        [                  Allocation Length                    ]
           5        [                    Control                            ]

Tested:
*******************************************************************************/

CETYPE _USBHostMSDSCSI_DiskMount( DISK *dsk )
{
    BYTE        attempts;
    DWORD       byteCount;
    BYTE        commandBlock[10];
    BOOL        commandComplete;
    CETYPE      error               = CE_GOOD;
    BYTE        errorCode;
    BYTE        inquiryData[36];

    dsk->mount = FALSE; // default invalid
    dsk->buffer = gDataBuffer;    // assign buffer

    // Make sure the device is still attached.
    if (deviceAddress == 0)
    {
        return USB_MSD_DEVICE_NOT_FOUND;
    }

    attempts = 3;
    commandComplete = FALSE;
    while ((attempts != 0) && !commandComplete)
    {
        attempts --;

        #ifdef DEBUG_MODE
            UART2PrintString( "SCSI: READ CAPACITY 10..." );
        #endif

        // Fill in the command block with the READ CAPACITY 10 parameters.
        commandBlock[0] = 0x25;     // Operation Code
        commandBlock[1] = 0;        //
        commandBlock[2] = 0;        //
        commandBlock[3] = 0;        //
        commandBlock[4] = 0;        //
        commandBlock[5] = 0;        //
        commandBlock[6] = 0;        //
        commandBlock[7] = 0;        //
        commandBlock[8] = 0;        //
        commandBlock[9] = 0x00;     // Control

        errorCode = USBHostMSDRead( deviceAddress, 0, commandBlock, 10, inquiryData, 8 );
        #ifdef DEBUG_MODE
            UART2PutHex( errorCode ) ;
            UART2PutChar( ' ' );
        #endif

        if (!errorCode)
        {
            while (!USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
            {
                // TODO Any other timeout?  Maybe at the host level, using the 1ms timer...
                USBTasks();
            }
        }

        if (!errorCode)
        {
            commandComplete = TRUE;

            #ifdef DEBUG_MODE
                UART2PutHex( errorCode ) ;
                UART2PrintString( "\r\n" );
            #endif

            // Check for a 512 byte sector size
            if (!((inquiryData[4] == 0) && (inquiryData[5] == 0) &&
                  (inquiryData[6] == 2) && (inquiryData[7] == 0)))
            {
                #ifdef DEBUG_MODE
                    UART2PrintString( "SCSI: Bad sector size\r\n" );
                #endif
                return USB_SCSI_ERROR_SECTOR_SIZE;
            }
        }
        else
        {
            // Perform a Request Sense to try to clear the stall.
            #ifdef DEBUG_MODE
                UART2PrintString( "SCSI: REQUEST SENSE..." );
            #endif

            // Fill in the command block with the REQUEST SENSE parameters.
            commandBlock[0] = 0x03;     // Operation Code
            commandBlock[1] = 0;        //
            commandBlock[2] = 0;        //
            commandBlock[3] = 0;        //
            commandBlock[4] = 18;       // Allocation length
            commandBlock[5] = 0;        // Control

            errorCode = USBHostMSDRead( deviceAddress, 0, commandBlock, 6, inquiryData, 18 );
            #ifdef DEBUG_MODE
                UART2PutHex( errorCode ) ;
                UART2PutChar( ' ' );
            #endif

            if (!errorCode)
            {
                while (!USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
                {
                    // TODO Any other timeout?  Maybe at the host level, using the 1ms timer...
                    USBTasks();
                }
            }
        }
    }

    if (!commandComplete)
    {
        return USB_SCSI_ERROR_DISK_MOUNT;
    }

    // Load the Master Boot Record (partition)
    if ((error = _USBHostMSDSCSI_LoadMasterBootRecord( dsk )) == CE_GOOD)
    {
        // Load the boot sector
        if ((error = _USBHostMSDSCSI_LoadBootSector( dsk )) == CE_GOOD)
        {
            dsk->mount = TRUE; // Mark that the DISK mounted successfully
        }
    }

    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: Mount: " );
        UART2PutHex( error );
        UART2PrintString( "\r\n" );
    #endif

    return(error);
}


/*******************************************************************************
Function:       BYTE _USBHostMSDSCSI_LoadBootSector( DISK *dsk )

Precondition:   None

Overview:       This function loads the boot sector and extracts the required
                information.

Input:          DISK *dsk   - Disk to read

Output:         DISK *dsk   - Updated disk information

Return:         CE_GOOD             - Boot sector loaded
                CE_BAD_SECTOR_READ  - A bad read occured of a sector
                CE_NOT_FORMATTED    - The disk is of an unsupported format
                CE_CARDFAT12        - FAT12 during intial testing we are not supporting FAT12
                CE_CARDFAT32        - FAT 32 - card not supported

Notes:          None

Tested:
*******************************************************************************/

BYTE _USBHostMSDSCSI_LoadBootSector( DISK *dsk )
{
    BootSec bootSector;        // boot sector, assume its FAT16 til we know better
    WORD    bytesPerSector;
    DWORD   dataSectors;
    BYTE    error = CE_GOOD;
    WORD    rootDirectorySectors;
    DWORD   totalSectors;

    // Get the Boot sector
    if ( USBHostMSDSCSISectorRead( dsk->firsts, dsk->buffer) != TRUE)
    {
        error = CE_BAD_SECTOR_READ;
    }
    else
    {
        // Assign the type.
        bootSector = (BootSec)dsk->buffer;

        //Verify that the Boot Sector is valid.
        if((bootSector->Signature0 != FAT_GOOD_SIGN_0) || (bootSector->Signature1 != FAT_GOOD_SIGN_1))
        {
            error = CE_NOT_FORMATTED;
        }
        else
        {
            // Determine the number of sectors in one FAT (FATSz).
            dsk->fatsize    = ReadWord( dsk->buffer, BSI_SPF );

            // Determine the total number of sectors.
            totalSectors    = ReadWord( dsk->buffer, BSI_TOTSEC16 );
            if( totalSectors == 0 )
            {
                totalSectors = ReadDWord( dsk->buffer, BSI_TOTSEC32 );
            }

            // 6.  Get the full partition/drive layout
            // 6.1 Determine the size of a cluster
            dsk->SecPerClus = ReadByte( dsk->buffer, BSI_SPC );

            // 6.2 Determine FAT, root and data lbas
            // FAT = first sector in partition (boot record) + reserved records
            dsk->fat        = dsk->firsts + ReadWord( dsk->buffer, BSI_RESRVSEC );

            // 6.3 fatcopy is the number of FAT tables
            dsk->fatcopy    = ReadByte( dsk->buffer, BSI_FATCOUNT );

            // 6.4 MAX ROOT is the maximum number of entries in the root directory
            dsk->maxroot    = ReadWord( dsk->buffer, BSI_ROOTDIRENTS );

            // The "root" is the sector location of the root directory
            dsk->root       = dsk->fat + (dsk->fatcopy * dsk->fatsize);

            bytesPerSector  = ReadWord( dsk->buffer, BSI_BPS );
            if ((bytesPerSector == 0) || ((bytesPerSector & 1) == 1)) //cannot be 0 or odd
            {
                return( CE_NOT_FORMATTED );
            }

            rootDirectorySectors = ((dsk->maxroot * 32) + (bytesPerSector - 1)) / bytesPerSector;

            // Determine how many data sectors there are.
            dataSectors = totalSectors - (dsk->root + rootDirectorySectors) + dsk->firsts + 2;

            dsk->maxcls = dataSectors / dsk->SecPerClus;

            // Determine file system type
            if(dsk->maxcls < 4085)
            {
                /* Volume is FAT12 */
                dsk->type = FAT12;
            }
            else
            {
                if(dsk->maxcls < 65525)
                {
                    /* Volume is FAT16 */
                    dsk->type = FAT16;
                }
                else
                    /* Volume is FAT32 */
                    // We don't support FAT32
                    error = CE_CARDFAT32;
            }

            // 6.5 DATA = ROOT + (MAXIMUM ROOT *32 / 512)
            dsk->data = dsk->root + ( dsk->maxroot >> 4); // assuming maxroot % 16 == 0!!!

            // Make sure that the sector size is compatible with our code.
            if (bytesPerSector != MEDIA_SECTOR_SIZE)
            {
                error = CE_NOT_FORMATTED;
            }
        }
    }

    return(error);
}


/*******************************************************************************
Function:       CETYPE _USBHostMSDSCSI_LoadMasterBootRecord( DISK *dsk )

Precondition:   None

Overview:       This function loads the master boot record and extracts the required
                information.

Input:          DISK *dsk   - Disk to read

Output:         DISK *dsk   - Updated disk information

Return:         CE_GOOD             - Boot sector loaded
                CE_BAD_SECTOR_READ  - A bad read occured of a sector
                CE_BAD_PARTITION    - The boot record is bad

Notes:          None

Tested:
*******************************************************************************/

CETYPE _USBHostMSDSCSI_LoadMasterBootRecord( DISK *dsk )
{
    BootSec bootSector;
    CETYPE  error = CE_GOOD;
    PT_MBR  partition;
    BYTE    type;

    // Get the partition table from the Master Boot Record.
    if (USBHostMSDSCSISectorRead( FO_MBR, dsk->buffer ) != TRUE)
        error = CE_BAD_SECTOR_READ;
    else
    {
        // Check if there is no Master Boot Record.
        bootSector = (BootSec) dsk->buffer;

        if((bootSector->Signature0 == FAT_GOOD_SIGN_0) && (bootSector->Signature1 == FAT_GOOD_SIGN_1))
        {
            // Technically, the OEM name is not for indication.
            // The alternative is to read the CIS from attribute
            // memory.  See the PCMCIA metaformat for more details.
            if (ReadByte( dsk->buffer, BSI_FSTYPE     ) == 'F' &&
                ReadByte( dsk->buffer, BSI_FSTYPE + 1 ) == 'A' &&
                ReadByte( dsk->buffer, BSI_FSTYPE + 2 ) == 'T' &&
                ReadByte( dsk->buffer, BSI_FSTYPE + 3 ) == '1' &&
                ReadByte( dsk->buffer, BSI_FSTYPE + 4 ) == '6' &&
                ReadByte( dsk->buffer, BSI_BOOTSIG) == 0x29)
            {
                dsk->firsts = 0;
                dsk->type = FAT16;
            }
        }

        // Look at the data buffer as a partition table.
        partition = (PT_MBR)dsk->buffer;

        // Ensure that the partition table is good.
        if ((partition->Signature0 != FAT_GOOD_SIGN_0) || (partition->Signature1 != FAT_GOOD_SIGN_1))
        {
            error = CE_BAD_PARTITION;
        }
        else
        {
            // A valid Master Boot Record is loaded.

            // Get the 32 bit offset to the first partition.
            dsk->firsts = partition->Partition0.PTE_FrstSect;

            // See if the partition type is acceptable.
            type = partition->Partition0.PTE_FSDesc;

            switch (type)
            {
                case 0x01:
                    dsk->type = FAT12;
                    break;

                case 0x04:
                case 0x06:
                case 0x0E:
                    dsk->type = FAT16;
                    break;

                case 0x0B:
                case 0x0C:
                    dsk->type = FAT32;
                    error = CE_CARDFAT32;
                    break;

                default:
                    error = CE_BAD_PARTITION;
            }
        }
    }

    return(error);
}


/*******************************************************************************
Function:       BOOL _USBHostMSDSCSI_TestUnitReady( void )

Precondition:   None

Overview:       This function sends the TEST UNIT READY SCSI command

Input:          None - None

Output:         None - None

Return:         TRUE    - Command completed without error
                FALSE   - Error while performing command

Notes:          None

Command Block:  The format of the TEST UNIT READY command is as follows:

        Byte/Bit    7       6       5       4       3       2       1       0
           0                    Operation Code (0x00)
           1        [
           2                               Reserved
           3
           4                                                                ]
           5        [                    Control                            ]

Tested:
*******************************************************************************/

#ifdef PERFORM_TEST_UNIT_READY
BOOL _USBHostMSDSCSI_TestUnitReady( void )
{
    DWORD       byteCount;
    BYTE        commandBlock[10];
    BYTE        errorCode;
    BYTE        inquiryData[36];
    BYTE        unitReadyCount;

    // Issue a TEST UNIT READY
    #ifdef DEBUG_MODE
        UART2PrintString( "SCSI: TEST UNIT READY..." );
    #endif

    unitReadyCount = 0;
    while (unitReadyCount < 5)
    {
        unitReadyCount ++;

        // Fill in the command block with the TEST UNIT READY parameters.
        commandBlock[0] = 0x00;     // Operation Code
        commandBlock[1] = 0;        // Reserved
        commandBlock[2] = 0;        // Reserved
        commandBlock[3] = 0;        // Reserved
        commandBlock[4] = 0;        // Reserved
        commandBlock[5] = 0x00;     // Control

        errorCode = USBHostMSDRead( deviceAddress, 0, commandBlock, 6, inquiryData, 0 );
        #ifdef DEBUG_MODE
            UART2PutHex( errorCode ) ;
            UART2PutChar( ' ' );
        #endif

        if (!errorCode)
        {
            while (!USBHostMSDTransferIsComplete( deviceAddress, &errorCode, &byteCount ))
            {
                USBTasks();
            }
        }
        #ifdef DEBUG_MODE
            UART2PutHex( errorCode ) ;
            UART2PrintString( "\r\n" );
        #endif
        if (!errorCode)
        {
            return TRUE;
        }
    }

    return FALSE;
}
#endif


/*******************************************************************************
Function:       WORD ReadWord( BYTE *pBuffer, WORD index )

Precondition:   None

Overview:       This function reads a 16-bit word from a byte buffer.

Input:          BYTE *pBuffer   - pointer to data buffer
                WORD index      - starting point of data in the buffer

Output:         None - None

Return:         WORD - Word value at specified location

Notes:          This function is necessary, since the WORD may not be aligned
                in the byte buffer.

Tested:
*******************************************************************************/

WORD ReadWord( BYTE* pBuffer, WORD index )
{
    BYTE    loByte, hiByte;
    WORD    res;

    loByte = pBuffer[index];
    hiByte = pBuffer[index+1];

    res = hiByte;
    res *= 0x100;
    res |= loByte;
    return( res );
}

/*******************************************************************************
Function:       DWORD ReadDWord( BYTE *pBuffer, WORD index )

Precondition:   None

Overview:       This function reads a 32-bit double word from a byte buffer.

Input:          BYTE *pBuffer   - pointer to data buffer
                WORD index      - starting point of data in the buffer

Output:         None - None

Return:         DWORD - Double word at specified location

Notes:          This function is necessary, since the DWORD may not be aligned
                in the byte buffer.

Tested:
*******************************************************************************/

DWORD ReadDWord( BYTE *pBuffer, WORD index )
{
    WORD    loWord, hiWord;
    DWORD   result;

    loWord = ReadWord( pBuffer, index );
    hiWord = ReadWord( pBuffer, index+2 );

    result = hiWord;
    result *= 0x10000;
    result |= loWord;
    return result;
}

