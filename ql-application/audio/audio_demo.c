/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"

#include "ql_log.h"
#include "audio_demo.h"
#include "ql_osi_def.h"
#include "ql_audio.h"
#include "ql_fs.h"
#include "ql_i2c.h"
#include "quec_pin_index.h"
#include "ql_gpio.h"

#define QL_AUDIO_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_AUDIO_LOG(msg, ...)			QL_LOG(QL_AUDIO_LOG_LEVEL, "ql_audio", msg, ##__VA_ARGS__)
#define QL_AUDIO_LOG_PUSH(msg, ...)		QL_LOG_PUSH("ql_AUDIO", msg, ##__VA_ARGS__)

#if !defined(audio_demo_no_err)
	#define audio_demo_no_err(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x != 0)                                                                        				\
				{                                                                                               \
					QL_AUDIO_LOG(str);																			\
					{action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif

/* for headset_det */
#ifdef QL_APP_FEATURE_HEADSET_DET
#define QL_HEADSET_DETECT_PIN              QUEC_PIN_DNAME_GPIO_8
#define QL_HEADSET_DETECT_GPIO             QUEC_GPIO_DNAME_GPIO_8
#define QL_HEADSET_DETECT_FUNC_GPIO        0
#define QL_HEADSET_DETECT_DEBOUNCE_TIME    300    //unit: ms

#define DEMO_HEADSET_DETECT_PLUG_IN          1
#define DEMO_HEADSET_DETECT_PLUG_OUT         0

ql_task_t headset_det_task = NULL;
ql_timer_t headset_det_debounce_timer = NULL;
#endif

typedef struct
{
	PCM_HANDLE_T recorder;
	PCM_HANDLE_T player;
}ql_demo_poc_t;

static uint8 *pcm_buffer = NULL;
static uint  pcm_data_size = 0;
static bool  ring_tone_start = 0;
ql_task_t ql_play_task = NULL;

static int play_callback(char *p_data, int len, enum_aud_player_state state)
{
	if(state == AUD_PLAYER_START)
	{
		QL_AUDIO_LOG("player start run");
	}
	else if(state == AUD_PLAYER_FINISHED)
	{
		QL_AUDIO_LOG("player stop run");
	}
	else
	{
		QL_AUDIO_LOG("type is %d", state);
	}

	return QL_AUDIO_SUCCESS;
}

static int record_callback(char *p_data, int len, enum_aud_record_state state)
{	
	if(state == AUD_RECORD_START)
	{
		QL_AUDIO_LOG("recorder start run");
	}
	else if(state == AUD_RECORD_CLOSE)
	{
		QL_AUDIO_LOG("recorder stop run");
	}
	else if(state == AUD_RECORD_DATA)
	{
		if(len <= 0)
			return -1;

		if(pcm_data_size > RECORD_BUFFER_MAX){
			return -1;
		}
		else{
			memcpy(pcm_buffer+pcm_data_size, p_data, len);
			pcm_data_size += len;
		}
	}

	return QL_AUDIO_SUCCESS;
}

static int ringtone_callback(bool start, void *ctx)
{
	if(start)
	{
		QL_AUDIO_LOG("ringtone start play");
		
		if(ql_aud_get_play_state() != QL_AUDIO_STATUS_RUNNING)
		{
			ql_event_t event = {0};
			
			event.id = QL_AUDIO_RINGTONE_PLAY;
			ring_tone_start = (ql_rtos_event_send(ql_play_task, &event) ? FALSE:TRUE);
		}
	}
	else
	{
		QL_AUDIO_LOG("ringtone stop play");
		if(ring_tone_start){
			ql_aud_player_stop();
			ring_tone_start = FALSE;
		}
	}

	return 0;
}

static void ql_audio_demo_thread(void *param)
{
	QL_AUDIO_LOG("enter audio demo");

#ifdef QL_APP_FEATURE_EXT_CODEC
//	test_codec();
#endif

//	config_internal_codec_gain(); //配置内置codec的音频增益,此处不建议调整,保持默认值; 用户若要调整可参考此函数
//	test_pcm();
//	test_mp3();
//	test_wav();
//	test_amr();
//	test_record_file();
//	test_record_stream();
//	test_amr_stream();
//	test_poc_full_duplex();
//	test_poc_half_duplex();
//	test_ring_tone();  //the demo "ql_voice_call_app_init" must be opened

	QL_AUDIO_LOG("test done, exit audio demo");
	ql_rtos_task_delete(NULL);
}

static void ql_audio_play_thread(void *ctx)
{
	int err = 0;
	ql_event_t event = {0};

	while(1)
	{
		err = ql_event_try_wait(&event);
		audio_demo_no_err(err, continue, "wait event failed");

		switch(event.id)
		{
		case QL_AUDIO_RINGTONE_PLAY:
			do
			{
				err = ql_aud_play_file_start("ring_tone.mp3", QL_AUDIO_PLAY_TYPE_LOCAL, NULL);
				if(err)
				{
					ring_tone_start = FALSE;
					break;
				}			
				ql_aud_wait_play_finish(QL_WAIT_FOREVER);		
			}while(ring_tone_start);
		break;
		}
	}	
}

void config_internal_codec_gain(void)
{
	int err = 0;
//音频增益设置后, 不实时生效, 而是在下一次播放/通话时生效; 只有使用AudioCalibrationTool工具调试增益时，才可实时生效;
//这里使用api调增益, 必须关闭实时生效功能，否则会设置失败; 
	err = ql_aud_set_calib_real_time(0);
	audio_demo_no_err(err, return, "config real_time failed");

//配置本地播放模式下, 喇叭输出的1级音量对应的dac增益为 -13.5db, 算法增益为0db
	err = ql_aud_set_icvolume_level_gain(QL_AUDIO_PLAY_TYPE_LOCAL,
								   QL_OUTPUT_SPEAKER,
								   AUDIOHAL_SPK_VOL_1,
								   25,
								   15);
	audio_demo_no_err(err, return, "config volume failed");


//配置非volte通话模式下, 耳机的侧音增益为MUTE(关闭侧音)
	ql_aud_set_icsidet_gain(QL_AUD_VOICECALL_NB,
							QL_OUTPUT_HEADPHONE,
							QL_ICMIC_SIDET_GAIN_MUTE);
	audio_demo_no_err(err, return, "config side tone gain failed");
}

void test_ring_tone(void)
{
	ql_aud_set_ringtone_type(QL_AUD_RING_CUSTOMER_DEF);
	ql_bind_ring_tone_cb(ringtone_callback);
}


void test_poc_full_duplex(void)
{
	QL_PCM_CONFIG_T config = {0};
	int err = 0, cnt_read=0, cnt_write;
	char *buffer = NULL;
	static ql_demo_poc_t *demo_poc = NULL;

	config.channels = 1;  //单声道
	config.samplerate = 16000;

	ql_set_audio_path_earphone();
	ql_aud_set_volume(QL_AUDIO_PLAY_TYPE_VOICE, AUDIOHAL_SPK_VOL_6);  //POC mode use the param of voice call
	
	buffer = calloc(1, 1024);
	audio_demo_no_err(!buffer, return, "no memory");

	demo_poc = calloc(1, sizeof(ql_demo_poc_t));
	audio_demo_no_err(!demo_poc, return, "no memory");

	demo_poc->recorder = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_PCM, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG, QL_PCM_POC);
	audio_demo_no_err(!demo_poc->recorder, goto exit, "player created failed");

	demo_poc->player = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_PCM, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG, QL_PCM_POC);
	audio_demo_no_err(!demo_poc->player, goto exit, "player created failed");

	err = ql_aud_start_poc_mode(QL_POC_TYPE_FULL_DUPLEX);
	audio_demo_no_err(err, goto exit, "player created failed");

	while(1)
	{
		memset(buffer, 0, 1024);
	
		cnt_read = ql_pcm_read(demo_poc->recorder, buffer, 640);
		audio_demo_no_err((cnt_read<=0), goto exit, "read data failed");

		cnt_write = ql_pcm_write(demo_poc->player, buffer, cnt_read);
		audio_demo_no_err((cnt_write!=cnt_read), goto exit, "read data failed");	
	}

exit:
	if(buffer)
	{
		free(buffer);
	}

	ql_aud_stop_poc_mode();  //users must call "ql_aud_stop_poc_mode" then call "ql_pcm_close", otherwise may leed to some errors

	if(demo_poc)
	{
		if(demo_poc->player)
		{
			ql_pcm_close(demo_poc->player);
		}	
			
		if(demo_poc->recorder)
		{
			ql_pcm_close(demo_poc->recorder);
		}
		
		free(demo_poc);	
	}
}

void test_poc_half_duplex(void)
{
	PCM_HANDLE_T recorder = NULL;
	PCM_HANDLE_T player = NULL;

	QL_PCM_CONFIG_T config;
	void *data = NULL;
	int size, total_size = 0, cnt=0, write_cnt=0;

	config.channels = 1;  //单声道
	config.samplerate = 16000;
	config.amrwb_param.amrwb_mode = AMRWB_MODE_2385;

	data = calloc(1, 200*1024);
	if(data == NULL)
	{
		goto exit;	
	}

	ql_set_audio_path_earphone();
	ql_aud_set_volume(QL_AUDIO_PLAY_TYPE_VOICE, AUDIOHAL_SPK_VOL_6);  //POC mode use the param of voice call

	recorder = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_PCM, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG, QL_PCM_POC);
	audio_demo_no_err(!recorder, goto exit, "recorder created failed");

	player = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_PCM, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG, QL_PCM_POC);
	audio_demo_no_err(!player, goto exit, "player created failed");
	
	int err = ql_aud_start_poc_mode(QL_POC_TYPE_HALF_DUPLEX);
	audio_demo_no_err(err, goto exit, "poc mode start failed");

	err = ql_aud_poc_switch(QL_POC_MODE_REC);
	audio_demo_no_err(err, goto exit, "poc mode switch failed");

	//start record
	while(total_size < 200*1024)
	{
		size = ql_pcm_read(recorder, data+total_size, 1024);

		if(size <= 0)
		{
			break;
		}
		
		total_size += size;
	}

	err = ql_aud_poc_switch(QL_POC_MODE_PLAY);
	audio_demo_no_err(err, goto exit, "poc mode switch failed");

	while(write_cnt < total_size)
	{
		if(total_size - write_cnt > PACKET_WRITE_MAX_SIZE)	//单次最多可写入3K字节
		{
			cnt = ql_pcm_write(player, data+write_cnt, PACKET_WRITE_MAX_SIZE);
		}
		else
		{
			cnt = ql_pcm_write(player, data+write_cnt, total_size - write_cnt);
		}
		if(cnt <= 0)
		{
			QL_AUDIO_LOG("write failed");
			goto exit;	
		}
		write_cnt += cnt;
	}

	while(ql_pcm_buffer_used(player))  //in poc mode, player will not stop if not ql_aud_stop_poc_mode called
	{
		ql_rtos_task_sleep_ms(20); //wait the write buffer empty
	}	
	ql_rtos_task_sleep_ms(200);
	
exit:
	ql_aud_stop_poc_mode();  //users must call "ql_aud_stop_poc_mode" then call "ql_pcm_close", otherwise may leed to some errors
	ql_pcm_close(recorder);
	ql_pcm_close(player);
	
	if(data)
	{
		free(data);
	}
	QL_AUDIO_LOG("test done");
}


void ql_audio_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	ql_task_t ql_audio_task = NULL;
	
    QL_AUDIO_LOG("audio demo enter");
	
    err = ql_rtos_task_create(&ql_audio_task, 4096*2, APP_PRIORITY_NORMAL, "ql_audio", ql_audio_demo_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_AUDIO_LOG("audio task create failed");
	}

	err = ql_rtos_task_create(&ql_play_task, 4096, APP_PRIORITY_NORMAL, "ql_audio", ql_audio_play_thread, NULL, 2);
	if(err != QL_OSI_SUCCESS)
    {
		QL_AUDIO_LOG("audio task create failed");
	}
}

void test_amr_stream(void)
{
	PCM_HANDLE_T PCM = NULL;
	QL_PCM_CONFIG_T config;
	void *data = NULL;
	int size, total_size = 0, cnt=0, write_cnt=0;

	config.channels = 1;  //单声道
	config.samplerate = 16000;
	config.amrwb_param.amrwb_mode = AMRWB_MODE_2385;
	
	PCM = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_AMRWB, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG, QL_PCM_LOCAL);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}
	data = malloc(100*1024);
	if(data == NULL)
	{
		goto exit;	
	}

	QL_AUDIO_LOG("start read");

//start record
	while(total_size < 10*1024)
	{
		size = ql_pcm_read(PCM, data+total_size, 41);

		if(size <= 0)
		{
			break;
		}
		
		total_size += size;
	}	

	QL_AUDIO_LOG("exit record");

	if(total_size <= 0)
	{
		QL_AUDIO_LOG("read pcm failed");
		goto exit;			
	}
	QL_AUDIO_LOG("size is %d", total_size);

	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;

	PCM = ql_aud_pcm_open(&config, QL_AUDIO_FORMAT_AMRWB, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG, QL_PCM_LOCAL);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}

	while(write_cnt < total_size)
	{
		if(total_size - write_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可写3K字节
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, PACKET_WRITE_MAX_SIZE);
		}
		else
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, total_size - write_cnt);
		}
		if(cnt <= 0)
		{
			QL_AUDIO_LOG("write failed");
			goto exit;	
		}
		write_cnt += cnt;
	}

	ql_aud_data_done();
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	
	QL_AUDIO_LOG("play finish");	

exit:
	if(PCM != NULL)
	{
		ql_pcm_close(PCM);
	}
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}
}


void test_record_stream(void)
{
	ql_aud_config config = {0};
	int cnt = 0, total_cnt=0, err;

	config.amrwb_param.amrwb_mode = AMRWB_MODE_2385;
	
	pcm_buffer = malloc(RECORD_BUFFER_MAX);
	if(!pcm_buffer){
		return;
	}
	
/*	录音    */	
	if(ql_aud_record_stream_start_ex(&config, QL_REC_TYPE_MIC, QL_AUDIO_FORMAT_AMRWB, record_callback))
	{
		QL_AUDIO_LOG("record fail");
		goto exit;
	}
	ql_rtos_task_sleep_s(5);   //record 5s
	ql_aud_record_stop();

	if(pcm_data_size <= 0){
		QL_AUDIO_LOG("data invalid");
		goto exit;			
	}

/*	读取录音文件用于播放，此处也可调用	ql_aud_play_file_start,或者ql_pcm_open+ql_pcm_write去播放    				  */
	ql_set_audio_path_speaker();
	ql_set_volume(TEST_PLAY_VOLUME);

	while(total_cnt < pcm_data_size)
	{
		if(pcm_data_size - total_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可播放3K字节
		{
			cnt = PACKET_WRITE_MAX_SIZE;   
			err = ql_aud_play_stream_start(QL_AUDIO_FORMAT_AMRWB, pcm_buffer+total_cnt, cnt, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback);
		}
		else
		{
			cnt = pcm_data_size - total_cnt;
			err = ql_aud_play_stream_start(QL_AUDIO_FORMAT_AMRWB, pcm_buffer+total_cnt, cnt, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback);
		}
		
		if(err < 0)
		{
			QL_AUDIO_LOG("start failed");
			goto exit;
		}
		else
		{
			QL_AUDIO_LOG("play %d bytes, total %d", cnt, total_cnt);
			total_cnt += cnt;
		}
	}

	ql_aud_data_done();	
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	ql_aud_player_stop();
	
	QL_AUDIO_LOG("test successful");

exit:

	if(pcm_buffer){
		free(pcm_buffer);
		pcm_buffer = NULL;
		pcm_data_size = 0;
	}
}

void test_record_file(void)
{
	ql_aud_config config = {0};
	config.samplerate = 8000;

	if(ql_aud_record_file_start(TEST_RECORD_WAV_NAME, &config, QL_REC_TYPE_MIC, NULL) != QL_AUDIO_SUCCESS)
	{
		QL_AUDIO_LOG("record failed");
		return;
	}
	QL_AUDIO_LOG("record start");
	ql_rtos_task_sleep_s(5);   //record 5s

	ql_aud_record_stop();
	QL_AUDIO_LOG("record finish, start play");

	ql_set_audio_path_speaker();
	if(ql_aud_play_file_start(TEST_RECORD_WAV_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}

	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	ql_aud_player_stop();
	QL_AUDIO_LOG("test successful");
}

void test_mp3(void)
{
	if(ql_aud_play_file_start(TEST_MP3_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	if(ql_aud_play_file_start(TEST_MP3_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	ql_aud_player_stop(); //播放结束，释放播放资源
	
	QL_AUDIO_LOG("test mp3 successful");
}


void test_wav(void)
{
	int cnt = 0;

	if(ql_aud_play_file_start(TEST_WAV_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	if(ql_aud_play_file_start(TEST_WAV_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
		return;
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	
	QL_AUDIO_LOG("play %d times ok", ++cnt);
	
	ql_aud_player_stop(); //播放结束，释放播放资源
	QL_AUDIO_LOG("test wav successful");
}

void test_amr(void)
{
	int cnt = 0;
	
	if(ql_aud_play_file_start(TEST_AMR_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	if(ql_aud_play_file_start(TEST_AMR_FILE_NAME, QL_AUDIO_PLAY_TYPE_LOCAL, play_callback))
	{
		QL_AUDIO_LOG("play failed");
	}
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);
	QL_AUDIO_LOG("play %d times ok", ++cnt);

	ql_aud_player_stop(); //播放结束，释放播放资源
	QL_AUDIO_LOG("test wav successful");
}

void test_pcm(void)
{
	PCM_HANDLE_T PCM = NULL;
	QL_PCM_CONFIG_T config;
	void *data = NULL;
	int size=0, write_cnt=0, cnt=0;

	config.channels = 1; //单声道
	config.samplerate = 8000;
	
	PCM = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_READ_FLAG);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}
	data = malloc(50*1024);
	if(data == NULL)
	{
		goto exit;	
	}

	QL_AUDIO_LOG("start read");
	size = ql_pcm_read(PCM, data, 50*1024);
	if(size <= 0)
	{
		QL_AUDIO_LOG("read pcm failed");
		goto exit;			
	}
	QL_AUDIO_LOG("size is %d", size);

	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;
	
	ql_set_audio_path_speaker();
	
	PCM = ql_pcm_open(&config, QL_PCM_BLOCK_FLAG|QL_PCM_WRITE_FLAG);
	if(PCM == NULL)
	{
		QL_AUDIO_LOG("open pcm failed");
		goto exit;		
	}

	QL_AUDIO_LOG("start write");

	while(write_cnt < size)
	{
		if(size - write_cnt > PACKET_WRITE_MAX_SIZE)  //单次最多可播放3K字节
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, PACKET_WRITE_MAX_SIZE);
		}
		else
		{
			cnt = ql_pcm_write(PCM, data+write_cnt, size - write_cnt);
		}
		if(cnt <= 0)
		{
			QL_AUDIO_LOG("write failed");
			goto exit;	
		}
		write_cnt += cnt;
	}

	ql_aud_data_done();
	ql_aud_wait_play_finish(QL_WAIT_FOREVER);

	QL_AUDIO_LOG("play done");

	
	if(ql_pcm_close(PCM) != 0)
	{
		QL_AUDIO_LOG("close pcm failed");
		goto exit;			
	}
	PCM = NULL;

	QL_AUDIO_LOG("play finish");	

exit:
	if(PCM != NULL)
	{
		ql_pcm_close(PCM);
	}
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}
}

#ifdef  QL_APP_FEATURE_EXT_CODEC
void test_codec(void)
{
	ql_aud_codec_clk_enable();
	ql_I2cInit(QL_AUDIO_IIC_CHANNEL, STANDARD_MODE);

	aud_codec_init();
	ql_switch_ext_codec(TRUE);    //need reset, as dsp only read audio nv when start
}

void ALC5616_reg_nv_load(QL_HAL_CODEC_CFG_T *nv_cfg)
{
	AUD_CODEC_REG_T init_reg[]  		= RT5616_NV_INIT_REG,       \
					close_reg[] 		= RT5616_NV_CLOSE_REG,	    \
					sample_8k_reg[]     = RT5616_NV_SAMPLE_8K_REG,  \
					sample_16k_reg[]    = RT5616_NV_SAMPLE_16K_REG,	\
					sample_32k_reg[]	= RT5616_NV_SAMPLE_32K_REG, \
                    sample_44_1k_reg[]	= RT5616_NV_SAMPLE_44_1K_REG,\
                    sample_48k_reg[]	= RT5616_NV_SAMPLE_48K_REG;
    
	memset(&nv_cfg->initRegCfg[0], 0, 100*sizeof(AUD_CODEC_REG_T));         //clear codec init register nv
	memset(&nv_cfg->closeRegCfg[0], 0, 50*sizeof(AUD_CODEC_REG_T));   		//clear codec close register nv
	memset(&nv_cfg->sampleRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec samprate register nv
	memset(&nv_cfg->inpathRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec in path register nv
	memset(&nv_cfg->outpathRegCfg[0][0], 0, 80*sizeof(AUD_CODEC_REG_T));    //clear codec output path register nv

	memcpy(nv_cfg->initRegCfg, init_reg, sizeof(init_reg));
	memcpy(nv_cfg->closeRegCfg, close_reg, sizeof(close_reg));
	memcpy(&nv_cfg->sampleRegCfg[0], sample_8k_reg, sizeof(sample_8k_reg));
	memcpy(&nv_cfg->sampleRegCfg[3], sample_16k_reg, sizeof(sample_16k_reg));
	memcpy(&nv_cfg->sampleRegCfg[6], sample_32k_reg, sizeof(sample_32k_reg));
    memcpy(&nv_cfg->sampleRegCfg[7], sample_44_1k_reg, sizeof(sample_44_1k_reg));
    memcpy(&nv_cfg->sampleRegCfg[8], sample_48k_reg, sizeof(sample_48k_reg));
}
void NAU8810_reg_nv_load(QL_HAL_CODEC_CFG_T *nv_cfg)
{
	AUD_CODEC_REG_T init_reg[]  		= NAU8810_NV_INIT_REG,       \
					close_reg[] 		= NAU8810_NV_CLOSE_REG,	    \
					sample_8k_reg[]     = NAU8810_NV_SAMPLE_8K_REG,  \
					sample_16k_reg[]    = NAU8810_NV_SAMPLE_16K_REG,	\
					sample_32k_reg[]	= NAU8810_NV_SAMPLE_32K_REG,  \
	                sample_44_1k_reg[]	= NAU8810_NV_SAMPLE_44_1K_REG,\
                    sample_48k_reg[]	= NAU8810_NV_SAMPLE_48K_REG;	
    
	memset(&nv_cfg->initRegCfg[0], 0, 100*sizeof(AUD_CODEC_REG_T));         //clear codec init register nv
	memset(&nv_cfg->closeRegCfg[0], 0, 50*sizeof(AUD_CODEC_REG_T));   		//clear codec close register nv
	memset(&nv_cfg->sampleRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec samprate register nv
	memset(&nv_cfg->inpathRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec in path register nv
	memset(&nv_cfg->outpathRegCfg[0][0], 0, 80*sizeof(AUD_CODEC_REG_T));    //clear codec output path register nv

	memcpy(nv_cfg->initRegCfg, init_reg, sizeof(init_reg));
	memcpy(nv_cfg->closeRegCfg, close_reg, sizeof(close_reg));
	memcpy(&nv_cfg->sampleRegCfg[0], sample_8k_reg, sizeof(sample_8k_reg));
	memcpy(&nv_cfg->sampleRegCfg[3], sample_16k_reg, sizeof(sample_16k_reg));
	memcpy(&nv_cfg->sampleRegCfg[6], sample_32k_reg, sizeof(sample_32k_reg));
    memcpy(&nv_cfg->sampleRegCfg[7], sample_44_1k_reg, sizeof(sample_44_1k_reg));
    memcpy(&nv_cfg->sampleRegCfg[8], sample_48k_reg, sizeof(sample_48k_reg));
}

void ALC5616e_reg_nv_load(QL_HAL_CODEC_CFG_T *nv_cfg)
{
	AUD_CODEC_REG_T init_reg[]  		= RT5616E_NV_INIT_REG,       \
					close_reg[] 		= RT5616E_NV_CLOSE_REG,	    \
					sample_8k_reg[]     = RT5616E_NV_SAMPLE_8K_REG,  \
					sample_16k_reg[]    = RT5616E_NV_SAMPLE_16K_REG,	\
					sample_32k_reg[]	= RT5616E_NV_SAMPLE_32K_REG;
	
	memset(&nv_cfg->initRegCfg[0], 0, 100*sizeof(AUD_CODEC_REG_T));         //clear codec init register nv
	memset(&nv_cfg->closeRegCfg[0], 0, 50*sizeof(AUD_CODEC_REG_T));   		//clear codec close register nv
	memset(&nv_cfg->sampleRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec samprate register nv
	memset(&nv_cfg->inpathRegCfg[0][0], 0, 120*sizeof(AUD_CODEC_REG_T));    //clear codec in path register nv
	memset(&nv_cfg->outpathRegCfg[0][0], 0, 80*sizeof(AUD_CODEC_REG_T));    //clear codec output path register nv

	memcpy(nv_cfg->initRegCfg, init_reg, sizeof(init_reg));
	memcpy(nv_cfg->closeRegCfg, close_reg, sizeof(close_reg));
	memcpy(&nv_cfg->sampleRegCfg[0], sample_8k_reg, sizeof(sample_8k_reg));
	memcpy(&nv_cfg->sampleRegCfg[3], sample_16k_reg, sizeof(sample_16k_reg));
	memcpy(&nv_cfg->sampleRegCfg[6], sample_32k_reg, sizeof(sample_32k_reg));
}

//switch to ext codec ,need reset
ql_audio_errcode_e ql_switch_ext_codec(bool enable)
{
	ql_audio_errcode_e err = QL_AUDIO_SUCCESS;
	uint16 codec_id;
	QL_HAL_CODEC_CFG_T *halCodecCfg = NULL;

	halCodecCfg = calloc(1, sizeof(QL_HAL_CODEC_CFG_T));
	if(!halCodecCfg){
		return QL_AUDIO_UNKNOWN_ERROR;
		goto exit;
	}

	err = ql_aud_codec_read_nv(halCodecCfg);
    audio_demo_no_err(err, goto exit, "read failed");
	
	switch(enable)
	{
	case 1:   //ext codec

        err = ql_audio_iic_read(RT5616E_I2C_SLAVE_ADDR, RT5616E_VENDOR_ID_REG, 2, &codec_id);  //read 5616e firstly, if is not 5616e, then read other codec id 
        //at_audio_no_err(err, goto exit, "read codec failed");
        
        if(codec_id == RT5616E_VENDOR_ID)  //init codec 5616e nv
        {   
            QL_AUDIO_LOG("ext codec is ALC5616E, start config");
            halCodecCfg->i2cCfg.id = (QL_AUDIO_IIC_CHANNEL == i2c_1 ? 0:1);  //0 for use iic1, 1 for use iic 2, and iic 2 is default
            halCodecCfg->basicCfg.iic_data_width = 3;  //iic register 16bit, hsb first
            halCodecCfg->basicCfg.codecAddr = RT5616E_I2C_SLAVE_ADDR;   //iic addr, dsp will use it to connect codec
            ALC5616e_reg_nv_load(halCodecCfg);  //load 5616e register to struct
        }
        else
        {     
            err = ql_audio_iic_read(RT5616_I2C_SLAVE_ADDR, RT5616_VENDOR_ID_REG, 1, &codec_id);  //read 5616 firstly, if is not 5616, then read other codec id 
            //at_audio_no_err(err, goto exit, "read codec failed");
            
            if(codec_id == RT5616_VENDOR_ID)  //init codec 5616 nv
            {   
                QL_AUDIO_LOG("ext codec is ALC5616, start config");
                halCodecCfg->i2cCfg.id = (QL_AUDIO_IIC_CHANNEL == i2c_1 ? 0:1);  //0 for use iic1, 1 for use iic 2, and iic 2 is default
                halCodecCfg->basicCfg.iic_data_width = 3;  //iic register 16bit, hsb first
                halCodecCfg->basicCfg.codecAddr = RT5616_I2C_SLAVE_ADDR;   //iic addr, dsp will use it to connect codec
                ALC5616_reg_nv_load(halCodecCfg);  //load 5616 register to struct
            }
            else
            {               
                err = ql_audio_iic_read(NAU8810_I2C_SLAVE_ADDR, NAU8810_VENDOR_ID_REG, 1, &codec_id);  //read 8810 firstly, if is not 8810, then read other codec id 
                audio_demo_no_err(err, goto exit, "read codec failed");
                if(codec_id == NAU8810_VENDOR_ID)
                {
                    QL_AUDIO_LOG("ext codec is NAU8810, start config");
                    halCodecCfg->i2cCfg.id = (QL_AUDIO_IIC_CHANNEL == i2c_1 ? 0:1);  //0 for use iic1, 1 for use iic 2, and iic 2 is default
                    halCodecCfg->basicCfg.iic_data_width = 1;  //iic register 9bit, hsb first
                    halCodecCfg->basicCfg.codecAddr = NAU8810_I2C_SLAVE_ADDR;   //iic addr, dsp will use it to connect codec
                    NAU8810_reg_nv_load(halCodecCfg);  //load 8810 register to struct
                }
                else
                {
                    err = QL_AUDIO_OPER_NOT_SUPPORTED;
                    audio_demo_no_err(err, goto exit, "not support current codec");
                    
            
                }
            
            }
        
        }

		halCodecCfg->basicCfg.externalCodec = 1;   //switch to ext codec
		
		err = ql_aud_codec_write_nv(halCodecCfg);
		audio_demo_no_err(err, goto exit, "write nv failed");	
	break;

	case 0:   //internal codec
		halCodecCfg->basicCfg.externalCodec = 0;			
		err = ql_aud_codec_write_nv(halCodecCfg);
		audio_demo_no_err(err, goto exit, "write nv failed");
	break;	
	}

exit:
	if(halCodecCfg){
		free(halCodecCfg);
	}

	return err;
}

ql_audio_errcode_e aud_codec_init(void)
{
	int err = QL_AUDIO_SUCCESS, i=0;
	uint16 codec_id;
	QL_HAL_CODEC_CFG_T *halCodecCfg = NULL;
	
	halCodecCfg = calloc(1, sizeof(QL_HAL_CODEC_CFG_T));
	audio_demo_no_err(!halCodecCfg, return QL_AUDIO_UNKNOWN_ERROR, "malloc memory failed");

	err = ql_aud_codec_read_nv(halCodecCfg);
	audio_demo_no_err(err, goto exit, "read failed");

	if(halCodecCfg->basicCfg.externalCodec)  //1 for use external codec
	{
		QL_AUDIO_LOG("codec iic addr is %p, clk mode is %d", halCodecCfg->basicCfg.codecAddr, halCodecCfg->i2cCfg.clockMode);

		ql_aud_codec_clk_enable();

        err = ql_audio_iic_read(RT5616E_I2C_SLAVE_ADDR, RT5616E_VENDOR_ID_REG, 2, &codec_id);
    //  require_action(err, goto exit, "read codec failed");
    
        if(codec_id == RT5616E_VENDOR_ID)  //init codec 5616e
        {   
            AUD_CODEC_REG_T reg_list[] = RT5616E_INIT_REG;   
            for (i = 0; i < sizeof(reg_list)/sizeof(reg_list[0]); i++)
            {
                err = ql_audio_iic_write(RT5616E_I2C_SLAVE_ADDR, reg_list[i].regAddr, 2, reg_list[i].val);
                audio_demo_no_err(err, goto exit, "write codec register fail");
            }
            
            ql_pin_set_func(QUEC_PIN_DNAME_GPIO_0,0);//for codec5616e i2s clock can running

            ql_gpio_init(QUEC_GPIO_DNAME_GPIO_0,GPIO_OUTPUT,PULL_NONE,LVL_HIGH);
            ql_rtos_task_sleep_ms(10);
            
            ql_gpio_init(QUEC_GPIO_DNAME_GPIO_0,GPIO_OUTPUT,PULL_NONE,LVL_LOW);
            ql_rtos_task_sleep_ms(10);
            
            ql_gpio_init(QUEC_GPIO_DNAME_GPIO_0,GPIO_OUTPUT,PULL_NONE,LVL_HIGH);
            ql_rtos_task_sleep_ms(10);
            
            ql_gpio_init(QUEC_GPIO_DNAME_GPIO_0,GPIO_OUTPUT,PULL_NONE,LVL_LOW);
            ql_rtos_task_sleep_ms(10);
            
        }
        else
        {

            err = ql_audio_iic_read(RT5616_I2C_SLAVE_ADDR, RT5616_VENDOR_ID_REG, 1, &codec_id);
        //  require_action(err, goto exit, "read codec failed");
        
            if(codec_id == RT5616_VENDOR_ID)  //init codec 5616
            {   
                AUD_CODEC_REG_T reg_list[] = RT5616_INIT_REG;   
                for (i = 0; i < sizeof(reg_list)/sizeof(reg_list[0]); i++)
                {     
                    err = ql_audio_iic_write(RT5616_I2C_SLAVE_ADDR, reg_list[i].regAddr, 1, reg_list[i].val);
                    audio_demo_no_err(err, goto exit, "write codec register fail");
                }
            }
            else  //NAU8810 support now
            {
                err = ql_audio_iic_read(NAU8810_I2C_SLAVE_ADDR, NAU8810_VENDOR_ID_REG, 1, &codec_id);
                audio_demo_no_err(err, goto exit, "read codec failed");
                
                if(codec_id == NAU8810_VENDOR_ID)  //init codec 8810
                {   
                    
                    AUD_CODEC_REG_T reg_list[] = NAU8810_INIT_REG;   
                    for (i = 0; i < sizeof(reg_list)/sizeof(reg_list[0]); i++)
                    {
                        err = ql_audio_iic_write(NAU8810_I2C_SLAVE_ADDR, reg_list[i].regAddr, 1, reg_list[i].val);
                        audio_demo_no_err(err, goto exit, "write codec register fail");
                    }
                }
                else
                {
                    err = QL_AUDIO_UNKNOWN_ERROR;
                    audio_demo_no_err(err, goto exit, "codec not valid");
        
                }
            }
        }
	}
	
exit:
	if(halCodecCfg){
		free(halCodecCfg);
	}
	return err;
}

static ql_audio_errcode_e ql_rt5616_write_reg(uint8 RegAddr, uint16 RegData)
{
    ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
    uint8 param_data[3] = {0x00};
    uint8 retry_count = 5;

    param_data[0] = (uint8)((RegData >> 8) & 0xFF);
    param_data[1] = (uint8)(RegData & 0xff);

    do 
    {
        status = (ql_audio_errcode_e)ql_I2cWrite(QL_AUDIO_IIC_CHANNEL, RT5616_I2C_SLAVE_ADDR, RegAddr, param_data, 2);
        if (status != QL_AUDIO_SUCCESS)
        {
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] data[0x%x] failed", retry_count, RT5616_I2C_SLAVE_ADDR, RegAddr, RegData);
        }
		else
		{
			break;
		}
    } while (--retry_count);

    return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_WR_FAIL);
}

static ql_audio_errcode_e ql_rt5616_read_reg(uint8 RegAddr, uint16 *p_value)
{
	ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
	uint8 temp_buf[2];
	uint8 retry_count = 5;

	do 
    {
        status = (ql_audio_errcode_e)ql_I2cRead(QL_AUDIO_IIC_CHANNEL, RT5616_I2C_SLAVE_ADDR, RegAddr, temp_buf, 2);
		if (status != QL_AUDIO_SUCCESS)
		{
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] failed", retry_count, RT5616_I2C_SLAVE_ADDR, RegAddr);
        }	
		else
        {
			*p_value = (((uint16)temp_buf[0]) << 8) | temp_buf[1];
			break;
		}
	} while (--retry_count);

	return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_RD_FAIL);
}

static ql_audio_errcode_e ql_nau8810_write_reg(uint8 RegAddr, uint16 RegData)
{
    ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
    uint8 param_data[3] = {0x00};
    uint8 retry_count = 5;
 
    param_data[0] = (uint8)((RegData >> 8) & 0xFF);
    param_data[1] = (uint8)(RegData & 0xff);
	RegAddr = (RegAddr << 1) & 0xFE;
    RegAddr = RegAddr|((RegData>>8)&0x01);
    param_data[0] = (uint8)(RegData & 0xFF);

    do 
    {
        status = (ql_audio_errcode_e)ql_I2cWrite(QL_AUDIO_IIC_CHANNEL, NAU8810_I2C_SLAVE_ADDR, RegAddr, param_data, 1);
        if (status != QL_AUDIO_SUCCESS)
        {
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] data[0x%x] failed", retry_count, NAU8810_I2C_SLAVE_ADDR, RegAddr, RegData);
        }        
    } while (--retry_count);

    return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_WR_FAIL);
}

static ql_audio_errcode_e ql_nau8810_read_reg(uint8 RegAddr, uint16 *p_value)
{
	ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
	uint8 temp_buf[2];
	uint8 retry_count = 5;
    RegAddr = (RegAddr << 1) & 0xFE;

	do 
    {
        status = (ql_audio_errcode_e)ql_I2cRead(QL_AUDIO_IIC_CHANNEL, NAU8810_I2C_SLAVE_ADDR, RegAddr, temp_buf, 2);
		if (status != QL_AUDIO_SUCCESS)
		{
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] failed", retry_count, NAU8810_I2C_SLAVE_ADDR, RegAddr);
        }	
		else
        {
			*p_value = (((uint16)temp_buf[0]) << 8) | temp_buf[1];
			break;
		}
	} while (--retry_count);

	return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_RD_FAIL);
}

static ql_audio_errcode_e ql_rt5616e_write_reg(uint16 RegAddr, uint16 RegData)
{
    ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
    uint8 param_data[3] = {0x00};
    uint8 retry_count = 5;

    param_data[0] = (uint8)((RegData >> 8) & 0xFF);
    param_data[1] = (uint8)(RegData & 0xff);

    do 
    {
        status = (ql_audio_errcode_e)ql_I2cWrite_16bit_addr(QL_AUDIO_IIC_CHANNEL, RT5616E_I2C_SLAVE_ADDR, RegAddr, param_data, 2);
        if (status != QL_AUDIO_SUCCESS)
        {
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] data[0x%x] failed", retry_count, RT5616E_I2C_SLAVE_ADDR, RegAddr, RegData);
        }        
    } while (--retry_count);

    return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_WR_FAIL);
}

static ql_audio_errcode_e ql_rt5616e_read_reg(uint16 RegAddr, uint16 *p_value)
{
	ql_audio_errcode_e status = QL_AUDIO_SUCCESS;
	uint8 temp_buf[2];
	uint8 retry_count = 5;

	do 
    {
        status = (ql_audio_errcode_e)ql_I2cRead_16bit_addr(QL_AUDIO_IIC_CHANNEL, RT5616E_I2C_SLAVE_ADDR, RegAddr, temp_buf, 2);
		if (status != QL_AUDIO_SUCCESS)
		{
            QL_AUDIO_LOG("Error:[%dth] device[0x%x] addr[0x%x] failed", retry_count, RT5616E_I2C_SLAVE_ADDR, RegAddr);
        }	
		else
        {
			*p_value = (((uint16)temp_buf[0]) << 8) | temp_buf[1];
			break;
		}
	} while (--retry_count);

	return (status == 0 ? QL_AUDIO_SUCCESS :  QL_AUDIO_CODEC_RD_FAIL);
}

ql_audio_errcode_e ql_audio_iic_read(uint8 dev_addr, uint16 reg_addr, uint8 size, uint16 *p_val)
{
	ql_audio_errcode_e err = 0;

    if(size==2)
    {

        /* 5616E */
        if(0x1B == dev_addr)
        {
            err = ql_rt5616e_read_reg(reg_addr, p_val);
        }
        else
        {
            return QL_AUDIO_INVALID_PARAM;

        }
    }
    else
    {
        /* 5616 */
        if(0x1B == dev_addr)
        {
            err = ql_rt5616_read_reg((uint8)reg_addr, p_val);
        }
        /* 8810 */
        else if (0x1A == dev_addr)
        {
            // support now
            err = ql_nau8810_read_reg((uint8)reg_addr, p_val);
        }
        else
        {
            return QL_AUDIO_INVALID_PARAM;
        }

    }

    return err;	
}


ql_audio_errcode_e ql_audio_iic_write(uint8 dev_addr, uint16 reg_addr, uint8 size, uint16 val)
{
    ql_audio_errcode_e err = QL_AUDIO_SUCCESS;

    if(size==2)
    {

        /* 5616E */
        if(0x1B == dev_addr)
        {
            err = ql_rt5616e_write_reg(reg_addr, val);
        }
        else
        {
            return QL_AUDIO_INVALID_PARAM;

        }
    }
    else
    {
        /* 5616 */
        if(0x1B == dev_addr)
        {
            err = ql_rt5616_write_reg((uint8)reg_addr, val);
        }
        /* 8810 */
        else if (0x1A == dev_addr)
        {
            // support now
            err = ql_nau8810_write_reg((uint8)reg_addr, val);
        }
        else
        {
            return QL_AUDIO_INVALID_PARAM;
        }

    }


    return err;
}

#endif

#ifdef QL_APP_FEATURE_HEADSET_DET
void _ql_headset_det_debounce_callback(void *ctx)
{
    if( headset_det_debounce_timer == NULL || ql_rtos_timer_is_running(headset_det_debounce_timer) )
    {
        return;
    }
    ql_rtos_timer_start(headset_det_debounce_timer, QL_HEADSET_DETECT_DEBOUNCE_TIME, 1);
    QL_AUDIO_LOG("headset timer start");
}

static void _ql_headset_detect_callback(void *ctx)
{
    ql_LvlMode headsetdet_value;
    ql_event_t event;

    ql_rtos_timer_stop(headset_det_debounce_timer);

    event.id = QUEC_HEADSET_DET_EVENT_IND;

    ql_gpio_get_level(QL_HEADSET_DETECT_GPIO, &headsetdet_value);
    if( headsetdet_value == LVL_LOW )       //detect plug_in
    {
        event.param1 = DEMO_HEADSET_DETECT_PLUG_IN;
    }
    else/* headsetdet_value == LVL_HIGH */  //detect plug_out
    {
        event.param1 = DEMO_HEADSET_DETECT_PLUG_OUT;
    }

    ql_rtos_event_send(headset_det_task, &event);
}

int ql_headset_det_pin_init(void)
{
    int err;

    err = ql_pin_set_func(QL_HEADSET_DETECT_PIN, QL_HEADSET_DETECT_FUNC_GPIO);
    if( err != QL_GPIO_SUCCESS )
    {
        QL_AUDIO_LOG("headset_det err = %d", err);
        return err;
    }

    return 0;
}

int ql_headset_det_interrupt_init(void)
{
    int err;

    err = ql_int_register(QL_HEADSET_DETECT_GPIO, EDGE_TRIGGER, DEBOUNCE_EN, EDGE_BOTH, PULL_UP, _ql_headset_det_debounce_callback, NULL);
    if( err != QL_GPIO_SUCCESS )
    {
        QL_AUDIO_LOG("headset_det err = %d", err);
        return err;
    }

    ql_int_enable(QL_HEADSET_DETECT_GPIO);

    return 0;
}

static void ql_headset_det_demo_thread(void *param)
{
    QL_AUDIO_LOG("headset_det demo thread enter, param 0x%x", param);

    ql_event_t event;

    if( ql_headset_det_pin_init() != 0 )
    {
        QL_AUDIO_LOG("headset_det pin init failed");
        ql_rtos_task_delete(NULL);
    }

    if( ql_headset_det_interrupt_init() != 0 )
    {
        QL_AUDIO_LOG("headset_det interrupt init failed");
        ql_rtos_task_delete(NULL);
    }

    ql_LvlMode headsetdet_value;

    ql_gpio_get_level(QL_HEADSET_DETECT_GPIO, &headsetdet_value);
    if( headsetdet_value == LVL_LOW )       //detect plug_in
    {
        QL_AUDIO_LOG("start up headset plug_in");
        ql_set_audio_path_earphone();
    }
    else/* headsetdet_value == LVL_HIGH */  //detect plug_out
    {
        QL_AUDIO_LOG("start up headset plug_out");
        ql_set_audio_path_speaker();
    }

    while(1)
    {
        if( ql_event_try_wait(&event) != 0 )
        {
            continue;
        }

        if( event.id == QUEC_HEADSET_DET_EVENT_IND )
        {
            if( event.param1 == DEMO_HEADSET_DETECT_PLUG_IN )
            {
                QL_AUDIO_LOG("headset_det detect plug_in");
                ql_aud_player_pause();
                ql_set_audio_path_earphone();
                ql_aud_player_resume();
            }
            else if( event.param1 == DEMO_HEADSET_DETECT_PLUG_OUT )
            {
                QL_AUDIO_LOG("headset_det detect plug_out");
                ql_aud_player_pause();
                ql_set_audio_path_speaker();
                ql_aud_player_resume();
            }
        }
    }

    ql_rtos_task_delete(NULL);
}

void ql_headset_det_app_init(void)
{
    QlOSStatus err = QL_OSI_SUCCESS;

    err = ql_rtos_task_create(&headset_det_task, 1024, APP_PRIORITY_NORMAL, "ql_headset_demo", ql_headset_det_demo_thread, NULL, 1);
    if( err != QL_OSI_SUCCESS )
    {
        QL_AUDIO_LOG("headset_det demo task created failed");
    }

    err = ql_rtos_timer_create(&headset_det_debounce_timer, headset_det_task, _ql_headset_detect_callback, NULL);
    if( err != QL_OSI_SUCCESS )
    {
        QL_AUDIO_LOG("headset_det demo timer created failed");
    }

}
#endif

