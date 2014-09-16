/*******************************************************************************

    USB Core Header File

*******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************

* FileName:        USBCore.h
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

 ******************************************************************************/

//*******************************************************************
// Section: USB Descriptors
//*******************************************************************

#define USB_DESCRIPTOR_DEVICE           0x01    // bDescriptorType for a Device Descriptor.
#define USB_DESCRIPTOR_CONFIGURATION    0x02    // bDescriptorType for a Configuration Descriptor.
#define USB_DESCRIPTOR_STRING           0x03    // bDescriptorType for a String Descriptor.
#define USB_DESCRIPTOR_INTERFACE        0x04    // bDescriptorType for an Interface Descriptor.
#define USB_DESCRIPTOR_ENDPOINT         0x05    // bDescriptorType for an Endpoint Descriptor.
#define USB_DESCRIPTOR_DEVICE_QUALIFIER 0x06    // bDescriptorType for a Device Qualifier.
#define USB_DESCRIPTOR_OTHER_SPEED      0x07    // bDescriptorType for a Other Speed Configuration.
#define USB_DESCRIPTOR_INTERFACE_POWER  0x08    // bDescriptorType for Interface Power.
#define USB_DESCRIPTOR_OTG              0x09    // bDescriptorType for an OTG Descriptor.

// *****************************************************************************
/* USB Device Descriptor Structure

This struct defines the structure of a USB Device Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_DEVICE_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // DEVICE descriptor type (USB_DESCRIPTOR_DEVICE).
    WORD bcdUSB;                // USB Spec Release Number (BCD).
    BYTE bDeviceClass;          // Class code (assigned by the USB-IF). 0xFF-Vendor specific.
    BYTE bDeviceSubClass;       // Subclass code (assigned by the USB-IF).
    BYTE bDeviceProtocol;       // Protocol code (assigned by the USB-IF). 0xFF-Vendor specific.
    BYTE bMaxPacketSize0;       // Maximum packet size for endpoint 0.
    WORD idVendor;              // Vendor ID (assigned by the USB-IF).
    WORD idProduct;             // Product ID (assigned by the manufacturer).
    WORD bcdDevice;             // Device release number (BCD).
    BYTE iManufacturer;         // Index of String Descriptor describing the manufacturer.
    BYTE iProduct;              // Index of String Descriptor describing the product.
    BYTE iSerialNumber;         // Index of String Descriptor with the device's serial number.
    BYTE bNumConfigurations;    // Number of possible configurations.
} USB_DEVICE_DESCRIPTOR;


// *****************************************************************************
/* USB Configuration Descriptor Structure

This struct defines the structure of a USB Configuration Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_CONFIGURATION_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // CONFIGURATION descriptor type (USB_DESCRIPTOR_CONFIGURATION).
    WORD wTotalLength;          // Total length of all descriptors for this configuration.
    BYTE bNumInterfaces;        // Number of interfaces in this configuration.
    BYTE bConfigurationValue;   // Value of this configuration (1 based).
    BYTE iConfiguration;        // Index of String Descriptor describing the configuration.
    BYTE bmAttributes;          // Configuration characteristics.
    BYTE bMaxPower;             // Maximum power consumed by this configuration.
} USB_CONFIGURATION_DESCRIPTOR;


// *****************************************************************************
/* USB Interface Descriptor Structure

This struct defines the structure of a USB Interface Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_INTERFACE_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // INTERFACE descriptor type (USB_DESCRIPTOR_INTERFACE).
    BYTE bInterfaceNumber;      // Number of this interface (0 based).
    BYTE bAlternateSetting;     // Value of this alternate interface setting.
    BYTE bNumEndpoints;         // Number of endpoints in this interface.
    BYTE bInterfaceClass;       // Class code (assigned by the USB-IF).  0xFF-Vendor specific.
    BYTE bInterfaceSubClass;    // Subclass code (assigned by the USB-IF).
    BYTE bInterfaceProtocol;    // Protocol code (assigned by the USB-IF).  0xFF-Vendor specific.
    BYTE iInterface;            // Index of String Descriptor describing the interface.
} USB_INTERFACE_DESCRIPTOR;


// *****************************************************************************
/* USB Endpoint Descriptor Structure

This struct defines the structure of a USB Endpoint Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_ENDPOINT_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // ENDPOINT descriptor type (USB_DESCRIPTOR_ENDPOINT).
    BYTE bEndpointAddress;      // Endpoint address. Bit 7 indicates direction (0=OUT, 1=IN).
    BYTE bmAttributes;          // Endpoint transfer type.
    WORD wMaxPacketSize;        // Maximum packet size.
    BYTE bInterval;             // Polling interval in frames.
} USB_ENDPOINT_DESCRIPTOR;


// *****************************************************************************
/* USB OTG Descriptor Structure

This struct defines the structure of a USB OTG Descriptor.  Note that this
structure may need to be packed, or even accessed as bytes, to properly access
the correct fields when used on some device architectures.
*/
typedef struct _USB_OTG_DESCRIPTOR
{
    BYTE bLength;               // Length of this descriptor.
    BYTE bDescriptorType;       // OTG descriptor type (USB_DESCRIPTOR_OTG).
    BYTE bmAttributes;          // OTG attributes.
} USB_OTG_DESCRIPTOR;


// ******************************************************************
// USB Specification Constants
// ******************************************************************

// Section: Valid PID Values

#define PID_OUT                                 0x01    // PID for an OUT token
#define PID_IN                                  0x09    // PID for a IN token
#define PID_SOF                                 0x05    // PID for a SOF token
#define PID_SETUP                               0x0D    // PID for a SETUP token
#define PID_DATA0                               0x03    // PID for DATA0 data
#define PID_DATA1                               0x0B    // PID for DATA1 data
#define PID_DATA2                               0x07    // PID for DATA2 data
#define PID_MDATA                               0x0F    // PID for MDATA data
#define PID_ACK                                 0x02    // PID for an ACK handshake
#define PID_NAK                                 0x0A    // PID for a NAK handshake
#define PID_STALL                               0x0E    // PID for a STALL handshake
#define PID_NYET                                0x06    // PID for a NYET handshake
#define PID_PRE                                 0x0C    // Special PID PRE (Same as PID_ERR)
#define PID_ERR                                 0x0C    // Special PID ERR (Same as PID_PRE)
#define PID_SPLIT                               0x08    // Special PID SPLIT
#define PID_PING                                0x04    // Special PID PING

#define PID_MASK_DATA                           0x03    // Data PID mask
#define PID_MASK_DATA_SHIFTED                  (PID_MASK_DATA << 2) // Data PID shift to proper position

// Section: OTG Descriptor Constants

#define OTG_HNP_SUPPORT                         0x02    // OTG Descriptor bmAttributes - HNP support flag
#define OTG_SRP_SUPPORT                         0x01    // OTG Descriptor bmAttributes - SRP support flag

// Section: Endpoint Directions

#define USB_IN_EP                               0x80    // IN endpoint mask
#define USB_OUT_EP                              0x00    // OUT endpoint mask

/********************************************************************
 * Buffer Descriptor Status Register
 *******************************************************************/
//DOM-IGNORE-BEGIN

#if defined( __18CXX )
    /* Buffer Descriptor Status Register Initialization Parameters */
    #define _BSTALL     0x04        //Buffer Stall enable
    #define _DTSEN      0x08        //Data Toggle Synch enable
    #define _INCDIS     0x10        //Address increment disable
    #define _KEN        0x20        //SIE keeps buff descriptors enable
    #define _DAT0       0x00        //DATA0 packet expected next
    #define _DAT1       0x40        //DATA1 packet expected next
    #define _DTSMASK    0x40        //DTS Mask
    #define _USIE       0x80        //SIE owns buffer
    #define _UCPU       0x00        //CPU owns buffer

    #define _STAT_MASK  0xFF

    /* BDT entry structure definition */
    typedef union _BD_STAT
    {
        BYTE Val;
        struct{
            //If the CPU owns the buffer then these are the values
            unsigned BC8:1;         //bit 8 of the byte count
            unsigned BC9:1;         //bit 9 of the byte count
            unsigned BSTALL:1;      //Buffer Stall Enable
            unsigned DTSEN:1;       //Data Toggle Synch Enable
            unsigned INCDIS:1;      //Address Increment Disable
            unsigned KEN:1;         //BD Keep Enable
            unsigned DTS:1;         //Data Toggle Synch Value
            unsigned UOWN:1;        //USB Ownership
        };
        struct{
            //if the USB module owns the buffer then these are
            // the values
            unsigned BC8:1;         //bit 8 of the byte count
            unsigned BC9:1;         //bit 9 of the byte count
            unsigned PID0:1;        //Packet Identifier
            unsigned PID1:1;
            unsigned PID2:1;
            unsigned PID3:1;
            unsigned :1;
            unsigned UOWN:1;        //USB Ownership
        };
        struct{
            unsigned :2;
            unsigned PID:4;         //Packet Identifier
            unsigned :2;
        };
    } BD_STAT;                      //Buffer Descriptor Status Register

#elif defined( __C30__)
    /* Buffer Descriptor Status Register Initialization Parameters */

    //The _BSTALL definition is changed from 0x04 to 0x00 to 
    // fix a difference in the PIC18 and PIC24 definitions of this
    // bit.  This should be changed back once the definitions are
    // synced.
    #define _BSTALL     0x00//0x04        //Buffer Stall enable
    #define _DTSEN      0x08        //Data Toggle Synch enable
    #define _DAT0       0x00        //DATA0 packet expected next
    #define _DAT1       0x40        //DATA1 packet expected next
    #define _DTSMASK    0x40        //DTS Mask
    #define _USIE       0x80        //SIE owns buffer
    #define _UCPU       0x00        //CPU owns buffer

    #define _STAT_MASK  0xFC

    // Buffer Descriptor Status Register layout.
    typedef union _BD_STAT
    {
        struct{
            unsigned            :2;      //Byte count
            unsigned    BSTALL  :1;     //Buffer Stall Enable
            unsigned    DTSEN   :1;     //Data Toggle Synch Enable
            unsigned            :2;     //Reserved - write as 00
            unsigned    DTS     :1;     //Data Toggle Synch Value
            unsigned    UOWN    :1;     //USB Ownership
        };
        struct{
            unsigned            :2;
            unsigned    PID0    :1;
            unsigned    PID1    :1;
            unsigned    PID2    :1;
            unsigned    PID3    :1;
        };
        struct{
            unsigned            :2;
            unsigned    PID     :4;     // Packet Identifier
        };
        BYTE            Val; 
    } BD_STAT;                      //Buffer Descriptor Status Register
#else
    #error Cannot define BD status structure.
#endif

//DOM-IGNORE-END

/********************************************************************
 * Buffer Descriptor Table Mapping
 *******************************************************************/
//DOM-IGNORE-BEGIN

#if defined (__18CXX)

    // BDT Entry Layout
    typedef union __BDT
    {
        struct
        {
            BD_STAT STAT; 
            BYTE CNT;
            BYTE ADRL;                      //Buffer Address Low
            BYTE ADRH;                      //Buffer Address High
        };
        struct
        {
            unsigned :8;
            unsigned :8;
            BYTE* ADR;                      //Buffer Address
        };
        DWORD Val;
        BYTE v[4];
    } BDT_ENTRY;

#elif defined( __C30__)

    // BDT Entry Layout
    typedef union __BDT
    {
        union
        {
            struct
            {
                BYTE CNT         __attribute__ ((packed));
                BD_STAT     STAT __attribute__ ((packed));
            };
            struct
            {
                WORD        count:10;   //test
                BYTE        :6; 
                BYTE*       ADR; //Buffer Address
            };
        };
        DWORD           Val;
        WORD            v[2];
    } BDT_ENTRY;
#else
    #error Cannot define BDT entry structure.
#endif

//DOM-IGNORE-END

//******************************************************************************
// USB Endpoint Control Registers
//
// In USB Host mode, only EP0 control registers are used.  The other registers
// should be disabled.
//******************************************************************************
//DOM-IGNORE-BEGIN

#if defined (__18CXX)
    typedef union
    {
        BYTE UEP[16];
    } _UEP;
#elif defined( __C30__)
    typedef union
    {
        WORD UEP[16];
    } _UEP;
#endif


//DOM-IGNORE-END

/********************************************************************
 * USB - PIC Endpoint Definitions
 * PIC Endpoint Address Format: X:EP3:EP2:EP1:EP0:DIR:PPBI:X
 * This is used when checking the value read from USTAT
 *
 * NOTE: These definitions are not used in the descriptors.
 * EP addresses used in the descriptors have different format.
 *******************************************************************/
//DOM-IGNORE-BEGIN

#if defined(__18CXX)
    #define USTAT_EP0_PP_MASK   ~0x02
    #define USTAT_EP_MASK       0x7E
    #define USTAT_EP0_OUT       0x00
    #define USTAT_EP0_OUT_EVEN  0x00
    #define USTAT_EP0_OUT_ODD   0x02
    
    #define USTAT_EP0_IN        0x04
    #define USTAT_EP0_IN_EVEN   0x04
    #define USTAT_EP0_IN_ODD    0x06
#elif defined(__C30__)
    #define USTAT_EP0_PP_MASK   ~0x04
    #define USTAT_EP_MASK       0xFC
    #define USTAT_EP0_OUT       0x00
    #define USTAT_EP0_OUT_EVEN  0x00
    #define USTAT_EP0_OUT_ODD   0x04
    
    #define USTAT_EP0_IN        0x08
    #define USTAT_EP0_IN_EVEN   0x08
    #define USTAT_EP0_IN_ODD    0x0C
#else
    #error "unimplmented"
#endif

#define OUT         0                           //
#define IN          1                           //

#if (USB_PING_PONG_MODE == USB_PING_PONG__NO_PING_PONG)
    #define USB_NEXT_EP0_OUT_PING_PONG 0x0000   // Used in USB Device Mode only
    #define USB_NEXT_EP0_IN_PING_PONG 0x0000    // Used in USB Device Mode only
    #define USB_NEXT_PING_PONG 0x0000           // Used in USB Device Mode only
    #define EP0_OUT_EVEN    0                   // Used in USB Device Mode only
    #define EP0_OUT_ODD     0                   // Used in USB Device Mode only
    #define EP0_IN_EVEN     1                   // Used in USB Device Mode only
    #define EP0_IN_ODD      1                   // Used in USB Device Mode only
    #define EP1_OUT_EVEN    2                   // Used in USB Device Mode only
    #define EP1_OUT_ODD     2                   // Used in USB Device Mode only
    #define EP1_IN_EVEN     3                   // Used in USB Device Mode only
    #define EP1_IN_ODD      3                   // Used in USB Device Mode only
    #define EP2_OUT_EVEN    4                   // Used in USB Device Mode only
    #define EP2_OUT_ODD     4                   // Used in USB Device Mode only
    #define EP2_IN_EVEN     5                   // Used in USB Device Mode only
    #define EP2_IN_ODD      5                   // Used in USB Device Mode only
    #define EP3_OUT_EVEN    6                   // Used in USB Device Mode only
    #define EP3_OUT_ODD     6                   // Used in USB Device Mode only
    #define EP3_IN_EVEN     7                   // Used in USB Device Mode only
    #define EP3_IN_ODD      7                   // Used in USB Device Mode only
    #define EP4_OUT_EVEN    8                   // Used in USB Device Mode only
    #define EP4_OUT_ODD     8                   // Used in USB Device Mode only
    #define EP4_IN_EVEN     9                   // Used in USB Device Mode only
    #define EP4_IN_ODD      9                   // Used in USB Device Mode only
    #define EP5_OUT_EVEN    10                  // Used in USB Device Mode only
    #define EP5_OUT_ODD     10                  // Used in USB Device Mode only
    #define EP5_IN_EVEN     11                  // Used in USB Device Mode only
    #define EP5_IN_ODD      11                  // Used in USB Device Mode only
    #define EP6_OUT_EVEN    12                  // Used in USB Device Mode only
    #define EP6_OUT_ODD     12                  // Used in USB Device Mode only
    #define EP6_IN_EVEN     13                  // Used in USB Device Mode only
    #define EP6_IN_ODD      13                  // Used in USB Device Mode only
    #define EP7_OUT_EVEN    14                  // Used in USB Device Mode only
    #define EP7_OUT_ODD     14                  // Used in USB Device Mode only
    #define EP7_IN_EVEN     15                  // Used in USB Device Mode only
    #define EP7_IN_ODD      15                  // Used in USB Device Mode only
    #define EP8_OUT_EVEN    16                  // Used in USB Device Mode only
    #define EP8_OUT_ODD     16                  // Used in USB Device Mode only
    #define EP8_IN_EVEN     17                  // Used in USB Device Mode only
    #define EP8_IN_ODD      17                  // Used in USB Device Mode only
    #define EP9_OUT_EVEN    18                  // Used in USB Device Mode only
    #define EP9_OUT_ODD     18                  // Used in USB Device Mode only
    #define EP9_IN_EVEN     19                  // Used in USB Device Mode only
    #define EP9_IN_ODD      19                  // Used in USB Device Mode only
    #define EP10_OUT_EVEN   20                  // Used in USB Device Mode only
    #define EP10_OUT_ODD    20                  // Used in USB Device Mode only
    #define EP10_IN_EVEN    21                  // Used in USB Device Mode only
    #define EP10_IN_ODD     21                  // Used in USB Device Mode only
    #define EP11_OUT_EVEN   22                  // Used in USB Device Mode only
    #define EP11_OUT_ODD    22                  // Used in USB Device Mode only
    #define EP11_IN_EVEN    23                  // Used in USB Device Mode only
    #define EP11_IN_ODD     23                  // Used in USB Device Mode only
    #define EP12_OUT_EVEN   24                  // Used in USB Device Mode only
    #define EP12_OUT_ODD    24                  // Used in USB Device Mode only
    #define EP12_IN_EVEN    25                  // Used in USB Device Mode only
    #define EP12_IN_ODD     25                  // Used in USB Device Mode only
    #define EP13_OUT_EVEN   26                  // Used in USB Device Mode only
    #define EP13_OUT_ODD    26                  // Used in USB Device Mode only
    #define EP13_IN_EVEN    27                  // Used in USB Device Mode only
    #define EP13_IN_ODD     27                  // Used in USB Device Mode only
    #define EP14_OUT_EVEN   28                  // Used in USB Device Mode only
    #define EP14_OUT_ODD    28                  // Used in USB Device Mode only
    #define EP14_IN_EVEN    29                  // Used in USB Device Mode only
    #define EP14_IN_ODD     29                  // Used in USB Device Mode only
    #define EP15_OUT_EVEN   30                  // Used in USB Device Mode only
    #define EP15_OUT_ODD    30                  // Used in USB Device Mode only
    #define EP15_IN_EVEN    31                  // Used in USB Device Mode only
    #define EP15_IN_ODD     31                  // Used in USB Device Mode only

    #define EP(ep,dir,pp) (2*ep+dir)            // Used in USB Device Mode only

    #define BD(ep,dir,pp)   ((8 * ep) + (4 * dir))      // Used in USB Device Mode only

#elif (USB_PING_PONG_MODE == USB_PING_PONG__EP0_OUT_ONLY)
    #define USB_NEXT_EP0_OUT_PING_PONG 0x0004
    #define USB_NEXT_EP0_IN_PING_PONG 0x0000
    #define USB_NEXT_PING_PONG 0x0000
    #define EP0_OUT_EVEN    0
    #define EP0_OUT_ODD     1
    #define EP0_IN_EVEN     2
    #define EP0_IN_ODD      2
    #define EP1_OUT_EVEN    3
    #define EP1_OUT_ODD     3
    #define EP1_IN_EVEN     4
    #define EP1_IN_ODD      4
    #define EP2_OUT_EVEN    5
    #define EP2_OUT_ODD     5
    #define EP2_IN_EVEN     6
    #define EP2_IN_ODD      6
    #define EP3_OUT_EVEN    7
    #define EP3_OUT_ODD     7
    #define EP3_IN_EVEN     8
    #define EP3_IN_ODD      8
    #define EP4_OUT_EVEN    9
    #define EP4_OUT_ODD     9
    #define EP4_IN_EVEN     10
    #define EP4_IN_ODD      10
    #define EP5_OUT_EVEN    11
    #define EP5_OUT_ODD     11
    #define EP5_IN_EVEN     12
    #define EP5_IN_ODD      12
    #define EP6_OUT_EVEN    13
    #define EP6_OUT_ODD     13
    #define EP6_IN_EVEN     14
    #define EP6_IN_ODD      14
    #define EP7_OUT_EVEN    15
    #define EP7_OUT_ODD     15
    #define EP7_IN_EVEN     16
    #define EP7_IN_ODD      16
    #define EP8_OUT_EVEN    17
    #define EP8_OUT_ODD     17
    #define EP8_IN_EVEN     18
    #define EP8_IN_ODD      18
    #define EP9_OUT_EVEN    19
    #define EP9_OUT_ODD     19
    #define EP9_IN_EVEN     20
    #define EP9_IN_ODD      20
    #define EP10_OUT_EVEN   21
    #define EP10_OUT_ODD    21
    #define EP10_IN_EVEN    22
    #define EP10_IN_ODD     22
    #define EP11_OUT_EVEN   23
    #define EP11_OUT_ODD    23
    #define EP11_IN_EVEN    24
    #define EP11_IN_ODD     24
    #define EP12_OUT_EVEN   25
    #define EP12_OUT_ODD    25
    #define EP12_IN_EVEN    26
    #define EP12_IN_ODD     26
    #define EP13_OUT_EVEN   27
    #define EP13_OUT_ODD    27
    #define EP13_IN_EVEN    28
    #define EP13_IN_ODD     28
    #define EP14_OUT_EVEN   29
    #define EP14_OUT_ODD    29
    #define EP14_IN_EVEN    30
    #define EP14_IN_ODD     30
    #define EP15_OUT_EVEN   31
    #define EP15_OUT_ODD    31
    #define EP15_IN_EVEN    32
    #define EP15_IN_ODD     32

    #define EP(ep,dir,pp) (2*ep+dir+((ep==0)&&(dir==0))?pp:2))
    #define BD(ep,dir,pp) (4*(ep+dir+(((ep==0)&&(dir==0))?pp:2)))

#elif (USB_PING_PONG_MODE == USB_PING_PONG__FULL_PING_PONG)
    #define USB_NEXT_EP0_OUT_PING_PONG 0x0004
    #define USB_NEXT_EP0_IN_PING_PONG 0x0004
    #define USB_NEXT_PING_PONG 0x0004
    #define EP0_OUT_EVEN    0
    #define EP0_OUT_ODD     1
    #define EP0_IN_EVEN     2
    #define EP0_IN_ODD      3
    #define EP1_OUT_EVEN    4
    #define EP1_OUT_ODD     5
    #define EP1_IN_EVEN     6
    #define EP1_IN_ODD      7
    #define EP2_OUT_EVEN    8
    #define EP2_OUT_ODD     9
    #define EP2_IN_EVEN     10
    #define EP2_IN_ODD      11
    #define EP3_OUT_EVEN    12
    #define EP3_OUT_ODD     13
    #define EP3_IN_EVEN     14
    #define EP3_IN_ODD      15
    #define EP4_OUT_EVEN    16
    #define EP4_OUT_ODD     17
    #define EP4_IN_EVEN     18
    #define EP4_IN_ODD      19
    #define EP5_OUT_EVEN    20
    #define EP5_OUT_ODD     21
    #define EP5_IN_EVEN     22
    #define EP5_IN_ODD      23
    #define EP6_OUT_EVEN    24
    #define EP6_OUT_ODD     25
    #define EP6_IN_EVEN     26
    #define EP6_IN_ODD      27
    #define EP7_OUT_EVEN    28
    #define EP7_OUT_ODD     29
    #define EP7_IN_EVEN     30
    #define EP7_IN_ODD      31
    #define EP8_OUT_EVEN    32
    #define EP8_OUT_ODD     33
    #define EP8_IN_EVEN     34
    #define EP8_IN_ODD      35
    #define EP9_OUT_EVEN    36
    #define EP9_OUT_ODD     37
    #define EP9_IN_EVEN     38
    #define EP9_IN_ODD      39
    #define EP10_OUT_EVEN   40
    #define EP10_OUT_ODD    41
    #define EP10_IN_EVEN    42
    #define EP10_IN_ODD     43
    #define EP11_OUT_EVEN   44
    #define EP11_OUT_ODD    45
    #define EP11_IN_EVEN    46
    #define EP11_IN_ODD     47
    #define EP12_OUT_EVEN   48
    #define EP12_OUT_ODD    49
    #define EP12_IN_EVEN    50
    #define EP12_IN_ODD     51
    #define EP13_OUT_EVEN   52
    #define EP13_OUT_ODD    53
    #define EP13_IN_EVEN    54
    #define EP13_IN_ODD     55
    #define EP14_OUT_EVEN   56
    #define EP14_OUT_ODD    57
    #define EP14_IN_EVEN    58
    #define EP14_IN_ODD     59
    #define EP15_OUT_EVEN   60
    #define EP15_OUT_ODD    61
    #define EP15_IN_EVEN    62
    #define EP15_IN_ODD     63

    #define EP(ep,dir,pp) (4*ep+2*dir+pp)

    #define BD(ep,dir,pp) (4*(4*ep+2*dir+pp))

#elif (USB_PING_PONG_MODE == USB_PING_PONG__ALL_BUT_EP0)
    #define USB_NEXT_EP0_OUT_PING_PONG 0x0000
    #define USB_NEXT_EP0_IN_PING_PONG 0x0000
    #define USB_NEXT_PING_PONG 0x0004
    #define EP0_OUT_EVEN    0
    #define EP0_OUT_ODD     0
    #define EP0_IN_EVEN     1
    #define EP0_IN_ODD      1
    #define EP1_OUT_EVEN    2
    #define EP1_OUT_ODD     3
    #define EP1_IN_EVEN     4
    #define EP1_IN_ODD      5
    #define EP2_OUT_EVEN    6
    #define EP2_OUT_ODD     7
    #define EP2_IN_EVEN     8
    #define EP2_IN_ODD      9
    #define EP3_OUT_EVEN    10
    #define EP3_OUT_ODD     11
    #define EP3_IN_EVEN     12
    #define EP3_IN_ODD      13
    #define EP4_OUT_EVEN    14
    #define EP4_OUT_ODD     15
    #define EP4_IN_EVEN     16
    #define EP4_IN_ODD      17
    #define EP5_OUT_EVEN    18
    #define EP5_OUT_ODD     19
    #define EP5_IN_EVEN     20
    #define EP5_IN_ODD      21
    #define EP6_OUT_EVEN    22
    #define EP6_OUT_ODD     23
    #define EP6_IN_EVEN     24
    #define EP6_IN_ODD      25
    #define EP7_OUT_EVEN    26
    #define EP7_OUT_ODD     27
    #define EP7_IN_EVEN     28
    #define EP7_IN_ODD      29
    #define EP8_OUT_EVEN    30
    #define EP8_OUT_ODD     31
    #define EP8_IN_EVEN     32
    #define EP8_IN_ODD      33
    #define EP9_OUT_EVEN    34
    #define EP9_OUT_ODD     35
    #define EP9_IN_EVEN     36
    #define EP9_IN_ODD      37
    #define EP10_OUT_EVEN   38
    #define EP10_OUT_ODD    39
    #define EP10_IN_EVEN    40
    #define EP10_IN_ODD     41
    #define EP11_OUT_EVEN   42
    #define EP11_OUT_ODD    43
    #define EP11_IN_EVEN    44
    #define EP11_IN_ODD     45
    #define EP12_OUT_EVEN   46
    #define EP12_OUT_ODD    47
    #define EP12_IN_EVEN    48
    #define EP12_IN_ODD     49
    #define EP13_OUT_EVEN   50
    #define EP13_OUT_ODD    51
    #define EP13_IN_EVEN    52
    #define EP13_IN_ODD     53
    #define EP14_OUT_EVEN   54
    #define EP14_OUT_ODD    55
    #define EP14_IN_EVEN    56
    #define EP14_IN_ODD     57
    #define EP15_OUT_EVEN   58
    #define EP15_OUT_ODD    59
    #define EP15_IN_EVEN    60
    #define EP15_IN_ODD     61

    #define EP(ep,dir,pp) (4*ep+2*dir+((ep==0)?0:(pp-2)))
    #define BD(ep,dir,pp) (4*(4*ep+2*dir+((ep==0)?0:(pp-2))))

#else
    #error "No ping pong mode defined."
#endif

//DOM-IGNORE-END


