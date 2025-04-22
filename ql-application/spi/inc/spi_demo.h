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

//WINBOND FLASH 128Mbit 16 MB INSTRUCTION AS PER DATA SHEET





//RESET ID'S
#define ENABLE_RESET_ID             0x66
#define RESET_ID                    0x99


//ERASE ID'S
#define SPI_64KB_BLOCK_ERASE_ID     0xD8
#define SPI_32KB_BLOCK_ERASE_ID     0x52   
#define SPI_4KB_BLOCK_ERASE_ID      0x20     
#define SPI_CHIP_ERASE_ID           0xC7


//READ ID'S
#define MANF_ID                     0x90
#define JDEC_ID                     0X9F
#define READ_ID                     0xAB
#define READ_DATA_ID                0x03
#define READ_UID_ID                 0x4B

//WRITE ID'S
#define WRITE_ENABLE_ID             0x06
#define WRITE_DISABLE_ID            0x04
#define PAGE_WRITE_ID               0x02


#define PAGE_READ_SIZE              250 // may increase upto 255
#define MANF_ID_RESPONSE_LEN         3
#define JDEC_ID_RESPONSE_LEN         3
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


