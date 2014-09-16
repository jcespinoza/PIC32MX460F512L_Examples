/******************************************************************************
 *
 *               Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        FS Phys Interface Template.c
 * Dependencies:    TEMPLATEFILE.h
 *					string.h
 *                  FSIO.h
 *                  FSDefs.h
 * Processor:       None
 * Compiler:        None
 * Company:         Microchip Technology, Inc.
 * Version:         1.0.0
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/
#ifndef _SSD_SD_CARD_HEADER_FILE
#define _SSD_SD_CARD_HEADER_FILE

#include <GenericTypeDefs.h>

DWORD ReadCapacity(void);
WORD ReadSectorSize(void);
void InitIO(void);
BYTE MediaDetect(void);
BYTE MediaInitialize(void);
BYTE SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
BYTE WriteProtectState(void);

/******************************************************************************
 * User Defines
 *****************************************************************************/
#define SSD_SYS_CLOCK           (4000000)
#define SSD_SYS_CLOCK           (4000000)
#define SSD_SD_CLK_MAX          (50000000)
#define SSD_SD_CLK_INIT         (400000)


/******************************************************************************
 * Registers
 *****************************************************************************/
#define SSD_REG_PLL_CONFIG0                     0x0126
#define SSD_REG_PLL_CONFIG1                     0x0127
#define SSD_REG_MCLK_CONIG                      0x0004
#define SSD_SDCARD_SD_CLK                       0x1001
#define SSD_SDCARD_REG_DMA_ADDR				    0x1100
#define SSD_SDCARD_REG_BLK_SIZE 				0x1104
#define SSD_SDCARD_REG_BLK_CNT 			        0x1106
#define SSD_SDCARD_REG_ARG_32BIT    	        0x1108
#define SSD_SDCARD_REG_XFR_MODE 			    0x110c
#define SSD_SDCARD_REG_CMD 				        0x110e
#define SSD_SDCARD_REG_RSP				        0x1110
#define SSD_SDCARD_REG_DATA_PORT				0x1120
#define SSD_SDCARD_REG_RSVD1		    	    0x1121
#define SSD_SDCARD_REG_PRSNT_STATE_0		    0x1124
#define SSD_SDCARD_REG_PRSNT_STATE_1		    0x1125
#define SSD_SDCARD_REG_PRSNT_STATE_2		    0x1126
#define SSD_SDCARD_REG_PRSNT_STATE_3		    0x1127
#define SSD_SDCARD_REG_HST_CNTL				    0x1128
#define SSD_SDCARD_REG_PWR_CNTL				    0x1129
#define SSD_SDCARD_REG_BLK_GAP_CNTL			    0x112a
#define SSD_SDCARD_REG_WKUP_CNTL			    0x112b
#define SSD_SDCARD_REG_CLK_CNTL				    0x112c
#define SSD_SDCARD_REG_CLK_DIV				    0x112d
#define SSD_SDCARD_REG_TOUT_CNTL			    0x112e
#define SSD_SDCARD_REG_SW_RESET				    0x112f
#define SSD_SDCARD_REG_NRM_INTR_STATUS		    0x1130
#define SSD_SDCARD_REG_ERR_INTR_STATUS		    0x1132
#define SSD_SDCARD_REG_NRM_INTR_STATUS_EN	    0x1134
#define SSD_SDCARD_REG_ERR_INTR_STATUS_EN	    0x1136
#define SSD_SDCARD_REG_NRM_INTR_SIG_EN		    0x1138
#define SSD_SDCARD_REG_ERR_INTR_SIG_EN		    0x113a
#define SSD_SDCARD_REG_ACMD12_ERR_STATUS	    0x113c
#define SSD_SDCARD_REG_RSVD2				    0x113e
#define SSD_SDCARD_REG_CAPABILITIES			    0x1140
#define SSD_SDCARD_REG_CAP_RSVD				    0x1144
#define SSD_SDCARD_REG_MAX_CURR_CAP			    0x1148
#define SSD_SDCARD_REG_MAX_CURR_CAP_RSVD	    0x114c
#define SSD_SDCARD_REG_RSVD3				    0x1150	/*SSD_SDCARD_REGRsvd[(0xFC - 0x50) >> 2]*/
#define SSD_SDCARD_REG_SLOT_INTR_STATUS		    0x11fc
#define SSD_SDCARD_REG_HCVER				    0x11fe	
/******************************************************************************
 * SD Commands
 *****************************************************************************/
#define CMD_RESET				                0
#define CMD_SEND_OCR				            1	// used exclusively in MMC
#define CMD_SEND_ALL_CID			            2	// R2: R136
#define CMD_SEND_RCA				            3	// R1 (MMC) or R6(SDMEM)
#define CMD_SET_DSR				                4
#define CMD_IO_SEND_OCR				            5	// R4, unique to IO cards
#define CMD_SELECT_CARD				            7	// R1, arg=rca[31..16] or 0
#define CMD_SEND_CSD				            9	// R2: R136
#define CMD_SEND_CID				            10	// R2: R136
#define CMD_STOP_TRANSMISSION		            12	// R1b: arg=stuff bits
#define CMD_SEND_STATUS				            13	// R1
#define CMD_GO_INACTIVE				            15	// None, arg=rca[31..16], stuff[15..0]
#define CMD_SET_BLKLEN				            16	// R1, arg=block len[31..0]
#define CMD_RD_SINGLE				            17	// R1, arg=block address[31..0]
#define CMD_RD_MULTIPLE				            18	// R1, arg=block address[31..0]
#define CMD_WR_SINGLE				            24	// R1, arg=block address[31..0]
#define CMD_WR_MULTIPLE				            25 	// R1, arg=block address[31..0]
#define CMD_SET_WP				                28	// R1b, arg=wp address[31..0]
#define CMD_CLR_WP				                29	// R1b, arg=wp address[31..0]
#define CMD_SEND_WP				                30	// R1, DATA, arg=wp address[31..0]
#define CMD_ERASE_SADDR				            32	// R1, arg=block address[31..0]
#define CMD_ERASE_EADDR				            33	// R1, arg=block address[31..0]
#define CMD_ERASE_GRP_SADDR			            35	// R1, arg=block address[31..0]
#define CMD_ERASE_GRP_EADDR			            36	// R1, arg=block address[31..0]
#define CMD_ERASE				                38	// R1b, arg=stuff bits[31..0]
#define CMD_IO_RW_DIRECT			            52	// R5
#define CMD_IO_RW_EXTENDED			            53	// R1, data transfer
#define CMD_APP_CMD				                55	// R1, arg=rca[31..16], stuff[15..0]
#define CMD_GEN_CMD				                56	// R1, data, arg=stuff[31..1], RD/WR[0]
                                                
#define ACMD_SET_BUS_WIDTH			            6	// R1, arg=[1..0] = bus width, [31:2] stuff bits
#define ACMD_SEND_STATUS			            13	// R1, DATA, arg=stuff bits [31..0]
#define ACMD_SEND_NUM_WR_BLK		            22	// R1, DATA, arg=stuff bits [31..0]
#define ACMD_SEND_OCR				            41
#define ACMD_SEND_SCR				            51	// R1, arg=stuff bits[31..0]
/******************************************************************************
 * Flags
 *****************************************************************************/
#define SSD_SD_CLK_CTRL_ON                      0x80000000
#define SSD_SD_CLK_ENABLE                       0x00000004
#define SSD_SD_INT_CLK_STABLE                   0x00000002
#define SSD_SD_INT_CLK_ENABLE                   0x00000001
#define SSD_SD_CLK_FLAGS                        (SSD_SD_CLK_CTRL_ON | SSD_SD_CLK_ENABLE | SSD_SD_INT_CLK_ENABLE)

#define SSD_CMD_TYPE_ABORT                      0xC0
#define SDD_CMD_TYPE_RESUME                     0x80
#define SDD_CMD_TYPE_SUSPEND                    0x40
#define SDD_CMD_TYPE_NORMAL                     0x00
#define SSD_DATA_PRESENT                        0x20
#define SSD_CMD_IDX_CHK                         0x10
#define SSD_CMD_CRC_CHK                         0x08
#define SSD_NO_RESPONSE                         0x00
#define SSD_RESPONSE_136                        0x01
#define SSD_RESPONSE_48                         0x02
#define SSD_RESPONSE_48_BUSY                    0x03

#define SSD_CARD_DETECT                         0x04
#define SSD_CARD_STABLE                         0x02
#define SSD_CARD_INSERTED                       0x01

#define SSD_WRITE_PROTECT                       0x08

#define SSD_RESET_ALL                           0x01
#define SSD_RESET_CMD                           0x02
#define SSD_RESET_DATA                          0x04

#define WAIT_CNT                                10000000l

#endif
