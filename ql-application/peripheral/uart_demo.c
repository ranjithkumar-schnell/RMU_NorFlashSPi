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
#include <stdlib.h>
#include <string.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_uart.h"
#include "ql_gpio.h"
#include "ql_pin_cfg.h"
#include "gpio_demo.h"

/*===========================================================================
 *Definition
 ===========================================================================*/
#define QL_UART_DEMO_LOG_LEVEL			QL_LOG_LEVEL_INFO
#define QL_UART_DEMO_LOG(msg, ...)		QL_LOG(QL_UART_DEMO_LOG_LEVEL, "ql_uart_demo", msg, ##__VA_ARGS__)


#define QL_UART_TASK_STACK_SIZE     		4096
#define QL_UART_TASK_PRIO          	 	    APP_PRIORITY_NORMAL
#define QL_UART_TASK_EVENT_CNT      		5


#define QL_UART_RX_BUFF_SIZE                2048
#define QL_UART_TX_BUFF_SIZE                2048

#define MIN(a,b) ((a) < (b) ? (a) : (b))


/*===========================================================================
 * Variate
 ===========================================================================*/
 char Usart_rcvbf_collect_flag=0;   
 char rcv_data_buf[250]={'\0'};
 unsigned char tot_rcvd=0;
 int uartCheck = 0;
/*===========================================================================
 * Functions
 ===========================================================================*/

void ql_uart_notify_cb(unsigned int ind_type, ql_uart_port_number_e port, unsigned int size)
{
    unsigned char *recv_buff = calloc(1, QL_UART_RX_BUFF_SIZE+1);
    unsigned int real_size = 0;
    int read_len = 0;
    //int lenght_of_rcvBUffer;
    QL_UART_DEMO_LOG("UART port %d receive ind type:0x%x, receive data size:%d", port, ind_type, size);
    switch(ind_type)
    {
        case QUEC_UART_RX_OVERFLOW_IND:  //rx buffer overflow
        case QUEC_UART_RX_RECV_DATA_IND:
        {
            memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE+1);
			real_size= MIN(size, QL_UART_RX_BUFF_SIZE);
			read_len = ql_uart_read(port, recv_buff, real_size);                   // reads only 64 bytes of data at a time
			 QL_UART_DEMO_LOG("read_len=%d, recv_data=%s", read_len, recv_buff);
			if((recv_buff[0]=='$')||(Usart_rcvbf_collect_flag==1))                 // "$"" is entry point for data head deliminator
			{
                if(Usart_rcvbf_collect_flag==0)	                                // clears buffer for next new cycle	of data	
				{	
					memset(rcv_data_buf, 0, sizeof(rcv_data_buf));
					
                }
				strncat((char *)rcv_data_buf,(const char *)recv_buff,(read_len)); // 64 bytes of data added to collect total 180 bytes
				Usart_rcvbf_collect_flag=1;
				tot_rcvd=tot_rcvd+read_len;
				QL_UART_DEMO_LOG("rcv_data_buf data=%s and tot_rcvd:%d\n",rcv_data_buf,tot_rcvd);
				if(recv_buff[0]=='#')                                             // indicates data end deliminator
				{
					Usart_rcvbf_collect_flag=0;
					tot_rcvd=0;
					QL_UART_DEMO_LOG("rcv_data_buf inside data=%s\n",rcv_data_buf);
					QL_UART_DEMO_LOG(" strlen of(rcv_data_buf) is %d\n",strlen(rcv_data_buf));// gives total data size
					QL_UART_DEMO_LOG(" strlen of(rcv_data_buf) is %d\n",sizeof(rcv_data_buf));// gives total buffer size
                     //lenght_of_rcvBUffer = strlen(rcv_data_buf);
					if(strlen(rcv_data_buf)>70)        // received event data and periodid data response
					{
					    QL_UART_DEMO_LOG("Received periodic push on interval data\n");
						uartCheck =1;

					}
					else                   // attributes dat was received 60 bytes of config data
                     {
                        QL_UART_DEMO_LOG("Received Attributes data\n");  

					} 
				}
				
				memset(recv_buff, 0, QL_UART_RX_BUFF_SIZE+1);
			

            }			

            break;
        }
        case QUEC_UART_TX_FIFO_COMPLETE_IND: 
        {
            QL_UART_DEMO_LOG("tx fifo complete");
            break;
        }
    }
    free(recv_buff);
    recv_buff = NULL;
}

static void ql_uart_demo_thread(void *param)
{
    int ret = 0;
	QlOSStatus err = 0;
    ql_uart_config_s uart_cfg = {0};
    int write_len = 0;
    ql_uart_tx_status_e tx_status;
    unsigned char data[] = "$POPI#";

    /***********************************************************
	Note start:
        1.If the BAUD rate is QL UART BAUD_AUTO,a string of 'at'should be sent at least once to identify the baud rate.
        2.Once the baud rate is identified, it cannot be changed unless restarted.
    ************************************************************/
    uart_cfg.baudrate = QL_UART_BAUD_9600;
    uart_cfg.flow_ctrl = QL_FC_NONE;
    uart_cfg.data_bit = QL_UART_DATABIT_8;
    uart_cfg.stop_bit = QL_UART_STOP_1;
    uart_cfg.parity_bit = QL_UART_PARITY_NONE;

    ret = ql_uart_set_dcbconfig(QL_UART_PORT_1, &uart_cfg);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);
	if(QL_UART_SUCCESS != ret)
	{
		goto exit;
	}
	
	/***********************************************************
	Note start:
		1. If QL_UART_PORT_1 is selected for use, there is no need to set TX and RX pin and function
		2. According to the QuecOpen GPIO table, user should select the correct PIN to set function
		3. CTS and RTS pins (UART2 and UART3) also need to be initialized if hardware flow control function is required
	************************************************************/

	// /*Note end*/
	
    ret = ql_uart_open(QL_UART_PORT_1);
    QL_UART_DEMO_LOG("ret: 0x%x", ret);

	if(QL_UART_SUCCESS == ret)
	{
	    ret = ql_uart_register_cb(QL_UART_PORT_1, ql_uart_notify_cb);
	    QL_UART_DEMO_LOG("ret: 0x%x", ret);

	    memset(&uart_cfg, 0, sizeof(ql_uart_config_s));
	    ret = ql_uart_get_dcbconfig(QL_UART_PORT_1, &uart_cfg);
	    QL_UART_DEMO_LOG("ret: 0x%x, baudrate=%d, flow_ctrl=%d, data_bit=%d, stop_bit=%d, parity_bit=%d", 
	                        ret, uart_cfg.baudrate, uart_cfg.flow_ctrl, uart_cfg.data_bit, uart_cfg.stop_bit, uart_cfg.parity_bit);
	    
	    while(1)
	    {
			ql_rtos_task_sleep_s(5);
			if(uartCheck == 0)
			{
				write_len = ql_uart_write(QL_UART_PORT_1, data, strlen((char *)data));
				QL_UART_DEMO_LOG("write_len:%d", write_len);
				ql_uart_get_tx_fifo_status(QL_UART_PORT_1, &tx_status);
				QL_UART_DEMO_LOG("tx_status:%d", tx_status);
				ql_rtos_task_sleep_s(30);				
			}
    }
	}

exit:
    err = ql_rtos_task_delete(NULL);
	if(err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("task deleted failed");
	}
}

void ql_uart_app_init(void)
{
	QlOSStatus err = 0;
	ql_task_t uart_task = NULL;
	
	err = ql_rtos_task_create(&uart_task, QL_UART_TASK_STACK_SIZE, QL_UART_TASK_PRIO, "QUARTDEMO", ql_uart_demo_thread, NULL, QL_UART_TASK_EVENT_CNT);
	if (err != QL_OSI_SUCCESS)
	{
		QL_UART_DEMO_LOG("demo task created failed");
        return;
	}
}


