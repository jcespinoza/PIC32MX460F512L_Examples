/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright © 2008 Microchip Technology Inc. and its licensors.  All         
rights reserved.                                                                
                                                                                
Microchip licenses to you the right to: (1) install Software on a single        
computer and use the Software with Microchip 16-bit microcontrollers and        
16-bit digital signal controllers ("Microchip Product"); and (2) at your        
own discretion and risk, use, modify, copy and distribute the device            
driver files of the Software that are provided to you in Source Code;           
provided that such Device Drivers are only used with Microchip Products         
and that no open source or free software is incorporated into the Device        
Drivers without Microchip's prior written consent in each instance.             
                                                                                
You should refer to the license agreement accompanying this Software for        
additional information regarding your rights and obligations.                   
                                                                                
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY         
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY              
WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A          
PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE             
LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY,               
CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY           
DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY         
INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR         
LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY,                 
SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY         
DEFENSE THEREOF), OR OTHER SIMILAR COSTS.                                       
                                                                                
********************************************************************************
*/

#include <string.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "boot_config.h"
#include "boot_load.h"
#include <plib.h>

#define PA_TO_KVA1(pa)	((void *) ((pa) | 0xa0000000))

//******************************************************************************
//******************************************************************************
// Constants
//******************************************************************************
//******************************************************************************

// Record status codes
#define RECORD_DATA                     0   // Data record found
#define RECORD_FOUND          RECORD_DATA   // Alias for RECORD_DATA
#define RECORD_NON_DATA                 1   // Identified non-data record
#define RECORD_EOF                      2   // Last record in the file
#define RECORD_NOT_FOUND               10   // No record found
#define RECORD_UNSUPPORTED             11   // Unable to identify record type
#define RECORD_CHECKSUM_ERR            12   // Record was corrupted

// Record buffer array size
#define MAX_RECORD_LENGTH             255   // Max Hex-Record Length (converted)


//******************************************************************************
//******************************************************************************
// Global Data
//******************************************************************************
//******************************************************************************


// This structure holds the translated version of the hex record
typedef struct
{
    unsigned char       RecordLength;   // Length record data payload (adjusted 
                                        //   if Offset is non-zero).
    unsigned int        LoadOffset;     // 16-bit offset to which the data will 
                                        //   be loaded
    unsigned char       RecordType;     // Type of data in the record
    unsigned char       ValidIndex;     // Indicates first valid byte in a record

    unsigned char       data[MAX_RECORD_LENGTH];      // Record data buffer

} RECORD_STRUCT; // hexadecimal format data for transfer to aggregator

// Buffer used to build a translated record.
RECORD_STRUCT   recordBuffer;

// Base address of current section being written to Flash.
unsigned long int baseAddress = 0;

// Flash block erasure tracking
//
// Each bit represents one Flash block.  Assumes an unsigned long int has 32 bits.
unsigned long int flashPageStatus[NUMBER_OF_FLASH_BLOCKS/32];


//******************************************************************************
//******************************************************************************
// Local Routines
//******************************************************************************
//******************************************************************************

/****************************************************************************
  Function:
    void TrackPageEraseInit(void)

  Description:
    Initializes array used to track pages that have been erased.
    
  Precondition:
    None

  Parameters:
    None
    
  Returns:
    None
        
  Remarks:
    None
***************************************************************************/
void TrackPageEraseInit(void)
{
    // Clear all bits in the tracking array.
    memset(flashPageStatus, 0, sizeof(flashPageStatus));

} //TrackPageEraseInit


/****************************************************************************
  Function:
    unsigned char TrackPageEraseTest(unsigned long int currentAddress)

  Description:
    Checks to see if a given page has been erased (or if it needs to be 
    erased). 
    
  Precondition:
    None

  Parameters:
    PageAddress         - Address of page to test for prior erasure

  Returns:
    TRUE                - Page has aready been erased
    FALSE               - Page has not yet been erased
    
  Remarks:
    None
***************************************************************************/

BOOL TrackPageEraseTest(unsigned long int PageAddress)
{
    unsigned long int PageNumber;

    // Calculate page address
    PageNumber = (PageAddress - PROGRAM_FLASH_BASE) / FLASH_BLOCK_SIZE;
    
    // Check the bit that tracks that page
    //
    // Note:  Each bit tracks 1 page so each word tracks 32 pages
    //   Word position = PageNumber / 32
    //   Bit  position = PageNumber % 32
    if ( flashPageStatus[PageNumber/32] & (1 << (PageNumber%32)) )
    {
        // page has been erased
        return TRUE;
    }
    else
    {
        // page has not been erased yet
        return FALSE;
    }

} //TrackPageEraseTest
    

/****************************************************************************
  Function:
    void TrackPageErase(unsigned long int PageAddress, BOOL PageErased)
    
  Description:
    This routine sets (or clears) a bit in a tracking array to track if a
    Flash page has been erased or if it needs to be erased.

  Precondition:
    None, but the tracking array will not contain correct data about the
    other Flash pages if it has not been properly initialized.

  Parameters:
    PageAddress     - Address of the page being erased
    PageErased      - Flag indicating if the page has been erased or if it
                      needs to be erased.

  Returns:
    None
    
  Remarks:
    None
    
***************************************************************************/

// Parameter flags definitions
#define PAGE_ERASED         TRUE
#define PAGE_NOT_ERASED     FALSE

void TrackPageErase(unsigned long int PageAddress, BOOL PageErased)
{
    unsigned long int PageNumber;

    // Identify which page was just erased.
    PageNumber = (PageAddress - PROGRAM_FLASH_BASE) / FLASH_BLOCK_SIZE;
    
    // Update the bit that tracks that page
    //
    // Note:  Each bit tracks 1 page so each word tracks 32 pages
    //   Word position = PageNumber / 32
    //   Bit  position = PageNumber % 32
    if (PageErased)
    {
        // Set the bit to indicate the page has been erased
        flashPageStatus[PageNumber/32] |=  ( 1 << (PageNumber % 32) );
    }
    else
    {
        // Clear the bit to indicate the page needs to be erased
        flashPageStatus[PageNumber/32] &= ~( 1 << (PageNumber % 32) );
    }

} //TrackPageErase


/****************************************************************************
  Function:
    unsigned char AsciiToHexNibble(unsigned char data)
    
  Description:
    Converts an ASCII byte in the range of "0 - 9", "A - F", or "a - f" to a 
    hex nibble
    
  Precondition:
    None

  Parameters:
    data            - char containing the data to convert
    
  Returns:
    hex value represented by the input ASCII character (0x00 - 0x0F)
    0 for input values out of range.

  Remarks:
     Example:         value = AsciiToHexNibble('A'); // value = 0x0A
***************************************************************************/

unsigned char AsciiToHexNibble(unsigned char data)
{
    if (data < '0')                     // return 0 for an invalid characters
    {
        return 0;
    }
    else if (data <= '9')               // handle numbers
    {
        return ( data - '0' );
    }
    else if (data < 'A')
    {
        return 0;
    }
    else if (data <= 'F')               // handle uppercase letters
    {
        return ( data - 'A' + 10 );
    }
    else if (data < 'a')
    {
        return 0;
    }
    else if (data <= 'f')               // handle lowercase letters
    {
        return ( data - 'a' + 10 );
    }
    else
    {
        return 0;
    }

} // AsciiToHexNibble


/****************************************************************************
  Function:
    unsigned char AsciiToHexByte ( unsigned char m, unsigned char, l )

  Description:
    Converts two ASCII characters representing hex nibbles into a single
    hex (or binary) byte.
    
  Precondition:
    None

  Parameters:
    m           - Most significant nibble (in ASCII-hex representation)
    l           - Least significant nibble (in ASCII-hex representation)

  Returns:
    1-byte hex (binary) value represented by the two ASCII nibbles

  Remarks:
    Implemented as a macro
***************************************************************************/

#define AsciiToHexByte(m,l) ( (AsciiToHexNibble(m) << 4 ) | AsciiToHexNibble(l) )


/****************************************************************************
  Function:
    unsigned int FindHexRecord ( char *pBufferData, unsigned int nSize )

  Description:
    Scans the specified buffer from start to end seeking the start of 
    record character (':') and provides the position of the first byte of 
    the record (past the start character).
    
  Precondition:
    None

  Parameters:
    pBufferData     - Pointer to buffer
    nSize           - Size of the buffer in bytes

  Returns:
    Postion starting position record (next byte after the start of record
    character ':').  If the return value is greater or equal to the size of
    the given buffer, no record was found.

  Remarks:
    None
***************************************************************************/

unsigned int FindHexRecord ( char *pBufferData, unsigned int nSize )
{
    unsigned int position;  // Position of record in buffer
    
    // Scan the buffer for the start-of-record character
    for (position = 0; position < nSize; position++)
    {
        if ( pBufferData[position] == ':' )
        {
            // Skip the start-of-record character
            position++;
            break;
        }
    }
    return (position);

} // FindRecord


/****************************************************************************
  Function:
    void RecordInitialize( RECORD_STRUCT *pRecord )

  Description:
    Initializes a Record structure
    
  Precondition:
    None

  Parameters:
    pRecord     - Pointer to the record data structure

  Returns:
    None

  Remarks:
    Implemented as a macro
***************************************************************************/

#define RecordInitialize(r)    ((r)->RecordLength = 0,(r)->ValidIndex = 0)


/****************************************************************************
  Function:
    unsigned char ProcessRecord( RECORD_STRUCT *pRecord )

  Description:
    Executes the command for each hexfile record type.
    
  Precondition:
    pRecord must contain valid record data

  Parameters:
    pRecord               - Pointer to the record

  Returns:
    RECORD_DATA           - Data record found
    RECORD_NON_DATA       - Identified non-data record
    RECORD_EOF            - Last record in the file
    RECORD_NOT_FOUND      - No record found
    RECORD_UNSUPPORTED    - Unable to identify record type
    RECORD_CHECKSUM_ERR   - Record was corrupted

  Remarks:
    None
***************************************************************************/

unsigned char ProcessRecord( RECORD_STRUCT *pRecord )
{
    // executes command from the hex file line
    unsigned char   returnVal = 0;
        
    switch (pRecord->RecordType)
    {
    
    case 0x00: // data entry
        returnVal = RECORD_DATA;
        break;
    
    case 0x01: // end of file
        // this was the last record in the hexfile
        RecordInitialize(pRecord);
        returnVal = RECORD_EOF; 
        break;
    
    case 0x02: // Extended segment address record
        RecordInitialize(pRecord);
        returnVal = RECORD_UNSUPPORTED;
        break;
    
    case 0x03: // Start Segment Address record
        RecordInitialize(pRecord);
        returnVal = RECORD_UNSUPPORTED;
        break;
    
    case 0x04: // Extended Linear Address record
        // set upper 16bits to value from record, lower 16bits are forced to 0 ** update to point to ULBA
        baseAddress = (pRecord->data[0] << 8) | pRecord->data[1];
        baseAddress = baseAddress << 16;
        RecordInitialize(pRecord);
        returnVal   = RECORD_NON_DATA;
        break;
    
    case 0x05: // Start Linear Address record
        RecordInitialize(pRecord);
        returnVal = RECORD_UNSUPPORTED;
        break;

    #ifdef EXTENDED_HEXFILE_SUPPORT // non-Intel commands
                
    // If this mode is not enabled extened commands will generate 
    // unsupported hexfile command errors(messages), they will be ignored

    case 0x10: // serial number
        // Only allow programming if the stored SN matches the one stored in the file
        returnVal = Loader_ValidateSerialNumber(pRecord->data, pRecord->RecordLength);
        RecordInitialize(pRecord);
        break;

    case 0x11: // revision number
        // Intended to  only allow programming if current revision is 
        // lower than the one contained in the file
        returnVal = Loader_ValidateRevisionNumber(pRecord->data, pRecord->RecordLength);
        RecordInitialize(pRecord);
        break;

    case 0x12: // Check Error Detection Accumulator (csum, CRC, MD-5, etc)
        returnVal = Loader_CheckErrorDetection(pRecord->data, pRecord->RecordLength);
        RecordInitialize(pRecord);
        break;
            
    #endif
    
    default:
        RecordInitialize(pRecord);
        returnVal = RECORD_UNSUPPORTED;
        break;

    } // switch
    
    return (returnVal);

} //DecodeHexFileCommand()


/****************************************************************************
  Function:
    unsigned int GetRecord( RECORD_STRUCT *pOutputBuffer,  
                            unsigned int  *nInput, 
                            char          *pInputBuffer ) 

  Description:
    Takes data from raw hex buffer and translates it into a record structure
    
  Precondition:
    The pInputBuffer must point to nInput bytes of hex-record format data.

  Parameters:
    pOutputBuffer       - Pointer to a record structure
    nInput              - Number of bytes of raw data available (updated)
    pInputBuffer        - Pointer to input data buffer

  Returns:
    RECORD_FOUND        - Record found, translated, placed in output buffer,
                          and "removed" from input buffer
    RECORD_NOT_FOUND    - No record found, no data removed from input buffer
    RECORD_CHECKSUM_ERR - Record was corrupted
    
  Note:  
    If RECORD_FOUND is returned, *nInput will be reduced by the number of 
    ASCII bytes that made up the record (plus any preceeding bytes).
  
  Record format:
    +--------------------------------------------------------------+
    | ':' | RecLen | Load Offset | RecType | Data Payload | ChkSum |
    +--------------------------------------------------------------+
      -1  | 0   1  | 2  3  4  5  | 6   7   | 0 ...  n*2  | +0  +1  |
      
***************************************************************************/

unsigned int GetRecord( RECORD_STRUCT *pOutputBuffer,  unsigned int *nInput, char *pInputBuffer ) 
{
    unsigned char   Sum = 0;                // Accumulated sum of translated bytes
    unsigned char   Temp;                   // Used for MSB of multi-byte values
    unsigned char   iData;                  // Data payload index variable
    unsigned int    nUsed;                  // Number of input buffer data bytes used
    unsigned char   RecordLength;           // Length record data payload
    unsigned int    LoadOffset;             // 16-bit offset to which the data will be loaded
    unsigned char   RecordType;             // Type of data in the record
    unsigned char   Checksum;               // Checksum given in the record

    // Check to make sure the record buffer is empty
    if ( pOutputBuffer->RecordLength - pOutputBuffer->ValidIndex > 0 )
    {
        return RECORD_FOUND;
    }

    // Make sure we have input data
    if (*nInput > 0)
    {
        // Scan for a start of record
        nUsed         = FindHexRecord(pInputBuffer, *nInput);
        pInputBuffer += nUsed;

        // Check to see of we have enough data for a record (10 bytes overhead)
        if ( nUsed + 8  < *nInput )
        {
            //
            // Translate record and accumulate checksum:
            //

            // Extract the record length
            RecordLength  = AsciiToHexByte(pInputBuffer[0], pInputBuffer[1]);
            Sum          += RecordLength;

            // Extract the load offset
            Temp          = AsciiToHexByte(pInputBuffer[2], pInputBuffer[3]);
            Sum          += Temp;
            LoadOffset    = AsciiToHexByte(pInputBuffer[4], pInputBuffer[5]);
            Sum          += LoadOffset;
            LoadOffset   += Temp << 8;

            // Extract the record type
            RecordType    = AsciiToHexByte(pInputBuffer[6], pInputBuffer[7]);
            Sum          += RecordType;

            pInputBuffer += 8;


            // Make sure the input buffer contains enough data for the payload and checksum
            nUsed += (2 * RecordLength) + 10;
            if ( nUsed > *nInput )
            {
                // Insufficient data to form a record
                return RECORD_NOT_FOUND;
            }

            // Extract the data payload
            for (iData=0; iData/2 < RecordLength; iData+=2)
            {
                pOutputBuffer->data[iData/2]  = AsciiToHexByte(pInputBuffer[iData+0], pInputBuffer[iData+1]);
                Sum                          += pOutputBuffer->data[iData/2];
            }

            pInputBuffer += iData;


            // Extract and verify check sum
            Checksum  = AsciiToHexByte(pInputBuffer[0], pInputBuffer[1]);
            Sum      += Checksum;
            if (Sum != 0 )
            {
                Status("CHECKSUM");
                return RECORD_CHECKSUM_ERR;
            }

            // Checksum valid, "remove" data from input buffer
            *nInput -= nUsed;

            // Fill in the record parameters
            pOutputBuffer->RecordLength = RecordLength;
            pOutputBuffer->LoadOffset   = LoadOffset;
            pOutputBuffer->RecordType   = RecordType;
            pOutputBuffer->ValidIndex   = 0;

            return RECORD_FOUND;
        }
    }
    
    // Insufficient data to form a record
    return RECORD_NOT_FOUND;

} // GetRecord


/****************************************************************************
  Function:
    void BlockInitialize( FLASH_BLOCK *pFlashBlock )

  Description:
    Initializes a Flash Block structure and blank-fills the buffer.
    
  Precondition:
    None

  Parameters:
    pFlashBlock     - Pointer to the flash block data structure

  Returns:
    None

  Remarks:
    FLASH_BLOCK_SIZE    - Defines the size of the buffer
    BLOCK_FILL_DEFAULT  - Defines the blank-fill character
***************************************************************************/

void BlockInitialize( FLASH_BLOCK *pFlashBlock )
{    
    unsigned int i;

    // Initialize the block state
    pFlashBlock->address    = 0;
    pFlashBlock->blockState = BLOCK_STATE_NEW;

    // Blank-Fill the block 
    memset(pFlashBlock->data, 0xFF, FLASH_BLOCK_SIZE);

}


/****************************************************************************
  Function:
    unsigned int AddToBlock( FLASH_BLOCK *pFlashBlock, RECORD_STRUCT *pRecord )

  Description:
    Takes data from the record buffer and copies it to the appropriate place
    in the Flash block buffer.  
    
  Precondition:
    Both the record and Flash block structures must have been properly
    initialized.

  Parameters:
    pFlashBlock - Pointer to the Flash block structure
    pRecord     - Pointer to the record structure

  Returns:
    TRUE        - If the block is ready to be written to Flash
    FALSE       - If the block is not yet ready to be written to Flash

  Remarks:
    This routine will update both structures.  Either may contain 
    residual data.  If the return value is TRUE, the caller must write and
    re-initialize the Flash block structure before passing it to this 
    routine again.
***************************************************************************/

unsigned int AddToBlock( FLASH_BLOCK *pFlashBlock, RECORD_STRUCT *pRecord )
{
    unsigned long int   RecordTargetStart;
    unsigned long int   RecordTargetEnd;
    unsigned long int   BlockTargetStart;
    unsigned long int   BlockTargetEnd;
    unsigned long int   CopySize;
    unsigned char       iCopy;
    unsigned char      *pCopyFrom;
    unsigned char      *pCopyTo;
    
    // Calculate the start and end addresses for  record
    RecordTargetStart   = baseAddress + pRecord->LoadOffset + pRecord->ValidIndex;
    RecordTargetEnd     = baseAddress + pRecord->LoadOffset + pRecord->RecordLength;

    // Initialize the block address if block is new
    if ( pFlashBlock->blockState == BLOCK_STATE_NEW) 
    {
        // Initialize block paramters
        pFlashBlock->address    = RecordTargetStart & BLOCK_ALIGNMENT_MASK;
        pFlashBlock->blockState = BLOCK_STATE_PARTIAL;

        // If the block has already been erased once, we've probably written data to it.
        if (TrackPageEraseTest(pFlashBlock->address))
        {
            // Fill the block buffer from Flash so that we can overlay new data on it and 
            // mark it as not erased so we will erase it again before writing this block.
            memcpy(pFlashBlock->data, PA_TO_KVA1(pFlashBlock->address), FLASH_BLOCK_SIZE);
            TrackPageErase(pFlashBlock->address, PAGE_NOT_ERASED);
        }
    }

    // Calculate the start and end addresses for the block
    BlockTargetStart    = pFlashBlock->address;
    BlockTargetEnd      = pFlashBlock->address + FLASH_BLOCK_SIZE;


    // Determine the copy size and from/to addresses
    // (regardless of relative size of record and block buffers).

    if (RecordTargetEnd <= BlockTargetStart)
    {
        // Record target is outside (before) block target
        pFlashBlock->blockState = BLOCK_STATE_READY;
        CopySize = 0;
    }
    else
    {
        if (RecordTargetStart <= BlockTargetStart)
        {
            // Record overlaps start of block
            pCopyFrom = &pRecord->data[BlockTargetStart - RecordTargetStart];
            pCopyTo   = &pFlashBlock->data[0];

            if (RecordTargetEnd <= BlockTargetEnd)
            {
                // Record ends before block ends
                CopySize = RecordTargetEnd - BlockTargetStart;
            }
            else
            {
                // Block ends before record ends
                CopySize = FLASH_BLOCK_SIZE;
            }

            // Reduce the record length by the date we're copying
            pRecord->RecordLength -= CopySize;
        }
        else
        {
            if (RecordTargetStart < BlockTargetEnd)
            {
                // Record starts before block ends
                pCopyFrom = &pRecord->data[pRecord->ValidIndex];
                pCopyTo   = &pFlashBlock->data[RecordTargetStart - BlockTargetStart];

                if (RecordTargetEnd <= BlockTargetEnd)
                {
                    // Record contained entirely within block
                    CopySize = RecordTargetEnd - RecordTargetStart;
                }
                else
                {
                    // Record overlaps end of block
                    CopySize = BlockTargetEnd - RecordTargetStart;
                }

                // Update the record valid data index
                pRecord->ValidIndex += CopySize;
            }
            else
            {
                // Record target is outside (after) block target
                pFlashBlock->blockState = BLOCK_STATE_READY;
                CopySize = 0;
            }
        }
    }
    
    // Copy data from the record buffer to the block buffer
    for (iCopy=0; iCopy < CopySize; iCopy++)
    {
        pCopyTo[iCopy] = pCopyFrom[iCopy];
    }

    // Return TRUE of the block is ready to program to Flash
    if (pFlashBlock->blockState == BLOCK_STATE_READY)
    {
        return TRUE;
    }
    
    return FALSE;

} // AddToBlock


/****************************************************************************
  Function:
    unsigned char AddressWriteable(unsigned long int address, 
                                   unsigned int      length)

  Description:
    Tracks which pages have been erased. Updates table based on passed
    address

  Precondition:
    Requires MIN_FLASH_ADDRESS & MAX_FLASH_ADDRESS to be defined in 
    boot_config.h
    
  Parameters:
    address     - Address to write to
    length      - Number of words

  Returns:
    TRUE        - specified address can be written
    FALSE       - do not write to specified
    
  Remarks:
    None
    
***************************************************************************/
unsigned char AddressWriteable(unsigned long int address, unsigned int length)
{
    if (address < MIN_FLASH_ADDRESS)
    {
        Status( "ADDRESS");
        return FALSE; 
    }
    
    if (address + length >= MAX_FLASH_ADDRESS)
    {
        Status( "ADDRESS");
        return FALSE; 
    }
    
    return TRUE;

} // AddressWriteable


//******************************************************************************
//******************************************************************************
// Hex File Loader Interface Routines
//******************************************************************************
//******************************************************************************

/****************************************************************************
  Function:
    void Loader_Initialize ( FLASH_BLOCK *pFlashBlock )

  Description:
    This routine initializes the loader by initializing the FLASH_BLOCK 
    structure, so that it can begin accumulating data, and initializing the
    Flash page erasure tracking array.

  Precondition:
    None

  Parameters:
    pFlashBlock     - Pointer to a FLASH_BLOCK structure

  Returns:
    None
    
  Remarks:
    Initializes the Flash buffer space to all 1's (0xFFFFFFFF)
***************************************************************************/

void Loader_Initialize ( FLASH_BLOCK *pFlashBlock )
{
    BlockInitialize(pFlashBlock);

    TrackPageEraseInit();

}


/****************************************************************************
  Function:
    unsigned int Loader_GetFlashBlock ( FLASH_BLOCK  *pFlashBlock, 
                                        unsigned int *pRawSize, 
                                        char         *pRawBuffer  )

  Description:
    This routine gets raw data from the input buffer (RawBuffer), translates 
    it from Hex Record ASCII format into binary data, and copies it to the 
    Flash block buffer (FLASH_BLOCK).  In the process, it updates the state
    variables in the FLASH_BLOCK structure as well as the count (pRawSize)
    of bytes in the raw buffer.

  Precondition:
    The FlashBlock must have been initialized.  (It may already be partially
    filled.)  Also, the raw buffer must contain *pRawSize bytes of data.

  Parameters:
    pFlashBlock - Pointer to a structure to receive data to program to Flash
                  along with information about where to program it

    pRawSize    - Pointer to a variable providing the number of bytes of data
                  pointed to by pRawBuffer.
                  
    pRawBuffer  - Pointer to the data to translate and place into the 
                  FLASH_BLOCK buffer

  Returns:
    LOADER_BLOCK_READY      - Block is ready to program to Flash
    LOADER_NEED_DATA        - Need additional data to form block
    LOADER_DECODE_ERROR     - Error decoding raw data

  Remarks:
    This routine may leave residual data in either the Flash block or the
    in the raw buffer.  It will update the state of the Flash block and
    *pRawSize count appropriately.
***************************************************************************/

unsigned int Loader_GetFlashBlock ( FLASH_BLOCK  *pFlashBlock, 
                                    unsigned int *pRawSize, 
                                    char         *pRawBuffer  )
{
    unsigned int    blockResult;
    unsigned int    decodeResult;
    unsigned int    processResult;
    unsigned int    blockAddResult;
    unsigned int    nCurrent;
    unsigned int    iBuffer;


    // Continue translating records from the raw buffer
    // until the Flash Block is full (if possible).
    iBuffer       = 0;
    nCurrent      = *pRawSize;
    blockResult   = LOADER_NEED_DATA;
    processResult = RECORD_UNSUPPORTED;
    while ( (decodeResult=GetRecord(&recordBuffer, &nCurrent, &pRawBuffer[iBuffer])) == RECORD_FOUND )
    {
        // Copy data records to the Flash Block if possible
        if ( (processResult=ProcessRecord(&recordBuffer)) == RECORD_DATA )
        {
            // Copy record data to Flash Block
            if ( AddToBlock(pFlashBlock, &recordBuffer) )
            {
                // Exit the loop when we fill the block
                blockResult = LOADER_BLOCK_READY;
                break;
            }
        }

        // Check for EOF record
        if (processResult == RECORD_EOF)
        {
            return LOADER_EOF;
        }

        // Update raw buffer index
        iBuffer = *pRawSize - nCurrent;
    }

    // Update the raw buffer size
    *pRawSize = nCurrent;

    // Report results
    if ( decodeResult == RECORD_CHECKSUM_ERR || 
         processResult > RECORD_UNSUPPORTED   )
    {
        return LOADER_DECODE_ERROR;
    }

    return blockResult;

} // Loader_GetFlashBlock


/****************************************************************************
  Function:
    BOOL Loader_ProgramFlashBlock( FLASH_BLOCK *pFlashBlock )
    
  Description:
    This routine programs the data from the FlashBlock structure to the 
    appropriate Flash memory address (also contained the structure).

  Precondition:
    The FlashBlock must have been appropriately initialized and "filled" 
    with data.

  Parameters:
    pFlashBlock  - Pointer to a FlashBlock structure to Program to Flash

  Returns:
    TRUE        - If able to program the data to Flash
    FALSE       - If not
    
  Remarks:
    This routine leaves the FlashBlock structure in an empty 
    "BLOCK_STATE_NEW" state.
***************************************************************************/
BOOL Loader_ProgramFlashBlock( FLASH_BLOCK *pFlashBlock )
{
    unsigned long int CurrentAddress;
    unsigned long int PageEndAddress;
    unsigned int     *pData;
    unsigned int      nvmResult;

    // Calculate the beginning and ending addresses of the page.
    CurrentAddress = pFlashBlock->address;
    PageEndAddress = CurrentAddress + FLASH_BLOCK_SIZE;
    pData          = (unsigned int *)pFlashBlock->data;

    // Check to see if it's safe to program the Flash
    if ( Loader_PermitProgramming() && 
         AddressWriteable(CurrentAddress, FLASH_BLOCK_SIZE) )
    {
        // Check to see if the page has been erased
        if (TrackPageEraseTest(CurrentAddress) == FALSE)
        { 
            // If not, erase the page & log track it
            nvmResult = NVMErasePage((void *)CurrentAddress);
            if (nvmResult != 0)
            {
                // Error erasing Flash page
                Status( "ERASE");
                return FALSE;
            }
            TrackPageErase(CurrentAddress, PAGE_ERASED);
        }

        // Program the block to Flash
        while (CurrentAddress < PageEndAddress)
        {
            if ( NVMWriteWord( (void *)CurrentAddress, *pData ) != FALSE )
            {
                // Error Writing Flash
                Status( "WRITE");
                return FALSE;
            }
            pData++;
            CurrentAddress += sizeof(unsigned int);
        }

        // Verify that the block was written correctly
        // (This check will identify writes to a Flash block that was not fully erased.)
        CurrentAddress = pFlashBlock->address;
        pData          = (unsigned int *)pFlashBlock->data;
        while (CurrentAddress < PageEndAddress)
        {
            // Compare buffer contents to Flash contents
            if (*((unsigned int *)PA_TO_KVA1(CurrentAddress)) != *pData)
            {
                // Flash and buffer did not match.
                Status( "VERIFY");
                return FALSE;
            }
            pData++;
            CurrentAddress += sizeof(unsigned int);
        }

        // Re-initialize the block
        BlockInitialize(pFlashBlock);

        return TRUE;
    }

    return FALSE;

} // Loader_ProgramFlashBlock


/*
*******************************************************************************
EOF
*******************************************************************************
*/

