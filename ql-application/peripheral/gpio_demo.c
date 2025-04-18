  /*=================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_pin_cfg.h"
#include "gpio_demo.h"
#include "ql_adc.h"
//#include "gpio_int_demo.h"
//#include "ql_sdmmc.h"
#include "ql_api_dev.h"
#include "ql_api_nw.h"
/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_GPIODEMO_LOG_LEVEL             QL_LOG_LEVEL_INFO
#define QL_GPIODEMO_LOG(msg, ...)         QL_LOG(QL_GPIODEMO_LOG_LEVEL, "ql_GPIODEMO", msg, ##__VA_ARGS__)
#define QL_GPIODEMO_LOG_PUSH(msg, ...)    QL_LOG_PUSH("ql_GPIODEMO", msg, ##__VA_ARGS__)

/*===========================================================================
 * Variate
 ===========================================================================*/

//SD Card pin init

// #define QL_SDMMC_PIN_DET            QUEC_PIN_DNAME_GPIO_15
// #define QL_PIN_SDMMC_CMD            QUEC_PIN_DNAME_SDMMC1_CMD
// #define QL_PIN_SDMMC_DATA_0         QUEC_PIN_DNAME_SDMMC1_DATA_0
// #define QL_PIN_SDMMC_DATA_1         QUEC_PIN_DNAME_SDMMC1_DATA_1
// #define QL_PIN_SDMMC_DATA_2         QUEC_PIN_DNAME_SDMMC1_DATA_2
// #define QL_PIN_SDMMC_DATA_3         QUEC_PIN_DNAME_SDMMC1_DATA_3
// #define QL_PIN_SDMMC_CLK            QUEC_PIN_DNAME_SDMMC1_CLK

// #define QL_PIN_SDMMC_MODE_FUNC      0
// #define QL_PIN_SDMMC_MODE_FUNC_GPIO 0
// #define QL_FM_FAT32                     0x02
// #define QL_SDMMC_CLK_FREQ               25000000
// #define QL_SDMMC_BLOCK_NUM              10

//ADC pin init
bool AdcCheck =FALSE;
uint8_t sim_ids = 0;
unsigned char csq=0;
char imei_num[20] = {'\0'};
int adc_value1 = 0;
int adc_value2 = 0;
int adc_value3 = 0;
int gpioFunction = 1;
/*===========================================================================
 * Functions
 ===========================================================================*/

 void signal_quality()
 {
         ql_nw_get_csq(sim_ids, &csq);
         QL_GPIODEMO_LOG("SIGNAL STRENGTH Is :%d\r\n", csq);	
 }

 void getIMEInumber(){
     
     ql_dev_get_imei(imei_num, sizeof(imei_num), 0);
     QL_GPIODEMO_LOG("IMEI: %s\r\n", imei_num);
 }

 /*===========================================================================
 * Functions for SD card init and Detect
 ===========================================================================*/
//  void ql_sdmmc_pin_init(void)
//  {
//      ql_pin_set_func(QL_PIN_SDMMC_CMD    , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//      ql_pin_set_func(QL_PIN_SDMMC_DATA_0 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//      ql_pin_set_func(QL_PIN_SDMMC_DATA_1 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//      ql_pin_set_func(QL_PIN_SDMMC_DATA_2 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//      ql_pin_set_func(QL_PIN_SDMMC_DATA_3 , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//      ql_pin_set_func(QL_PIN_SDMMC_CLK    , QL_PIN_SDMMC_MODE_FUNC);     //Pin reuse
//  }
 

//  ql_errcode_sdmmc_e ql_sdmmc_mount_demo(void)
// {
// 	if(QL_SDMMC_SUCCESS != ql_sdmmc_mount())
// 	{
// 		//QL_GPIODEMO_LOG("Mount failed");
// 		return QL_SDMMC_MOUNT_ERR;
// 	}
// 	else 
// 	{
// 		//QL_GPIODEMO_LOG("Mount succeed");
// 	}
// 	//ql_rtos_task_sleep_s(3);
// 	return QL_SDMMC_SUCCESS;
// }


 /*===========================================================================
 * Functions for ADC init and Detect
 ===========================================================================*/
 void adc_demo_test(  void)
{
	// int adc_value1 = 0;
    // int adc_value2 = 0;
    // int adc_value3 = 0;

    ql_rtos_task_sleep_ms(1500);

    //obtain raw data in order to improve the speed, customers need to optimize the accuracy themselves
    ql_adc_get_volt_raw(QL_ADC0_CHANNEL, QL_ADC_SCALE_MAX, &adc_value1);
    QL_GPIODEMO_LOG("RAW ADC[0]: adc_value=[%d]", adc_value1 );
	ql_rtos_task_sleep_ms(1500);
    ql_adc_get_volt_raw(QL_ADC1_CHANNEL, QL_ADC_SCALE_MAX, &adc_value2);
    QL_GPIODEMO_LOG("RAW ADC[1]: adc_value=[%d]", adc_value2 );
	ql_rtos_task_sleep_ms(1500);
    ql_adc_get_volt_raw(QL_ADC2_CHANNEL, QL_ADC_SCALE_MAX, &adc_value3);
    QL_GPIODEMO_LOG("RAW ADC[2]: adc_value=[%d]", adc_value3 );
    if((adc_value1 >30) && (adc_value1 > 30) && (adc_value3 > 30))
    {
        AdcCheck = TRUE;
        QL_GPIODEMO_LOG("ALL ADC is OK");

    }
    else
    {
        AdcCheck = FALSE;
        QL_GPIODEMO_LOG("ADC0 is NG");
    }
}

 /*===========================================================================
 * Functions for GPIO init and Detect
 ===========================================================================*/

void _ql_gpio_demo_init( void)
{
   ql_pin_set_func(GPIO_29 ,0);
   ql_gpio_init(GPIO_29,GPIO_OUTPUT,PULL_NONE,LVL_LOW);
//     ql_pin_set_func(GPIO_30 ,0);
//    ql_gpio_init(GPIO_30,GPIO_OUTPUT,PULL_NONE,LVL_LOW);
   ql_pin_set_func(GPIO_8,0);
   ql_gpio_init(GPIO_8,GPIO_OUTPUT,PULL_NONE,LVL_LOW); 
//    ql_pin_set_func(QL_TEST1_PIN_GPIO2,0);
//    ql_gpio_init(GPIO_3,GPIO_OUTPUT,PULL_NONE,LVL_LOW); 
}

 /*===========================================================================
 * Functions for Main Application
 ===========================================================================*/

static void ql_gpio_demo_thread(void *param)
{
    QL_GPIODEMO_LOG("gpio demo thread enter, param 0x%x", param);

    ql_event_t event;
    ql_errcode_gpio Gpo_check;
   // ql_errcode_sdmmc_e Sd_check;
    _ql_gpio_demo_init();
    //ql_sdmmc_pin_init();
    getIMEInumber();
     while(1)
    {
        signal_quality();
        ql_event_wait(&event, 1);
        Gpo_check = ql_gpio_set_level(GPIO_8 ,LVL_LOW);  
        if(uartCheck ==0 )
        {
            // ql_gpio_set_level(GPIO_3 ,LVL_HIGH);
            // QL_GPIODEMO_LOG("led 3 ON");
            // ql_rtos_task_sleep_ms(500);
            // ql_gpio_set_level(GPIO_3 ,LVL_LOW);
            QL_GPIODEMO_LOG("led 3 OFF");
            QL_GPIODEMO_LOG("Dig out result is :%d",Gpo_check);    
           ql_rtos_task_sleep_ms(5000);
              
        } 

        else
        {
           // ql_gpio_set_level(GPIO_3 ,LVL_HIGH);
            QL_GPIODEMO_LOG("led 3 ON");
            QL_GPIODEMO_LOG("uart read and write is sucess :%d",uartCheck);
        }
        
        

         // check ADC i/O 's
        if(AdcCheck == FALSE)
        {
            adc_demo_test();
        }

         //Check interrupt pin signals   
        //QL_GPIODEMO_LOG("value of interrupt flag 1 is :%d,the value of intrpt flag 2 is :%d",intrpt_flag1,intrpt_flag2);
        if((AdcCheck ==1)&&(Gpo_check == QL_GPIO_SUCCESS))
        {
            QL_GPIODEMO_LOG("All I/O check pass");
            ql_gpio_set_level(GPIO_29 ,LVL_HIGH);
            QL_GPIODEMO_LOG("led 29 ON");
            gpioFunction =0;

        }
        else
        {
            QL_GPIODEMO_LOG("Wait for I/O check to clear");
            //ql_gpio_set_level(GPIO_29 ,LVL_HIGH);
            gpioFunction =1;
        }

        // Sd_check = ql_sdmmc_mount_demo();
        // if(Sd_check == QL_SDMMC_SUCCESS)
        // {
        //     QL_GPIODEMO_LOG("SD card mount success");
        //     ql_gpio_set_level(GPIO_8 ,LVL_HIGH);
        //     QL_GPIODEMO_LOG("led 8 ON");
        // }
        // else
        // {
        //     Sd_check = 1;
        //     QL_GPIODEMO_LOG("SD card mount failed");
        // }

        // QL_GPIODEMO_LOG("no of sattelite count is :%d",satteliteCount);
        // ql_rtos_task_sleep_ms(2000);
        // sprintf(PayloadBuffer,"{\"VER\":\"1\",\"IMEI\":%s,\"GSM\":%d,\"GPS\":%d,\"ADC1\":%d,\"ADC2\":%d,\"ADC3\":%d,\"IOStatus\":%d,\"UARTStatus\":%d}",imei_num,csq,satteliteCount,adc_value1,adc_value2,adc_value3,gpioFunction,uartCheck);     
        // QL_GPIODEMO_LOG("PayloadBuffer is :%s",PayloadBuffer);
        ql_rtos_task_sleep_ms(100);
    }

    ql_rtos_task_delete(NULL);
}

void ql_gpio_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;
    ql_task_t gpio_task = NULL;

    err = ql_rtos_task_create(&gpio_task, 1024, APP_PRIORITY_BELOW_NORMAL, "ql_gpiodemo", ql_gpio_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_GPIODEMO_LOG("gpio demo task created failed");
    }
}

