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

#define MANF_ID                     0x90
#define JDEC_ID                     0X9F
#define ENABLE_RESET_ID             0x66
#define RESET_ID                    0x99
#define READ_4BYTE_ID               0x13

#define MANF_ID_RESPONSE_LEN         4
#define JDEC_ID_RESPONSE_LEN         4
#define RESET_LEN                    2 

unsigned char Jdecid ;
unsigned char *outdata;
unsigned char ReadAddress[2];
unsigned char ReadData[5];
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


