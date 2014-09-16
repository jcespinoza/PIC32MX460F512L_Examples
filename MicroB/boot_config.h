/* Boot Status Codes
**************************************************************************
BOOT_STATUS is used to identify the current status of the boot loader.
**************************************************************************
*/

typedef enum
{
    // Boot loader status
    BL_RESET = 0,           // Device has been reset
    BL_LOADING,             // Loading of a new application image activated
    BL_TRANSPORT_INIT,      // Boot loader transport layer initialized
    BL_MEDIUM_DETACH,       // Indicates that the boot medium has been detached
    BL_FOUND_FILE,          // New application image file has been found
    BL_PROGRAMMED,          // The application image has been programmed to Flash
    BL_BOOTING,             // Booting application image

    // Transport-specific errors
    BL_USB_OC_ERR,          // USB Over-current error detected
    BL_FS_INIT_ERR,         // Unable to initialize the boot medium's file system
    BL_USB_HUB_ERR,         // USB hub error (hubs not supported)
    BL_BAD_DEV_ERR,         // Unsupported USB device attached
    USB_ENUM_ERR,           // Unable to enumerate USB device
    USB_MSD_INIT_ERR,       // Unable to initialize USB MSD client driver
    OUT_OF_MEMORY_ERR,      // Attempted to malloc memory and failed
    BL_FILE_ERR,            // Unable to open the boot image file
    USB_ERR,                // Unspecified USB error

    // Loader Errors
    LOADER_ADDRESS_LOW,     // Flash block address is out of range, low
    LOADER_ADDRESS_HIGH,    // Flash block address is out of range, high
    LOADER_CHECKSUM_ERR,    // Hex record checksum didn't match
    LOADER_FLASH_ERASE_ERR, // Erase had an error
    LOADER_FLASH_WRITE_ERR, // Unable to write to Flash
    LOADER_FLASH_VERIFY_ERR,// Data written to Flash did not match buffer data

    // General errors
    BL_BOOT_FAIL,           // Application not valid or failed to launch
    BL_FILE_NOT_FOUND       // Application image file not found

} BOOT_STATUS; 

// Defines the size of the buffer used to read the boot image file.
#define BL_READ_BUFFER_SIZE     512


/* "Loader" Interface Call Outs
*******************************************************************************
The following interface is used by the media layer to decode and program the 
image file to memory.
*******************************************************************************
*/

// Address of main application's Startup code
#define APPLICATION_ADDRESS         0x9D000000

// These macros define the maximum size of a Flash block.
#define PROGRAM_FLASH_BASE          0x1D000000          // Physical address
#define PROGRAM_FLASH_LENGTH        0x00060000          // Length in bytes
#define FLASH_BLOCK_SIZE            (1024 * 4)          // Size in bytes


#define Loader_PermitProgramming()  TRUE

/*
*******************************************************************************
EOF
*******************************************************************************
*/

