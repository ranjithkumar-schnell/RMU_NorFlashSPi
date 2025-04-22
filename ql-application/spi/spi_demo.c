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

// #define QL_CUR_SPI_PORT             QL_SPI_PORT1
// #define QL_CUR_SPI_CS_PIN           QL_CUR_SPI1_CS_PIN
// #define QL_CUR_SPI_CS_FUNC          QL_CUR_SPI1_CS_FUNC
// #define QL_CUR_SPI_CLK_PIN          QL_CUR_SPI1_CLK_PIN
// #define QL_CUR_SPI_CLK_FUNC         QL_CUR_SPI1_CLK_FUNC
// #define QL_CUR_SPI_DO_PIN           QL_CUR_SPI1_DO_PIN
// #define QL_CUR_SPI_DO_FUNC          QL_CUR_SPI1_DO_FUNC
// #define QL_CUR_SPI_DI_PIN           QL_CUR_SPI1_DI_PIN
// #define QL_CUR_SPI_DI_FUNC          QL_CUR_SPI1_DI_FUNC

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


unsigned char ResetSP[2] = {ENABLE_RESET_ID, RESET_ID};
unsigned char ReadAddress[2]= {READ_DATA_ID, 0x56};



void readJEDECregister(unsigned char *data, unsigned char len)
{
    ql_spi_cs_low(QL_CUR_SPI_PORT);

    // Write data using polling
   // ql_spi_write(QL_CUR_SPI_PORT, outdata, outlen);
   memset(JdecData, 0, sizeof(JdecData));
   ql_spi_write_read(QL_CUR_SPI_PORT, JdecData, data,len);
   QL_SPI_DEMO_LOG("The indata[0] is:%d",JdecData[0]);
    QL_SPI_DEMO_LOG("The indata[1] is:%d",JdecData[1]);
    QL_SPI_DEMO_LOG("The indata[2] is:%d",JdecData[2]);
    QL_SPI_DEMO_LOG("The indata[3] is:%d",JdecData[3]);


    // Read data using polling
    //ql_spi_read(QL_CUR_SPI_PORT, indata, inlen);

    ql_spi_cs_high(QL_CUR_SPI_PORT);


}

void readPage(unsigned char *data, unsigned char len)
{
    ql_spi_cs_low(QL_CUR_SPI_PORT);

    // Write data using polling
   ql_spi_write(QL_CUR_SPI_PORT, data, len);
   memset(ReadData, 0, sizeof(ReadData));
  ql_spi_read(QL_CUR_SPI_PORT, ReadData, sizeof(ReadData));
//ql_spi_write_read(QL_CUR_SPI_PORT, ReadData, data, len);
   QL_SPI_DEMO_LOG("The ReadData[0] is:%d",ReadData[0]);
    QL_SPI_DEMO_LOG("The ReadData[1] is:%d",ReadData[1]);
    QL_SPI_DEMO_LOG("The ReadData[2] is:%d",ReadData[2]);
    QL_SPI_DEMO_LOG("The ReadData[3] is:%d",ReadData[3]);


    // Read data using polling
    //ql_spi_read(QL_CUR_SPI_PORT, indata, inlen);

    ql_spi_cs_high(QL_CUR_SPI_PORT);


}

static void ql_spi_demo_task_pthread(void *ctx)
{
    QlOSStatus err = 0;
    Jdecid = JDEC_ID; 
    outdata = &Jdecid;
     ql_errcode_gpio ret;
    ql_spi_clk_e spiclk;
    ql_spi_transfer_mode_e transmode;
    unsigned int framesize;

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
    ql_spi_init_ext(spi_config);

     //ql_spi_flash_data_printf(outdata, outlen);
     ql_spi_cs_low(QL_CUR_SPI_PORT);
     ql_spi_write(QL_CUR_SPI_PORT, ResetSP, 2);
     ql_spi_cs_high(QL_CUR_SPI_PORT);
     ql_rtos_task_sleep_s(1);
     
   
    while (1) 
    {

        readJEDECregister(outdata,JDEC_ID_RESPONSE_LEN);
        ql_rtos_task_sleep_s(3);
        readPage(ReadAddress,2);
        //read4byteRegister(ReadAddress,MANF_ID_RESPONSE_LEN);
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
