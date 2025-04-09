#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ql_app_feature_config.h"
#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_dev.h"
#include "ql_osi_def.h"
#include "quec_pin_index.h"
#include "ql_gpio.h"
#include "ql_api_sim.h"
#include "ql_api_nw.h"
#include "application.h"
#include "ql_power.h"
#include "ql_api_common.h"

#define QL_DEV_TASK_STACK_SIZE      50000
#define QL_DEV_TASK_PRIO            APP_PRIORITY_NORMAL
#define QL_DEV_TASK_EVENT_CNT       5

#define QL_APP_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_APP_LOG(msg, ...)			QL_LOG(QL_APP_LOG_LEVEL, "ql_my_app_code", msg, ##__VA_ARGS__)
#define QL_APP_LOG_PUSH(msg, ...)		QL_LOG_PUSH("ql_MY_APP_CODE", msg, ##__VA_ARGS__)

#if !defined(application_no_err)
	#define application_no_err(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x != 0)                                                                        				\
				{                                                                                               \
					QL_APP_LOG(str);																			\
					{action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif

#define IMEI 20
#define P_NO 15

///////////////////////Variables////////////////////////////////////////////////////////////////
typedef enum
{
		GET_IMEI,
		SET_MODEM_FUN,
		GET_MODEM_FUN,
		SIM_STATUS,
		GET_NW_MODE,
		SET_NW_MODE,
		NW_REGISTRATION,
		OPERATOR,
		PHONE_NUMBER
		//NW_MODE,
		//NW_TIME	
}task;
task process;


ql_sim_status_e card_status = 0;
ql_nw_mode_type_e nw_mode = 0;
uint8_t cfun = 0;
uint8_t sim_id = 0;
char imei_no[IMEI]={'\0'},phone_number[P_NO]={'\0'};
void *ind_msg_buf1;
int nw_reg=0;

//ql_nw_reg_status_info_s *reg_info = (ql_nw_reg_status_info_s *)calloc(1, sizeof(ql_nw_reg_status_info_s));
//ql_nw_operator_info_s *oper_info = (ql_nw_operator_info_s *)calloc(1, sizeof(ql_nw_operator_info_s));
//////////////////////////////////////////////////////////////////////////////////////
void modem_funtionality()
{
	if(cfun!=1)
	{
		QL_APP_LOG("MODEM FUN NOT SET GOTO SET MODEM FUN\r\n");
		process=SET_MODEM_FUN;
	}
	else
	{
		QL_APP_LOG("MODEM FUNTIONALITY IS: %d\r\n", cfun);
		process=SIM_STATUS;
	}
}
void sim_status()
{
	if(QL_SIM_STATUS_NOSIM==card_status)
	{
		QL_APP_LOG("SIM CARD NOT INSERTED\r\n");
		ql_power_reset(RESET_NORMAL);
	}
	else if(QL_SIM_STATUS_READY==card_status)
	{
		QL_APP_LOG("SIM CARD INSERTED\r\n");
	}
}
void nw_mode_chack()
{
	if(nw_mode!=0)
	{
		QL_APP_LOG("SET NETWORK MODE TO AUTO\r\n");
		process=SET_NW_MODE;
	}
	else 
	{
		QL_APP_LOG("AUTO NETWORK MODE\r\n");
		process=NW_REGISTRATION;
	}
}
void ql_nw_notify_cb(uint8_t sim_id, unsigned int ind_type, void *ind_msg_buf)
{    
    switch(ind_type)
    {
        case QUEC_NW_DATA_REG_STATUS_IND:
        {
             ql_nw_common_reg_status_info_s  *data_reg_status=(ql_nw_common_reg_status_info_s  *)ind_msg_buf;
             QL_APP_LOG("Sim%d data: state:%d; lac:0x%X; cid:0x%X; act:%d",
			 			sim_id,
                        data_reg_status->state,
                        data_reg_status->lac,
                        data_reg_status->cid,
                        data_reg_status->act);
						nw_reg=data_reg_status->state;
              break;
        }
	}
}
void network_registration_check()
{
	if(ql_nw_register_cb(ql_nw_notify_cb) != QL_NW_SUCCESS)
    {
		QL_APP_LOG("register network callback fail");
		
	}
	if(nw_reg==0||nw_reg==2||nw_reg==3||nw_reg==4)
	{
		QL_APP_LOG("NETWORK REGISTRATION FAIL\r\n");
		ql_power_reset(RESET_NORMAL);
	}
	else QL_APP_LOG("NETWORK REGISTRATION PASS\r\n");
}
void check_operator()
{
	
	ql_nw_operator_info_s *oper_info = (ql_nw_operator_info_s *)calloc(1, sizeof(ql_nw_operator_info_s));
	ql_nw_get_operator_name(sim_id, oper_info);
	QL_APP_LOG("long_oper_name:%s, short_oper_name:%s\r\n",oper_info->long_oper_name, oper_info->short_oper_name);
	process=PHONE_NUMBER;
}
void ql_application_app_thread(void *param)
{
	int ret = 0;
	QL_APP_LOG("application demo thread enter1\r\n");
	process=GET_IMEI;
	while(1)
	{
		QL_APP_LOG("application demo thread enter\r\n");
		ql_rtos_task_sleep_ms(1000);
		switch(process)
		{
			case GET_IMEI:
				ql_dev_get_imei(imei_no, sizeof(imei_no), 0);
				QL_APP_LOG("IMEI: %s\r\n", imei_no);
				process=GET_MODEM_FUN;
				break;
			case GET_MODEM_FUN:
				ql_dev_get_modem_fun(&cfun, 0);
				modem_funtionality();
				break;	
			case SET_MODEM_FUN:
				QL_APP_LOG("SET MODEM FUNCTIONALITY\r\n");
				ql_dev_set_modem_fun(1, 0, 0);
				process=GET_MODEM_FUN;
				break;
			
			case SIM_STATUS:
				ret = ql_sim_get_card_status(sim_id, &card_status);
				QL_APP_LOG("SIM CARD STATUS: %d,ret=%d\r\n", card_status,ret);
				sim_status();
				process=GET_NW_MODE;
				break;
			case GET_NW_MODE:
				ret = ql_nw_get_mode(sim_id, &nw_mode);
				QL_APP_LOG("NW_MODE:%d,ret=%d\r\n", nw_mode,ret);
				nw_mode_chack();
				break;
			case SET_NW_MODE:
				ql_nw_set_mode(sim_id,0);
				QL_APP_LOG("NW_MODE SETTED TO AUTO\r\n");
				process=GET_NW_MODE;
				break;
			case NW_REGISTRATION:
				network_registration_check();
				process=OPERATOR;
				break;
			case OPERATOR:
				check_operator();
				break;
			case PHONE_NUMBER:
				ret = ql_sim_get_phonenumber(sim_id, phone_number, sizeof(phone_number));
				QL_APP_LOG("PHONE NUMBER: %s", phone_number);
				break;
		}
	}
}

void ql_application_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t ql_app_task = NULL;
	
    QL_APP_LOG("application demo enter");
	err = ql_rtos_task_create(&ql_app_task, QL_DEV_TASK_STACK_SIZE, QL_DEV_TASK_PRIO, "ql_application", ql_application_app_thread, NULL, QL_DEV_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
    {
		QL_APP_LOG("application task create failed");
	}

}