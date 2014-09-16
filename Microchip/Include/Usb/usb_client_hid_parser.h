#ifndef _USB_CLIENT_HID_PARSER_H_ /* usb_client_hid_parser.h */
#define _USB_CLIENT_HID_PARSER_H_ 


//------------------------------------------------------------------------------
//
// HID Header
//
// ---------------------------------------------------------
// |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
// |            Tag            |    Type     |    Size     |
// ---------------------------------------------------------
//------------------------------------------------------------------------------
#define HIDItem_SizeMask           0x03
#define HIDItem_TagMask            0xF0
#define HIDItem_TagShift           0x04
#define HIDItem_TypeMask           0xC
#define HIDItem_TypeShift          0x02
#define HIDLongItemHeader          0xFE

//------------------------------------------------------------------------------
//
// HID Item Type Definitions
//
//------------------------------------------------------------------------------
#define HIDType_Main               0
#define HIDType_Global             1
#define HIDType_Local              2
#define HIDType_Long               3

//------------------------------------------------------------------------------
//
// HID Item Tag Definitions - Main Items
//
//------------------------------------------------------------------------------
#define HIDTag_Input               0x08
#define HIDTag_Output              0x09
#define HIDTag_Collection          0x0A
#define HIDTag_Feature             0x0B
#define HIDTag_EndCollection       0x0C

//------------------------------------------------------------------------------
//
// HID Item Tag Definitions - Globals
//
//------------------------------------------------------------------------------
#define HIDTag_UsagePage           0x00
#define HIDTag_LogicalMinimum      0x01
#define HIDTag_LogicalMaximum      0x02
#define HIDTag_PhysicalMinimum     0x03
#define HIDTag_PhysicalMaximum     0x04
#define HIDTag_UnitExponent        0x05
#define HIDTag_Unit                0x06
#define HIDTag_ReportSize          0x07
#define HIDTag_ReportID            0x08
#define HIDTag_ReportCount         0x09
#define HIDTag_Push                0x0A
#define HIDTag_Pop                 0x0B

//------------------------------------------------------------------------------
//
// HID Item Tag Definitions - Locals
//
//------------------------------------------------------------------------------
#define HIDTag_Usage               0x00
#define HIDTag_UsageMinimum        0x01
#define HIDTag_UsageMaximum        0x02
#define HIDTag_DesignatorIndex     0x03
#define HIDTag_DesignatorMinimum   0x04
#define HIDTag_DesignatorMaximum   0x05
#define HIDTag_StringIndex         0x07
#define HIDTag_StringMinimum       0x08
#define HIDTag_StringMaximum       0x09
#define HIDTag_SetDelimiter        0x0A

//------------------------------------------------------------------------------
//
// HID Main Item Header Bit Definitions
//
//------------------------------------------------------------------------------
#define HIDData_BufferedBytes     0x100
#define HIDData_VolatileBit        0x80
#define HIDData_Volatile           0x80
#define HIDData_NullStateBit       0x40
#define HIDData_NullState          0x40
#define HIDData_NoPreferredBit     0x20
#define HIDData_NoPreferred        0x20
#define HIDData_NonlinearBit       0x10
#define HIDData_Nonlinear          0x10
#define HIDData_WrapBit            0x08
#define HIDData_Wrap               0x08
#define HIDData_RelativeBit        0x04
#define HIDData_Relative           0x04
#define HIDData_Absolute           0x00
#define HIDData_VariableBit        0x02
#define HIDData_Variable           0x02
#define HIDData_ArrayBit           0x02
#define HIDData_Array              0x00
#define HIDData_ConstantBit        0x01
#define HIDData_Constant           0x01

//------------------------------------------------------------------------------
//
// HID Collection Data Definitions
//
//------------------------------------------------------------------------------
#define HIDCollection_Physical     0x00
#define HIDCollection_Application  0x01


typedef enum {
    hidReportInput,
    hidReportOutput,
    hidReportFeature,
    hidReportUnknown
} HIDReportTypeEnum;


    
typedef struct
{
    BYTE                     *descriptor;
    WORD                      descriptorLength;
    WORD                      index;
    WORD                      bytes;
    WORD                      type;
    WORD                      tag;
    WORD                      iSigned;
    WORD                      iUnsigned;
}   HIDItem;

typedef struct _HID_ITEM_INFO
{
   union
    {
       struct 
       {
           BYTE             ItemSize :2;
           BYTE             ItemType :2; 
           BYTE             ItemTag  :4; 
       };
       BYTE                 val;
    }                       ItemDetails;

    union
    {
        LONG                    sItemData;
        DWORD                   uItemData;
        BYTE                    bItemData[4];
    }   Data;
}   HID_ITEM_INFO; 


typedef struct _HID_GLOBALS
{
    WORD                     usagePage;
    LONG                     logicalMinimum;   /* 32 bit signed val */
    LONG                     logicalMaximum;   /* */
    LONG                     physicalMinimum;  /* */
    LONG                     physicalMaximum;  /* */
    LONG                     unitExponent;     /* */
    LONG                     unit;
    BYTE                     reportsize;
    WORD                     reportID;
    BYTE                     reportCount;

    WORD                     reportIndex;
}   HID_GLOBALS;


typedef struct _HID_REPORT
{
    WORD                     reportID;
    WORD                     inputBits;
    WORD                     outputBits;
    WORD                     featureBits;
}   HID_REPORT;

typedef struct _HID_COLLECTION
{
    DWORD                     data;
    WORD                     usagePage;
    BYTE                     firstUsageItem;
    BYTE                     usageItems;
    BYTE                     firstReportItem;
    BYTE                     reportItems;
    BYTE                     parent;
    BYTE                     firstChild;
    BYTE                     nextSibling;
}   HID_COLLECTION;

typedef struct _HID_REPORTITEM
{
    HIDReportTypeEnum        reportType;
    HID_GLOBALS              globals;
    BYTE                     startBit;
    BYTE                     parent;
    DWORD                    dataModes;
    BYTE                     firstUsageItem;
    BYTE                     usageItems;
    BYTE                     firstStringItem;
    BYTE                     stringItems;
    BYTE                     firstDesignatorItem;
    BYTE                     designatorItems;
}   HID_REPORTITEM;

typedef struct _HID_USAGEITEM
{
    BOOL                     isRange;
    WORD                     usagePage;
    WORD                     usage;
    WORD                     usageMinimum;
    WORD                     usageMaximum;
}   HID_USAGEITEM;

typedef struct _HID_STRINGITEM
{
    BOOL                     isRange;
    WORD                     index;
    WORD                     minimum;
    WORD                     maximum;
}   HID_STRINGITEM, HID_DESIGITEM;


typedef struct _USB_HID_DEVICE_RPT_INFO
{
    BOOL haveDesignatorMax;
    BOOL haveDesignatorMin;
    BOOL haveStringMax;
    BOOL haveStringMin;
    BOOL haveUsageMax;
    BOOL haveUsageMin;
    
    BYTE collectionNesting;
    BYTE collections;
    BYTE designatorItems;
    WORD designatorMaximum;
    WORD designatorMinimum;
    WORD designatorRanges;
    WORD designators;
    BYTE firstUsageItem;
    BYTE firstDesignatorItem;
    BYTE firstStringItem;
    BYTE globalsNesting;
    BYTE maxCollectionNesting;
    BYTE maxGlobalsNesting;
    BYTE parent;
    WORD rangeUsagePage;
    BYTE reportItems;
    BYTE reports;
    BYTE sibling;
    BYTE stringItems;
    WORD stringMaximum;
    WORD stringMinimum;
    WORD stringRanges;
    BYTE strings;
    BYTE usageItems;
    WORD usageMaximum;
    WORD usageMinimum;
    BYTE usages;
    WORD usageRanges;

    HID_GLOBALS globals;

}   USB_HID_DEVICE_RPT_INFO;

typedef struct _USB_HID_ITEM_LIST
{
    HID_COLLECTION *collectionList;
    HID_DESIGITEM *designatorItemList;
    HID_GLOBALS *globalsStack;
    HID_REPORTITEM *reportItemList;
    HID_REPORT *reportList;
    HID_STRINGITEM *stringItemList;
    HID_USAGEITEM *usageItemList;
    BYTE *collectionStack;
}   USB_HID_ITEM_LIST;

//------------------------------------------------------------------------------
//
// Error Codes
//
//------------------------------------------------------------------------------
typedef enum {
    HID_ERR = 0,
    HID_ERR_NotEnoughMemory,
    HID_ERR_NullPointer,
    HID_ERR_UnexpectedEndCollection,
    HID_ERR_UnexpectedPop,
    HID_ERR_MissingEndCollection,
    HID_ERR_MissingTopLevelCollection,
    HID_ERR_NoReports,
    HID_ERR_UnmatchedUsageRange,
    HID_ERR_UnmatchedStringRange,
    HID_ERR_UnmatchedDesignatorRange,
    HID_ERR_UnexpectedEndOfDescriptor,
    HID_ERR_BadLogicalMin,
    HID_ERR_BadLogicalMax,
    HID_ERR_BadLogical,
    HID_ERR_ZeroReportSize,
    HID_ERR_ZeroReportID,
    HID_ERR_ZeroReportCount,
    HID_ERR_BadUsageRangePage,
    HID_ERR_BadUsageRange
} USB_HID_RPT_DESC_ERROR;

BOOL USBHostHID_HasUsage(HID_REPORTITEM *reportItem,WORD usagePage, WORD usage,WORD *pindex,BYTE* count);

#endif /* usb_client_hid_parser.h */
