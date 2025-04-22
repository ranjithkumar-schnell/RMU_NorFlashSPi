/**  @file
  spi_demo.h

  @brief
  This file is used to define bt demo for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#ifndef SPI_DEMO_H
#define SPI_DEMO_H


#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Variable Definition
 *========================================================================*/
#define SPI_DEMO_TASK_PRIO           12
#define SPI_DEMO_TASK_STACK_SIZE     8*1024
#define SPI_DEMO_TASK_EVENT_CNT      4

//SPI FLASH GENERAL SETTING
#define PAGE_SIZE                  256
#define SECTOR_SIZE                4096
#define BLOCK_SIZE                 0x10000   //65536 OR 64kb
#define DATA_LOG_START_BLOCK       0x10000   // 0 TO 0xFFFF is reserved for metadata
#define PACKET_SIZE                256  
#define PACKETS_PER_DAY            200
#define MAX_DAYS                   180
#define START_BLOCK                1    
#define METADATA_ADDR              0x000000
#define DAY_START_ADDRESS(x)       (((x)*BLOCK_SIZE))  // DATA_LOG_START_BLOCK + (x * 65536) // DATA_LOG_START_BLOCK + (x * 64kb) wher x IS DAY NUMBER
#define PACKET_ADDRESS(x,y)        ((((x)*BLOCK_SIZE))+((y)*PACKET_SIZE)) // DATA_LOG_START_BLOCK + (x * 65536) + (y * 256) where x IS DAY NUMBER AND y IS PACKET NUMBER
#define HIGH_BYTE(x)               ((x)>>16)  
#define MID_BYTE(x)                ((((x)>>8))&(0x00FF))
#define LOWBYTE(x)                 ((x)&(0x00FF))


//WINBOND FLASH 128Mbit 16 MB INSTRUCTION AS PER DATA SHEET

//RESET ID'S /commands
#define ENABLE_RESET_ID             0x66
#define RESET_ID                    0x99


//ERASE ID'S / commands
#define SPI_64KB_BLOCK_ERASE_ID     0xD8
#define SPI_32KB_BLOCK_ERASE_ID     0x52   
#define SPI_4KB_BLOCK_ERASE_ID      0x20     
#define SPI_CHIP_ERASE_ID           0xC7


//READ ID'S / commands
#define MANF_ID                     0x90
#define JDEC_ID                     0X9F
#define READ_ID                     0xAB
#define READ_DATA_ID                0x03
#define READ_UID_ID                 0x4B

//WRITE ID'S / commands
#define WRITE_ENABLE_ID             0x06
#define WRITE_DISABLE_ID            0x04
#define PAGE_WRITE_ID               0x02

// MACROS FOR READ WRITE AND ERASE REALTED
#define PAGE_READ_SIZE              250 // may increase upto 255
#define MANF_ID_RESPONSE_LEN         3
#define JDEC_ID_RESPONSE_LEN         4
#define RESET_ID_LEN                 2 

unsigned char Jdecid ;
unsigned char *outdata;
unsigned char ReadAddress[2];
unsigned char ReadData[PAGE_READ_SIZE];
unsigned char JdecData[5];
//0x90;    //unsigned char outlen = 5;
//unsigned char ManuFacture_id = 0x9f;
//unsigned short inlen =4;

/*========================================================================
 *  function Definition
 *========================================================================*/
QlOSStatus ql_spi_demo_init(void);



#ifdef __cplusplus
} /*"C" */
#endif

#endif /* SPI_DEMO_H */


