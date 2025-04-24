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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ql_api_osi.h"
#include "ql_api_spi.h"
#include "ql_log.h"
#include "spi_demo.h"
#include "ql_gpio.h"
#include "ql_power.h"


#define QL_SPI_DEMO_LOG_LEVEL       	        QL_LOG_LEVEL_INFO
#define QL_SPI_DEMO_LOG(msg, ...)			    QL_LOG(QL_SPI_DEMO_LOG_LEVEL, "ql_SPI_DEMO", msg, ##__VA_ARGS__)
#define QL_SPI_DEMO_LOG_PUSH(msg, ...)	        QL_LOG_PUSH("ql_SPI_DEMO", msg, ##__VA_ARGS__)



#define QL_SPI_16BIT_DMA                   0        //16bit DMA demo
#define QL_SPI_DEMO_LOW_POWER_USE          0        //0-not run in lower power mode；1-run in lower power mode

ql_task_t spi_demo_task = NULL;
ql_sem_t  spi_demo_write;
ql_sem_t  spi_demo_read;
int spi_power_lock = 0;


#define QL_SPI_DEMO_WAIT_NONE              0
#define QL_SPI_DEMO_WAIT_WRITE             1
#define QL_SPI_DEMO_WAIT_READ              2

unsigned char spi_demo_wait_write_read = QL_SPI_DEMO_WAIT_NONE;


#define QL_CUR_SPI_PORT             QL_SPI_PORT2
#define QL_CUR_SPI_CS_PIN           QL_CUR_SPI2_CS_PIN
#define QL_CUR_SPI_CS_FUNC          QL_CUR_SPI2_CS_FUNC
#define QL_CUR_SPI_CLK_PIN          QL_CUR_SPI2_CLK_PIN
#define QL_CUR_SPI_CLK_FUNC         QL_CUR_SPI2_CLK_FUNC
#define QL_CUR_SPI_DO_PIN           QL_CUR_SPI2_DO_PIN
#define QL_CUR_SPI_DO_FUNC          QL_CUR_SPI2_DO_FUNC
#define QL_CUR_SPI_DI_PIN           QL_CUR_SPI2_DI_PIN
#define QL_CUR_SPI_DI_FUNC          QL_CUR_SPI2_DI_FUNC



#define QL_TYPE_SHIFT_8             8



unsigned char ReadAddress[4]= {READ_DATA_ID,0x10,0x00,0x00}; // 0x10000

char rcv_buf[]  = " 050.00,018.00,086.00,2.0,10,050.00,050.00,20.00,230.00,45.00,550.68,50.00,650.00,035.00,546553.50,123.11,123456.00,12345678.00,456.12,55.89,789.32,911.00,85.69,14369.00,231129213,2025-02-04 13:12:35,25,278730,901943,18,1,10,160424,148 ";



/*********************************ENABLE WRITE****************************************** */

/*
   this function is used to enable writing to the flash memory by sending the WRITE_ENABLE_ID command.
   The function takes no parameters and returns a ql_errcode_spi_e type indicating the success or failure of the operation.
   This function should be called evry time during writing to the flash memory and Also During Erasing ok flash ic
    
   */
ql_errcode_spi_e WriteEnableFlashSPI(void)
{
    ql_errcode_spi_e ret ;
    unsigned char *WriteEnable;
    unsigned char writeEnableId = WRITE_ENABLE_ID;
    WriteEnable = &writeEnableId;
    
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ret = ql_spi_write(QL_CUR_SPI_PORT, WriteEnable, SINGLE_WRITE);
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ql_rtos_task_sleep_s(1);
    return ret;
}



/*********************************ERASE FLASH****************************************** */

//void eraseBlock(unsigned int DayIndex)
/**
 * @brief Erases a 64KB block of flash memory starting at the specified day index.
 *
 * This function sends the necessary SPI commands to erase a 64KB block of flash memory.
 * The starting address of the block is determined by the provided day index.
 *
 * @param DayIndex The index representing the starting day for the block to be erased.
 *                 This index is used to calculate the starting address of the block.
 * @return ql_errcode_spi_e Returns the result of the SPI write operation.
 *                          QL_SPI_SUCCESS indicates success, while other values indicate failure.
 */


ql_errcode_spi_e eraseBlock(unsigned char DayIndex)
{
    ql_errcode_spi_e ret ;
    unsigned char HighByte = (unsigned char)(((DAY_START_ADDRESS(DayIndex)) >> 16)) ; // type cast the value to unisgne char
    unsigned char MidByte  = (unsigned char)((DAY_START_ADDRESS(DayIndex) >> 8) & 0xFF);
    unsigned char lowByte  = (unsigned char)((DAY_START_ADDRESS(DayIndex) & (0xFF)));
    unsigned char EraseBlockid[] = {SPI_64KB_BLOCK_ERASE_ID,HighByte,MidByte,lowByte};
    
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ret = ql_spi_write(QL_CUR_SPI_PORT, EraseBlockid,sizeof(EraseBlockid));
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ql_rtos_task_sleep_s(5);
    return ret;
}

//Erases Whole chip

ql_errcode_spi_e eraseChip(void)
{
    ql_errcode_spi_e ret ;
    unsigned char *EraseChip;
    unsigned char EraseChipID = SPI_CHIP_ERASE_ID;
    EraseChip = &EraseChipID;
        
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ret = ql_spi_write(QL_CUR_SPI_PORT, EraseChip,SINGLE_WRITE);
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ql_rtos_task_sleep_s(5);
    return ret;
}


 //*********************************RESET FLASH****************************************** */

/*
     Used to terminate any ongoing instruction on the flash IC and reset it.
     This function is typically called during flash initialization or when starting a new session.
 */

ql_errcode_spi_e ResetFlashSPI(void)
{
    ql_errcode_spi_e ret ;
    unsigned char ResetSPI[2] = {ENABLE_RESET_ID, RESET_ID};
    
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ret = ql_spi_write(QL_CUR_SPI_PORT, ResetSPI, sizeof(ResetSPI));
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ql_rtos_task_sleep_s(1);
    return ret;
}

/*********************FLASH PAGE WRITE***************************************** */
/**
 * @brief Writes a page of data to the flash memory.
 *
 * This function sends the PAGE_WRITE_ID command followed by the address and data to be written.
 * The data is copied into the WritePage buffer, which is then sent to the flash memory.
 *
 * @param DayIndex The index representing the starting day for the page to be written.
 *                 This index is used to calculate the starting address of the page.
 * @param data The data to be written to the flash memory.
 * @return ql_errcode_spi_e Returns the result of the SPI write operation.
 *                          QL_SPI_SUCCESS indicates success, while other values indicate failure.
 */



ql_errcode_spi_e WritePage(unsigned char DayIndex,char *data)
{
    ql_errcode_spi_e ret;
    unsigned char WritePage[250] = {0};
   
    //unsigned char HighByteWr = (unsigned char)(((DAY_START_ADDRESS(DayIndex)) >> 16)); // type cast the value to unsigned char
    unsigned char MidByteWr = (unsigned char)((DAY_START_ADDRESS(DayIndex) >> 8) & 0xFF);
    unsigned char lowByteWr = (unsigned char)((DAY_START_ADDRESS(DayIndex) & (0xFF)));
    WritePage[0] = PAGE_WRITE_ID;
    WritePage[1] =  0x10;//HighByteWr;
    WritePage[2] = MidByteWr;
    WritePage[3] = lowByteWr;
    memcpy(&WritePage[4], data,240);
    QL_SPI_DEMO_LOG("write page data is :%s", WritePage);
    for (int i = 0; i < sizeof(WritePage); i++)
    {
        QL_SPI_DEMO_LOG("write page data is :%d", WritePage[i]);
    }
 
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ret = ql_spi_write(QL_CUR_SPI_PORT, WritePage, sizeof(WritePage));
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ql_rtos_task_sleep_s(5);
    return ret;
}




//**************************Reading JEDEC register************************************************ */
/**
 * @brief Reads the JEDEC register from the flash memory.
 *
 * This function sends the READ_ID command to the flash memory and reads the JEDEC ID.
 * It checks if the received data is valid and logs the results.
 *
 * @param data The command to send to the flash memory.
 * @param len The length of the command.
 * @return 0 if the JEDEC ID is valid, 1 otherwise.
 * used to check if Connection to Flash is Healthy or not.
 */
unsigned char readJEDECregister(unsigned char *data, unsigned char len)
{
    unsigned char ret = 1;
    ql_spi_cs_low(QL_CUR_SPI_PORT);

    memset(JdecData, 0, sizeof(JdecData));
    ql_spi_write_read(QL_CUR_SPI_PORT, JdecData, data,len);
    QL_SPI_DEMO_LOG("The indata[0] is:%d",JdecData[0]);
    QL_SPI_DEMO_LOG("The indata[1] is:%d",JdecData[1]);
    QL_SPI_DEMO_LOG("The indata[2] is:%d",JdecData[2]);
    QL_SPI_DEMO_LOG("The indata[3] is:%d",JdecData[3]);
    ql_spi_cs_high(QL_CUR_SPI_PORT);
    ret = JdecData[1] == 0xEF ? 0 : 1 ;
    QL_SPI_DEMO_LOG("jedec valid data return is JdecData[%d]:%d",ret,JdecData[1]);
    return ret;
 }





 //*********************************READ FLASH****************************************** */

/*
    * @brief Reads a page of data from the flash memory.
    *
    * This function sends the READ_DATA_ID command followed by the address to read from.
    * It then reads the data into the ReadData buffer and logs the results.
    *
    * @param address The address to read from.
    * @param len The length of data to read.
    
*/

void readPage(unsigned char *address, unsigned char len)
{
    ql_spi_cs_low(QL_CUR_SPI_PORT);
    ql_spi_write(QL_CUR_SPI_PORT, address, len);
    memset(ReadData, 0, sizeof(ReadData));
    ql_spi_read(QL_CUR_SPI_PORT, ReadData, (sizeof(ReadData) -12));
    ql_spi_cs_high(QL_CUR_SPI_PORT);

    for(int i = 0; i < sizeof(ReadData); i++)
    {
        QL_SPI_DEMO_LOG("read page data is ReadData[%d] :%d",i, ReadData[i]);
    }
   
}
//************************************************************************** */


static void ql_spi_demo_task_pthread(void *ctx)
{
    QlOSStatus err = 0;
    Jdecid = JDEC_ID; 
    outdata = &Jdecid;
    ql_errcode_gpio ret;
    ql_errcode_spi_e ret_spi;
    ql_spi_clk_e spiclk;
    ql_spi_transfer_mode_e transmode;
    unsigned int framesize;
    unsigned char ValidJDECReceived = 1;

    if (QL_CUR_SPI_CS_PIN == QUEC_PIN_NONE || QL_CUR_SPI_DO_PIN == QUEC_PIN_NONE || QL_CUR_SPI_DI_PIN == QUEC_PIN_NONE) {
        QL_SPI_DEMO_LOG("pin err");
        goto QL_SPI_EXIT;
    }

    ret = ql_pin_set_func(QL_CUR_SPI_CS_PIN, QL_CUR_SPI_CS_FUNC);
    if (ret != QL_GPIO_SUCCESS) {
        QL_SPI_DEMO_LOG("set pin err");
        goto QL_SPI_EXIT;
    }
    ret = ql_pin_set_func(QL_CUR_SPI_CLK_PIN, QL_CUR_SPI_CLK_FUNC);
    if (ret != QL_GPIO_SUCCESS) {
        QL_SPI_DEMO_LOG("set pin err");
        goto QL_SPI_EXIT;
    }
    ret = ql_pin_set_func(QL_CUR_SPI_DO_PIN, QL_CUR_SPI_DO_FUNC);
    if (ret != QL_GPIO_SUCCESS) {
        QL_SPI_DEMO_LOG("set pin err");
        goto QL_SPI_EXIT;
    }
    ret = ql_pin_set_func(QL_CUR_SPI_DI_PIN, QL_CUR_SPI_DI_FUNC);
    if (ret != QL_GPIO_SUCCESS) {
        QL_SPI_DEMO_LOG("set pin err");
        goto QL_SPI_EXIT;
    }

    // Configure SPI for polling mode
    ql_spi_config_s spi_config;
    spi_config.input_mode = QL_SPI_INPUT_TRUE;
    spi_config.port = QL_CUR_SPI_PORT;
    framesize = 8;
    transmode = QL_SPI_DIRECT_POLLING; // Polling mode
    spiclk = QL_SPI_CLK_1_5625MHZ;
    spi_config.spiclk = spiclk;
    spi_config.framesize = framesize;
    spi_config.cs_polarity0 = QL_SPI_CS_ACTIVE_LOW;
    spi_config.cs_polarity1 = QL_SPI_CS_ACTIVE_LOW;
    spi_config.cpol = QL_SPI_CPOL_LOW;
    spi_config.cpha = QL_SPI_CPHA_1Edge;
    spi_config.input_sel = QL_SPI_DI_1;
    spi_config.transmode = transmode;
    spi_config.cs = QL_SPI_CS0;
    spi_config.clk_delay = QL_SPI_CLK_DELAY_0;
     ret_spi = ql_spi_init_ext(spi_config);


    if(ret_spi == QL_SPI_SUCCESS) 
    {
        QL_SPI_DEMO_LOG("spi init Sucess");
    }

       ResetFlashSPI();// use retursn in actual code
       WriteEnableFlashSPI(); // use return in actual code
       eraseChip();  // use return in actual code, rpc controlled to reset and erase entire chip
       //eraseBlock(1); // use return in actual code,keep it below 255 for  128 Mbit
      // WritePage(1,rcv_buf);
       ql_rtos_task_sleep_s(2);
     
   
    while (1) 
    {
        ValidJDECReceived =  readJEDECregister(outdata,JDEC_ID_RESPONSE_LEN);
        ql_rtos_task_sleep_s(3);
        if (ValidJDECReceived == 0) 
        {
            QL_SPI_DEMO_LOG("SPI ic is connected, JDEC ID is 0xEF");
           
        } else {
            QL_SPI_DEMO_LOG("please check the SPI ic connection, JDEC ID is not 0xEF");
        }
        ql_rtos_task_sleep_s(3);
        readPage(ReadAddress,4);   // reads the flash page which has written dat present
        ql_rtos_task_sleep_s(3);
    }

QL_SPI_EXIT:
    ql_spi_release(QL_CUR_SPI_PORT);
  
    QL_SPI_DEMO_LOG("ql_rtos_task_delete");
    err = ql_rtos_task_delete(NULL);
    if (err != QL_OSI_SUCCESS) {
        QL_SPI_DEMO_LOG("task deleted failed");
    }
}





QlOSStatus ql_spi_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;
#if QL_SPI_DEMO_LOW_POWER_USE
    spi_power_lock = ql_lpm_wakelock_create("spi_irq", strlen("spi_irq"));
#endif
	err = ql_rtos_task_create(&spi_demo_task, SPI_DEMO_TASK_STACK_SIZE, SPI_DEMO_TASK_PRIO, "ql_spi_demo", ql_spi_demo_task_pthread, NULL, SPI_DEMO_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
	{
		QL_SPI_DEMO_LOG("demo_task created failed");
        return err;
	}
    
    return err;
}
