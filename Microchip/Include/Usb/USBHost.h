/*******************************************************************************

    USB Host Driver Header File

This file, with its associated C source file, provides the hardware interface
for a USB Host application.  This file should be included in all files that
utilize the USB host stack.  It must be included after the application-specific
USBConfig.h file, as USBConfig.h configures parts of this file.

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* FileName:        USBHost.h
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

#ifndef __USBHOST_H__
#define __USBHOST_H__
//DOM-IGNORE-END

#include "USB\USBCore.h"

// *****************************************************************************
// *****************************************************************************
// Section: Host Firmware Version
// *****************************************************************************
// *****************************************************************************

#define USB_HOST_FW_MAJOR_VER   1       // Firmware version, major release number.
#define USB_HOST_FW_MINOR_VER   0       // Firmware version, minor release number.
#define USB_HOST_FW_DOT_VER     0       // Firmware version, dot release number.


// *****************************************************************************
// *****************************************************************************
// Section: Set Default Configuration Constants
// *****************************************************************************
// *****************************************************************************

#ifndef USB_NUM_BULK_NAKS
    #define USB_NUM_BULK_NAKS       10000   // Define how many NAK's are allowed
                                            // during a bulk transfer before erroring.
#endif

#ifndef USB_NUM_COMMAND_TRIES
    #define USB_NUM_COMMAND_TRIES       3   // During enumeration, define how many
                                            // times each command will be tried before
                                            // giving up and resetting the device.
#endif

#ifndef USB_NUM_CONTROL_NAKS
    #define USB_NUM_CONTROL_NAKS        3   // Define how many NAK's are allowed
                                            // during a control transfer before erroring.
#endif

#ifndef USB_NUM_ENUMERATION_TRIES
    #define USB_NUM_ENUMERATION_TRIES   3   // Define how many times the host will try
                                            // to enumerate the device before giving
                                            // up and setting the state to DETACHED.
#endif

#ifndef USB_NUM_INTERRUPT_NAKS
    #define USB_NUM_INTERRUPT_NAKS      3   // Define how many NAK's are allowed
                                            // during an interrupt OUT transfer before
                                            // erroring.  Interrupt IN transfers that
                                            // are NAK'd are terminated without error.
#endif


// *****************************************************************************
// *****************************************************************************
// Section: USB Constants
// *****************************************************************************
// *****************************************************************************

// Section: Standard Device Requests

#define USB_REQUEST_GET_STATUS                  0       // Standard Device Request - GET STATUS
#define USB_REQUEST_CLEAR_FEATURE               1       // Standard Device Request - CLEAR FEATURE
#define USB_REQUEST_SET_FEATURE                 3       // Standard Device Request - SET FEATURE
#define USB_REQUEST_SET_ADDRESS                 5       // Standard Device Request - SET ADDRESS
#define USB_REQUEST_GET_DESCRIPTOR              6       // Standard Device Request - GET DESCRIPTOR
#define USB_REQUEST_SET_DESCRIPTOR              7       // Standard Device Request - SET DESCRIPTOR
#define USB_REQUEST_GET_CONFIGURATION           8       // Standard Device Request - GET CONFIGURATION
#define USB_REQUEST_SET_CONFIGURATION           9       // Standard Device Request - SET CONFIGURATION
#define USB_REQUEST_GET_INTERFACE               10      // Standard Device Request - GET INTERFACE
#define USB_REQUEST_SET_INTERFACE               11      // Standard Device Request - SET INTERFACE
#define USB_REQUEST_SYNCH_FRAME                 12      // Standard Device Request - SYNCH FRAME

#define USB_FEATURE_ENDPOINT_HALT               0       // CLEAR/SET FEATURE - Endpoint Halt
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP        1       // CLEAR/SET FEATURE - Device remote wake-up
#define USB_FEATURE_TEST_MODE                   2       // CLEAR/SET FEATURE - Test mode

// Section: Setup Data Constants

#define USB_SETUP_HOST_TO_DEVICE                0x00    // Device Request bmRequestType transfer direction - host to device transfer
#define USB_SETUP_DEVICE_TO_HOST                0x80    // Device Request bmRequestType transfer direction - device to host transfer
#define USB_SETUP_TYPE_STANDARD                 0x00    // Device Request bmRequestType type - standard
#define USB_SETUP_TYPE_CLASS                    0x20    // Device Request bmRequestType type - class
#define USB_SETUP_TYPE_VENDOR                   0x40    // Device Request bmRequestType type - vendor
#define USB_SETUP_RECIPIENT_DEVICE              0x00    // Device Request bmRequestType recipient - device
#define USB_SETUP_RECIPIENT_INTERFACE           0x01    // Device Request bmRequestType recipient - interface
#define USB_SETUP_RECIPIENT_ENDPOINT            0x02    // Device Request bmRequestType recipient - endpoint
#define USB_SETUP_RECIPIENT_OTHER               0x03    // Device Request bmRequestType recipient - other

// Section: OTG SET FEATURE Constants

#define OTG_FEATURE_B_HNP_ENABLE                3       // SET FEATURE OTG - Enable B device to perform HNP
#define OTG_FEATURE_A_HNP_SUPPORT               4       // SET FEATURE OTG - A device supports HNP
#define OTG_FEATURE_A_ALT_HNP_SUPPORT           5       // SET FEATURE OTG - Another port on the A device supports HNP

// Section: Error Code Values

#define USB_SUCCESS                             0x00    // USB operation successful.
#define USB_INVALID_STATE                       0x01    // Operation cannot be performed in current state.
#define USB_BUSY                                0x02    // A transaction is already in progress.
#define USB_ILLEGAL_REQUEST                     0x03    // Cannot perform requested operation.
#define USB_INVALID_CONFIGURATION               0x04    // Configuration descriptor not found.
#define USB_MEMORY_ALLOCATION_ERROR             0x05    // Out of dynamic memory.
#define USB_UNKNOWN_DEVICE                      0x06    // Device with specified address is not attached.
#define USB_CANNOT_ENUMERATE                    0x07    // Cannot enumerate the attached device.
#define USB_EVENT_QUEUE_FULL                    0x08    // Event queue was full when an event occured.
#define USB_ENDPOINT_BUSY                       0x10    // Endpoint is currently processing a transaction.
#define USB_ENDPOINT_STALLED                    0x11    // Endpoint is currently stalled. User must clear the condition.
#define USB_ENDPOINT_ERROR                      0x12    // Will need more than this eventually
#define USB_ENDPOINT_ERROR_ILLEGAL_PID          0x13    // Illegal PID received.
#define USB_ENDPOINT_NOT_FOUND                  0x14    // Requested endpoint does not exist on device.
#define USB_ENDPOINT_ILLEGAL_DIRECTION          0x15    // Reads must be performe on IN endpoints, writes on OUT endpoints.
//#define USB_ENDPOINT_TRANSACTION_IN_PROGRESS    0x16
#define USB_ENDPOINT_NAK_TIMEOUT                0x17    // Too many NAK's occurred while waiting for the current transaction.
#define USB_ENDPOINT_ILLEGAL_TYPE               0x18    // Transfer type must match endpoint description.
#define USB_ENDPOINT_UNRESOLVED_STATE           0x19    // Endpoint is in an unknown state after completing a transaction.
#define USB_ENDPOINT_ERROR_BIT_STUFF            0x20    // USB Module - Bit stuff error.
#define USB_ENDPOINT_ERROR_DMA                  0x21    // USB Module - DMA error.
#define USB_ENDPOINT_ERROR_TIMEOUT              0x22    // USB Module - Bus timeout.
#define USB_ENDPOINT_ERROR_DATA_FIELD           0x23    // USB Module - Data field size error.
#define USB_ENDPOINT_ERROR_CRC16                0x24    // USB Module - CRC16 failure.
#define USB_ENDPOINT_ERROR_END_OF_FRAME         0x25    // USB Module - End of Frame error.
#define USB_ENDPOINT_ERROR_PID_CHECK            0x26    // USB Module - Illegal PID received.
#define USB_ENDPOINT_ERROR_BMX                  0x27    // USB Module - Bus Matrix error.

// Section: Return values for USBHostDeviceStatus()

#define USB_DEVICE_STATUS                       0x30                        // Offset for USBHostDeviceStatus() return codes
#define USB_DEVICE_ATTACHED                     (USB_DEVICE_STATUS | 0x30)  // Device is attached and running
#define USB_DEVICE_DETACHED                     (USB_DEVICE_STATUS | 0x01)  // No device is attached
#define USB_DEVICE_ENUMERATING                  (USB_DEVICE_STATUS | 0x02)  // Device is enumerating
#define USB_HOLDING_OUT_OF_MEMORY               (USB_DEVICE_STATUS | 0x03)  // Not enough heap space available
#define USB_HOLDING_UNSUPPORTED_DEVICE          (USB_DEVICE_STATUS | 0x04)  // Invalid configuration or unsupported class
#define USB_HOLDING_UNSUPPORTED_HUB             (USB_DEVICE_STATUS | 0x05)  // Hubs are not supported
#define USB_HOLDING_INVALID_CONFIGURATION       (USB_DEVICE_STATUS | 0x06)  // Invalid configuration requested
#define USB_HOLDING_PROCESSING_CAPACITY         (USB_DEVICE_STATUS | 0x07)  // Processing requirement excessive
#define USB_HOLDING_POWER_REQUIREMENT           (USB_DEVICE_STATUS | 0x08)  // Power requirement excessive
#define USB_HOLDING_CLIENT_INIT_ERROR           (USB_DEVICE_STATUS | 0x09)  // Client driver failed to initialize

#define USB_ERROR_CLASS_DEFINED                 0x50    // Offset for application defined errors

// Section: Values for USBHostDeviceRequest(), dataDirection

#define USB_DEVICE_REQUEST_SET                  0       // USBHostDeviceRequest() will set information.
#define USB_DEVICE_REQUEST_GET                  1       // USBHostDeviceRequest() will get information.

// Section: USB Endpoint Transfer Types

#define USB_TRANSFER_TYPE_CONTROL               0x00    // Endpoint is a control endpoint.
#define USB_TRANSFER_TYPE_ISOCHRONOUS           0x01    // Endpoint is an isochronous endpoint.
#define USB_TRANSFER_TYPE_BULK                  0x02    // Endpoint is a bulk endpoint.
#define USB_TRANSFER_TYPE_INTERRUPT             0x03    // Endpoint is an interrupt endpoint.

// Section: Dummy Device ID's

#define USB_ROOT_HUB                            255     // Invalid Device ID used to indicate the root hub.


// *****************************************************************************
// *****************************************************************************
// Section: USB Data Structures
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Transfer Attributes

This structure describes the transfer attribues of an endpoint.
*/
typedef union
{
   BYTE     val;                            //
   struct
   {
       BYTE bfTransferType          : 2;    // See USB_TRANSFER_TYPE_* for values.
       BYTE bfSynchronizationType   : 2;    // For isochronous endpoints only.
       BYTE bfUsageType             : 2;    // For isochronous endpoints only.
   };
} TRANSFER_ATTRIBUTES;


// *****************************************************************************
/* Host Transfer Information

This structure is used when the event handler is used to notify the upper layer
of transfer completion.
*/
typedef struct _HOST_TRANSFER_DATA
{
   DWORD                dataCount;          // Count of bytes transferred.
   BYTE                *pUserData;          // Pointer to transfer data.
   BYTE                 bEndpointAddress;   // Transfer endpoint.
   BYTE                 bErrorCode;         // Transfer error code.
   TRANSFER_ATTRIBUTES  bmAttributes;       // Endpoint transfer attributes.
} HOST_TRANSFER_DATA;


// *****************************************************************************
/* Targeted Peripheral List

This structure is used to define the devices that this host can support.  If the
host is a USB Embedded Host or Dual Role Device that does not support OTG, the
TPL may contain both specific devices and generic classes.  If the host supports
OTG, then the TPL may contain ONLY specific devices.
*/
typedef struct _USB_TPL
{
    union
    {
        DWORD       val;                        //
        struct
        {
            WORD    idVendor;                   // Vendor ID
            WORD    idProduct;                  // Product ID
        };
        struct
        {
            BYTE    bClass;                     // Class ID
            BYTE    bSubClass;                  // SubClass ID
            BYTE    bProtocol;                  // Protocol ID
        };
    } device;                                   //
    BYTE            bConfiguration;             // Initial device configuration
    BYTE            ClientDriver;               // Index of client driver in the Client Driver table
    union
    {
        BYTE         val;                       //
        struct
        {
            BYTE     bfAllowHNP      : 1;       // Is HNP allowed?
            BYTE     bfIsClassDriver : 1;       // Client driver is a class-level driver
            BYTE     bfSetConfiguration : 1;    // bConfiguration is valid
        };
    } flags;                                    //
} USB_TPL;

// Section: TPL Initializers
#define INIT_VID_PID(v,p)   {((v)|((p)<<16))}           // Set VID/PID support in the TPL.
#define INIT_CL_SC_P(c,s,p) {((c)|((s)<<8)|((p)<<16))}  // Set class support in the TPL (non-OTG only).

// Section: TPL Flags
#define TPL_ALLOW_HNP   0x01                    // Bitmask for Host Negotiation Protocol.
#define TPL_CLASS_DRV   0x02                    // Bitmask for class driver support.
#define TPL_SET_CONFIG  0x04                    // Bitmask for setting the configuration.


/***************************************************************************
 * Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug *
 ***************************************************************************/
//DOM-IGNORE-BEGIN

typedef struct _STATE_TRACE
{
    DWORD   state;
    BYTE    layer;
} STATE_TRACE;

#ifdef ENABLE_STATE_TRACE
    extern STATE_TRACE  state_trace[STATE_TRACE_SIZE];
    extern unsigned int st_index;
    #define HOST_TRACE_STATE(l,s)                               \
            if (prevHostState != (s))                           \
            {                                                   \
                state_trace[st_index].layer=(l);                \
                state_trace[st_index].state=(DWORD)(s);         \
                st_index = (st_index + 1) % STATE_TRACE_SIZE;   \
                prevHostState = (s);                            \
            }
#else
    #define HOST_TRACE_STATE(l,s)
#endif

//DOM-IGNORE-END
/***************************************************************************
 * Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug Debug *
 ***************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: USB Host - Client Driver Interface
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* USB Events

This enumeration identifies USB events that occur.  It is used to
inform USB drivers and applications of events on the bus.  It is passed
as a parameter to the event-handling routine, which must match the
prototype of the USB_CLIENT_EVENT_HANDLER data type, when an event occurs.
*/

typedef enum
{
    EVENT_NONE = 0,             // No event occured (NULL event)
    EVENT_TRANSFER,             // A USB transfer has completed (data: HOST_TRANSFER_DATA)
    EVENT_SOF,                  // Start of frame
    EVENT_RESUME,               // Device-mode resume received
    EVENT_SUSPEND,              // Device-mode suspend/idle event received
    EVENT_RESET,                // Device-mode bus reset received
    EVENT_DETACH,               // USB cable has been detached (data: BYTE, address of device)
    EVENT_HUB_ATTACH,           // USB hub has been attached
    EVENT_STALL,                // A stall has occured
    EVENT_VBUS_SES_END,         // VBus Off,       (VBus < 0.8v),  Data: BYTE Port Number
    EVENT_VBUS_SES_REQUEST,     // VBus SRP Pulse, (VBus > 2.0v),  Data: BYTE Port Number
    EVENT_VBUS_SES_VALID,       // VBus On,        (VBus > 4.4v),  Data: BYTE Port Number
    EVENT_VBUS_OVERCURRENT,     // VBus droop,     (VBus < 4.4v),  Data: BYTE Port Number
    EVENT_REQUEST_POWER,        // Request power for an enumerating device

    // Class-defined event offsets start here:
    EVENT_GENERIC_BASE  = 100,      // Offset for Generic class events

    EVENT_MSD_BASE      = 200,      // Offset for Mass Storage Device class events

    EVENT_HID_BASE      = 300,      // Offset for Human Interface Device class events

    EVENT_USER_BASE     = 10000,    // Add integral values to this event number
                                    // to create user-defined events.

    EVENT_BUS_ERROR = 0xFFFF        // Transfer error on the USB (data: HOST_TRANSFER_DATA)

} USB_EVENT;


/*******************************************************************************
Function Pointer:   USB_CLIENT_INIT

Preconditions:      The device has been configured.

Input:              BYTE address - Device's address on the bus
                    WORD flags   - Initialization flags

Output:             None - None

Return:             TRUE    - Successful
                    FALSE   - Not successful

Side Effects:       The USB client driver called will be initialized.

Overview:           This routine is a call out from the host layer to a
                    USB client driver.  It is called when the system
                    has been configured as a USB host and a new device
                    has been attached to the bus.  Its purpose is to
                    initialize and activate the client driver.

Note:               There may be multiple client drivers.  If so, the
                    USB host layer will call the initialize routine
                    for each of the clients that are in the selected
                    configuration.
*******************************************************************************/

typedef BOOL (*USB_CLIENT_INIT)   ( BYTE address, DWORD flags );


/*******************************************************************************
Function Pointer:   USB_CLIENT_EVENT_HANDLER

Preconditions:      The client must have been initialized.

Input:              BYTE address    - Address of device where event occurred
                    USB_EVENT event - Identifies the event that occured
                    void *data      - Pointer to event-specific data
                    DWORD size      - Size of the event-specific data

Output:             None - None

Return:             TRUE    - The event was handled
                    FALSE   - The event was not handled

Side Effects:       Event-specific actions will be taken.

Overview:           This data type defines a pointer to a call-back
                    function that must be implemented by a client driver
                    if it needs to be aware of events on the USB.  When an
                    event occurs, the Host layer will call the client
                    driver via this pointer to handle the event.  Events
                    are identified by the "event" parameter and may have
                    associated data.  If the client driver was able to
                    handle the event, it should return TRUE.  If not (or
                    if additional processing is required), it should
                    return FALSE.

Note:               The application may also implement an event handling
                    routine if it requires knowledge of events.  To do so,
                    it must implement a routine that matches this function
                    signature and define the USB_HOST_APP_EVENT_HANDLER
                    macro as the name of that function.
*******************************************************************************/

typedef BOOL (*USB_CLIENT_EVENT_HANDLER) ( BYTE address, USB_EVENT event, void *data, DWORD size );


// *****************************************************************************
/* Application-level Event Handler Prototype

If the application wishes to support events such as power allocation for an
attached device, an application-level event handler must be implemented.  The
macro USB_HOST_APP_EVENT_HANDLER must be defined in USBConfig.h as the name of
the application's event handler.  See the USB_CLIENT_EVENT_HANDLER type
definition for more information about the function parameters.  If no event
handler is provided, then all application events are assumed to function without
error.
*/
#if defined( USB_HOST_APP_EVENT_HANDLER )
    BOOL USB_HOST_APP_EVENT_HANDLER ( BYTE address, USB_EVENT event, void *data, DWORD size );
#else
    // If the application does not provide an event handler, then we will
    // assume that all events function without error.
    #define USB_HOST_APP_EVENT_HANDLER(a,e,d,s) TRUE
#endif


// *****************************************************************************
/* Client Driver Table Structure

This structure is used to define an entry in the client-driver table.
Each entry provides the information that the Host layer needs to
manage a particular USB client driver, including pointers to the
interface routines that the Client Driver must implement.
 */

typedef struct _CLIENT_DRIVER_TABLE
{
    USB_CLIENT_INIT          Initialize;     // Initialization routine
    USB_CLIENT_EVENT_HANDLER EventHandler;   // Event routine
    DWORD                    flags;          // Initialization flags

} CLIENT_DRIVER_TABLE;


// *****************************************************************************
// *****************************************************************************
// Section: USB Host - Device Information Hooks
// *****************************************************************************
// *****************************************************************************

extern BYTE                *pCurrentConfigurationDescriptor;    // Pointer to the current Configuration Descriptor of the attached device.
extern BYTE                *pDeviceDescriptor;                  // Pointer to the Device Descriptor of the attached device.
extern USB_TPL              usbTPL[];                           // Application's Targeted Peripheral List.
extern CLIENT_DRIVER_TABLE  usbClientDrvTable[];                // Application's client driver table.


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes and Macro Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
Function:       BYTE USBHostClearEndpointErrors( BYTE deviceAddress, BYTE endpoint )

Preconditions:  None

Overview:       This function is called to clear the internal error condition
                of a device's endpoint.  It should be called after the application
                has dealt with the error condition on the device.  This routine
                clears internal status only; it does not interact with the
                device.

Input:          BYTE deviceAddress  - Address of device
                BYTE endpoint       - Endpoint to clear error condition

Output:         None - None

Return:         USB_SUCCESS             - Errors cleared
                USB_UNKNOWN_DEVICE      - Device not found
                USB_ENDPOINT_NOT_FOUND  - Specified endpoint not found

Remarks:        None
*******************************************************************************/
BYTE    USBHostClearEndpointErrors( BYTE deviceAddress, BYTE endpoint );

/*******************************************************************************
Function:       BYTE USBHostDeviceRequest( BYTE deviceAddress, BYTE bmRequestType,
                    BYTE bRequest, WORD wValue, WORD wIndex, WORD wLength,
                    BYTE *data, BYTE dataDirection )

Preconditions:  The host state machine should be in the running state, and no
                reads or writes to EP0 should be in progress.

Overview:       This function sends a standard device request to the attached
                device.  The user must pass in the parameters of the device
                request.  If there is input or output data associated with the
                request, a pointer to the data must be provided.  The direction
                of the associated data (input or output) must also be indicated.

                This function does no special processing in regards to the
                request except for three requests.  If SET INTERFACE is sent,
                then DTS is reset for all endpoints.  If CLEAR FEATURE (ENDPOINT
                HALT) is sent, then DTS is reset for that endpoint.  If SET
                CONFIGURATION is sent, the request is aborted with a failure.
                The function USBHostSetDeviceConfiguration() must be called to
                change the device configuration, since endpoint definitions
                may change.

Input:          BYTE deviceAddress  - Device address
                BYTE bmRequestType  - The request type as defined by the USB
                                        specification.
                BYTE bRequest       - The request as defined by the USB
                                        specification.
                WORD wValue         - The value for the request as defined by
                                        the USB specification.
                WORD wIndex         - The index for the request as defined by
                                        the USB specification.
                WORD wLength        - The data length for the request as
                                        defined by the USB specification.
                BYTE *data          - Pointer to the data for the request.
                BYTE dataDirection  - USB_DEVICE_REQUEST_SET or
                                        USB_DEVICE_REQUEST_GET

Output:         None - None

Return:         USB_SUCCESS                 - Request processing started
                USB_UNKNOWN_DEVICE          - Device not found
                USB_INVALID_STATE           - The host must be in a normal running
                                                state to do this request
                USB_ENDPOINT_BUSY           - A read or write is already in progress
                USB_ILLEGAL_REQUEST         - SET CONFIGURATION cannot be performed
                                                with this function.

Remarks:        None
*******************************************************************************/
BYTE    USBHostDeviceRequest( BYTE deviceAddress, BYTE bmRequestType, BYTE bRequest,
             WORD wValue, WORD wIndex, WORD wLength, BYTE *data, BYTE dataDirection );


/*******************************************************************************
Function:       BYTE USBHostDeviceStatus( BYTE deviceAddress )

Preconditions:  None

Overview:       This function returns the current status of a device.  If the
                device is in a holding state due to an error, the error is
                returned.

Input:          BYTE deviceAddress  - Device address

Output:         None - None

Return:         USB_DEVICE_ATTACHED                 - Device is attached and running
                USB_DEVICE_DETACHED                 - No device is attached
                USB_DEVICE_ENUMERATING              - Device is enumerating
                USB_HOLDING_OUT_OF_MEMORY           - Not enough heap space available
                USB_HOLDING_UNSUPPORTED_DEVICE      - Invalid configuration or unsupported class
                USB_HOLDING_UNSUPPORTED_HUB         - Hubs are not supported
                USB_HOLDING_INVALID_CONFIGURATION   - Invalid configuration requested
                Other                               - Device is holding in an error state.
                                                      The return value indicates the error.

Remarks:        None
*******************************************************************************/
BYTE    USBHostDeviceStatus( BYTE deviceAddress );

/*******************************************************************************
Function:       BYTE * USBHostGetCurrentConfigurationDescriptor( BYTE deviceAddress )

Preconditions:  None

Overview:       This function returns a pointer to the current configuration
                descriptor of the requested device.

Input:          BYTE deviceAddress  - Address of device

Output:         None - None

Return:         BYTE *  - Pointer to the Configuration Descriptor.

Remarks:        None

TODO:           This will need to be expanded to a full function when
                multiple device support is added.
*******************************************************************************/
#define USBHostGetCurrentConfigurationDescriptor( deviceAddress) ( pCurrentConfigurationDescriptor )

/*******************************************************************************
Function:       BYTE * USBHostGetDeviceDescriptor( BYTE deviceAddress )

Preconditions:  None

Overview:       This function returns a pointer to the device descriptor of the
                requested device.

Input:          BYTE deviceAddress  - Address of device

Output:         None - None

Return:         BYTE *  - Pointer to the Device Descriptor.

Remarks:        None

TODO:           This will need to be expanded to a full function when
                multiple device support is added.
*******************************************************************************/
#define USBHostGetDeviceDescriptor( deviceAddress )     ( pDeviceDescriptor )

/*******************************************************************************
Function:       BYTE USBHostGetStringDescriptor ( BYTE deviceAddress,  BYTE stringNumber,
                        BYTE *stringDescriptor, BYTE stringLength )

Preconditions:  None

Overview:       This routine initiates a request to obtains the requested string
                descriptor.  If the request cannot be started, the routine returns
                an error.  Otherwise, the request is started, and the requested
                string descriptor is stored in the designated location.

Input:          deviceAddress       - Address of the device
                stringNumber        - Index of the desired string descriptor
                *stringDescriptor   - Pointer to where to store the string.
                stringLength        - Maximum length of the returned string.

Output:         None - None

Return:         USB_SUCCESS             - The request was started successfully.
                USB_UNKNOWN_DEVICE      - Device not found
                USB_INVALID_STATE       - We must be in a normal running state.
                USB_ENDPOINT_BUSY       - The endpoint is currently processing a request.

Remarks:        The returned string descriptor will be in the exact format as
                obtained from the device.  The length of the entire descriptor
                will be in the first byte, and the descriptor type will be in
                the second.  The string itself is represented in UNICODE.  Refer
                to the USB 2.0 Specification for more information about the
                format of string descriptors.
*******************************************************************************/
#define USBHostGetStringDescriptor( deviceAddress, stringNumber, stringDescriptor, stringLength )                               \
        USBHostDeviceRequest( deviceAddress, USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_DEVICE,   \
                USB_REQUEST_GET_DESCRIPTOR, (USB_DESCRIPTOR_STRING << 8) | stringNumber,                                        \
                0, stringLength, stringDescriptor, USB_DEVICE_REQUEST_GET )


/*******************************************************************************
Function:       void USBHostInit( void )

Preconditions:  This routine has not yet been called.

Overview:       This function is called to initialize the variables of the
                USB host operation.  It does not initialize the hardware.  The
                peripheral itself is initialized in one of the state machine
                states.  Therefore, USBHostTasks() should be called soon after
                this function.

Input:          None - None

Output:         None - None

Return:         USB_SUCCESS                 - Initialization successful
                USB_MEMORY_ALLOCATION_ERROR - Could not allocate memory.

Remarks:        None
*******************************************************************************/
BYTE    USBHostInit( void );

/*******************************************************************************
Function:       BYTE USBHostRead( BYTE deviceAddress, BYTE endpoint, BYTE *pData,
                        DWORD size )

Preconditions:  None

Overview:       This function initiates a read from the attached device.

Input:          BYTE deviceAddress  - Device address
                BYTE endpoint       - Endpoint number
                BYTE *pData         - Pointer to where to store the data
                DWORD size          - Number of data bytes to store

Output:         None - None

Return:         USB_SUCCESS                     - Read started successfully.
                USB_INVALID_STATE               - We are not in a normal running state.
                USB_ENDPOINT_ILLEGAL_TYPE       - Must use USBHostControlRead to
                                                  read from a control endpoint.
                USB_ENDPOINT_ILLEGAL_DIRECTION  - Must read from an IN endpoint.
                USB_ENDPOINT_STALLED            - Endpoint is stalled.  Must be
                                                  cleared by the application.
                USB_ENDPOINT_ERROR              - Endpoint has too many errors.  Must
                                                  be cleared by the application.
                USB_ENDPOINT_BUSY               - A Read is already in progress.
                USB_ENDPOINT_NOT_FOUND          - Invalid endpoint.

Remarks:        None
*******************************************************************************/
BYTE    USBHostRead( BYTE deviceAddress, BYTE endpoint, BYTE *data, DWORD size );

/*******************************************************************************
Function:       BYTE USBHostResetDevice( BYTE deviceAddress )

Preconditions:  None

Overview:       This function places the device back in the RESET state, to
                issue RESET signaling.  It can be called only if the state
                machine is not in the DETACHED state.

Input:          BYTE deviceAddress  - Device address

Output:         None - None

Return:         USB_SUCCESS         - Success
                USB_UNKNOWN_DEVICE  - Device not found
                USB_ILLEGAL_REQUEST - Device cannot RESUME unless it is suspended

Remarks:        None
*******************************************************************************/
BYTE    USBHostResetDevice( BYTE deviceAddress );

/*******************************************************************************
Function:       BYTE USBHostResumeDevice( BYTE deviceAddress )

Preconditions:  None

Overview:       This function issues a RESUME to the attached device.  It can
                called only if the state machine is in the suspend state.

Input:          BYTE deviceAddress  - Device address

Output:         None - None

Return:         USB_SUCCESS         - Success
                USB_UNKNOWN_DEVICE  - Device not found
                USB_ILLEGAL_REQUEST - Device cannot RESUME unless it is suspended

Remarks:        None
*******************************************************************************/
BYTE    USBHostResumeDevice( BYTE deviceAddress );

/*******************************************************************************
Function:       BYTE USBHostSetDeviceConfiguration( BYTE deviceAddress,
                    BYTE configuration )

Preconditions:  The host state machine should be in the running state, and no
                reads or writes should be in progress.

Overview:       This function is used by the application to change the device's
                Configuration.  This function must be used instead of
                USBHostDeviceRequest(), because the endpoint definitions may
                change.

Input:          BYTE deviceAddress  - Device address
                BYTE configuration  - Index of the new configuration

Output:         None - None

Return:         USB_SUCCESS         - Process of changing the configuration
                                      was started successfully.
                USB_UNKNOWN_DEVICE  - Device not found
                USB_INVALID_STATE   - This function cannot be called during
                                      enumeration or while performing a
                                      device request.
                USB_BUSY            - No IN or OUT transfers may be in
                                      progress.

Remarks:        If an invalid configuration is specified, this function will
                not return an error.  Instead, the device will be placed in
                a holding state with a USB_HOLDING_UNSUPPORTED_DEVICE error.
*******************************************************************************/
BYTE    USBHostSetDeviceConfiguration( BYTE deviceAddress, BYTE configuration );

/*******************************************************************************
Function:       void USBHostShutdown( void )

Preconditions:  None

Overview:       This function turns off the USB module and frees all
                unnecessary memory.

Input:          None - None

Output:         None - None

Return:         None - None

Remarks:        None
*******************************************************************************/
void    USBHostShutdown( void );

/*******************************************************************************
Function:       BYTE USBHostSuspendDevice( BYTE deviceAddress )

Preconditions:  None

Overview:       This function put a device into an IDLE state.  It can only be
                called while the state machine is in normal running mode.
                After 3ms, the attached device should go into SUSPEND mode.

Input:          BYTE deviceAddress  - Device to suspend

Output:         None - None

Return:         USB_SUCCESS         - Success
                USB_UNKNOWN_DEVICE  - Device not found
                USB_ILLEGAL_REQUEST - Cannot suspend unless device is in normal
                                        run mode

Remarks:        None
*******************************************************************************/
BYTE    USBHostSuspendDevice( BYTE deviceAddress );

/*******************************************************************************
Function:       void USBHostTasks( void )

Preconditions:  USBHostInit() has been called.

Overview:       This function executes the host tasks for USB host operation.
                It must be executed on a regular basis to keep everything
                functioning.

Input:          None - None

Output:         None - None

Return:         None - None

Remarks:        None
*******************************************************************************/
void    USBHostTasks( void );

/*******************************************************************************
Function:       void USBHostTerminateTransfer( BYTE deviceAddress, BYTE endpoint )

Preconditions:  None

Overview:       This function terminates the current transfer for the given
                endpoint.  It can be used to terminate reads or writes that
                the device is not responding to.

Input:          BYTE deviceAddress  - Device address
                BYTE endpoint       - Endpoint number

Output:         None - None

Return:         None - None

Remarks:        None
*******************************************************************************/
void    USBHostTerminateTransfer( BYTE deviceAddress, BYTE endpoint );

/*******************************************************************************
Function:       BOOL USBHostTransferIsComplete( BYTE deviceAddress, BYTE endpoint,
                        BYTE *errorCode, DWORD *byteCount )

Preconditions:  None

Overview:       This function initiates whether the last endpoint transaction is
                complete.  If it is complete, an error code and the number of
                bytes transferred is returned.

Input:          BYTE deviceAddress  - Device address
                BYTE endpoint       - Endpoint number

Output:         BYTE *errorCode - Error code indicating the status of the
                                    transfer. Only valid if the transfer is complete.
                DWORD *byteCount - The number of bytes sent or received.

Return:         TRUE    - Transfer is complete.
                FALSE   - Transfer is not complete.

Remarks:        Possible values for errorCode are:
                        * USB_SUCCESS                     - Transfer successful
                        * USB_ENDPOINT_STALLED            - Endpoint STALL'd
                        * USB_ENDPOINT_ERROR_ILLEGAL_PID  - Illegal PID returned
                        * USB_ENDPOINT_ERROR_BIT_STUFF
                        * USB_ENDPOINT_ERROR_DMA
                        * USB_ENDPOINT_ERROR_TIMEOUT
                        * USB_ENDPOINT_ERROR_DATA_FIELD
                        * USB_ENDPOINT_ERROR_CRC16
                        * USB_ENDPOINT_ERROR_END_OF_FRAME
                        * USB_ENDPOINT_ERROR_PID_CHECK
                        * USB_ENDPOINT_ERROR              - Other error
*******************************************************************************/
BOOL    USBHostTransferIsComplete( BYTE deviceAddress, BYTE endpoint, BYTE *errorCode, DWORD *byteCount );

/*******************************************************************************
Function:       BYTE  USBHostVbusEvent( USB_EVENT vbusEvent, BYTE deviceNumber,
                                        BYTE portNumber)

Preconditions:  TBD

Overview:       Handles VBus events.

Input:          USB_EVENT vbusEvent     - Vbus event that occured.  Valid events:
                                            * EVENT_VBUS_OVERCURRENT
                                            * EVENT_VBUS_SES_REQUEST
                BYTE deviceNumber       - Number of the hub device (USB_ROOT_HUB
                                          for the root hub)
                BYTE portNumber         - Number of the port on the hub (0 - based)

Output:

Return:         USB_SUCCESS             - Event handled
                USB_ILLEGAL_REQUEST     - Invalid event, hub, or port

Remarks:          None
*******************************************************************************/
BYTE    USBHostVbusEvent(USB_EVENT vbusEvent, BYTE deviceNumber, BYTE portNumber);

/*******************************************************************************
Function:       BYTE USBHostWrite( BYTE deviceAddress, BYTE endpoint, BYTE *data,
                        DWORD size )

Preconditions:  None

Overview:       This function initiates a write to the attached device.

Input:          BYTE deviceAddress  - Device address
                BYTE endpoint       - Endpoint number
                BYTE *data          - Pointer to where the data is stored
                DWORD size          - Number of data bytes to send

Output:         None - None

Return:         USB_SUCCESS                     - Write started successfully.
                USB_INVALID_STATE               - We are not in a normal running state.
                USB_ENDPOINT_ILLEGAL_TYPE       - Must use USBHostControlWrite to
                                                  write to a control endpoint.
                USB_ENDPOINT_ILLEGAL_DIRECTION  - Must write to an OUT endpoint.
                USB_ENDPOINT_STALLED            - Endpoint is stalled.  Must be
                                                  cleared by the application.
                USB_ENDPOINT_ERROR              - Endpoint has too many errors.  Must
                                                  be cleared by the application.
                USB_ENDPOINT_BUSY               - A Read is already in progress.
                USB_ENDPOINT_NOT_FOUND          - Invalid endpoint.

Remarks:        None
*******************************************************************************/
BYTE    USBHostWrite( BYTE deviceAddress, BYTE endpoint, BYTE *data, DWORD size );


#endif

// *****************************************************************************
// EOF USBHost.h

