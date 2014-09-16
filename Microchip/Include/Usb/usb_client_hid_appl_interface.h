#ifndef _USBHIDAPPLINTERFACE_H_ /* _USBHIDApplInterface.h */
#define _USBHIDAPPLINTERFACE_H_

#include "GenericTypeDefs.h"
#include "system.h"
#include "usb_config.h"
#include "usb_host.h"
#include "usb_client_hid.h"
#include "usb_client_hid_parser.h"



typedef struct _HID_DATA_DETAILS
{
    BYTE reportLength;// reportLength - the expected length of the parent report       
    BYTE reportID;    // reportID - report ID - the first byte of the parent report          
    BYTE bitOffset;   // BitOffset - bit offset within the report                         
    BYTE bitLength;   // bitlength - length of the data in bits                           
    BYTE count;       // count - what's left of the message after this data   
    BYTE signExtend;  // extend - sign extend the data                                 
}   HID_DATA_DETAILS;     //                                                               
// Note: One would ordinarily get these values from a parser     
//       find. (e.g. USBHostHID_ApiFindValue)                                  
//

extern USB_HID_DEVICE_RPT_INFO deviceRptInfo;
extern USB_HID_ITEM_LIST       itemListPtrs;


// *****************************************************************************
// Prototypes for Application interface funtions for usb_client_hid.c
// *****************************************************************************

BOOL USBHostHID_APIEventHandler( BYTE address, USB_EVENT event, void *data, unsigned int size );
BOOL USBHostHID_APIInitialize( BYTE address, unsigned int flags );

// *****************************************************************************
// Prototypes for interface funtions for Application layer
// *****************************************************************************

/*******************************************************************************
Function:       BOOL USBHostHID_ApiDeviceDetect( void )

Precondition:   None

Overview:       This function determines if a HID device is attached
                and ready to use.

Input:          None

Output:         None

Return Values:  TRUE:   HID present and ready
                FALSE:  HID not present or not ready

Remarks:        Since this will often be called in a loop while waiting for
                a device, we'll make sure the tasks are executed.
*******************************************************************************/
BOOL USBHostHID_ApiDeviceDetect( void );

/*******************************************************************************
Function:       BYTE USBHostHID_ApiImportData( void )

Precondition:   None

Overview:       This function imports Data from a report

Input:          BYTE *report                   - the report buffer
                WORD reportLength              - length of the report
                HID_DATA_DETAILS *pDataDetails - This structure holds the information of the data
                                   One would ordinarily get these values from a parser
                                   (e.g. USBHostHID_ApiFindValue()). Please look into structure declaration
                                   for details.
                            
Output:         WORD *buffer - required data is copied into this location

Return Values:  TRUE:   Requested data is found in the report
                FALSE:  Requested data is not found int he report

Remarks:        
*******************************************************************************/
BOOL USBHostHID_ApiImportData(BYTE *report,WORD reportLength,HID_USER_DATA_SIZE *buffer, HID_DATA_DETAILS *pDataDetails);;

/*******************************************************************************
Function:       BYTE USBHostHID_ApiGetReport(BYTE reportid, BYTE size,BYTE* data)

Precondition:   None

Overview:       This function request device for a input report

Input:          BYTE reportid - Report ID of the requested report
                BYTE size     - Size of the report requested from device
                            
Output:         WORD *data - Report data is copied into buffer

Return Values:  USB_SUCCESS                 - Request started successfully
                USB_HID_DEVICE_NOT_FOUND    - No device with specified address
                USB_HID_DEVICE_BUSY         - Device not in proper state for
                                              performing a transfer

Remarks:        
*******************************************************************************/
BYTE USBHostHID_ApiGetReport(BYTE reportid, BYTE size,BYTE* data);

/*******************************************************************************
Function:       BOOL USBHostHID_ApiTransferIsComplete(BYTE* errorCodeDriver, BYTE* byteCount )

Precondition:   None

Overview:       This function indicates whether or not the last transfer is complete.
                If the functions returns TRUE, the returned byte count and error
                code are valid. Since only one transfer can be performed at once
                and only one endpoint can be used, we only need to know the
                device address.

Input:          None
                            
Output:         BYTE *errorCode     - Error code from last transfer
                DWORD *byteCount    - Number of bytes transferred

Return Values:  TRUE    - Transfer is complete, errorCode is valid
                FALSE   - Transfer is not complete, errorCode is not valid

Remarks:        
*******************************************************************************/
BOOL USBHostHID_ApiTransferIsComplete(BYTE* errorCodeDriver, BYTE* byteCount );

/*******************************************************************************
Function:       BOOL USBHostHID_ApiFindBit(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length, BYTE* Start_Bit)

Precondition:   None

Overview:       This function is used to find a specific Button or Indicator

Input:          WORD usagePage         - Specifies the usage Page to be searched in the report
                                         descriptor, e.g(Generic Desktop, Key Codes)
                WORD usage             - Usage tag for the specified item in the report
                HIDReportTypeEnum type - Input or output report type enum
                            
Output:         BYTE* Report_ID        - Report id of the report where the requested usage is located
                BYTE* Report_Length    - Lenght of the report associated with the usage
                BYTE* Start_Bit        - Start bit location of the requested item in the report

Return Values:  TRUE    - Requested Button is located
                FALSE   - Requested Button is not found

Remarks:        
*******************************************************************************/
BOOL USBHostHID_ApiFindBit(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length, BYTE* Start_Bit);

/*******************************************************************************
Function:       BOOL USBHostHID_ApiFindValue(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length,BYTE* Start_Bit, BYTE* Bit_Length)

Precondition:   None

Overview:       This function is used to find a specific Usage value

Input:          WORD usagePage         - Specifies the usage Page to be searched in the report
                                         descriptor, e.g(Generic Desktop, Key Codes)
                WORD usage             - Usage tag for the specified item in the report
                HIDReportTypeEnum type - Input or output report type enum
                            
Output:         BYTE* Report_ID        - Report id of the report where the requested usage is located
                BYTE* Report_Length    - Lenght of the report associated with the usage
                BYTE* Start_Bit        - Start bit location of the requested item in the report
                BYTE* Bit_Length       - Size of the usage requested

Return Values:  TRUE    - Requested usage Value is located
                FALSE   - Requested usage Value is not found

Remarks:        
*******************************************************************************/
BOOL USBHostHID_ApiFindValue(WORD usagePage,WORD usage,HIDReportTypeEnum type,BYTE* Report_ID,
                    BYTE* Report_Length,BYTE* Start_Bit, BYTE* Bit_Length);


#endif
