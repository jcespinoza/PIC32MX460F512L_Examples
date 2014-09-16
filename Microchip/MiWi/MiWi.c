/********************************************************************
* GUID=E25BE8D4-3281-4d98-867A-9160A7FD7A31
* GUIInterfaceVersion=1.00
* LibraryVersion=1.00
*
* FileName:		MiWi.c
* Dependencies:    
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.11 or higher
*				Microchip C30 v2.03 or higher		
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for MiWi Software:
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
*
*
* Change History:
*  Rev   Date         Author	Description
*  0.1   11/09/2006   df        Initial revision
*  1.0   01/09/2007   yfy       Initial release
********************************************************************/

/************************ HEADERS **********************************/

#include "ConfigApp.h"

#if defined(PROTOCOL_MIWI)
    #include "Common\MSPI.h"
    #include "WirelessProtocols\MiWi\MiWi.h"
    #include "Compiler.h"
    #include "GenericTypeDefs.h"
    #include "Common\Console.h"
    #include "Common\SymbolTime.h"
    #include "Transceivers\MCHP_MAC.h"
    #include "Transceivers\Transceivers.h"
    #include "WirelessProtocols\MCHP_API.h"

    /************************ VARIABLES ********************************/

    // Scan Duration formula for P2P Connection:
    //  60 * (2 ^ n + 1) symbols, where one symbol equals 16us
    #define SCAN_DURATION_0 SYMBOLS_TO_TICKS(120)
    #define SCAN_DURATION_1 SYMBOLS_TO_TICKS(180)
    #define SCAN_DURATION_2 SYMBOLS_TO_TICKS(300)
    #define SCAN_DURATION_3 SYMBOLS_TO_TICKS(540)
    #define SCAN_DURATION_4 SYMBOLS_TO_TICKS(1020)
    #define SCAN_DURATION_5 SYMBOLS_TO_TICKS(1980)
    #define SCAN_DURATION_6 SYMBOLS_TO_TICKS(3900)
    #define SCAN_DURATION_7 SYMBOLS_TO_TICKS(7740)
    #define SCAN_DURATION_8 SYMBOLS_TO_TICKS(15420)
    #define SCAN_DURATION_9 SYMBOLS_TO_TICKS(30780)
    #define SCAN_DURATION_10 SYMBOLS_TO_TICKS(61500)
    #define SCAN_DURATION_11 SYMBOLS_TO_TICKS(122940)
    #define SCAN_DURATION_12 SYMBOLS_TO_TICKS(245820)
    #define SCAN_DURATION_13 SYMBOLS_TO_TICKS(491580)
    #define SCAN_DURATION_14 SYMBOLS_TO_TICKS(983100)
    const ROM DWORD ScanTime[15] = {SCAN_DURATION_0,SCAN_DURATION_1,SCAN_DURATION_2,SCAN_DURATION_3,
        SCAN_DURATION_4,SCAN_DURATION_5,SCAN_DURATION_6,SCAN_DURATION_7,SCAN_DURATION_8,SCAN_DURATION_9,
        SCAN_DURATION_10, SCAN_DURATION_11, SCAN_DURATION_12, SCAN_DURATION_13,SCAN_DURATION_14
    };
    
    // permanent address definition
    #if MY_ADDRESS_LENGTH == 8
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6,EUI_7};
    #elif MY_ADDRESS_LENGTH == 7
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6};
    #elif MY_ADDRESS_LENGTH == 6
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5};
    #elif MY_ADDRESS_LENGTH == 5
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4};        
    #elif MY_ADDRESS_LENGTH == 4
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3};  
    #elif MY_ADDRESS_LENGTH == 3
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2};    
    #elif MY_ADDRESS_LENGTH == 2
        BYTE myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1};    
    #endif

    BYTE            currentChannel = 0;             // current operating channel for the device
    BYTE            ConnMode = 0;
    BYTE            MiWiCapacityInfo;
    TICK            currentTick;
    TICK            diffTick;
    RECEIVED_MESSAGE  rxMessage;                    // structure to store information for the received packet
    extern BYTE     AdditionalNodeID[];             // the additional information regarding the device
                                                    // that would like to share with the peer on the 
                                                    // other side of P2P connection. This information 
                                                    // is applicaiton specific. 

    BYTE    ActiveScanResultIndex;
    ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];  // The results for active scan, including
                                                                    // the PAN identifier, signal strength and 
                                                                    // operating channel

    #ifdef ENABLE_SLEEP
        TICK DataRequestTimer;
    #endif

    BYTE tempLongAddress[MY_ADDRESS_LENGTH];
    WORD_VAL tempShortAddress;
    WORD_VAL tempPANID;
    CONNECTION_STATUS tempNodeStatus;
    
    BYTE            TxData;    
    #if defined(__18CXX)
    #pragma udata TRX_BUFFER=0x100
    #endif
        BYTE TxBuffer[TX_BUFFER_SIZE+MIWI_HEADER_LEN];
        #if defined(ENABLE_SECURITY)
            DWORD_VAL IncomingFrameCounter[CONNECTION_SIZE];
        #endif
    #if defined(__18CXX)
    #pragma udata
    #endif
    WORD_VAL myShortAddress;
    WORD_VAL myPANID;
    BYTE myParent;
    MIWI_STATE_MACHINE MiWiStateMachine;
    BYTE AcknowledgementSeq = 0xFF;
    BYTE MiWiSeqNum;
    
    #ifdef NWK_ROLE_COORDINATOR
        #if defined(ENABLE_INDIRECT_MESSAGE)
            #if defined(__18CXX)
                #pragma udata INDIRECT_BUFFER=0x200
            #endif
            INDIRECT_MESSAGE indirectMessages[INDIRECT_MESSAGE_SIZE];
            #if defined(__18CXX)
                #pragma udata
            #endif
        #endif
        BYTE RoutingTable[8];
        BYTE RouterFailures[8];
        BYTE knownCoordinators;
        BYTE role;
    #endif
    
    OPEN_SOCKET openSocketInfo;

    MAC_TRANS_PARAM MTP;
    MAC_RECEIVED_PACKET MACRxPacket;
 
    #if defined(__18CXX)
        #pragma udata BIGvariables1 = 0x300
    #endif
    CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE]; 
    #if defined(__18CXX)
        #pragma udata
    #endif

    #if defined(ENABLE_SLEEP) && defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
        struct _BROADCAST_RECORD
        {
            WORD_VAL    AltSourceAddr;
            BYTE        MiWiSeq;
            BYTE        RxCounter;
        } BroadcastRecords[BROADCAST_RECORD_SIZE];
    #endif

    
    /************************ FUNCTIONS ********************************/
    
    #if defined(IEEE_802_15_4)
        BOOL SendMACPacket(BYTE *PANID, BYTE *Address, BYTE PacketType, BYTE ModeMask);
    #else
        BOOL SendMACPacket(BYTE *Address, BYTE PacketType);
    #endif
    
    BOOL RouteMessage(WORD_VAL PANID, WORD_VAL ShortAddress, BOOL SecEn);
    void StartChannelHopping(INPUT BYTE OptimalChannel);
    
    #if defined(IEEE_802_15_4)
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT WORD_VAL DestinationPANID, 
                                 INPUT BYTE *DestinationAddress, 
                                 INPUT BOOL isAltAddr, 
                                 INPUT BOOL SecurityEnabled);
    #else
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT BYTE *DestinationAddress, 
                                 INPUT BOOL isAltAddr, 
                                 INPUT BOOL SecurityEnabled);
    #endif  
    
    /*********************************************************************
     * Function:        void MiWiTasks( void )
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The stack receives, handles, buffers, and transmits 
     *                  packets.  It also handles all of the joining 
     *
     * Overview:        This function maintains the operation of the stack
     *                  It should be called as often as possible.  
     ********************************************************************/
    void MiWiTasks(void)
    {
        BYTE i;
        
        if( MiMAC_ReceivedPacket() )
        {
            if( MiWiStateMachine.bits.RxHasUserData )
            {
                return;
            }
            
            rxMessage.flags.Val = 0;
            rxMessage.flags.bits.broadcast = MACRxPacket.flags.bits.broadcast;
            rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
            rxMessage.flags.bits.command = (MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND) ? 1:0;
            rxMessage.flags.bits.srcPrsnt = MACRxPacket.flags.bits.sourcePrsnt;
            if( MACRxPacket.flags.bits.sourcePrsnt )
            {
                #if defined(IEEE_802_15_4)
                    rxMessage.flags.bits.altSrcAddr = MACRxPacket.altSourceAddress;
                #else
                    rxMessage.flags.bits.altSrcAddr = 1;
                #endif
                rxMessage.SourceAddress = MACRxPacket.SourceAddress;
            }
            #if defined(IEEE_802_15_4) 
                rxMessage.SourcePANID.Val = MACRxPacket.SourcePANID.Val;
            #endif
            rxMessage.PacketLQI = MACRxPacket.LQIValue;
            rxMessage.PacketRSSI = MACRxPacket.RSSIValue;
            
            MiWiStateMachine.bits.RxHasUserData = 0;

            //determine what type of packet it is.
            switch(MACRxPacket.flags.bits.packetType)
            {
                //if it is a data packet
                case PACKET_TYPE_DATA:
                    //The guts of MiWi ...
                    {
                        WORD_VAL destPANID;
                        WORD_VAL sourcePANID;
                        WORD_VAL sourceShortAddress;
                        WORD_VAL destShortAddress;

                        #if defined(ENABLE_SLEEP)
                            #if defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
                                for(i = 0; i < BROADCAST_RECORD_SIZE; i++)
                                {
                                    if( BroadcastRecords[i].RxCounter > 0)
                                    {
                                        BroadcastRecords[i].RxCounter--;
                                    }
                                }
                            #endif
                            
                            if( MACRxPacket.PayloadLen == 0 )
                            {
                                MiWiStateMachine.bits.DataRequesting = 0;
                                break;
                            }
                        #endif
                        
                        destPANID.v[0] = MACRxPacket.Payload[2];
                        destPANID.v[1] = MACRxPacket.Payload[3];
                        destShortAddress.v[0] = MACRxPacket.Payload[4];
                        destShortAddress.v[1] = MACRxPacket.Payload[5];
                        
                        sourcePANID.v[0] = MACRxPacket.Payload[6];
                        sourcePANID.v[1] = MACRxPacket.Payload[7];
                        sourceShortAddress.v[0] = MACRxPacket.Payload[8];
                        sourceShortAddress.v[1] = MACRxPacket.Payload[9];                 
                        
                        rxMessage.flags.Val = 0;
                        rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
                        
                        if(rxMessage.flags.bits.broadcast || destShortAddress.Val == 0xFFFF)
                        {
                            if( sourceShortAddress.Val == myShortAddress.Val && 
                                sourcePANID.Val == myPANID.Val )
                            {
                                break;   
                            }
                            
                            rxMessage.flags.bits.broadcast = 1;
                            #if defined(ENABLE_SLEEP) && defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
                                for(i = 0; i < BROADCAST_RECORD_SIZE; i++)
                                {
                                    if( BroadcastRecords[i].RxCounter && 
                                        BroadcastRecords[i].AltSourceAddr.Val == sourceShortAddress.Val &&
                                        BroadcastRecords[i].MiWiSeq == MACRxPacket.Payload[10] )
                                    {
                                        break;
                                    }
                                }

                                if( i < BROADCAST_RECORD_SIZE )
                                {
                                    MiWiStateMachine.bits.DataRequesting = 0;
                                    break;
                                }
                                
                                for(i = 0; i < BROADCAST_RECORD_SIZE; i++)
                                {
                                    if( BroadcastRecords[i].RxCounter == 0 )
                                    {
                                        break;
                                    }
                                }
                                
                                if( i < BROADCAST_RECORD_SIZE )
                                {
                                    BroadcastRecords[i].AltSourceAddr.Val = sourceShortAddress.Val;
                                    BroadcastRecords[i].RxCounter = INDIRECT_MESSAGE_TIMEOUT_CYCLE + 1;
                                    BroadcastRecords[i].MiWiSeq = MACRxPacket.Payload[10];
                                }
                            #endif
                            
                            #ifdef NWK_ROLE_COORDINATOR
                                //This is a broadcast packet
                                if(MACRxPacket.Payload[0]>0)
                                {
                                    
                                    if((sourcePANID.Val == myPANID.Val) && (sourceShortAddress.Val == myShortAddress.Val))
                                    {
                                        //this is from me... do not rebroadcast it
                                    }
                                    else
                                    {
                                        MACRxPacket.Payload[0]--;
                                        MAC_FlushTx();
                                        for(i = 0; i < MACRxPacket.PayloadLen; i++)
                                        {
                                            MiApp_WriteData(MACRxPacket.Payload[i]);
                                        }
                                        
                                        MTP.flags.Val = MACRxPacket.flags.Val;
                                        #if defined(IEEE_802_15_4)
                                            MTP.altSrcAddr = MACRxPacket.altSourceAddress;
                                        #endif
                                        MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                        #if defined(ENABLE_INDIRECT_MESSAGE)
                                            for(i = 0; i < CONNECTION_SIZE; i++)
                                            {
                                                if( ConnectionTable[i].status.bits.isValid && (ConnectionTable[i].status.bits.RXOnWhenIdle == 0) )    
                                                {
                                                    break;
                                                }
                                            }
                                            if( i < CONNECTION_SIZE )
                                            {
                                                #if defined(IEEE_802_15_4)
                                                    SaveIndirectMessage(TRUE, destPANID, NULL, FALSE, MACRxPacket.flags.bits.secEn);   
                                                #else
                                                    SaveIndirectMessage(TRUE, NULL, FALSE, MACRxPacket.flags.bits.secEn);
                                                #endif
                                            }
                                        #endif
                                    }
                                }
                            #endif
                            //since this is a broadcast we need to parse the packet as well.
                            goto ThisPacketIsForMe;
                        }
    
    
                        if((destPANID.Val == myPANID.Val) && (destShortAddress.Val == myShortAddress.Val))
                        {                           
                            //need to check to see if the MiWi Ack is set
                            if(MACRxPacket.Payload[1] & MIWI_ACK_REQ)
                            { 
                                MiApp_FlushTx();
                                MiApp_WriteData(MIWI_STACK_REPORT_TYPE);
                                MiApp_WriteData(ACK_REPORT_TYPE);
                                i = MiWiSeqNum;
                                MiWiSeqNum = MACRxPacket.Payload[MIWI_HEADER_LEN-1];
                                MiApp_UnicastAddress(sourceShortAddress.v, FALSE, FALSE);
                                MiWiSeqNum = i;
                                rxMessage.flags.bits.ackReq = 1;
                            }
                            
ThisPacketIsForMe:
    						//if the packet is a MiWi stack packet (but not an ACK report type
                            if(MACRxPacket.Payload[MIWI_HEADER_LEN] == MIWI_STACK_REPORT_TYPE)
                            {
                                if( MACRxPacket.Payload[MIWI_HEADER_LEN+1] != ACK_REPORT_TYPE)
                                {
                                    //determine which stack report it was
                                    switch(MACRxPacket.Payload[MIWI_HEADER_LEN+1])
                                    {                                
                                        #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                        #if defined(NWK_ROLE_COORDINATOR)       
                                        case EUI_ADDRESS_SEARCH_REQUEST:
                                        {
                                            BYTE handle,j;
                                            BOOL ItsMe;
        
                                            if(sourceShortAddress.Val == myShortAddress.Val)
                                            {
                                                if(sourcePANID.Val == myPANID.Val)
                                                {
                                                    //I started this message.  I don't need to re-broadcast it.
                                                    break;
                                                }
                                            }
                                            
                                            //copy the address into tempLongAddress
                                            ConsolePutROMString((ROM char*)"\r\nEUI address search request\r\n");
                                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                                            {
                                                tempLongAddress[j] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+j];
                                            }
        
                                            
                                            ItsMe = TRUE;
                                            for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                            {
                                                //see if the address in question is my own
                                                if(tempLongAddress[j] != myLongAddress[j])
                                                {
                                                    ItsMe = FALSE;
                                                    break;
                                                }
                                            }
                                            
                                            //load up the TX Buffer with the results
                                            MiApp_FlushTx();
                                            MiApp_WriteData(MIWI_STACK_REPORT_TYPE);      //Report Type
                                            MiApp_WriteData(EUI_ADDRESS_SEARCH_RESPONSE);      //Report ID
                                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                                            {
                                                MiApp_WriteData(tempLongAddress[j]);      //Data byte 0
                                            }
                                            
                                            //if it wasn't me
                                            if(ItsMe == FALSE)
                                            {
                                                //search the network table for the device
                                                handle = SearchForLongAddress();
                                                
                                                //if we found the device
                                                if(handle!=0xFF)
                                                {
                                                    //check to see if the short address is valid for that device
                                                    if(networkStatus[handle].bits.shortAddressValid)
                                                    {
                                                        //I know the device in question so I will send back a response
                                                        MiApp_WriteData(networkTable[handle].PANID.v[0]);
                                                        MiApp_WriteData(networkTable[handle].PANID.v[1]);
                                                        MiApp_WriteData(networkTable[handle].ShortAddress.v[0]);
                                                        MiApp_WriteData(networkTable[handle].ShortAddress.v[1]);
                                                         
                                                        //if everything is cool then send the response back
                                                        RouteMessage(sourcePANID, sourceShortAddress, FALSE);   
                                                        //SendReportByShortAddress(sourcePANID,sourceShortAddress,FALSE);
                                                    }
                                                    else
                                                    {
                                                        //if we didn't have the short address then we can't
                                                        //respond to this packet even though we know the long address
                                                        ConsolePutROMString((ROM char*)"found long address but short address not valid\r\n");
                                                        //clear out the data I already wrote
                                                        TxData = 0;     
                                                    }
            
                                                }
                                                else
                                                {
                                                    //if we couldn't find the long address of the device then forget it
                                                    ConsolePutROMString((ROM char*)"couldn't find the device");
                                                    TxData = 0;     
                                                }
                                            }
                                            else
                                            {
                                                //The node in question is me
                                                TxBuffer[TxData++] = myPANID.v[0];
                                                TxBuffer[TxData++] = myPANID.v[1];
                                                TxBuffer[TxData++] = myShortAddress.v[0];
                                                TxBuffer[TxData++] = myShortAddress.v[1];
                                                   
                                                //send a response back with my own information
                                                RouteMessage(sourcePANID, sourceShortAddress, FALSE);
                                                //SendReportByShortAddress(sourcePANID,sourceShortAddress,FALSE);
                                            }
                                            
                                            break;
                                        }
                                        #endif //COORDINATOR_CAPABLE
                                        #endif //SUPPORT_EUI_ADDRESS_SEARCH
                                        #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                        case EUI_ADDRESS_SEARCH_RESPONSE:
                                            //got a response back from the IEEE address search
                                            //record all of the information in the packet
                                            //long address
                                            
                                            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                            {
                                                tempLongAddress[i] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+i];
                                            }
                                            //PANID and short address
                                            tempPANID.v[0] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+MY_ADDRESS_LENGTH];
                                            tempPANID.v[1] = MACRxPacket.Payload[MIWI_HEADER_LEN+3+MY_ADDRESS_LENGTH];
                                            tempShortAddress.v[0] = MACRxPacket.Payload[MIWI_HEADER_LEN+4+MY_ADDRESS_LENGTH];
                                            tempShortAddress.v[1] = MACRxPacket.Payload[MIWI_HEADER_LEN+5+MY_ADDRESS_LENGTH];
                                            
                                            //create a new Status entry for the network table
                                            //tempNodeStatus.Val = 0x2D;
                                            tempNodeStatus.Val = 0x8D;
                                            if( tempShortAddress.v[0] & 0x80 )
                                            {
                                                tempNodeStatus.bits.RXOnWhenIdle = 0;
                                            }
                                            if((tempShortAddress.Val == sourceShortAddress.Val) 
                                                #if defined(IEEE_802_15_4)
                                                    && (tempPANID.Val == sourcePANID.Val)
                                                #endif
                                               )
                                            {
                                                tempNodeStatus.bits.directConnection = 1;
                                            }
                                            //add the node to the network table
                                            i = AddNodeToNetworkTable();
                                            break;
                                        #endif
        
                                        #ifdef NWK_ROLE_COORDINATOR
                                        case OPEN_CLUSTER_SOCKET_REQUEST:
                                            {
                                                BYTE j;
                                                
                                                //if we have a socket request open
                                                if(openSocketInfo.status.bits.requestIsOpen == 0)
                                                {
                                                    //if there isn't a request already open
                                                    //get the current time and the source address
                                                    openSocketInfo.socketStart = TickGet();
                                                    openSocketInfo.ShortAddress1.v[0] = MACRxPacket.Payload[MIWI_HEADER_LEN-3];
                                                    openSocketInfo.ShortAddress1.v[1] = MACRxPacket.Payload[MIWI_HEADER_LEN-2];
        
                                                    //copy the long address
                                                    for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                    {
                                                        openSocketInfo.LongAddress1[j] = MACRxPacket.Payload[j+2+MIWI_HEADER_LEN];
                                                    }
                                                    //mark a request as open
                                                    openSocketInfo.status.bits.requestIsOpen = 1;
                                                    openSocketInfo.status.bits.matchFound = 0;
                                                    
                                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                        {
                                                            openSocketInfo.AdditionalNodeID1[j] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+MY_ADDRESS_LENGTH+j];
                                                        }
                                                    #endif
                                                }
                                                else
                                                {
                                                    //if there was already a request open

                                                    if( (MACRxPacket.Payload[MIWI_HEADER_LEN-3] == openSocketInfo.ShortAddress1.v[0]) && 
                                                        (MACRxPacket.Payload[MIWI_HEADER_LEN-2] == openSocketInfo.ShortAddress1.v[1]))
                                                    {
                                                        //if we get a request from the same guy twice,
                                                        //just throw away the second request
                                                        //ConsolePutROMString((ROM char*)"got request from same guy twice\r\n");
                                                    }
                                                    else if(openSocketInfo.ShortAddress1.Val == 0x0000)
                                                    {                                                
                                                        //I am the PAN coordinator and I initiated the request so don't send back a response to myself
                                                        //copy the long and short address from the Rx Buffer
                                                        for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                        {
                                                            openSocketInfo.LongAddress2[j] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+j];      //Data byte
                                                            tempLongAddress[j] = openSocketInfo.LongAddress2[j];
                                                        }
                                                        
                                                        //save the important information about the node sending 
                                                        //the request
                                                        openSocketInfo.ShortAddress2.v[0] = MACRxPacket.Payload[MIWI_HEADER_LEN-3];
                                                        openSocketInfo.ShortAddress2.v[1] = MACRxPacket.Payload[MIWI_HEADER_LEN-2];
                                                        tempShortAddress.v[0] = openSocketInfo.ShortAddress2.v[0];
                                                        tempShortAddress.v[1] = openSocketInfo.ShortAddress2.v[1];
                                                        tempPANID.Val = myPANID.Val;
                                                        
                                                        //look to see if that device exists in our network table
                                                        openSocketInfo.socketHandle = SearchForShortAddress();
                                                        
                                                        if(openSocketInfo.socketHandle != 0xFF)
                                                        {
                                                            //if they do exists then copy their status information
                                                            tempNodeStatus.Val = ConnectionTable[openSocketInfo.socketHandle].status.Val;
                                                        }
                                                        else
                                                        {
                                                            //otherwise create it
                                                            //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                            tempNodeStatus.Val = 0x8D;
                                                            if(tempShortAddress.v[0] & 0x80)
                                                            {
                                                                tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                            }
                                                        }
                                                                                          
                                                        openSocketInfo.status.bits.requestIsOpen = 1;
                                                        openSocketInfo.status.bits.matchFound = 1; 
                                                    }
                                                    else
                                                    {                                            
                                                        ConsolePutROMString((ROM char*)"sending out response\r\n");
                                                        
                                                        //we got a match so let's send out the response
                                                        
                                                        MiApp_FlushTx();
                                                        MiApp_WriteData(MIWI_STACK_REPORT_TYPE);          //Report Type
                                                        MiApp_WriteData(OPEN_CLUSTER_SOCKET_RESPONSE);    //Report ID
        
                                                        //copy the long and short address from the Rx Buffer
                                                        for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                        {
                                                            openSocketInfo.LongAddress2[j] = MACRxPacket.Payload[MIWI_HEADER_LEN+2+j];      //Data byte
                                                            MiApp_WriteData(openSocketInfo.LongAddress2[j]); 
                                                        }
        
                                                        openSocketInfo.ShortAddress2.v[0] = MACRxPacket.Payload[MIWI_HEADER_LEN-3];
                                                        openSocketInfo.ShortAddress2.v[1] = MACRxPacket.Payload[MIWI_HEADER_LEN-4];
                                                        MiApp_WriteData(openSocketInfo.ShortAddress2.v[0]);
                                                        MiApp_WriteData(openSocketInfo.ShortAddress2.v[1]);
                                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                                            for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                            {
                                                                MiApp_WriteData(openSocketInfo.AdditionalNodeID2[j]);
                                                            }
                                                        #endif
                                                        
                                                        
                                                        MiApp_UnicastAddress(openSocketInfo.ShortAddress1.v, FALSE, FALSE);
                                                        openSocketInfo.status.bits.requestIsOpen = 1;
                                                        openSocketInfo.status.bits.matchFound = 1;    
                                                    }

                                                }
                                            }
                                            break;
                                        #endif
                                        
                                        case OPEN_CLUSTER_SOCKET_RESPONSE:
                                            //ConsolePutROMString((ROM char*)"found a response to my request\r\n");
                                            {
                                                BYTE j;
                                                
                                                //save the long address of the device
                                                for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                {
                                                    tempLongAddress[j] = MACRxPacket.Payload[j+13];
                                                }
                                                
                                                //create a new status for the device
                                                
                                                //tempNodeStatus.Val = 0x2D;
                                                tempNodeStatus.Val = 0x8D;
                                                //check the RxOnWhenIdle bit
                                                if(MACRxPacket.Payload[13+MY_ADDRESS_LENGTH] & 0x80)
                                                {
                                                    tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                }
                                                
                                                //copy the short address
                                                tempShortAddress.v[0] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+13];
                                                tempShortAddress.v[1] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+14];
                                                
                                                //use my PANID since this version of MiWi only supports single PANIDs
                                                tempPANID.Val = myPANID.Val;
                                                
                                                //see if this node already exists in my table
                                                openSocketInfo.socketHandle = SearchForShortAddress();
                                                
                                                if(openSocketInfo.socketHandle != 0xFF)
                                                {
                                                    //if it does then get the status of the node already in the table
                                                    tempNodeStatus.Val = ConnectionTable[openSocketInfo.socketHandle].status.Val;
                                                    tempNodeStatus.bits.longAddressValid = 1;
                                                    tempNodeStatus.bits.shortAddressValid = 1;
                                                }
                                                else
                                                {
                                                    //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                    //tempNodeStatus.Val = 0x2D;
                                                    tempNodeStatus.Val = 0x8D;
                                                    if(tempShortAddress.v[0] & 0x80)
                                                    {
                                                        tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                    }
                                                }
                                                //update the network information
                                                openSocketInfo.socketHandle = AddNodeToNetworkTable();
                                                #if ADDITIONAL_NODE_ID_SIZE > 0
                                                    for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                    {
                                                        ConnectionTable[openSocketInfo.socketHandle].PeerInfo[j] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+15+j];
                                                    }
                                                #endif
                                                
                                                //clear out out request
                                                openSocketInfo.status.bits.requestIsOpen = 0;
                                                openSocketInfo.status.bits.matchFound = 1;
                                            }
                                            break;


                                        #if defined(ENABLE_FREQUENCY_AGILITY)
                                        
                                            #if defined(NWK_ROLE_COORDINATOR)
                                                case CHANNEL_HOPPING_REQUEST:
                                                    {
                                                        if( MACRxPacket.Payload[13] != currentChannel )
                                                        {
                                                            break;
                                                        }
                                                        StartChannelHopping(MACRxPacket.Payload[14]);
                                                        printf("\r\nHopping Channel to ");
                                                        PrintDec(currentChannel);
                                                    }
                                                    break;
                                                    
                                                case RESYNCHRONIZATION_REQUEST:
                                                    {
                                                        if( MACRxPacket.Payload[13] != currentChannel )
                                                        {
                                                            break;
                                                        }
                                                        MAC_FlushTx();
                                                        TxBuffer[0] = MAX_HOPS;
                                                        TxBuffer[1] = 0x02;
                                                        TxBuffer[2] = myPANID.v[0];
                                                        TxBuffer[3] = myPANID.v[1];
                                                        TxBuffer[4] = MACRxPacket.Payload[8];
                                                        TxBuffer[5] = MACRxPacket.Payload[9];
                                                        TxBuffer[6] = myPANID.v[0];
                                                        TxBuffer[7] = myPANID.v[1];
                                                        TxBuffer[8] = myShortAddress.v[0];
                                                        TxBuffer[9] = myShortAddress.v[1];
                                                        TxBuffer[10] = MiWiSeqNum++;
                                                        TxBuffer[11] = MIWI_STACK_REPORT_TYPE;
                                                        TxBuffer[12] = RESYNCHRONIZATION_RESPONSE;
                                                        
                                                        MTP.flags.Val = 0;
                                                        MTP.flags.bits.ackReq = 1;
                                                        #if defined(IEEE_802_15_4)
                                                            MTP.altDestAddr = TRUE;
                                                            MTP.altSrcAddr = TRUE;
                                                            MTP.DestPANID.Val = myPANID.Val;
                                                            MTP.DestAddress = &(MACRxPacket.Payload[8]);
                                                        #else
                                                            tempShortAddress.v[0] = MACRxPacket.Payload[8];
                                                            tempShortAddress.v[1] = MACRxPacket.Payload[9];
                                                            i = SearchForShortAddress();
                                                            if( i == 0xFF )
                                                            {
                                                                break;
                                                            }
                                                            MTP.DestAddress = ConnectionTable[i].Address;
                                                        #endif
                                                        
                                                        MiMAC_SendPacket(MTP, TxBuffer, 13);
                                                    }
                                                    break;
                                            #endif
                                        
                                            case RESYNCHRONIZATION_RESPONSE:
                                                {
                                                    MiWiStateMachine.bits.Resynning = 0;
                                                }
                                                break;
                                        
                                        
                                        #endif

                                        default:
                                            //unknown stack request.  don't do anything
                                            //just let this packet die
                                            break;
                                    }
                                }
                                else
                                {
                                    // in case receive acknowledgement 
                                    AcknowledgementSeq = MACRxPacket.Payload[10];
                                    break;
                                }
                            }
                            else
                            {
                                //This data is for the user, pass it up to them
                                #if defined(ENABLE_SLEEP)
                                    MiWiStateMachine.bits.DataRequesting = 0;
                                #endif
                                rxMessage.PayloadSize = MACRxPacket.PayloadLen - 11;
                                rxMessage.Payload = &MACRxPacket.Payload[11];
                                rxMessage.SourcePANID.Val = sourcePANID.Val;
                                if( MACRxPacket.Payload[8] == 0xFF && MACRxPacket.Payload[9] == 0xFF )
                                {
                                    rxMessage.SourceAddress = MACRxPacket.SourceAddress;    
                                }
                                else
                                {
                                    rxMessage.flags.bits.altSrcAddr = 1;
                                    rxMessage.SourceAddress = &(MACRxPacket.Payload[8]);
                                }
                                rxMessage.flags.bits.srcPrsnt = 1;

                                if( rxMessage.PayloadSize > 0 )
                                {
                                    MiWiStateMachine.bits.RxHasUserData = 1;
                                }
                                
                            }
                        }
                        else
                        {
                            //if this packet wasn't for me
                            //ConsolePutROMString((ROM char*)"This packet isn't for me\r\n"); 
                            #ifdef NWK_ROLE_COORDINATOR
                                //then if I am a coordinator pass it along to the
                                //next hop, decrementing the number of hops available
                                MACRxPacket.Payload[0]--;      //decrement the hops counter
                                MAC_FlushTx();
                                for(i = 0; i < MACRxPacket.PayloadLen; i++)
                                {
                                    MiApp_WriteData(MACRxPacket.Payload[i]);
                                }
                                RouteMessage(destPANID, destShortAddress, MACRxPacket.flags.bits.secEn);
                                LED_8 ^= 1;
                                Printf("\r\nRoute Message to ");
                                PrintChar(destShortAddress.v[1]);
                                PrintChar(destShortAddress.v[0]);
                            #endif    
                        }
    
                    }
                    break;
    
                //if it is a beacon packet
                case PACKET_TYPE_BEACON:
                    {
                        BYTE rxIndex;
                        
                        #if defined(IEEE_802_15_4)
                            rxIndex = 0;
                        #else
                            rxIndex = 4;
                        #endif

                        if(MACRxPacket.Payload[rxIndex+4] != MIWI_PROTOCOL_ID)
                        {
                            break;
                        }
                        
                        #ifdef NWK_ROLE_COORDINATOR
                        {
                            //if I am a coordinator then get 
                            //the coordinators number
                            #if defined(IEEE_802_15_4)
                                BYTE coordinatorNumber = MACRxPacket.SourceAddress[1];
                            #else
                                BYTE coordinatorNumber = MACRxPacket.Payload[3];
                            #endif
                            BYTE mask = 1<<(coordinatorNumber);

                            //Make sure its a MiWi coordinator
                            #if defined(IEEE_802_15_4)
                                if( MACRxPacket.SourceAddress[0]== 0x00 )
                            #else
                                if(MACRxPacket.Payload[2] == 0x00)
                            #endif
                            {
                                //if it is then mark this device as known
                                knownCoordinators |= mask;
                            }
                            
                            RoutingTable[coordinatorNumber] = MACRxPacket.Payload[rxIndex+6];
                            MiWiStateMachine.bits.saveConnection = 1;
                        }
                        #endif
                        
                        #if !defined(IEEE_802_15_4) 
                            {
                                BYTE entry;
                                
                                #if defined(IEEE_802_15_4)
                                    tempPANID.Val = MACRxPacket.SourcePANID.Val;
                                    tempShortAddress.v[0] = MACRxPacket.SourceAddress[0];
                                    tempShortAddress.v[1] = MACRxPacket.SourceAddress[1];
                                #else
                                    tempPANID.v[0] = MACRxPacket.Payload[0];
                                    tempPANID.v[1] = MACRxPacket.Payload[1];
                                    tempShortAddress.v[0] = MACRxPacket.Payload[2];
                                    tempShortAddress.v[1] = MACRxPacket.Payload[3]; 
                                #endif
                                
                                entry = SearchForShortAddress();
                                if(entry == 0xFF)
                                {
                                    //this device doesn't already exist in our network table
                                    //let's create a new entry for it.           
                                    entry = findNextNetworkEntry();                      
                                }
       
                                ConnectionTable[entry].PANID.Val = tempPANID.Val;
                                ConnectionTable[entry].AltAddress.Val = tempShortAddress.Val;
                                  
                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                {
                                    ConnectionTable[entry].Address[i] = MACRxPacket.SourceAddress[i];
                                }
                                
                                //mark this as a valid entry 
                                ConnectionTable[entry].status.Val = 0;
                                ConnectionTable[entry].status.bits.directConnection = 1;
                                ConnectionTable[entry].status.bits.RXOnWhenIdle = 1;
                                ConnectionTable[entry].status.bits.longAddressValid = 1;
                                ConnectionTable[entry].status.bits.shortAddressValid = 1;
                                ConnectionTable[entry].status.bits.isValid = 1;
                            }
                        #endif   
                                      
                        //if we are looking for a network
                        if(MiWiStateMachine.bits.searchingForNetwork)
                        {
                            //BYTE entry;
                            
                            #if defined(IEEE_802_15_4)
                                tempPANID.Val = MACRxPacket.SourcePANID.Val;
                                tempShortAddress.v[0] = MACRxPacket.SourceAddress[0];
                                tempShortAddress.v[1] = MACRxPacket.SourceAddress[1];
                            #else
                                tempPANID.v[0] = MACRxPacket.Payload[0];
                                tempPANID.v[1] = MACRxPacket.Payload[1];
                                tempShortAddress.v[0] = MACRxPacket.Payload[2];
                                tempShortAddress.v[1] = MACRxPacket.Payload[3]; 
                            #endif                   
                   
                            //ignore all beacon networks
                            if(MACRxPacket.Payload[rxIndex] == 0xFF)
                            {
                                for(i = 0; i < ActiveScanResultIndex; i++)
                                {
                                    if( ActiveScanResults[i].Channel == currentChannel
                                    #if defined(IEEE_802_15_4)
                                        && ActiveScanResults[i].PANID.Val == MACRxPacket.SourcePANID.Val 
                                    #endif
                                    )
                                    {
                                        if( ActiveScanResults[i].RSSIValue < rxMessage.PacketRSSI )
                                        {
                                            BYTE j;
                                            
                                            ActiveScanResults[i].RSSIValue = rxMessage.PacketRSSI;
                                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                                            {
                                                ActiveScanResults[i].Address[j] = rxMessage.SourceAddress[j];
                                            }
                                        }
                                        break;
                                    }
                                }
                                if( i == ActiveScanResultIndex && (i < ACTIVE_SCAN_RESULT_SIZE))
                                {
                                    BYTE CapacityByte;
                                    ActiveScanResults[ActiveScanResultIndex].Channel = currentChannel;
                                    ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxMessage.PacketRSSI;
                                    ActiveScanResults[ActiveScanResultIndex].LQIValue = rxMessage.PacketLQI;
                                    ActiveScanResults[ActiveScanResultIndex].PANID.Val = tempPANID.Val;

                                    CapacityByte = MACRxPacket.Payload[rxIndex+1];
                                    ActiveScanResults[ActiveScanResultIndex].Capability.Val = 0;

                                    ActiveScanResults[ActiveScanResultIndex].Capability.bits.Direct = 1;
                                    if( CapacityByte & 0x08 )
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Capability.bits.SecurityEn = 1;
                                    }
                                    if( (CapacityByte & 0x30) == 0 )
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Capability.bits.AllowJoin = 1;
                                    }

                                    #if defined(IEEE_802_15_4)
                                        ActiveScanResults[ActiveScanResultIndex].Address[0] = rxMessage.SourceAddress[0];
                                        ActiveScanResults[ActiveScanResultIndex].Address[1] = rxMessage.SourceAddress[1];
                                    #else
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].Address[i] = rxMessage.SourceAddress[i];
                                        }
                                    #endif

                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].PeerInfo[i] = MACRxPacket.Payload[rxIndex+7+i];
                                        }
                                    #endif
                                    ActiveScanResultIndex++;
                                                        

                                }
                            }
                        }
                    }


                    break;
  
                
                //if it is a command packet
                case PACKET_TYPE_COMMAND:
                    //figure out which command packet it is
                    switch(MACRxPacket.Payload[0])
                    {
                        #ifdef NWK_ROLE_COORDINATOR
                        case MAC_COMMAND_ASSOCIATION_REQUEST:
                            {
                                BYTE handle;
                                
                                if( ConnMode > ENABLE_PREV_CONN )
                                {
                                    break;
                                }

                                //get the long address of the device
                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                {
                                    tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                }
    
                                handle = SearchForLongAddress();
                                if( handle == 0xFF )
                                {
                                    if( ConnMode > ENABLE_ALL_CONN )
                                    {
                                        break;
                                    }
                                    tempNodeStatus.Val = 0xB7;

                                    //add this node to the network table
                                    handle = AddNodeToNetworkTable();
                                }
                                else
                                {
                                    if( ConnectionTable[handle].status.bits.shortAddressValid )
                                    {
                                        tempShortAddress.Val = ConnectionTable[handle].AltAddress.Val;
                                        goto START_ASSOCIATION_RESPONSE;
                                    }
                                }
                                                           
                                //if entry was successful then assign the device a short address
                                if(handle != 0xFF)
                                {
                                    //set the short address as an end device address
                                    tempShortAddress.v[0] = handle + 1;
                                    tempShortAddress.v[1] = myShortAddress.v[1];
                                    tempPANID.Val = myPANID.Val;
                                    
                                    //if I am the PAN coordinator
                                    if(role == ROLE_PAN_COORDINATOR)
                                    {
                                        if(MACRxPacket.Payload[1] & 0x40)
                                        {
                                            //If this device is a potential coordinator
                                            BYTE j;
                                            WORD_VAL CoordAddress;
                                            BYTE entry;
                                                                                    
                                            CoordAddress.Val = tempShortAddress.Val;
                                            tempShortAddress.v[0] = 0;
                                            
                                            //search to see if there is a coordinator address available
                                            for(j=1;j<8;j++)
                                            {
                                                tempShortAddress.v[1] = j;                                        
                                                entry = SearchForShortAddress();
                                                
                                                if(entry == 0xFF)
                                                {
                                                    tempShortAddress.v[0] = 0x00;
                                                    tempShortAddress.v[1] = j;
                                                    knownCoordinators |= (1<<j);
                                                    break;
                                                }
                                            }
    
                                            if(j==8)
                                            {
                                                tempShortAddress.Val= CoordAddress.Val;
                                            }
                                            
                                        }
                                    }

                                    //create a new status entry for the device
                                    tempNodeStatus.Val = 0xBF;
                                    
                                    //mark if the Rx is on when idle
                                    if((MACRxPacket.Payload[1] & 0x01) == 0)
                                    {
                                        tempNodeStatus.bits.RXOnWhenIdle = 0;
                                        tempShortAddress.v[0] |= 0x80;                                   
                                    }

                                    //update the information in the network table
                                    handle = AddNodeToNetworkTable();

                                    ConnectionTable[handle].status.bits.FinishJoin = 1;    

                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            ConnectionTable[handle].PeerInfo[i] = MACRxPacket.Payload[2+i];
                                        }
                                    #endif
    
START_ASSOCIATION_RESPONSE:                          
                                    //send back the asociation response
                                    TxBuffer[0] = MAC_COMMAND_ASSOCIATION_RESPONSE;
                                    TxBuffer[1] = tempShortAddress.v[0];
                                    TxBuffer[2] = tempShortAddress.v[1];
                                    TxBuffer[3] = ASSOCIATION_SUCCESSFUL;
                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            TxBuffer[4+i] = AdditionalNodeID[i];
                                        }
                                    #endif
                                    TxData = 4+ADDITIONAL_NODE_ID_SIZE;
                                    
                                    #if defined(ENABLE_SECURITY)
                                        IncomingFrameCounter[handle].Val = 0;
        					        #endif
                                    
                                    #if defined(IEEE_802_15_4)
                                        SendMACPacket(myPANID.v, ConnectionTable[handle].Address, PACKET_TYPE_COMMAND, 0);
                                    #else
                                        SendMACPacket(ConnectionTable[handle].Address, PACKET_TYPE_COMMAND);
                                    #endif
                                    
                                }
                            }
                            break;
                        #endif
                        
                        case MAC_COMMAND_ASSOCIATION_RESPONSE:
                            //ConsolePutROMString((ROM char*)"Association Response\r\n");
                            if(MACRxPacket.Payload[3] == ASSOCIATION_SUCCESSFUL)
                            {
                                //The join was successful
                                //assign myself the correct short address
                                myShortAddress.v[0] = MACRxPacket.Payload[1];
                                myShortAddress.v[1] = MACRxPacket.Payload[2];
                                                            
                                #ifdef NWK_ROLE_COORDINATOR
                                if(myShortAddress.v[0] == 0x00)
                                {
                                    ConsolePutROMString((ROM char*)"I am a coordinator\r\n");
                                    role = ROLE_COORDINATOR;
                                    knownCoordinators = (1<<(myShortAddress.v[1] & 0x03)) + 1;
                                    //I know the PAN coordinator and myself
                                }
                                else
                                {
                                    ConsolePutROMString((ROM char*)"I am an end device\r\n");
                                    role = ROLE_FFD_END_DEVICE;
                                }
                                #endif
                                
                                //set the short address of the device
                                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
    
                                //mark as having joined a network
                                MiWiStateMachine.bits.memberOfNetwork = 1;
                                
                                //record the parents information
                                tempNodeStatus.Val = 0xBF;
                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                {
                                    tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                }
    
                                if(myShortAddress.v[0] == 0x00)
                                {
                                    //if I am a coordinator then my parent is the PAN coordinator
                                    //and I am a router
                                    tempShortAddress.Val = 0x0000;
                                }
                                else
                                {
                                    tempShortAddress.v[1] = myShortAddress.v[1];
                                    tempShortAddress.v[0] = 0x00;
                                }
                                
                                tempPANID.Val = myPANID.Val;
                                
                                //and add the parents information to the network table
                                myParent = AddNodeToNetworkTable();
                                #if defined(ENABLE_SECURITY)
    						        IncomingFrameCounter[myParent].Val = 0;
    					        #endif                            
    					        
    					        #if ADDITIONAL_NODE_ID_SIZE > 0
    					            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
    					            {
        					            ConnectionTable[myParent].PeerInfo[i] = MACRxPacket.Payload[4+i];
        					        }
    					        #endif					        

                                ConsolePutROMString((ROM char *)"\r\nJoin the network successfully\r\n");                            
                            }
                            else
                            {
                                //Access denied, remove this as my parent
                                myParent = 0xFF;
                                myPANID.Val = 0xFFFF;
                                tempShortAddress.Val = 0xFFFF;
                                MiMAC_SetAltAddress(tempShortAddress.v, myPANID.v);
                            }
                            
                            {
                                unsigned int p;
                                
                                for(p=0;p<10000;p++)
                                {
                                }
                            }
                            break;
                            

                        case MAC_COMMAND_DISASSOCIATION_NOTIFICATION:
                            {
                                BYTE cIndex;
                                
                                ConsolePutROMString((ROM char*)"Disassociation notification\r\n"); 
                                #if defined(IEEE_802_15_4)
                                    if( MACRxPacket.altSourceAddress )
                                    {
                                        tempShortAddress.v[0] = MACRxPacket.SourceAddress[0];
                                        tempShortAddress.v[1] = MACRxPacket.SourceAddress[1];
                                        
                                        if( (cIndex = SearchForShortAddress()) == 0xFF )
                                        {
                                            break;
                                        }
                                    }
                                    else
                                #endif
                                {
                                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                    {
                                        tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                    }
                                    if( (cIndex = SearchForLongAddress()) == 0xFF )
                                    {
                                        break;
                                    }
                                }
                                
                                #if defined(NWK_ROLE_COORDINATOR)
                                    if( ConnectionTable[cIndex].status.bits.isFamily )
                                    {
                                        if( cIndex != myParent ) // it is from my child
                                        {
                                            ConnectionTable[cIndex].status.Val = 0;
                                        }
                                        else // it is from my parent
                                        {
                                            // notify my children
                                            for(i = 0; i < CONNECTION_SIZE; i++)
                                            {
                                                if( ConnectionTable[i].status.bits.isValid &&
                                                    ConnectionTable[i].status.bits.isFamily && (i != myParent ))
                                                {
                                                    WORD j;
                                                    
                                                    MAC_FlushTx();
                                                    MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                                                    MTP.flags.Val = 0;
                                                    MTP.flags.bits.ackReq = 1;
                                                    MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;                                                
                                                    #if defined(IEEE_802_15_4)
                                                        MTP.altDestAddr = TRUE;
                                                        MTP.altSrcAddr = TRUE;
                                                        MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                                                    #else
                                                        MTP.flags.bits.sourcePrsnt = 1;
                                                        MTP.DestAddress = ConnectionTable[i].Address;
                                                    #endif
                                                    MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                                    for(j = 0; j < 0xFFF; j++) {}   // delay
                                                }
                                                ConnectionTable[i].status.Val = 0;
                                            }
                                            myParent = 0xFF;
                                            myShortAddress.Val = 0xFFFF;
                                            myPANID.Val = 0xFFFF;
                                            MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                                            MiWiStateMachine.bits.memberOfNetwork = 0;
                                        }
                                    }
                                #else
                                    if( ConnectionTable[cIndex].status.bits.isFamily ) // it is from my parent
                                    {
                                        for(i = 0; i < CONNECTION_SIZE; i++)
                                        {
                                            ConnectionTable[i].status.Val = 0;
                                        }
                                        myParent = 0xFF;
                                        myShortAddress.Val = 0xFFFF;
                                        myPANID.Val = 0xFFFF;
                                        MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                                        MiWiStateMachine.bits.memberOfNetwork = 0;
                                    }
                                #endif

                            }
                            break;

                        
                        
                        #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
                        case MAC_COMMAND_DATA_REQUEST:
                        {
                            BYTE handle;
                            
                            #if defined(IEEE_802_15_4)
                            
                                SendIndirectPacket(NULL, MACRxPacket.SourceAddress, TRUE);
                                
                            #else
                            
                                SendIndirectPacket(MACRxPacket.SourceAddress, &(MACRxPacket.Payload[1]), TRUE);

                            #endif

                            
                            break;
                        }
                        #endif
                        
                        #ifdef NWK_ROLE_COORDINATOR
                        case MAC_COMMAND_BEACON_REQUEST:
                            if( ConnMode > ENABL_ACTIVE_SCAN_RSP )
                            {
                                break;
                            }
                        
                            //if we are a member of a network
                            if(MiWiStateMachine.bits.memberOfNetwork)
                            {
                                if( MACRxPacket.Payload[1] != currentChannel )
                                {
                                    break;
                                }
                                
                                //send out a beacon as long as we are not
                                //currently acting as an FFD end device
                                if(role != ROLE_FFD_END_DEVICE)
                                {
                                    MAC_FlushTx();
                                    #if !defined(IEEE_802_15_4)
                                        MiApp_WriteData(myPANID.v[0]);
                                        MiApp_WriteData(myPANID.v[1]);
                                        MiApp_WriteData(myShortAddress.v[0]);
                                        MiApp_WriteData(myShortAddress.v[1]);
                                    #endif
                                    MiApp_WriteData(0xFF);    //superframe specification (BO = 0xF, SO = 0xF)
    								MiApp_WriteData(MiWiCapacityInfo);
                                    MiApp_WriteData(0x00);    // GTS
                                    MiApp_WriteData(0x00);    // Pending addresses
                                    MiApp_WriteData(0x4D);    // Protocol ID
                                    MiApp_WriteData(0x10);    // Version Number
                                    MiApp_WriteData(knownCoordinators);
                                    #if ADDITIONAL_NODE_ID_SIZE > 0 
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            MiApp_WriteData(AdditionalNodeID[i]);
                                        }
                                    #endif
                                    
                                    
                                    //tempPANID.Val = 0xFFFF;
                                    #if defined(IEEE_802_15_4)
                                        SendMACPacket(myPANID.v, NULL, PACKET_TYPE_RESERVE, MSK_ALT_SRC_ADDR);
                                    #else
                                        SendMACPacket(NULL, PACKET_TYPE_RESERVE);
                                    #endif
                                }
                            }
                            break;
                        #endif

                        default:
                            break;
                      }
                    break;
                    
                default:
                    break;

            }
            
            if(MiWiStateMachine.bits.RxHasUserData == 0)
            {
                MiMAC_DiscardPacket();
            }
        }   

        //if there really isn't anything going on 
        #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
            // check indirect message periodically. If an indirect message is not acquired within
            // time of INDIRECT_MESSAGE_TIMEOUT
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {
                    currentTick = TickGet();
                    if( TickGetDiff(currentTick, indirectMessages[i].TickStart) > INDIRECT_MESSAGE_TIMEOUT )
                    {
                        indirectMessages[i].flags.Val = 0x00;   
                        //printf("\r\nIndirect message expired");
                    }    
                }    
            }
        #endif //COORDINATOR_CAPABLE


        #if defined(ENABLE_SLEEP)
            //if we are an RFD and a member of the network
            if(MiWiStateMachine.bits.memberOfNetwork)
            {
                if(MiWiStateMachine.bits.DataRequesting)
                {
                    currentTick = TickGet();
                    diffTick.Val = TickGetDiff(currentTick, DataRequestTimer);
                    if(diffTick.Val > RFD_DATA_WAIT)
                    {
                        //printf("\r\nData Request Timeout");
                        MiWiStateMachine.bits.DataRequesting = 0;
                    }
                }
            }
        #endif
               
        //clean up an old socket if one still exists
        if(openSocketInfo.status.bits.requestIsOpen)
        {
            if(openSocketInfo.status.bits.matchFound)
            {  
                #ifdef NWK_ROLE_COORDINATOR
                    BYTE j;
        
                    ConsolePutROMString((ROM char*)"sending out second response\r\n");

                    MiApp_FlushTx();
                    TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_RESPONSE;      //Report ID

                
                    //copy the long and short address from the Rx Buffer
                    for(j=0;j<8;j++)
                    {
                        TxBuffer[TxData++] = openSocketInfo.LongAddress1[j]; 
                    }
                
                    TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[0];
                    TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[1];
                    
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            TxBuffer[TxData++] = openSocketInfo.AdditionalNodeID1[j];    
                        }
                    #endif
                    
                    MiApp_UnicastAddress(openSocketInfo.ShortAddress2.v, FALSE, FALSE);
                    openSocketInfo.status.bits.requestIsOpen = 0;
                #endif

                //openSocketInfo.status.bits.requestIsOpen = 0;
            }
            else
            {
                currentTick = TickGet();
                diffTick.Val = TickGetDiff(currentTick, openSocketInfo.socketStart);
                
                if(diffTick.Val > OPEN_CLUSTER_SOCKET_TIMEOUT)
                {
                    //invalidate the expired socket request
                    openSocketInfo.status.bits.requestIsOpen = 0;
    
                    //ConsolePutROMString((ROM char*)"Socket request timeout\r\n");
                }
            }
        }
    }


    #ifdef NWK_ROLE_COORDINATOR
        BOOL RouteMessage(WORD_VAL PANID, WORD_VAL ShortAddress, BOOL SecEn)
        {
            BYTE parentNode = (ShortAddress.v[1] & 0x07);
            BYTE i;
            
            if( parentNode == myShortAddress.v[1] )
            {
                // destination is my child
                if( ShortAddress.v[0] & 0x80 )
                {
                    #if defined(ENABLE_INDIRECT_MESSAGE)
                        // this is a sleeping device, need indirect message
                        #if defined(IEEE_802_15_4)
                            SaveIndirectMessage(FALSE, PANID, ShortAddress.v, TRUE, SecEn);
                        #else
                            tempShortAddress.Val = ShortAddress.Val;
                            if( (i = SearchForShortAddress()) == 0xFF )
                            {
                                SaveIndirectMessage(FALSE, ShortAddress.v, TRUE, SecEn); 
                            }
                            else
                            {
                                SaveIndirectMessage(FALSE, ConnectionTable[i].Address, FALSE, SecEn);
                            }
                        #endif
                        return TRUE;
                    #else
                        return FALSE;
                    #endif
                }
                else
                {
                    MTP.flags.Val = 0;
                    MTP.flags.bits.ackReq = 1;
                    MTP.flags.bits.secEn = SecEn;
                    
                    tempShortAddress.Val = ShortAddress.Val;
                    
                    #if defined(IEEE_802_15_4)
                        MTP.altDestAddr = TRUE;
                        MTP.altSrcAddr = TRUE;
                        MTP.DestAddress = tempShortAddress.v;
                        MTP.DestPANID.Val = myPANID.Val;
                    #else
                        if( (i = SearchForShortAddress()) != 0xFF )
                        {
                            MTP.DestAddress = ConnectionTable[i].Address;
                        }
                        else
                        {
                            // the children's info is not in the connection table
                            // should not happen
                            return FALSE;
                        }
                    #endif
                    return MiMAC_SendPacket(MTP, TxBuffer, TxData);
                }
            }
    
            if( (knownCoordinators & (1 << parentNode) ) > 0 )
            {
                if( RouterFailures[parentNode] > MAX_ROUTING_FAILURE )
                {
                    RouterFailures[parentNode] = 0;
                    knownCoordinators &= ((1<<parentNode) & 0xFF);
                    RoutingTable[parentNode] = 0;
                }
                else
                {
                    // we know the destination's parent directly
                    MTP.flags.Val = 0;
                    MTP.flags.bits.ackReq = 1;
                    MTP.flags.bits.secEn = SecEn;
                    
                    tempShortAddress.v[0] = 0;
                    tempShortAddress.v[1] = parentNode;
                    
                    #if defined(IEEE_802_15_4)
                        MTP.altDestAddr = TRUE;
                        MTP.altSrcAddr = TRUE;
                        MTP.DestAddress = tempShortAddress.v;
                        MTP.DestPANID.Val = myPANID.Val;
                    #else
                        if( (i = SearchForShortAddress()) != 0xFF )
                        {
                            MTP.DestAddress = ConnectionTable[i].Address;
                        }
                        else
                        {
                            // highly impossible for none 15.4, where the
                            // access of Coordinators not through beacon
                            goto ROUTE_THROUGH_NEIGHBOR;
                        }
                    #endif
                    if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == FALSE )
                    {
                        RouterFailures[parentNode]++;
                        return FALSE;
                    }
                    else
                    {
                        RouterFailures[parentNode] = 0;
                        return TRUE;
                    }
                }
            }
                
ROUTE_THROUGH_NEIGHBOR: 
            for(i = 0; i < 8; i++)
            {
                if( (RoutingTable[i] & (1 << parentNode) ) > 0 )
                {
                    if( RouterFailures[i] > MAX_ROUTING_FAILURE )
                    {
                        RouterFailures[i] = 0;
                        knownCoordinators &= ((1<<i) & 0xFF);
                        RoutingTable[i] = 0;
                    }
                    else
                    {
                        // we know the destination's neighbor directly
                        MTP.flags.Val = 0;
                        MTP.flags.bits.ackReq = 1;
                        MTP.flags.bits.secEn = SecEn;
                        
                        tempShortAddress.v[0] = 0;
                        tempShortAddress.v[1] = i;
                        
                        #if defined(IEEE_802_15_4)
                            MTP.altDestAddr = TRUE;
                            MTP.altSrcAddr = TRUE;
                            MTP.DestAddress = tempShortAddress.v;
                            MTP.DestPANID.Val = myPANID.Val;
                        #else
                            if( (i = SearchForShortAddress()) != 0xFF )
                            {
                                MTP.DestAddress = ConnectionTable[i].Address;
                            }
                            else
                            {
                                // highly impossible for 15.4, where the
                                // access of Coordinators not through beacon
                                goto ROUTE_THROUGH_TREE;
                            }
                        #endif
                        if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == FALSE )
                        {
                            RouterFailures[i]++;
                            return FALSE;
                        }
                        else
                        {
                            RouterFailures[i] = 0;
                            return TRUE;
                        }
                    }
                }
            }
    
ROUTE_THROUGH_TREE:
            if( role != ROLE_PAN_COORDINATOR )
            {
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.secEn = SecEn;
                
                #if defined(IEEE_802_15_4)
                    MTP.altDestAddr = TRUE;
                    MTP.altSrcAddr = TRUE;
                    MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                    MTP.DestPANID.Val = myPANID.Val;
                #else
                    MTP.DestAddress = ConnectionTable[myParent].Address;
                #endif

                if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == FALSE )
                {
                    RouterFailures[ConnectionTable[myParent].AltAddress.v[1]]++;
                    return FALSE;
                }
                else
                {
                    RouterFailures[ConnectionTable[myParent].AltAddress.v[1]] = 0;
                    return TRUE;
                }      
            }
        
            return TRUE;    
        }
    #endif








    
    #if defined(IEEE_802_15_4)
        BOOL SendMACPacket(BYTE *PANID, BYTE *Address, BYTE PacketType, BYTE ModeMask)
    #else
        BOOL SendMACPacket(BYTE *Address, BYTE PacketType)
    #endif
    {   
        MTP.flags.Val = 0;
        
        MTP.flags.bits.packetType = PacketType;
        if( Address == NULL )
        {
            MTP.flags.bits.broadcast = 1;
        }
        MTP.flags.bits.ackReq = (MTP.flags.bits.broadcast) ? 0:1;
        MTP.flags.bits.sourcePrsnt = 1;
        
        MTP.DestAddress = Address;
        
        #if defined(IEEE_802_15_4)
            if( (ModeMask & MSK_ALT_DST_ADDR) > 0)
            {
                MTP.altDestAddr = TRUE;
            }
            else
            {
                MTP.altDestAddr = FALSE;
            }
            if( (ModeMask & MSK_ALT_SRC_ADDR) > 0)
            {
                MTP.altSrcAddr = TRUE;
            }
            else
            {
                MTP.altSrcAddr = FALSE;
            }
            MTP.DestPANID.v[0] = PANID[0];
            MTP.DestPANID.v[1] = PANID[1];
        #endif
        
        return MiMAC_SendPacket(MTP, TxBuffer, TxData);
    }


    #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
        void SendIndirectPacket(BYTE *Address, BYTE *AltAddress, BOOL isAltAddress)
        {
            BYTE i,j,k;
    
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {
                    if( (indirectMessages[i].flags.bits.isBroadcast == 0) && 
                        indirectMessages[i].flags.bits.isAltAddr && isAltAddress &&
                        (AltAddress[0] == indirectMessages[i].DestAddress[0]) && 
                        (AltAddress[1] == indirectMessages[i].DestAddress[1]) )
                    {
                        MAC_FlushTx();
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        
                        #if defined(IEEE_802_15_4)
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                SendMACPacket(myPANID.v, AltAddress, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR);
                            }
                            else
                            { 
                                MTP.flags.Val = 0;
                                MTP.flags.bits.ackReq = 1;
                                MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                                MTP.DestAddress = AltAddress;
                                MTP.altDestAddr = TRUE;
                                MTP.altSrcAddr = TRUE;
                                MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;
                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                //SendMACPacket(myPANID.v, AltAddress, PACKET_TYPE_DATA, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR);
                            }
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.DestAddress = Address;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                MTP.flags.bits.sourcePrsnt = 1;
                            }
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif
                        
                        indirectMessages[i].flags.bits.isValid = 0;
                        return;
                    }
    
                    if( (indirectMessages[i].flags.bits.isBroadcast == 0) &&
                        (indirectMessages[i].flags.bits.isAltAddr == 0) &&
                        #if defined(IEEE_802_15_4)
                            ( isAltAddress == FALSE ) &&
                        #endif
                        isSameAddress(Address, indirectMessages[i].DestAddress) )
                    {
                        MAC_FlushTx();
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        #if defined(IEEE_802_15_4)
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                SendMACPacket(myPANID.v, Address, PACKET_TYPE_COMMAND, 0);
                            }
                            else
                            {
                                MTP.flags.Val = 0;
                                MTP.flags.bits.ackReq = 1;
                                MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                                MTP.DestAddress = Address;
                                MTP.altDestAddr = FALSE;
                                MTP.altSrcAddr = TRUE;
                                MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;
                                
                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                //SendMACPacket(myPANID.v, Address, PACKET_TYPE_DATA, 0);
                            }
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.DestAddress = Address;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                MTP.flags.bits.sourcePrsnt = 1;
                            }
                            
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif
                        
                        indirectMessages[i].flags.bits.isValid = 0;
                        return;
                    }
                }
            }
            
            
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {        
                    if( indirectMessages[i].flags.bits.isBroadcast )
                    {
                        MAC_FlushTx();
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        #if defined(IEEE_802_15_4)
                            MTP.flags.Val = 0;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                            }
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.sourcePrsnt = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.altSrcAddr = TRUE;
                            if( isAltAddress )
                            {
                                MTP.altDestAddr = TRUE;
                                MTP.DestAddress = AltAddress;
                            }
                            else
                            {
                                MTP.altDestAddr = FALSE;
                                MTP.DestAddress = Address;
                            }
                            MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;                           
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                            }
                            MTP.DestAddress = Address;
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif 
                        return;   
                    }
                }
            }
            
            // no indirect message found
            MAC_FlushTx();
            #if defined(IEEE_802_15_4)
                {
                    if( isAltAddress )
                    {
                        SendMACPacket(myPANID.v, AltAddress, PACKET_TYPE_DATA, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR );
                    }
                    else
                    {
                        SendMACPacket(myPANID.v, Address, PACKET_TYPE_DATA, MSK_ALT_SRC_ADDR);
                    }
                }
            #else
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.DestAddress = Address;
                MiMAC_SendPacket(MTP, TxBuffer, TxData);
            #endif
        }
    #endif
    
    
    /*********************************************************************
     * Function:        BYTE findNextNetworkEntry(void)
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          BYTE 0xFF if there is no room in the network table
     *                  otherwise this function returns the index of the 
     *                  first blank entry in the table
     *
     * Side Effects:    None
     *
     * Overview:        This function is used to determine the next available
     *                  slot in the network table.  This function can be used
     *                  to manually create entries in the networkTable
     ********************************************************************/
    BYTE findNextNetworkEntry(void)
    {
        BYTE i;
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid == 0)
            {
                return i;
            }
        }
        return 0xFF;
    }
    
    
    
    /*********************************************************************
     * Function:        void DumpNetworkTable(void)
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    None
     *
     * Overview:        This function dumps the contents of many tables to
     *                  the console.  This is a debugging\helper function only
     *                  and is not used by the stack
     ********************************************************************/
    #if defined(ENABLE_DUMP)
    void DumpConnection(BYTE index)
    {
        BYTE i, j;
        
        ConsolePutROMString((ROM char*)"\r\nAvailable nodes\r\nHandle RX DC PANID ADDR LONG_ADDR        PEER_INFO\r\n");
        if( index == 0xFF )
        {
            for(i=0;i<CONNECTION_SIZE;i++)
            {
                if(ConnectionTable[i].status.bits.isValid)
                {
                    PrintChar(i);
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    
                    if(ConnectionTable[i].status.bits.RXOnWhenIdle == 1)
                    {
                        ConsolePut('Y');
                    }
                    else
                    {
                        ConsolePut('N');
                    }
                    ConsolePut(' ');
                    ConsolePut(' ');
                    
                    if(ConnectionTable[i].status.bits.directConnection == 1)
                    {
                        ConsolePut('Y');
                    }
                    else
                    {
                        ConsolePut('N');
                    }
                    ConsolePut(' ');
                    ConsolePut(' ');
                                  
                    if(ConnectionTable[i].status.bits.shortAddressValid)
                    {
                        //this is a neighbor
                        PrintChar(ConnectionTable[i].PANID.v[1]);
                        PrintChar(ConnectionTable[i].PANID.v[0]);
                        ConsolePut(' ');
                        ConsolePut(' ');
                        PrintChar(ConnectionTable[i].AltAddress.v[1]);
                        PrintChar(ConnectionTable[i].AltAddress.v[0]);
                        ConsolePut(' ');
                    }
                    else
                    {
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut(' ');
                        ConsolePut(' ');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut(' ');
                    }
                    
                    if(ConnectionTable[i].status.bits.longAddressValid)
                    {
                        for(j = 0; j < 8; j++)
                        {
                            if( j < MY_ADDRESS_LENGTH )
                            {
                                ConsolePut( ConnectionTable[i].Address[MY_ADDRESS_LENGTH-1-j] );
                            }
                            else
                            {
                                Printf(" ");
                            }
                        }
                        Printf("         ");
                    }
                    
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            ConsolePut( ConnectionTable[i].PeerInfo[j] );
                        }
                    #endif
                    printf("\r\n");
                }
            }
        }
        else
        {
            if(ConnectionTable[index].status.bits.isValid)
            {
                PrintChar(index);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(ConnectionTable[index].status.bits.RXOnWhenIdle == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(ConnectionTable[index].status.bits.directConnection == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                              
                if(ConnectionTable[index].status.bits.shortAddressValid)
                {
                    //this is a neighbor
                    PrintChar(ConnectionTable[index].PANID.v[1]);
                    PrintChar(ConnectionTable[index].PANID.v[0]);
                    ConsolePut(' ');
                    ConsolePut(' ');
                    PrintChar(ConnectionTable[index].AltAddress.v[1]);
                    PrintChar(ConnectionTable[index].AltAddress.v[0]);
                    ConsolePut(' ');
                }
                else
                {
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                }
                
                    
                if(ConnectionTable[index].status.bits.longAddressValid)
                {
                    for(j = 0; j < 8; j++)
                    {
                        if( j < MY_ADDRESS_LENGTH )
                        {
                            ConsolePut( ConnectionTable[index].Address[MY_ADDRESS_LENGTH-1-j] );
                        }
                        else
                        {
                            Printf(" ");
                        }
                    }
                    Printf("         ");
                }
                
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                    {
                        ConsolePut( ConnectionTable[index].PeerInfo[j] );
                    }
                #endif
                printf("\r\n");
            }
        }

        #ifdef NWK_ROLE_COORDINATOR
            ConsolePutROMString((ROM char*)"\r\nRouting Table\r\ncoord nextHop\r\n");
            for(i=0;i<8;i++)
            {
                PrintChar(i);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(RoutingTable[i]);
                ConsolePut('\r');
                ConsolePut('\n');
            }
        #endif
    }
    #else
    void DumpNetworkTable(void){}
    #endif
    
    
    /*********************************************************************
     * Function:        void DiscoverNodeByEUI(void)
     *
     * PreCondition:    tempLongAddress[0-7] need to be set to the address
     *                  that needs to be discovered
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    None
     *
     * Overview:        This function constructs and sends out the from to
     *                  discover a device by their EUI address
     ********************************************************************/

    #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
    void DiscoverNodeByEUI(void)
    {
        BYTE i;
        
        #ifndef NWK_ROLE_COORDINATOR
            //if we a member of the network
            if(MiWiStateMachine.bits.memberOfNetwork == 1)
            {
                MAC_FlushTx();
    
                TxBuffer[TxData++] = MAX_HOPS;
                TxBuffer[TxData++] = 0x06;                          //Frame Control
                TxBuffer[TxData++] = 0xFF;                          //dest PANID LSB
                TxBuffer[TxData++] = 0xFF;                          //dest PANID MSB
                TxBuffer[TxData++] = 0xFF;                          //dest address LSB
                TxBuffer[TxData++] = 0xFF;                          //dest address MSB
                TxBuffer[TxData++] = myPANID.v[0];                  //source PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];                  //source PANID MSB
                TxBuffer[TxData++] = myShortAddress.v[0];           //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];           //source address MSB
                TxBuffer[TxData++] = MiWiSeqNum++;                  //seq num
                TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;        //Report Type
                TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;    //Report ID
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    TxBuffer[TxData++] = tempLongAddress[i];            //Data byte 0
                }
                
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.sourcePrsnt = 1;
                
                #if defined(IEEE_802_15_4)
                    MTP.DestAddress = networkTable[myParent].ShortAddress.v;
                    MTP.altDestAddr = TRUE;
                    MTP.altSrcAddr = TRUE;
                #else
                    MTP.DestAddress = networkTable[myParent].LongAddress;
                #endif
                
                MiMAC_SendPacket(MTP, TxBuffer, TxData);
            }
        #else
            //if I am a coordinator capable device
            if(MiWiStateMachine.bits.memberOfNetwork == 1)
            {
                MAC_FlushTx();
                TxBuffer[TxData++] = MAX_HOPS;      
                TxBuffer[TxData++] = 0x06;      //Frame Control
                TxBuffer[TxData++] = 0xFF;      //dest PANID LSB
                TxBuffer[TxData++] = 0xFF;      //dest PANID MSB
                TxBuffer[TxData++] = 0xFF;      //dest address LSB
                TxBuffer[TxData++] = 0xFF;      //dest address MSB
                TxBuffer[TxData++] = myPANID.v[0];      //source PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];      //source PANID MSB
                TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
                TxBuffer[TxData++] = MiWiSeqNum++;      //seq num
                TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;      //Report ID
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    TxBuffer[TxData++] = tempLongAddress[i];      //Data byte 0
                }
                
                MTP.flags.Val = 0;
                MTP.flags.bits.broadcast = 1;
                
                #if defined(IEEE_802_15_4)
                    MTP.DestAddress = NULL;
                    MTP.altDestAddr = TRUE;
                    MTP.altSrcAddr = TRUE;
                #else
                    MTP.DestAddress = NULL;
                #endif
                
                MiMAC_SendPacket(MTP, TxBuffer, TxData);

            }
        #endif
    }
    #endif

    /*********************************************************************
     * Function:        void OpenSocket(void)
     *
     * PreCondition:    None
     *
     * Input:           BYTE socketType - either CLUSTER_SOCKET for a cluster
     *                  socket or P2P_SOCKET for a P2P socket.
     *
     * Output:          None
     *
     * Side Effects:    This will send out a packet trying to create a socket
     *
     * Overview:        This will send out a packet to the PAN coordinator 
     *                  that is trying to link this device to another device.
     *                  the socket operation is complete when OpenSocketComplete()
     *                  returns TRUE.  The status of the operation is retreived through
     *                  the OpenSocketSuccessful() function.  If it returns TRUE
     *                  then OpenSocketHandle() returns the handle of the created
     *                  socket.  This value is valid until openSocket() is called
     *                  again.  If OpenSocketComplete() returns FALSE then the 
     *                  scan in still in progress.
     ********************************************************************/
    void OpenSocket(void)
    {
        BYTE i;
        
        openSocketInfo.status.bits.matchFound = 0;
        
        #ifdef NWK_ROLE_COORDINATOR
        
        //If I am the PAN coordinator
        if(role == ROLE_PAN_COORDINATOR)
        {
            //if there isn't a request currently open
            if(openSocketInfo.status.bits.requestIsOpen == 0)
            {
                //I am the PAN coordinator, there is no reason to send a request out
                openSocketInfo.socketStart = TickGet();
                openSocketInfo.ShortAddress1.Val = 0x0000;
                for(i=0;i<MY_ADDRESS_LENGTH;i++)
                {
                    openSocketInfo.LongAddress1[i] = myLongAddress[i];
                }
                openSocketInfo.status.bits.requestIsOpen = 1;
                openSocketInfo.status.bits.matchFound = 0;
                #if ADDITIONAL_NODE_ID_SIZE > 0 
                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                    {
                        openSocketInfo.AdditionalNodeID1[i] = AdditionalNodeID[i];
                    }
                #endif
            }
            else
            {
                if(openSocketInfo.ShortAddress1.Val == 0x0000)
                {
                    //I am the PAN coordinator and I don't want to talk to myself so lets forget we even tried
                    return;
                }

                if(openSocketInfo.ShortAddress1.Val != myShortAddress.Val)
                {
	                MiApp_FlushTx();
                    TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_RESPONSE;      //Report ID
                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                    {
                        TxBuffer[TxData++] = myLongAddress[i];     
                    }
                    TxBuffer[TxData++] = myShortAddress.v[0];
                    TxBuffer[TxData++] = myShortAddress.v[1];
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                        {
                            TxBuffer[TxData++] = AdditionalNodeID[i];    
                        }
                    #endif
                    
                    for(i=0;i<MY_ADDRESS_LENGTH;i++)
                    {
                        tempLongAddress[i] = openSocketInfo.LongAddress1[i];
                    }
                    tempShortAddress.Val = openSocketInfo.ShortAddress1.Val;

                    openSocketInfo.socketHandle = SearchForShortAddress();
                
                    if(openSocketInfo.socketHandle != 0xFF)
                    {
                        tempNodeStatus.Val = ConnectionTable[openSocketInfo.socketHandle].status.Val;
                    }
                    else
                    {
                        //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                        //tempNodeStatus.Val = 0x2D;
                        tempNodeStatus.Val = 0x8C;
                        if((tempShortAddress.v[0] & 0x80) == 0)
                        {
                            tempNodeStatus.bits.RXOnWhenIdle = 1;
                        }
                    }
                                                    
                    openSocketInfo.socketHandle = AddNodeToNetworkTable();
                    
                    //RouteMessage(myPANID,openSocketInfo.ShortAddress1,FALSE);
                    MiApp_UnicastAddress(openSocketInfo.ShortAddress1.v, FALSE, FALSE);
                    openSocketInfo.status.bits.requestIsOpen = 0;
                    openSocketInfo.status.bits.matchFound = 1;    

                    
                }            
            }
            return;
        }
        else
        {
            //take a record of when you started to send the socket request
            //and send it to the PAN coordinator
            openSocketInfo.socketStart = TickGet();
        }
        
        #else
            openSocketInfo.socketStart = TickGet();
        #endif
           
        tempShortAddress.Val = 0x0000;

		MiApp_FlushTx();
        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
        TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_REQUEST;      //Report ID
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            TxBuffer[TxData++] = myLongAddress[i];      
        }
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                TxBuffer[TxData++] = AdditionalNodeID[i];    
            }
        #endif
        
        MiApp_UnicastAddress(tempShortAddress.v, FALSE, FALSE);
        
        openSocketInfo.status.bits.requestIsOpen = 1;
    

    }

    #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
    /*********************************************************************
     * BOOL IndirectPacket(BOOL Broadcast, 
     *                     WORD_VAL DestinationPANID, 
     *                     BYTE *DestinationAddress, 
     *                     BOOL isCommand, 
     *                     BOOL SecurityEnabled)
     *
     * Overview:        This function store the indirect message for node
     *                  that turns off radio when idle     
     *
     * PreCondition:    None
     *
     * Input:           Broadcast           - Boolean to indicate if the indirect
     *                                        message a broadcast message
     *                  DestinationPANID    - The PAN Identifier of the 
     *                                        destination node
     *                  DestinationAddress  - The pointer to the destination
     *                                        long address
     *                  isCommand           - The boolean to indicate if the packet
     *                                        is command
     *                  SecurityEnabled     - The boolean to indicate if the 
     *                                        packet needs encryption
     *
     * Output:          boolean to indicate if operation successful
     *
     * Side Effects:    An indirect message stored and waiting to deliever
     *                  to sleeping device. An indirect message timer
     *                  has started to expire the indirect message in case
     *                  RFD does not acquire data in predefined interval
     *
     ********************************************************************/
    #if defined(IEEE_802_15_4)
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT WORD_VAL DestinationPANID, 
                                 INPUT BYTE *DestinationAddress, 
                                 INPUT BOOL isAltAddress, 
                                 INPUT BOOL SecurityEnabled)
    #else
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT BYTE *DestinationAddress, 
                                 INPUT BOOL isAltAddress, 
                                 INPUT BOOL SecurityEnabled)
    #endif    
    {
        BYTE i;
        BYTE j;
        
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid == 0)
            {
                indirectMessages[i].flags.Val = 0;
                indirectMessages[i].flags.bits.isBroadcast = Broadcast;
                indirectMessages[i].flags.bits.isSecured = SecurityEnabled;
                indirectMessages[i].flags.bits.isValid = 1;
                indirectMessages[i].flags.bits.isAltAddr = isAltAddress;
                #if defined(IEEE_802_15_4)
                    if( isAltAddress == FALSE )
                    {
                        if( Broadcast == FALSE )
                        {
                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                            {
                                indirectMessages[i].DestAddress[j] = DestinationAddress[j];
                            }
                        }
                    }
                    else
                    {
                        if( Broadcast == FALSE )
                        {
                            indirectMessages[i].DestAddress[0] = DestinationAddress[0];
                            indirectMessages[i].DestAddress[1] = DestinationAddress[1];
                        }
                    }
                    indirectMessages[i].DestPANID.Val = DestinationPANID.Val;
                #else
                    if( Broadcast == FALSE )
                    {
                        if( isAltAddress )
                        {
                            indirectMessages[i].DestAddress[0] = DestinationAddress[0];
                            indirectMessages[i].DestAddress[1] = DestinationAddress[1]; 
                        }
                        else
                        {
                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                            {
                                indirectMessages[i].DestAddress[j] = DestinationAddress[j];
                            }
                        }     
                    }
                #endif
                
                indirectMessages[i].PayLoadSize = TxData;
                for(j = 0; j < TxData; j++)
                {
                    indirectMessages[i].PayLoad[j] = TxBuffer[j];
                }
                indirectMessages[i].TickStart = TickGet();
                return TRUE;
            }
        }
        
        return FALSE;
    }
    #endif
    

    
    /*********************************************************************
     * Function:        BYTE SearchForShortAddress(void)
     *
     * PreCondition:    tempShortAddress and tempPANID are set to the device
     *                  that you are looking for
     *
     * Input:           BYTE entryType - this is set to NEIGHBOR if you
     *                  want to find a Node on the network.  This value
     *                  is set to NETWORK if you are looking for a specific network
     *                  and not a node on the current operating network
     *
     * Output:          BYTE - the index of the network table entry of the
     *                  requested device.  0xFF indicates that the requested
     *                  device doesn't exist in the network table
     *
     * Side Effects:    None
     *
     * Overview:        This function looks up the index of a node or network
     *                  in the network table by short address.
     ********************************************************************/
    BYTE SearchForShortAddress(void)
    {
        BYTE i;   
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.shortAddressValid)
            {
                if(ConnectionTable[i].AltAddress.Val == tempShortAddress.Val)
                {
                    return i;
                }
            }
        }
        return 0xFF;
    }

    
    /*********************************************************************
     * Function:        BYTE SearchForLongAddress(void)
     *
     * PreCondition:    tempLongAddress is set to the device
     *                  that you are looking for
     *
     * Input:           None
     *
     * Output:          BYTE - the index of the network table entry of the
     *                  requested device.  0xFF indicates that the requested
     *                  device doesn't exist in the network table
     *
     * Side Effects:    None
     *
     * Overview:        This function looks up the index of a node or network
     *                  in the network table by long address.
     ********************************************************************/
    BYTE SearchForLongAddress(void)
    {
        BYTE i,j;   
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.longAddressValid)
            {
                for(j=0;j<MY_ADDRESS_LENGTH;j++)
                {
                    if(ConnectionTable[i].Address[j] != tempLongAddress[j])
                    {
                        goto EndOfSearchLoop;
                    }
                }
                return i;
            }
EndOfSearchLoop:
            ;
        }
        return 0xFF;
    }

    
    
    /*********************************************************************
     * Function:        BYTE AddNodeToNetworkTable(void)
     *
     * PreCondition:    tempLongAddress, tempShortAddress, tempPANID, and 
     *                  tempNodeStatus are set to the correct values.  If 
     *                  tempNodeStatus.bits.longAddressValid is set then
     *                  tempLongAddress needs to be set.  If 
     *                  tempNodeStatus.bits.shortAddressValid is set then
     *                  tempShortAddress and tempPANID need to be set.
     *
     * Input:           None
     *
     * Output:          BYTE - the index of the network table entry 
     *                  where the device was inserted.  0xFF indicates that
     *                  the requested device couldn't be inserted into the table
     *
     * Side Effects:    Network table is updated with the devices info
     *
     * Overview:        This function is used to insert new device into the 
     *                  network table (or update already existing entries)
     ********************************************************************/
    BYTE AddNodeToNetworkTable(void)
    {
        BYTE handle;
        
        if(tempNodeStatus.bits.longAddressValid)
        {
            handle = SearchForLongAddress();
        }
        else
        {
            if(tempNodeStatus.bits.shortAddressValid)
            {
                handle = SearchForShortAddress();
            }
        }
                                       
        if(handle==0xFF)
        {
            handle = findNextNetworkEntry();
        }
        
        if(handle != 0xFF)
        { 
            //isFamily = 1, RxOnWhenIdle = 1, Is a neighbor and not a network,
            //not a P2P connection, short and long addresses valid as well 
            //as the entire entry
            
            //If it is not family then update the node information
            ConnectionTable[handle].status.Val = tempNodeStatus.Val;
            if(tempNodeStatus.bits.longAddressValid)
            {
                BYTE i;
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    ConnectionTable[handle].Address[i] = tempLongAddress[i];
                }
            }
                   
            if(tempNodeStatus.bits.shortAddressValid)
            {        
                ConnectionTable[handle].AltAddress.Val = tempShortAddress.Val;
            }
            
            ConnectionTable[handle].PANID.Val = tempPANID.Val;
            #if defined(ENABLE_SECURITY)
                IncomingFrameCounter[handle].Val = 0;
            #endif
        }
        
        return handle;
    }

    /*********************************************************************
     * BOOL    isSameAddress(BYTE *Address1, BYTE *Address2)
     *
     * Overview:        This function compares two long addresses and returns
     *                  the boolean to indicate if they are the same
     *
     * PreCondition:    
     *
     * Input:  
     *          Address1    - Pointer to the first long address to be compared
     *          Address2    - Pointer to the second long address to be compared
     *                  
     * Output: 
     *          If the two address are the same
     *
     * Side Effects:    
     *
     ********************************************************************/
    BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2)
    {
        BYTE i;
        
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            if( Address1[i] != Address2[i] )
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    

    #if defined(ENABLE_SLEEP)
     /*********************************************************************
     * BOOL CheckForData(void)
     *
     * Overview:        This function sends out a Data Request to the peer
     *                  device of the first P2P connection. 
     *
     * PreCondition:    Transceiver is initialized and fully waken up
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The P2P stack is waiting for the response from
     *                  the peer device. A data request timer has been
     *                  started. In case there is no response from the
     *                  peer device, the data request will time-out itself
     *
     ********************************************************************/
     BOOL CheckForData(void)
     {
        MAC_FlushTx();
        MiApp_WriteData(MAC_COMMAND_DATA_REQUEST);
        
        #if defined(IEEE_802_15_4)
            if( SendMACPacket(ConnectionTable[0].PANID.v, ConnectionTable[0].AltAddress.v, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR|MSK_ALT_SRC_ADDR) )
        #else
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            
            if( SendMACPacket(ConnectionTable[0].Address, PACKET_TYPE_COMMAND) )
        #endif           
        {
            MiWiStateMachine.bits.DataRequesting = 1;
            DataRequestTimer = TickGet();  
            return TRUE;
        }
        return FALSE;
    }
    #endif


    
    
    BOOL MiApp_ProtocolInit(void)
    {
        MACINIT_PARAM MIP;
        BYTE i;
        
        #if defined(ENABLE_NVM)
            #if defined(ENABLE_NVM_MAC)
                if( MY_ADDRESS_LENGTH > 6 )
                {
                    for(i = 0; i < 3; i++)
                    {
                        EEPROMRead(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                    }
                    myLongAddress[4] = 0xFF;
                    if( MY_ADDRESS_LENGTH > 7 )
                    {
                        myLongAddress[3] = 0xFE;
                    }
                    for(i = 0; i < 3; i++)
                    {
                        EEPROMRead(&(myLongAddress[2-i]), EEPROM_MAC_ADDR+3+i, 1);
                    }
                }
                else
                {
                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                    {
                        EEPROMRead(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                    }   
                }
            
            #endif
        #endif

        MIP.actionFlags.Val = 0;
        MIP.actionFlags.bits.PAddrLength = MY_ADDRESS_LENGTH;
        MIP.actionFlags.bits.CCAEnable = 1;
        MIP.PAddress = myLongAddress;
        MiMAC_Init(MIP);
        
        myShortAddress.Val = 0xFFFF;
        myPANID.Val = 0xFFFF;

        MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
    
        //clear the network table
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            ConnectionTable[i].status.Val = 0;
        }
        
        #ifdef NWK_ROLE_COORDINATOR
            for(i=0;i<8;i++)
            {
                RoutingTable[i] = 0;
                RouterFailures[i] = 0;
            }
            knownCoordinators = 0;
        #endif
        MiWiStateMachine.Val = 0;

		openSocketInfo.status.Val = 0;
     
        InitSymbolTimer();
        
        TxData = 0;
        #ifdef ENABLE_INDIRECT_MESSAGE
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                indirectMessages[i].flags.Val = 0;
            }
        #endif 
        
        #if defined(ENABLE_SLEEP) && defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
            for(i = 0; i < BROADCAST_RECORD_SIZE; i++)
            {
                BroadcastRecords[i].RxCounter = 0;
            }
        #endif
               
        MiWiCapacityInfo = 0;
        #if !defined(ENABLE_SLEEP)
            MiWiCapacityInfo |= 0x01;
        #endif
        #if defined(ENABLE_SECURITY)
            MiWiCapacityInfo |= 0x08;
        #endif
        MiWiCapacityInfo |= (ConnMode << 4);
        #ifdef NWK_ROLE_COORDINATOR
            MiWiCapacityInfo |= 0x40;
        #endif
        
        RFIF = 0;
        RFIE = 1;
        return TRUE;        
    
    }
    
    #if defined(ENABLE_SLEEP)
    /************************************************************************************
     * Function:
     *      BYTE    MiApp_TransceiverPowerState(BYTE Mode)
     *
     * Summary:
     *      This function put the RF transceiver into different power state. i.e. Put the 
     *      RF transceiver into sleep or wake it up.
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      put RF transceiver into sleep or wake it up. This function is only available
     *      to those wireless nodes that may have to disable the transceiver to save 
     *      battery power.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      BYTE Mode - The mode of power state for the RF transceiver to be set. The possible
     *                  power states are following
     *                  * POWER_STATE_SLEEP     The deep sleep mode for RF transceiver
     *                  * POWER_STATE_WAKEUP    Wake up state, or operating state for RF transceiver
     *                  * POWER_STATE_WAKEUP_DR Put device into wakeup mode and then transmit a 
     *                                          data request to the device's associated device
     *
     * Returns: 
     *      The status of the operation. The following are the possible status
     *      * SUCCESS           Operation successful
     *      * ERR_TRX_FAIL      Transceiver fails to go to sleep or wake up
     *      * ERR_TX_FAIL       Transmission of Data Request command failed. Only available if the
     *                          input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_RX_FAIL       Failed to receive any response to Data Request command. Only available
     *                          if input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_INVLAID_INPUT Invalid input mode. 
     *
     * Example:
     *      <code>
     *      // put RF transceiver into sleep
     *      MiApp_TransceiverPowerState(POWER_STATE_SLEEP;
     *
     *      // Put the MCU into sleep
     *      Sleep();    
     *
     *      // wakes up the MCU by WDT, external interrupt or any other means
     *      
     *      // make sure that RF transceiver to wake up and send out Data Request
     *      MiApp_TransceiverPowerState(POWER_STATE_WAKEUP_DR);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    BYTE MiApp_TransceiverPowerState(BYTE Mode)
    {
        BYTE status;
        
        switch(Mode)
        {
            case POWER_STATE_SLEEP:
                {
                    if( MiMAC_PowerState(POWER_STATE_DEEP_SLEEP) )
                    {
                        //MiWiStateMachine.bits.PHY_SLEEPING = 1;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
                
            case POWER_STATE_WAKEUP:
                {
                    if( MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        //MiWiStateMachine.bits.PHY_SLEEPING = 0;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
               
            case POWER_STATE_WAKEUP_DR:
                {
                    if( FALSE == MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        return ERR_TRX_FAIL;
                    } 
                    
                    //MiWiStateMachine.bits.PHY_SLEEPING = 0;
                    if( CheckForData() == FALSE )
                    {
                        return ERR_TX_FAIL;
                    }
                    while( MiWiStateMachine.bits.DataRequesting ) 
                    {
                        MiWiTasks();
                    }
                    return SUCCESS;
                }
                
             default:
                break;
        }
        
        return ERR_INVALID_INPUT;    
    }    
    #endif
    
    
BOOL MiApp_SetChannel(BYTE channel)
{
    if( MiMAC_SetChannel(channel, 0) )
    {
        currentChannel = channel;
        return TRUE;
    }
    return FALSE;
}    
    
    
/************************************************************************************
 * Function:
 *      void    MiApp_ConnectionMode(BYTE Mode)
 *
 * Summary:
 *      This function set the current connection mode.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      configure the way that the host device accept connection request.
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters:           
 *      BYTE Mode -     The mode to accept connection request. The privilege for those modes
 *                      decreases gradually as defined. The higher privilege mode has all the 
 *                      rights of the lower privilege modes.
 *                      The possible modes are
 *                      * ENABLE_ALL_CONN       Enable response to all connection request
 *                      * ENABLE_PREV_CONN      Enable response to connection request
 *                                              from device already in the connection
 *                                              table.
 *                      * ENABLE_ACTIVE_SCAN_RSP    Enable response to active scan only
 *                      * DISABLE_ALL_CONN      Disable response to connection request, including
 *                                              an acitve scan request.
 *
 * Returns: 
 *      None
 *
 * Example:
 *      <code>
 *      // Enable all connection request
 *      MiApp_ConnectionMode(ENABLE_ALL_CONN);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
void MiApp_ConnectionMode(INPUT BYTE Mode)
{
    if( Mode > 3 )
    {
        return;
    }
    ConnMode = Mode;
    MiWiCapacityInfo = (MiWiCapacityInfo & 0xCF) | (ConnMode << 4);
}    
    


/************************************************************************************
 * Function:
 *      BYTE    MiApp_SearchConnection(BYTE ScanDuartion, DWORD ChannelMap)
 *
 * Summary:
 *      This function perform an active scan to locate operating PANs in the
 *      neighborhood.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      perform an active scan. After this function call, all active scan response
 *      will be stored in the global variable ActiveScanResults in the format of 
 *      structure ACTIVE_SCAN_RESULT. The return value indicates the total number
 *      of valid active scan response in the active scan result array.
 *
 * PreCondition:    
 *      Protocol initialization has been done.
 *
 * Parameters:           
 *      BYTE ScanDuration - The maximum time to perform scan on single channel. The
 *                          value is from 5 to 14. The real time to perform scan can
 *                          be calculated in following formula from IEEE 802.15.4 
 *                          specification 
 *                              960 * (2^ScanDuration + 1) * 10^(-6) second
 *      DWORD ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
 *                          double word parameter use one bit to represent corresponding
 *                          channels from 0 to 31. For instance, 0x00000003 represent to 
 *                          scan channel 0 and channel 1. 
 *                  
 * Returns: 
 *      The number of valid active scan response stored in the global variable ActiveScanResults.
 *
 * Example:
 *      <code>
 *      // Perform an active scan on all possible channels
 *      NumOfActiveScanResponse = MiApp_SearchConnection(10, 0xFFFFFFFF);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/
BYTE MiApp_SearchConnection(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap)
{
    BYTE i;
    DWORD channelMask = 0x00000001;  
    BYTE backupChannel = currentChannel;
    
    for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
    {
        ActiveScanResults[i].Channel = 0xFF;
    }
    ActiveScanResultIndex = 0;
    MiWiStateMachine.bits.searchingForNetwork = 1;
    
    i = 0;
    while(i < 32 )
    {
        if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
        {
            printf("\r\nScan Channel ");
            PrintDec(i);
            /* choose appropriate channel */
            MiApp_SetChannel( i );
 
            MAC_FlushTx();
            MiApp_WriteData(MAC_COMMAND_BEACON_REQUEST);
            MiApp_WriteData(currentChannel);
            #if defined(IEEE_802_15_4)
                tempPANID.Val = 0xFFFF;
                SendMACPacket(tempPANID.v, NULL, PACKET_TYPE_COMMAND, 0);
            #else
                SendMACPacket(NULL, PACKET_TYPE_COMMAND);
            #endif
            
            currentTick = TickGet();
            while(1)
            {
                MiWiTasks();
                diffTick = TickGet();
                if( TickGetDiff(diffTick, currentTick) > ((DWORD)(ScanTime[ScanDuration])) )
                {
                    // if scan time exceed scan duration, prepare to scan the next channel
                    break;
                }
            }          
        }  
        i++;
    }
    
    MiApp_SetChannel(backupChannel);
    MiWiStateMachine.bits.searchingForNetwork = 0;
    
    return ActiveScanResultIndex;
}







    
/************************************************************************************
 * Function:
 *      BYTE    MiApp_EstablishConnection(BYTE ActiveScanIndex, BYTE Mode)
 *
 * Summary:
 *      This function establish a connection with one or more nodes in an existing
 *      PAN.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      start communication with an existing PAN. For P2P protocol, this function
 *      call can establish one or more connections. For network protocol, this 
 *      function can be used to join the network, or establish a virtual socket
 *      connection with a node out of the radio range. There are multiple ways to
 *      establish connection(s), all depends on the input parameters.
 *
 * PreCondition:    
 *      Protocol initialization has been done. If only to establish connection with
 *      a predefined device, an active scan must be performed before and valid active
 *      scan result has been saved.
 *
 * Parameters:           
 *      BYTE ActiveScanIndex -  The index of the target device in the ActiveScanResults
 *                              array, if a predefined device is targeted. If the 
 *                              value of ActiveScanIndex is 0xFF, the protocol stack will
 *                              try to establish a connection with any device.
 *      BYTE Mode - The mode to establish a connection. This parameter is generally valid in
 *                  a network protocol. The possible modes are:
 *                  * CONN_MODE_DIRECT      Establish a connection without radio range.
 *                  * CONN_MODE_INDIRECT    Establish a virtual connection with a device 
 *                                          that may be in or out of the radio range. This
 *                                          mode sometimes is called cluster socket, which
 *                                          is only valid for network protocol. The PAN 
 *                                          Coordinator will be involved to establish a 
 *                                          virtual indirect socket connection.
 *                  
 * Returns: 
 *      The index of the peer device on the connection table.
 *
 * Example:
 *      <code>
 *      // Establish one or more connections with any device
 *      PeerIndex = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
 *      </code>
 *
 * Remarks:    
 *      If more than one connections have been established through this function call, the
 *      return value points to the index of one of the peer devices.
 *
 *****************************************************************************************/  
BYTE    MiApp_EstablishConnection(INPUT BYTE ActiveScanIndex, INPUT BYTE Mode)
{
    BYTE retry = CONNECTION_RETRY_TIMES;
    //BYTE connectionInterval = CONNECTION_INTERVAL;
    BYTE i;
    
    
    if( Mode == CONN_MODE_INDIRECT )
    {
        #if defined(ENABLE_SLEEP)
            TICK startTick = TickGet();;
        #endif
        OpenSocket();
        while(openSocketInfo.status.bits.requestIsOpen)
        {
            MiWiTasks();
            #if defined(ENABLE_SLEEP)
                currentTick = TickGet();
                if( TickGetDiff(currentTick, startTick) > (ONE_SECOND) )
                {
                    CheckForData();
                    startTick.Val = currentTick.Val;
                }
            #endif
        }
        if( openSocketInfo.status.bits.matchFound )
        {
            return openSocketInfo.socketHandle;
        }
        return 0xFF;
    }
    else if( Mode == CONN_MODE_DIRECT)
    {
        if( ActiveScanIndex == 0xFF )
        {
            while( MiApp_SearchConnection(10, ((DWORD)0x00000001)<<currentChannel) == 0 )
            {
                if( retry-- == 0 )
                {
                    return 0xFF;
                }
            }
            ActiveScanIndex = 0;
        }
        
        
        if( (myParent = findNextNetworkEntry()) == 0xFF )
        {
            return 0xFF;
        }
        
        ConnectionTable[myParent].status.Val = 0;
        ConnectionTable[myParent].PANID.Val = ActiveScanResults[ActiveScanIndex].PANID.Val;
        #if defined(IEEE_802_15_4)
            ConnectionTable[myParent].AltAddress.v[0] = ActiveScanResults[ActiveScanIndex].Address[0];
            ConnectionTable[myParent].AltAddress.v[1] = ActiveScanResults[ActiveScanIndex].Address[1];
            ConnectionTable[myParent].status.bits.shortAddressValid = 1;
        #else
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                ConnectionTable[myParent].Address[i] = ActiveScanResults[ActiveScanIndex].Address[i];
            }
            ConnectionTable[myParent].status.bits.longAddressValid = 1;
        #endif
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                ConnectionTable[myParent].PeerInfo[i] = ActiveScanResults[ActiveScanIndex].PeerInfo[i];
            }
        #endif
        
        ConnectionTable[myParent].status.bits.directConnection = 1;
        ConnectionTable[myParent].status.bits.isFamily = 1;
        ConnectionTable[myParent].status.bits.RXOnWhenIdle = 1;
        
        MiApp_SetChannel(ActiveScanResults[ActiveScanIndex].Channel);
        
        /* Program the PANID to the attempted network */
        myPANID.Val = ConnectionTable[myParent].PANID.Val;
        tempShortAddress.Val = 0xFFFF;
        MiMAC_SetAltAddress(tempShortAddress.v, myPANID.v);

        MAC_FlushTx();
        MiApp_WriteData(MAC_COMMAND_ASSOCIATION_REQUEST);   
        MiApp_WriteData(MiWiCapacityInfo);
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                MiApp_WriteData(AdditionalNodeID[i]);
            }
        #endif
        
        #if defined(IEEE_802_15_4)
            SendMACPacket(ConnectionTable[myParent].PANID.v, ConnectionTable[myParent].AltAddress.v, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR);
        #else
            SendMACPacket(ConnectionTable[myParent].Address, PACKET_TYPE_COMMAND);
        #endif
        
        currentTick = TickGet();
        while(ConnectionTable[myParent].status.bits.FinishJoin == 0 )
        {
            MiWiTasks();
            diffTick = TickGet();
            diffTick.Val = TickGetDiff(diffTick, currentTick);
            if( diffTick.Val > ONE_SECOND )
            {
                return 0xFF;
            }
        }
        return myParent;
    }
    return 0xFF;
}

BOOL MiApp_MessageAvailable(void)
{
    MiWiTasks();

    return MiWiStateMachine.bits.RxHasUserData;
}
    
void MiApp_DiscardMessage(void)
{
    MiWiStateMachine.bits.RxHasUserData = 0;
    MiMAC_DiscardPacket();    
}    
    


/************************************************************************************
 * Function:
 *      BOOL MiApp_BroadcastPacket(BOOL SecEn )
 *
 * Summary:
 *      This function broadcast a message in the TxBuffer.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      broadcast a message. The application payload is filled in the global char
 *      array TxBuffer.
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters:           
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
 *                      secured before transmission.
 *
 * Returns: 
 *      A boolean to indicates if the broadcast procedure is succcessful.
 *
 * Example:
 *      <code>
 *      // Secure and then broadcast the message stored in TxBuffer
 *      MiApp_BroadcastPacket(TRUE);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
BOOL MiApp_BroadcastPacket( INPUT BOOL SecEn )   
{

    TxBuffer[0] = MAX_HOPS;
    TxBuffer[1] = 0x02;
    TxBuffer[2] = myPANID.v[0];
    TxBuffer[3] = myPANID.v[1];
    TxBuffer[4] = 0xFF;
    TxBuffer[5] = 0xFF;
    TxBuffer[6] = myPANID.v[0];
    TxBuffer[7] = myPANID.v[1];
    TxBuffer[8] = myShortAddress.v[0];
    TxBuffer[9] = myShortAddress.v[1];
    TxBuffer[10] = MiWiSeqNum++;
    
    #if defined(ENABLE_INDIRECT_MESSAGE)

        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            if( ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.RXOnWhenIdle == 0 )
            {
                break;
            }     
        }
        
        if( i < CONNECTION_SIZE )
        {
            #if defined(IEEE_802_15_4)
                SaveIndirectMessage(TRUE, myPANID, NULL, FALSE, SecEn);
            #else
                SaveIndirectMessage(TRUE, NULL, FALSE, SecEn);
            #endif
        }
    #endif
    
    MTP.flags.Val = 0;
    MTP.flags.bits.broadcast = 1;
    MTP.flags.bits.secEn = SecEn;
        
    #if defined(IEEE_802_15_4)
        MTP.altSrcAddr = TRUE;
        MTP.DestPANID.Val = myPANID.Val;
    #endif    
    MiMAC_SendPacket(MTP, TxBuffer, TxData);    
    return TRUE;
}    


/************************************************************************************
 * Function:
 *      BOOL MiApp_UnicastConnection(BYTE ConnectionIndex, BOOL SecEn)
 *
 * Summary:
 *      This function unicast a message in the TxBuffer to the device with the input 
 *      ConnectionIndex in the connection table. 
 *
 * Description:        
 *      This is one of the primary user interface functions for the application layer to 
 *      unicast a message. The destination device is in the connection table specified by 
 *      the input parameter ConnectionIndex. The application payload is filled in the 
 *      global char array TxBuffer.
 *
 * PreCondition:    
 *      Protocol initialization has been done. The input parameter ConnectionIndex points to
 *      a valid peer device in the connection table.
 *
 * Parameters: 
 *      BYTE ConnectionIndex -  The index of the destination device in the connection table.          
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
 *                      secured before transmission.
 *
 * Returns: 
 *      A boolean to indicates if the unicast procedure is succcessful.
 *
 * Example:
 *      <code>
 *      // Secure and then unicast the message stored in TxBuffer to the first device in 
 *      // the connection table
 *      MiApp_UnicastConnection(0, TRUE);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/  
BOOL MiApp_UnicastConnection( INPUT BYTE ConnectionIndex, 
                        INPUT BOOL SecEn)
{
    if( ConnectionTable[ConnectionIndex].status.bits.isValid == 0 )
    {
        return FALSE;
    }

    TxBuffer[0] = MAX_HOPS;				//number of hops
    TxBuffer[1] = 0x02;					//Frame Control
    TxBuffer[2] = ConnectionTable[ConnectionIndex].PANID.v[0];			
    TxBuffer[3] = ConnectionTable[ConnectionIndex].PANID.v[1];
    TxBuffer[4] = ConnectionTable[ConnectionIndex].AltAddress.v[0];
    TxBuffer[5] = ConnectionTable[ConnectionIndex].AltAddress.v[1];
    TxBuffer[6] = myPANID.v[0];			//source PANID LSB
    TxBuffer[7] = myPANID.v[1];			//source PANID MSB
    TxBuffer[8] = myShortAddress.v[0];	//source address LSB
    TxBuffer[9] = myShortAddress.v[1];	//source address MSB
    TxBuffer[10] = MiWiSeqNum++;			    //sequence number
    
    #if defined(NWK_ROLE_COORDINATOR)
        return RouteMessage(ConnectionTable[ConnectionIndex].PANID, ConnectionTable[ConnectionIndex].AltAddress, SecEn);
    #else
        // for end device, always send the message to its parent
        MTP.flags.Val = 0;
        MTP.flags.bits.ackReq = 1;
        MTP.flags.bits.secEn = SecEn;
            
        #if defined(IEEE_802_15_4)
            MTP.altDestAddr = TRUE;
            MTP.altSrcAddr = TRUE;
            MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
            MTP.DestPANID.Val = myPANID.Val;
        #else
            MTP.DestAddress = ConnectionTable[myParent].Address;
        #endif
        
        return MiMAC_SendPacket(MTP, TxBuffer, TxData);
    #endif


}                        



BOOL MiApp_UnicastAddress(BYTE *DestAddress, BOOL PermanentAddr, BOOL SecEn)
{
    BYTE handle;
    BYTE i;
    
    if( PermanentAddr )
    {
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            tempLongAddress[i] = DestAddress[i];
        }
        if( (handle = SearchForLongAddress()) == 0xFF )
        {
DIRECT_LONG_ADDRESS:

            TxBuffer[0] = MAX_HOPS;				//number of hops
            TxBuffer[1] = 0x02;					//Frame Control
            TxBuffer[2] = 0xFF;			
            TxBuffer[3] = 0xFF;
            TxBuffer[4] = 0xFF;
            TxBuffer[5] = 0xFF;
            TxBuffer[6] = myPANID.v[0];			//source PANID LSB
            TxBuffer[7] = myPANID.v[1];			//source PANID MSB
            TxBuffer[8] = myShortAddress.v[0];	//source address LSB
            TxBuffer[9] = myShortAddress.v[1];	//source address MSB
            TxBuffer[10] = MiWiSeqNum++;			    //sequence number

            MTP.flags.Val = 0;
            MTP.flags.bits.ackReq = 1;
            MTP.flags.bits.secEn = SecEn;   
            MTP.DestAddress = DestAddress;
            #if defined(IEEE_802_15_4)
                MTP.DestPANID.Val = myPANID.Val;
            #endif
            
            return MiMAC_SendPacket(MTP, TxBuffer, TxData);
        }
        else
        {
            if( ConnectionTable[handle].status.bits.shortAddressValid == 0 )
            {
                goto DIRECT_LONG_ADDRESS;
            }
            
            TxBuffer[0] = MAX_HOPS;				//number of hops
            TxBuffer[1] = 0x02;					//Frame Control
            TxBuffer[2] = ConnectionTable[handle].PANID.v[0];			
            TxBuffer[3] = ConnectionTable[handle].PANID.v[1];
            TxBuffer[4] = ConnectionTable[handle].AltAddress.v[0];
            TxBuffer[5] = ConnectionTable[handle].AltAddress.v[1];
            TxBuffer[6] = myPANID.v[0];			//source PANID LSB
            TxBuffer[7] = myPANID.v[1];			//source PANID MSB
            TxBuffer[8] = myShortAddress.v[0];	//source address LSB
            TxBuffer[9] = myShortAddress.v[1];	//source address MSB
            TxBuffer[10] = MiWiSeqNum++;	    //sequence number
            #if defined(NWK_ROLE_COORDINATOR)
                return RouteMessage(ConnectionTable[handle].PANID, ConnectionTable[handle].AltAddress, SecEn);
            #else
                // for end device, always send data to its parent
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.secEn = SecEn;   
                
                #if defined(IEEE_802_15_4)
                    MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
                    MTP.altDestAddr = TRUE;
                    MTP.altSrcAddr = TRUE;
                    MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                #else
                    MTP.DestAddress = ConnectionTable[myParent].Address;
                #endif
                
                return MiMAC_SendPacket(MTP, TxBuffer, TxData);
            #endif
        }
    }

    TxBuffer[0] = MAX_HOPS;				//number of hops
    TxBuffer[1] = 0x02;					//Frame Control
    TxBuffer[2] = myPANID.v[0];			
    TxBuffer[3] = myPANID.v[1];
    TxBuffer[4] = DestAddress[0];
    TxBuffer[5] = DestAddress[1];
    TxBuffer[6] = myPANID.v[0];			//source PANID LSB
    TxBuffer[7] = myPANID.v[1];			//source PANID MSB
    TxBuffer[8] = myShortAddress.v[0];	//source address LSB
    TxBuffer[9] = myShortAddress.v[1];	//source address MSB
    TxBuffer[10] = MiWiSeqNum++;	    //sequence number
    #if defined(NWK_ROLE_COORDINATOR)
        tempShortAddress.v[0] = DestAddress[0];
        tempShortAddress.v[1] = DestAddress[1];
        return RouteMessage(myPANID, tempShortAddress, SecEn);
    #else
        // for end device, always send data to its parent
        MTP.flags.Val = 0;
        MTP.flags.bits.ackReq = 1;
        MTP.flags.bits.secEn = SecEn;   
        
        #if defined(IEEE_802_15_4)
            MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
            MTP.altDestAddr = TRUE;
            MTP.altSrcAddr = TRUE;
            MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
        #else
            MTP.DestAddress = ConnectionTable[myParent].Address;
        #endif
             
        return MiMAC_SendPacket(MTP, TxBuffer, TxData);
    #endif


}



BOOL MiApp_StartConnection(BYTE Mode, BYTE ScanDuration, DWORD ChannelMap)
{
    switch(Mode)
    {
        #if defined(NWK_ROLE_COORDINATOR)
            case START_CONN_DIRECT:
                myShortAddress.Val = 0;
                myParent = 0xFF;
                myPANID.Val = MY_PAN_ID;
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                MiWiStateMachine.bits.memberOfNetwork = 1;
                role = ROLE_PAN_COORDINATOR;
                knownCoordinators = 0x01;   //I know myself
                return TRUE;
        #endif
            
        case START_CONN_ENERGY_SCN:
            #if defined(ENABLE_ED_SCAN) && defined(NWK_ROLE_COORDINATOR)
            {
                BYTE channel;
                BYTE RSSIValue;
                
                channel = MiApp_NoiseDetection(ChannelMap, ScanDuration, NOISE_DETECT_ENERGY, &RSSIValue);
                MiApp_SetChannel(channel);
                Printf("\r\nStart Wireless Communication on Channel ");
                PrintDec(channel);
                Printf("\r\n");
                MiWiStateMachine.bits.memberOfNetwork = 1;
                myShortAddress.Val = 0;
                myParent = 0xFF;
                myPANID.Val = MY_PAN_ID;
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                role = ROLE_PAN_COORDINATOR;
                knownCoordinators = 0x01;   //I know myself
                return TRUE;
            }
            #else
                return FALSE;
            #endif
            
        case START_CONN_CS_SCN:
            // Carrier sense scan is not supported for current available transceivers
            return FALSE;         
        
        default:
            break;
    }    
    return FALSE;
}



    #ifdef ENABLE_ED_SCAN
        /************************************************************************************
         * Function:
         *      BYTE MiApp_NoiseDetection(  DWORD ChannelMap, BYTE ScanDuration, 
         *                                  BYTE DetectionMode, BYTE *NoiseLevel)
         *
         * Summary:
         *      This function perform a noise scan and returns the channel with least noise
         *
         * Description:        
         *      This is the primary user interface functions for the application layer to 
         *      perform noise detection on multiple channels.
         *
         * PreCondition:    
         *      Protocol initialization has been done. 
         *
         * Parameters: 
         *      DWORD ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
         *                          double word parameter use one bit to represent corresponding
         *                          channels from 0 to 31. For instance, 0x00000003 represent to 
         *                          scan channel 0 and channel 1. 
         *      BYTE ScanDuration - The maximum time to perform scan on single channel. The
         *                          value is from 5 to 14. The real time to perform scan can
         *                          be calculated in following formula from IEEE 802.15.4 
         *                          specification 
         *                              960 * (2^ScanDuration + 1) * 10^(-6) second
         *      BYTE DetectionMode -    The noise detection mode to perform the scan. The two possible
         *                              scan modes are
         *                              * NOISE_DETECT_ENERGY   Energy detection scan mode
         *                              * NOISE_DETECT_CS       Carrier sense detection scan mode
         *      BYTE *NoiseLevel -  The noise level at the channel with least noise level 
         *
         * Returns: 
         *      The channel that has the lowest noise level
         *
         * Example:
         *      <code>
         *      BYTE NoiseLevel;
         *      OptimalChannel = MiApp_NoiseDetection(0xFFFFFFFF, 10, NOISE_DETECT_ENERGY, &NoiseLevel);
         *      </code>
         *
         * Remarks:    
         *      None
         *
         *****************************************************************************************/
        BYTE MiApp_NoiseDetection(INPUT DWORD ChannelMap, INPUT BYTE ScanDuration, INPUT BYTE DetectionMode, OUTPUT BYTE *RSSIValue)
        {
            BYTE i;
            BYTE OptimalChannel;
            BYTE minRSSI = 0xFF;
            DWORD channelMask = 0x00000001;
            
            if( DetectionMode != NOISE_DETECT_ENERGY )
            {
                return 0xFF;
            }
            
            ConsolePutROMString((ROM char*)"\r\nEnergy Scan Results:");
            i = 0;
            while( i < 32 )
            {
                if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
                {
                    BYTE RSSIcheck;
                    BYTE maxRSSI = 0;
                    BYTE j, k;
    
                    /* choose appropriate channel */
                    MiApp_SetChannel(i);
                    
                    diffTick = TickGet();
                    
                    while(1)
                    {
                        RSSIcheck = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_ENERGY_DETECT);
                        //PrintChar(RSSIcheck);
                        if( RSSIcheck > maxRSSI )
                        {
                            maxRSSI = RSSIcheck;
                        }
                        
                        
                        
                        currentTick = TickGet();
                        if( TickGetDiff(currentTick, diffTick) > ((DWORD)(ScanTime[ScanDuration])) )
                        {
                            // if scan time exceed scan duration, prepare to scan the next channel
                            break;
                        }
                    } 
                    
                    printf("\r\nChannel ");
                    PrintDec(i);
                    printf(": ");
                    j = maxRSSI/5;
                    for(k = 0; k < j; k++)
                    {
                        ConsolePut('-');
                    }
                    printf(" ");
                    PrintChar(maxRSSI);
                    
                    if( maxRSSI < minRSSI )
                    {
                        minRSSI = maxRSSI;
                        OptimalChannel = i;
                        if( RSSIValue )
                        {
                            *RSSIValue = minRSSI;
                        }   
                    }              
                }  
                i++;
            }        
           
            return OptimalChannel;
        }
       
    #endif


    #if defined(ENABLE_FREQUENCY_AGILITY)
        /********************************************************************************************
         * Function:
         *      BOOL MiApp_ResyncConnection(BYTE ConnectionIndex, DWORD ChannelMap)
         *
         * Summary:
         *      This function tries to resynchronize the lost connection with 
         *      peers, probably due to channel hopping
         *
         * Description:        
         *      This is the primary user interface function for the application to resynchronize a 
         *      lost connection. For a RFD device that goes to sleep periodically, it may not 
         *      receive the channel hopping command that is sent when it is sleep. The sleeping 
         *      RFD device depends on this function to hop to the channel that the rest of
         *      the PAN has jumped to. This function call is usually triggered by continously 
         *      communication failure with the peers.
         *
         * PreCondition:    
         *      Transceiver has been initialized
         *
         * Parameters:      
         *      DWORD ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
         *                          double word parameter use one bit to represent corresponding
         *                          channels from 0 to 31. For instance, 0x00000003 represent to 
         *                          scan channel 0 and channel 1. 
         *                  
         * Returns: 
         *                  a boolean to indicate if resynchronization of connection is successful
         *
         * Example:
         *      <code>
         *      // Sleeping RFD device resync with its associated device, usually the first peer
         *      // in the connection table
         *      MiApp_ResyncConnection(0, 0xFFFFFFFF);
         *      </code>
         *
         * Remark:          
         *      If operation is successful, the wireless node will be hopped to the channel that 
         *      the rest of the PAN is operating on.
         *
         *********************************************************************************************/ 
        BOOL MiApp_ResyncConnection(INPUT BYTE ConnectionIndex, INPUT DWORD ChannelMap)
        {
            BYTE i;
            BYTE j;
            BYTE backupChannel = currentChannel;
            
            currentTick = TickGet();
            MiWiStateMachine.bits.Resynning = 1;
            for(i = 0; i < RESYNC_TIMES; i++)
            {
                DWORD ChannelMask = 0x00000001;
                
                j = 0;
                while(MiWiStateMachine.bits.Resynning)
                {
                    diffTick = TickGet();
                    
                    if( TickGetDiff(diffTick, currentTick) > SCAN_DURATION_9 )
                    {
                        currentTick.Val = diffTick.Val;
                        
                        if( j > 31 )
                        {
                            break;
                        }
                        
                        while( (ChannelMap & FULL_CHANNEL_MAP & (ChannelMask << j)) == 0 )
                        {
                            if( ++j > 31 )
                            {
                                goto GetOutOfLoop;
                            }
                        }
                        
                        printf("\r\nChecking Channel ");
                        PrintDec(j);
                        MiApp_SetChannel(j);
                        j++;
    
                    
                        MiApp_FlushTx();
                        MiApp_WriteData(MIWI_STACK_REPORT_TYPE);
                        MiApp_WriteData(RESYNCHRONIZATION_REQUEST);
                        MiApp_WriteData(currentChannel);
                        MiApp_UnicastConnection(ConnectionIndex, FALSE);
                    }
                    MiWiTasks();
                }
                if( MiWiStateMachine.bits.Resynning == 0 )
                {
                    Printf("\r\nResynchronized Connection to Channel ");
                    PrintDec(currentChannel);
                    Printf("\r\n");
                    return TRUE;
                }
GetOutOfLoop:
                Nop();         
            }
            
            MiApp_SetChannel(backupChannel);
            MiWiStateMachine.bits.Resynning = 0;
            return FALSE;
        }

        #if defined(NWK_ROLE_COORDINATOR)
            /*********************************************************************
             * void StartChannelHopping(BYTE OptimalChannel)
             *
             * Overview:        This function broadcast the channel hopping command
             *                  and after that, change operating channel to the 
             *                  input optimal channel     
             *
             * PreCondition:    Transceiver has been initialized
             *
             * Input:           OptimalChannel  - The channel to hop to
             *                  
             * Output: 
             *          None
             *
             * Side Effects:    The operating channel for current device will change
             *                  to the specified channel
             *
             ********************************************************************/
            void StartChannelHopping(INPUT BYTE OptimalChannel)
            {
                BYTE i;
        
                for( i = 0; i < FA_BROADCAST_TIME; i++)
                {
                    currentTick = TickGet();
                    while(1)
                    {
                        diffTick = TickGet();
                        if( TickGetDiff(diffTick, currentTick) > SCAN_DURATION_9 )
                        {
                            TxBuffer[0] = MAX_HOPS;
                            TxBuffer[1] = 0x02;
                            TxBuffer[2] = myPANID.v[0];
                            TxBuffer[3] = myPANID.v[1];
                            TxBuffer[4] = 0xFF;
                            TxBuffer[5] = 0xFF;
                            TxBuffer[6] = myPANID.v[0];
                            TxBuffer[7] = myPANID.v[1];
                            TxBuffer[8] = myShortAddress.v[0];
                            TxBuffer[9] = myShortAddress.v[1];
                            TxBuffer[10] = MiWiSeqNum++;
                            TxBuffer[11] = MIWI_STACK_REPORT_TYPE;
                            TxBuffer[12] = CHANNEL_HOPPING_REQUEST;
                            TxBuffer[13] = currentChannel;
                            TxBuffer[14] = OptimalChannel;
                            
                            MTP.flags.Val = 0;
                            MTP.flags.bits.broadcast = 1;           
                            #if defined(IEEE_802_15_4)
                                MTP.altSrcAddr = TRUE;
                                MTP.DestPANID.Val = myPANID.Val;
                            #endif    
                            
                            MiMAC_SendPacket(MTP, TxBuffer, 15);    
                            break;
                        }
                    }
                }
                MiApp_SetChannel(OptimalChannel);  
            }
        
            BOOL MiApp_InitChannelHopping( INPUT DWORD ChannelMap)
            {
                BYTE RSSIValue;
                BYTE backupChannel = currentChannel;
                BYTE backupConnMode = ConnMode;
                BYTE optimalChannel;
                
                MiApp_ConnectionMode(DISABLE_ALL_CONN);
                optimalChannel = MiApp_NoiseDetection(ChannelMap, 10, NOISE_DETECT_ENERGY, &RSSIValue);
                MiApp_ConnectionMode(backupConnMode);
                
                MiApp_SetChannel(backupChannel);
                if( optimalChannel == backupChannel )
                {
                    return FALSE;
                }
                
                Printf("\r\nHopping to Channel ");
                PrintDec(optimalChannel);
                Printf("\r\n");
                StartChannelHopping(optimalChannel);
                return TRUE;
                
                
                return FALSE;
            }
        #endif
    
    #endif
    
    
    #if !defined(TARGET_SMALL)
        /*********************************************************************
         * Function:
         *      void MiApp_RemoveConnection(BYTE ConnectionIndex)
         *
         * Summary:
         *      This function remove connection(s) in connection table
         *
         * Description:        
         *      This is the primary user interface function to disconnect connection(s).
         *      For a P2P protocol, it simply remove the connection. For a network protocol,
         *      if the device referred by the input parameter is the parent of the device
         *      calling this function, the calling device will get out of network along with
         *      its children. If the device referred by the input parameter is children of
         *      the device calling this function, the target device will get out of network.
         * 
         * PreCondition:    
         *      Transceiver has been initialized. Node has establish
         *      one or more connections
         *
         * Parameters:           
         *      BYTE ConnectionIndex -  The index of the connection in the 
         *                              connection table to be removed
         *                  
         * Returns: 
         *      None
         *
         * Example:
         *      <code>
         *      MiApp_RemoveConnection(0x00);
         *      </code>
         *
         * Remarks:    
         *      None
         *
         ********************************************************************/
        void MiApp_RemoveConnection(INPUT BYTE ConnectionIndex)
        {
            BYTE i;
            WORD j;
                                
            if( ConnectionIndex == 0xFF )
            {
                for(i = 0; i < CONNECTION_SIZE; i++)
                {
                    if( ConnectionTable[i].status.bits.isValid )
                    {
                        if( ConnectionTable[i].status.bits.isFamily )
                        {
                            MAC_FlushTx();
                            MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                            MTP.flags.Val = 0;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                            #if defined(IEEE_802_15_4)
                                MTP.altDestAddr = TRUE;
                                MTP.altSrcAddr = TRUE;
                                MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                            #else
                                MTP.flags.bits.sourcePrsnt = 1;
                                MTP.DestAddress = ConnectionTable[i].Address;
                            #endif
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                            for(j = 0; j < 0xFFF; j++) {}
                        }
                    }
                    ConnectionTable[i].status.Val = 0;
                } 
                
                myParent = 0xFF;
                myShortAddress.Val = 0xFFFF;
                myPANID.Val = 0xFFFF;
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                MiWiStateMachine.bits.memberOfNetwork = 0;
            }
            else if( ConnectionTable[ConnectionIndex].status.bits.isValid )
            {    
                if( ConnectionTable[ConnectionIndex].status.bits.isFamily && (ConnectionIndex == myParent) ) // parent
                {
                    // first notify my parent
                    MAC_FlushTx();
                    MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                    MTP.flags.Val = 0;
                    MTP.flags.bits.ackReq = 1;
                    MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                    #if defined(IEEE_802_15_4)
                        MTP.altDestAddr = TRUE;
                        MTP.altSrcAddr = TRUE;
                        MTP.DestAddress = ConnectionTable[ConnectionIndex].AltAddress.v;
                    #else
                        MTP.flags.bits.sourcePrsnt = 1;
                        MTP.DestAddress = ConnectionTable[ConnectionIndex].Address;
                    #endif
                    MiMAC_SendPacket(MTP, TxBuffer, TxData);
                    for(j = 0; j < 0xFFF; j++) {}
                    

                    for(i = 0; i < CONNECTION_SIZE; i++)
                    {
                        #if defined(NWK_ROLE_COORDINATOR)
                            if( ConnectionTable[i].status.bits.isValid && 
                                ConnectionTable[i].status.bits.isFamily && (myParent != i) ) // children
                            {
                                #if defined(IEEE_802_15_4)
                                    MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                                #else
                                    MTP.DestAddress = ConnectionTable[i].Address;
                                #endif 
                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                for(j = 0; j < 0xFFF; j++) {}      
                            }
                        #endif
                        ConnectionTable[i].status.Val = 0;
                    }

                    // get myself out of network
                    myParent = 0xFF;
                    myShortAddress.Val = 0xFFFF;
                    myPANID.Val = 0xFFFF;
                    MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                    MiWiStateMachine.bits.memberOfNetwork = 0;
                }
                #if defined(NWK_ROLE_COORDINATOR)                
                    else if( ConnectionTable[ConnectionIndex].status.bits.isFamily && (ConnectionIndex != myParent) ) // child
                    {
                        MAC_FlushTx();
                        MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                        MTP.flags.Val = 0;
                        MTP.flags.bits.ackReq = 1;
                        MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                        #if defined(IEEE_802_15_4)
                            MTP.altDestAddr = TRUE;
                            MTP.altSrcAddr = TRUE;
                            MTP.DestAddress = ConnectionTable[ConnectionIndex].AltAddress.v;
                        #else
                            MTP.flags.bits.sourcePrsnt = 1;
                            MTP.DestAddress = ConnectionTable[ConnectionIndex].Address;
                        #endif
                        MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        ConnectionTable[ConnectionIndex].status.Val = 0;
                    }
                #endif
            }
        }
    #endif

    
#else
    // define a bogus variable to bypass limitation of C18 compiler not able to compile an empty file
    extern char bogusVar;
#endif

