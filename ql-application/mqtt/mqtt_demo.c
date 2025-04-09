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
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_mqttclient.h"
#include "ql_api_dev.h"
#include "../peripheral/inc/gpio_demo.h"


#define QL_MQTT_LOG_LEVEL                 QL_LOG_LEVEL_INFO
#define QL_MQTT_LOG(msg, ...)                     QL_LOG(QL_MQTT_LOG_LEVEL, "ql_MQTT", msg, ##__VA_ARGS__)
#define QL_MQTT_LOG_PUSH(msg, ...)          QL_LOG_PUSH("ql_MQTT", msg, ##__VA_ARGS__)
static ql_task_t mqtt_task = NULL;

#define MQTT_CLIENT_IDENTITY        "860057063278401"
#define MQTT_CLIENT_USER            "860057063278401"
#define MQTT_CLIENT_PASS            "860057063278401"



#define MQTT_CLIENT_QUECTEL_URL    "mqtt://prod.schnelliot.in:1883"//"mqtt://cri.schnelliot.in:1883"// "mqtt://prod.schnelliot.in:1883"// "mqtt://schnelliot.in:1883"  //"mqtt://cri.schnelliot.in:1883" //"mqtt://prod.schnelliot.in:1883"


//sprintf(autoProvisionValue, "{ \"deviceName\": %s,\"provisionDeviceKey\": \"aco9u4zlyah71sq8zd7t\",\"provisionDeviceSecret\": \"6r3b4cr8qn196i4vw3a9\",  \"credentialsType\": \"ACCESS_TOKEN\",\"token\": \"%s\"}",imei_no,imei_no);
//char PayloadBuf[50]= "{\"variant\":\"SRMUSP4GGPSXX01A\"}";                      
#define MQTT_PUB_MSG0 "{\"deviceName\":\"860057063288996\",\"provisionDeviceKey\":\"si0u9idwfla3a3p9pu53\",\"provisionDeviceSecret\":\"ccwfg2h7l2wf1gabeq3o\",\"credentialsType\":\"ACCESS_TOKEN\",\"token\":\"860057063288996\"}"
#define MQTT_ATTR_PUB_MSG "{\"variant\":\"SRMUSP4GGPSXX01A\"}"
#define MQTT_PUB_MSG2 "{\"temperatrue\": 19,\"power\": 86}"

static ql_sem_t  mqtt_semp;
static int  mqtt_connected = 0;
mqtt_client_t  mqtt_cli;
struct mqtt_connect_client_info_t  client_info = {0};
char imei_no[20];
uint8_t apn_set=0;
uint8_t sim_id;
char operator_name[15]={'\0'};
char PayloadBuffer[500]= {'\0'};
char AppProvision = 0;
char HardwareTestPub = 0;
/*===========================================================================
 * USER FUNCTION STARTS FROM HERE*
 ===========================================================================*/



 /*To refresh modem when mqtt conection is lost*/

 void airplane_mode() {
    int err;
    
    err=ql_dev_set_modem_fun(0, 0, 0);
     QL_MQTT_LOG("my_log_airplan mode before=%d",err);
    QL_MQTT_LOG("my_log_airplan mode");
     ql_rtos_task_sleep_s(2);
    err=ql_dev_set_modem_fun(1, 0, 0);
    QL_MQTT_LOG("my_log_airplan mode after=%d",err);
  
  }

/*===========================================================================
 * Funcction Sets operatorName and APN*
 ===========================================================================*/
 void check_operator()
 {
     ql_nw_operator_info_s *oper_info = (ql_nw_operator_info_s *)calloc(1, sizeof(ql_nw_operator_info_s));
     ql_nw_get_operator_name(sim_id, oper_info);
     QL_MQTT_LOG("long_oper_name:%s, short_oper_name:%s\r\n",oper_info->long_oper_name, oper_info->short_oper_name);
     strcpy(operator_name,oper_info->long_oper_name);
     QL_MQTT_LOG("THE OPERATOR NAME IS=%s\r\n",operator_name);
     if((strcmp(operator_name,"Airtel")==0)||(strcmp(operator_name,"airtel")==0)||(strcmp(operator_name,"AIRTEL")==0)||(strcmp(operator_name,"AirTel")==0))apn_set=1;
     else if((strcmp(operator_name,"CellOne")==0)||(strcmp(operator_name,"cellone")==0)||(strcmp(operator_name,"CELLONE")==0)||(strcmp(operator_name,"Cellone")==0))apn_set=2;
     else if((strcmp(operator_name,"Vodafone")==0)||(strcmp(operator_name,"vodafone")==0)||(strcmp(operator_name,"VODAFONE")==0)||(strcmp(operator_name,"VI")==0)||(strcmp(operator_name,"Vi")==0)||(strcmp(operator_name,"vi")==0))apn_set=3;
     else if((strcmp(operator_name,"RJIO")==0)||(strcmp(operator_name,"Rjio")==0)||(strcmp(operator_name,"rjio")==0)||(strcmp(operator_name,"JIO")==0)||(strcmp(operator_name,"Jio")==0)||(strcmp(operator_name,"jio")==0))apn_set=4;
     else
     {
      
     
          apn_set=5;
          QL_MQTT_LOG("inside apn_set 5\r\n");
          QL_MQTT_LOG("apn undefined in check operator fn\r\n");	
     
     }
 }
 

/*===========================================================================
 * Sim network registration  Function*
 ===========================================================================*/

 int sim_net_register(int nSim, int profile_idx)
 {
     int ret = 0, i = 0;
     ql_data_call_info_s info;
     char ip4_addr_str[16] = {0};
 
     while((ret = ql_network_register_wait(nSim, 20)) != 0 && i < 5){
         i++;
         ql_rtos_task_sleep_s(1);
     }
     if(ret == 0) {
         i = 0;
         QL_MQTT_LOG("====network registered!!!!====");
     } else {
         QL_MQTT_LOG("====network register failure!!!!!====");
         return ret;
     }
     check_operator();
     ql_set_data_call_asyn_mode(nSim, profile_idx, 0);
     
     if(apn_set==1)
             {
                 QL_MQTT_LOG("apn set as airtel\r\n");
                 ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "airtelgprs.com", NULL, NULL, 0);
             }		
             else if(apn_set==2)
             {
                 QL_MQTT_LOG("apn set as bsnl\r\n");
                 ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "bsnlnet", NULL, NULL, 0); 
             }
             else if(apn_set==3)
             {
                 QL_MQTT_LOG("apn set as vodafone\r\n");
                 ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "www", NULL, NULL, 0); 
             }
             else if(apn_set==4)
             {
                 QL_MQTT_LOG("apn set as jio\r\n");
                 ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "jionet", NULL, NULL, 0); 
             }
 
             else if(apn_set==5)
             {
                   QL_MQTT_LOG("apn undefined\r\n");
                 ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "www", NULL, NULL, 0); 
                 
             }
 
             
     
     if(ret != 0) {
         QL_MQTT_LOG("====data call failure!!!!=====");
     }
     memset(&info, 0x00, sizeof(ql_data_call_info_s));
     
     ret = ql_get_data_call_info(nSim, profile_idx, &info);
     if(ret != 0) {
         QL_MQTT_LOG("ql_get_data_call_info ret: %d", ret);
         ql_stop_data_call(nSim, profile_idx);
         return ret;
     }
         
     QL_MQTT_LOG("info->profile_idx: %d", info.profile_idx);
     QL_MQTT_LOG("info->ip_version: %d", info.ip_version);
                     
     QL_MQTT_LOG("info->v4.state: %d", info.v4.state); 
     inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
     QL_MQTT_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);
         
     inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
     QL_MQTT_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);
         
     inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
     QL_MQTT_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
     return ret;
 }
 

static void mqtt_state_exception_cb(mqtt_client_t *client)
{
          QL_MQTT_LOG("mqtt session abnormal disconnect");
          mqtt_connected = 0;
}

static void mqtt_connect_result_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_e status)
{
          QL_MQTT_LOG("status: %d", status);
          if(status == 0){
                    mqtt_connected = 1;
          }
          ql_rtos_semaphore_release(mqtt_semp);
}

static void mqtt_requst_result_cb(mqtt_client_t *client, void *arg,int err)
{
          QL_MQTT_LOG("err: %d", err);
          
          ql_rtos_semaphore_release(mqtt_semp);
}

static void mqtt_inpub_data_cb(mqtt_client_t *client, void *arg, int pkt_id, const char *topic, const unsigned char *payload, unsigned short payload_len)
{
          QL_MQTT_LOG("topic: %s", topic);
          QL_MQTT_LOG("payload: %s", payload);
          if(AppProvision==0) {
            if(((strstr((char *)payload,"SUCCESS"))||(strstr((char *)payload,"FAILURE"))))
            AppProvision=1;
           }
}

// static void mqtt_disconnect_result_cb(mqtt_client_t *client, void *arg,int err){
//           QL_MQTT_LOG("err: %d", err);
          
//           ql_rtos_semaphore_release(mqtt_semp);
// }

void ChangeClientInfo()
{
    
    ql_mqtt_client_deinit(&mqtt_cli);
    QL_MQTT_LOG("Changing Client info,after Auto provisioning");
    client_info.keep_alive = 60;
    client_info.clean_session = 1;
    client_info.will_qos = 0;
    client_info.will_retain = 0;
    client_info.will_topic = NULL;
    client_info.will_msg = NULL;

    client_info.client_id = imei_no;
    client_info.client_user = imei_no;//imei_no;
    client_info.client_pass = imei_no;//imei_no;

    struct mqtt_ssl_config_t quectel_ssl_cfg = {
        .ssl_ctx_id =1,
        .verify_level = MQTT_SSL_VERIFY_NONE,
         .cacert_path = NULL,
         .client_cert_path = NULL,
       .client_key_path = NULL,
        .client_key_pwd = NULL

       };

client_info.ssl_cfg = &quectel_ssl_cfg;
}

int ConnectAfterAutoProvision()
{
    int ret = 0;
    ret = ql_mqtt_connect(&mqtt_cli, MQTT_CLIENT_QUECTEL_URL , mqtt_connect_result_cb, NULL, (const struct mqtt_connect_client_info_t *)&client_info, mqtt_state_exception_cb);
if(ret  == MQTTCLIENT_WOUNDBLOCK)
{
   QL_MQTT_LOG("====wait connect result for telemetry data profile connect");
   ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
   if(mqtt_connected == 0)
   {
       ql_mqtt_client_deinit(&mqtt_cli);
      
   }
}
else
{
   QL_MQTT_LOG("===mqtt connect failed ,ret = %d",ret);
   
}

ql_mqtt_set_inpub_callback(&mqtt_cli, mqtt_inpub_data_cb, NULL);

if(ql_mqtt_sub_unsub(&mqtt_cli, "v1/devices/me/rpc/request/+", 1, mqtt_requst_result_cb,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
    QL_MQTT_LOG("======wait subscrible result");
    ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
}
 return ret;

}
static void mqtt_app_thread(void * arg)
{
          int ret = 0;
          int  run_num = 1;
          int profile_idx = 1;
          uint8_t nSim = 0;
          uint16_t sim_cid = 0;
  
          // int is_user_onenet = 1;
          ql_rtos_semaphore_create(&mqtt_semp, 0);
          ql_rtos_task_sleep_s(15);
    
          
	    ql_dev_get_imei(imei_no, sizeof(imei_no), 0);
	     QL_MQTT_LOG("IMEI: %s\r\n", imei_no);

          QL_MQTT_LOG("========== mqtt demo start ==========");
          QL_MQTT_LOG("wait for network register done");
            
          sim_net_register(nSim, profile_idx);  // checks network registration and returns the status
		  ret = MQTTCLIENT_SUCCESS;
		  QL_MQTT_LOG("==============mqtt_client_test[%d]================\n",run_num);

		  if(QL_DATACALL_SUCCESS != ql_bind_sim_and_profile(nSim, profile_idx, &sim_cid))
			  {
						QL_MQTT_LOG("nSim or profile_idx is invalid!!!!");
						
			  }
retry:			  
			  if(ql_mqtt_client_init(&mqtt_cli, sim_cid) != MQTTCLIENT_SUCCESS){
						QL_MQTT_LOG("mqtt client init failed!!!!");
						
			  }

			 QL_MQTT_LOG("mqtt_cli:%d", mqtt_cli);

			  client_info.keep_alive = 60;
			  client_info.clean_session = 1;
			  client_info.will_qos = 0;
			  client_info.will_retain = 0;
			  client_info.will_topic = NULL;
			  client_info.will_msg = NULL;
	  
			  client_info.client_id = imei_no;
			  client_info.client_user = "provision";//imei_no;
			  client_info.client_pass = "provision";//imei_no;

			  struct mqtt_ssl_config_t quectel_ssl_cfg = {
				  .ssl_ctx_id =1,
				  .verify_level = MQTT_SSL_VERIFY_NONE,
				   .cacert_path = NULL,
				   .client_cert_path = NULL,
				 .client_key_path = NULL,
				  .client_key_pwd = NULL

				 };

		client_info.ssl_cfg = &quectel_ssl_cfg;




			  ret = ql_mqtt_connect(&mqtt_cli, MQTT_CLIENT_QUECTEL_URL , mqtt_connect_result_cb, NULL, (const struct mqtt_connect_client_info_t *)&client_info, mqtt_state_exception_cb);
			  if(ret  == MQTTCLIENT_WOUNDBLOCK)
			  {
				  QL_MQTT_LOG("====wait connect result");
				  ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
				  if(mqtt_connected == 0)
				  {
					  ql_mqtt_client_deinit(&mqtt_cli);
					 
				  }
			  }
			  else
			  {
				  QL_MQTT_LOG("===mqtt connect failed ,ret = %d",ret);
				  
			  }

     	  ql_mqtt_set_inpub_callback(&mqtt_cli, mqtt_inpub_data_cb, NULL);

		  if(ql_mqtt_sub_unsub(&mqtt_cli, "/provision/response", 1, mqtt_requst_result_cb,NULL, 1) == MQTTCLIENT_WOUNDBLOCK){
			  QL_MQTT_LOG("======wait subscrible result");
			  ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
		  }

          while(run_num <= 100)
          {        
            
                if(mqtt_connected == 1)
                {
                   
                    if(AppProvision ==0)
                    {
                        char autoProvisionPayload[200];
                        //sprintf(autoProvisionPayload, "{ \"deviceName\": %s,\"provisionDeviceKey\": \"si0u9idwfla3a3p9pu53\",\"provisionDeviceSecret\": \"ccwfg2h7l2wf1gabeq3o\",  \"credentialsType\": \"ACCESS_TOKEN\",\"token\": \"%s\"}",imei_no,imei_no);
                        //sprintf(autoProvisionPayload, "{ \"deviceName\": %s,\"provisionDeviceKey\": \"0l0n7mokcbylv5p0m68n\",\"provisionDeviceSecret\": \"oavj17khhjo3xkd4q20f\",  \"credentialsType\": \"ACCESS_TOKEN\",\"token\": \"%s\"}",imei_no,imei_no);// for prod.Schnelliot.in                        
                        sprintf(autoProvisionPayload, "{ \"deviceName\": %s,\"provisionDeviceKey\": \"aco9u4zlyah71sq8zd7t\",\"provisionDeviceSecret\": \"6r3b4cr8qn196i4vw3a9\",  \"credentialsType\": \"ACCESS_TOKEN\",\"token\": \"%s\"}",imei_no,imei_no);
                        
                        if(ql_mqtt_publish(&mqtt_cli, "/provision/request",autoProvisionPayload, strlen(autoProvisionPayload), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
                        {
                            QL_MQTT_LOG("======wait publish result Auto provision result");
                            ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                            ql_rtos_task_sleep_s(2);
                        }

                    }

                    else
                    {
                        QL_MQTT_LOG("PayloadBuffer is :%s",PayloadBuffer);
                        QL_MQTT_LOG(" Provisioning succesfull");
                        ql_rtos_task_sleep_s(1);
                        if(HardwareTestPub ==0)
                        {
                            HardwareTestPub =1;
                            ql_mqtt_client_deinit(&mqtt_cli);
                            ChangeClientInfo();
                            if(ql_mqtt_client_init(&mqtt_cli, sim_cid) != MQTTCLIENT_SUCCESS)
                            {
                                QL_MQTT_LOG("mqtt client init failed!!!!");
    
                            }
                           ql_rtos_task_sleep_s(10);
                           ConnectAfterAutoProvision(); 
                           if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/attributes",MQTT_ATTR_PUB_MSG, strlen(MQTT_ATTR_PUB_MSG), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
                           {
                            QL_MQTT_LOG("======wait publish result attributes");
                            ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                            ql_rtos_task_sleep_s(5);
                          }
                        
                           
                        }
                                                    
                        if(ql_mqtt_publish(&mqtt_cli, "v1/devices/me/telemetry",PayloadBuffer, strlen(PayloadBuffer), 1, 0, mqtt_requst_result_cb,NULL) == MQTTCLIENT_WOUNDBLOCK)
                       {
                        QL_MQTT_LOG("======wait publish result telemetry");
                        ql_rtos_semaphore_wait(mqtt_semp, QL_WAIT_FOREVER);
                        ql_rtos_task_sleep_s(5);
                      }
                    
                    }                 
                }

                else
                {
                    ql_mqtt_client_deinit( & mqtt_cli);
                    goto retry;
                }

            }

   return;          
}


int ql_mqtt_app_init(void)
{
          QlOSStatus err = QL_OSI_SUCCESS;
          
    err = ql_rtos_task_create(&mqtt_task, 10*1024, APP_PRIORITY_NORMAL, "mqtt_app", mqtt_app_thread, NULL, 5);
          if(err != QL_OSI_SUCCESS)
    {
                    QL_MQTT_LOG("mqtt_app init failed");
          }

          return err;
}
