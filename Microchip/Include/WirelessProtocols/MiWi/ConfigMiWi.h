/********************************************************************
* FileName:		ConfigMiWi.h
* Dependencies: 
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice
*
* Copyright © 2007-2008 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and distribute 
* Software only when embedded on a Microchip microcontroller or digital 
* signal controller and used with a Microchip radio frequency transceiver, 
* which are integrated into your product or third party product (pursuant 
* to the terms in the accompanying license agreement).   
*
* You should refer to the license agreement accompanying this Software for 
* additional information regarding your rights and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
* PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
* LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
* CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
* DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
* ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
* TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
* NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
*  This file provides configuration settings for P2P protocol stack
*
* Change History:
*  Rev   Date         Author        Description
*  2.0   4/15/2009    yfy           MiMAC and MiApp revision
********************************************************************/

#ifndef __CONFIGURE_MIWI_H

    #define __CONFIGURE_MIWI_H
    
    #include "ConfigApp.h"
    #include "Common\SymbolTime.h"
    //#include "Transceivers\Transceivers.h"

    /*********************************************************************/
    // ENABLE_DUMP will enable the stack to be able to print out the 
    // content of the P2P connection entry. It is useful in the debugging
    // process
    /*********************************************************************/
    #define ENABLE_DUMP
    
    /*********************************************************************/
    // RFD_DATA_WAIT is the timeout defined for sleeping device to receive 
    // a message from the associate device after Data Request. After this
    // timeout, the RFD device can continue to operate and then go to 
    // sleep to conserve battery power.
    /*********************************************************************/
    #define RFD_DATA_WAIT                   0x00003FFF
    
    /*********************************************************************/
    // CONNECTION_RETRY_TIMES is the maximum time that the wireless node
    // can try to establish a connection. Once the retry times are exhausted
    // control will be return to application layer to decide what to do next
    /*********************************************************************/
    #define CONNECTION_RETRY_TIMES          10

    /*********************************************************************/
    // CONNECTION_INTERVAL defines the interval in second between two 
    // connection request. 
    /*********************************************************************/
    #define CONNECTION_INTERVAL             2

    #define OPEN_CLUSTER_SOCKET_TIMEOUT     (ONE_SECOND * 3)

    #define ENABLE_NETWORK_FREEZER

    #define ENABLE_BROADCAST_TO_SLEEP_DEVICE
    
    #if defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
        #define BROADCAST_RECORD_SIZE   8
    #endif
    
    #define MAX_ROUTING_FAILURE 3
    
    #define INDIRECT_MESSAGE_TIMEOUT_CYCLE  3
    
    #define ACTIVE_SCAN_RESULT_SIZE 4
        
    #if defined(ENABLE_INDIRECT_MESSAGE)
        /*********************************************************************/
        // INDIRECT_MESSAGE_SIZE defines the maximum number of packets that
        // the device can store for the sleeping device(s)
        /*********************************************************************/
        #define INDIRECT_MESSAGE_SIZE   2

        /*********************************************************************/
        // INDIRECT_MESSAGE_TIMEOUT defines the timeout interval in seconds
        // for the stored packets for sleeping devices
        /*********************************************************************/
        #define INDIRECT_MESSAGE_TIMEOUT (ONE_SECOND * RFD_WAKEUP_INTERVAL * INDIRECT_MESSAGE_TIMEOUT_CYCLE)

    #endif

    #ifdef ENABLE_FREQUENCY_AGILITY
        /*********************************************************************/
        // FA_BROADCAST_TIME defines the total number of times to broadcast
        // the channel hopping message to the rest of PAN, before the 
        // Frequency Agility initiator jump to the new channel
        /*********************************************************************/
        #define FA_BROADCAST_TIME           0x03

        /*********************************************************************/
        // RESYNC_TIMES defines the maximum number of times to try resynchronization
        // in all available channels before hand over the control to the application
        // layer
        /*********************************************************************/
        #define RESYNC_TIMES                0x03
    #endif

    
#endif

