#ifndef _QL_AUDIO_H_
#define _QL_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ql_api_osi.h"
#include "ql_api_common.h"
#include "ql_codec_config.h"


#define QL_PCM_BLOCK_FLAG		(0x01)
#define QL_PCM_NONBLOCK_FLAG	(0x02)
#define QL_PCM_READ_FLAG    	(0x04)
#define QL_PCM_WRITE_FLAG		(0x08)

#define QL_AUDIO_IIC_CHANNEL     i2c_2
#define PACKET_WRITE_MAX_SIZE 	 3*1024

typedef enum
{
    AUDIOHAL_SPK_MUTE         = 0,
    AUDIOHAL_SPK_VOL_1,	     
    AUDIOHAL_SPK_VOL_2,
    AUDIOHAL_SPK_VOL_3,
    AUDIOHAL_SPK_VOL_4,
    AUDIOHAL_SPK_VOL_5,
    AUDIOHAL_SPK_VOL_6,
    AUDIOHAL_SPK_VOL_7,
    AUDIOHAL_SPK_VOL_8,
    AUDIOHAL_SPK_VOL_9,
    AUDIOHAL_SPK_VOL_10,
    AUDIOHAL_SPK_VOL_11,	// 11

	AUDIOHAL_SPK_VOL_QTY	// 12
} AUDIOHAL_SPK_LEVEL_T;
 
/*
	for internal codec uplink mic adc gain in voice
	call mode or POC mode
*/
typedef enum
{
	QL_ICMIC_ADC_GAIN_MUTE	= 0,	// ==> MUTE
	QL_ICMIC_ADC_GAIN_LEVEL_1,		// ==> -12db
	QL_ICMIC_ADC_GAIN_LEVEL_2,		// ==> -10db
	QL_ICMIC_ADC_GAIN_LEVEL_3,		// ==> -8db
	QL_ICMIC_ADC_GAIN_LEVEL_4,		// ==> -6db
	QL_ICMIC_ADC_GAIN_LEVEL_5,		// ==> -4db
	QL_ICMIC_ADC_GAIN_LEVEL_6,		// ==> -2db
	QL_ICMIC_ADC_GAIN_LEVEL_7,		// ==>  0db
	QL_ICMIC_ADC_GAIN_LEVEL_8,		// ==>  2db
	QL_ICMIC_ADC_GAIN_LEVEL_9,		// ==>  4db
	QL_ICMIC_ADC_GAIN_LEVEL_10,		// ==>  6db
	QL_ICMIC_ADC_GAIN_LEVEL_11,		// ==>  8db
	QL_ICMIC_ADC_GAIN_LEVEL_12,		// ==>  10db
	QL_ICMIC_ADC_GAIN_LEVEL_13,		// ==>  12db
	QL_ICMIC_ADC_GAIN_LEVEL_14,		// ==>  14db
	QL_ICMIC_ADC_GAIN_LEVEL_15,		// ==>  16db
	
	QL_ICMIC_ADC_GAIN_LEVEL_MAX,
}QL_ICMIC_ADC_GAIN_E;        

/*
	for internal codec uplink mic analog gain in voice
	call mode or POC mode
*/
typedef enum
{
	QL_ICMIC_ANA_GAIN_LEVEL_0 = 0,	// ==>  0db
	QL_ICMIC_ANA_GAIN_LEVEL_1,		// ==>  3db
	QL_ICMIC_ANA_GAIN_LEVEL_2,		// ==>  6db
	QL_ICMIC_ANA_GAIN_LEVEL_3,		// ==>  12db
	QL_ICMIC_ANA_GAIN_LEVEL_4,		// ==>  18db
	QL_ICMIC_ANA_GAIN_LEVEL_5,		// ==>  24db
	QL_ICMIC_ANA_GAIN_LEVEL_6,		// ==>  30db
	QL_ICMIC_ANA_GAIN_LEVEL_7,		// ==>  36db

	QL_ICMIC_ANA_GAIN_LEVEL_MAX,
}QL_ICMIC_ANA_GAIN_E; 

/*
	for internal codec side tone gain in voice call mode or POC mode
*/
typedef enum
{
	QL_ICMIC_SIDET_GAIN_MUTE = 0,	//	==> MUTE
	QL_ICMIC_SIDET_GAIN_LEVEL_1,  	//  ==> -36db
	QL_ICMIC_SIDET_GAIN_LEVEL_2,	//  ==> -33db	
	QL_ICMIC_SIDET_GAIN_LEVEL_3,	//  ==> -30db
	QL_ICMIC_SIDET_GAIN_LEVEL_4,	//  ==> -27db
	QL_ICMIC_SIDET_GAIN_LEVEL_5,	//  ==> -24db
	QL_ICMIC_SIDET_GAIN_LEVEL_6,	//  ==> -21db
	QL_ICMIC_SIDET_GAIN_LEVEL_7,	//  ==> -18db
	QL_ICMIC_SIDET_GAIN_LEVEL_8,	//  ==> -15db
	QL_ICMIC_SIDET_GAIN_LEVEL_9,	//  ==> -12db
	QL_ICMIC_SIDET_GAIN_LEVEL_10,	//  ==> -9db
	QL_ICMIC_SIDET_GAIN_LEVEL_11,	//  ==> -6db
	QL_ICMIC_SIDET_GAIN_LEVEL_12,	//  ==> -3db
	QL_ICMIC_SIDET_GAIN_LEVEL_13,	//  ==>  0db
	QL_ICMIC_SIDET_GAIN_LEVEL_14,	//  ==>  3db
	QL_ICMIC_SIDET_GAIN_LEVEL_15,	//  ==>  6db
	
	QL_ICMIC_SIDET_GAIN_LEVEL_MAX,
}QL_ICSIDET_GAIN_E; 

typedef enum
{
	QL_AUDIO_SUCCESS 			    = 0,

	QL_AUDIO_UNKNOWN_ERROR		    = (QL_COMPONENT_AUDIO << 16) | 901,
	QL_AUDIO_INVALID_PARAM		    = (QL_COMPONENT_AUDIO << 16) | 902,
	QL_AUDIO_OPER_NOT_SUPPORTED     = (QL_COMPONENT_AUDIO << 16) | 903,
	QL_AUDIO_DEVICE_BUSY		    = (QL_COMPONENT_AUDIO << 16) | 904,
	QL_AUDIO_FILE_NOT_EXIST 	    = (QL_COMPONENT_AUDIO << 16) | 905,
	QL_AUDIO_FREE_SIZE_NOT_ENOUGH   = (QL_COMPONENT_AUDIO << 16) | 906,
	QL_AUDIO_NO_MEMORY  		    = (QL_COMPONENT_AUDIO << 16) | 907, 			
	QL_AUDIO_SET_PATH_FAIL  	    = (QL_COMPONENT_AUDIO << 16) | 908,
	QL_AUDIO_FILE_IN_OPERATION      = (QL_COMPONENT_AUDIO << 16) | 909,
	QL_AUDIO_ERROR_AUDIO_PATH       = (QL_COMPONENT_AUDIO << 16) | 910,
	QL_AUDIO_FILE_TYPE_NOT_SUPPORT  = (QL_COMPONENT_AUDIO << 16) | 911,
	QL_AUDIO_DEVICE_NOT_EXIST 	    = (QL_COMPONENT_AUDIO << 16) | 912,
	QL_AUDIO_PLAY_FAIL              = (QL_COMPONENT_AUDIO << 16) | 913,
	QL_AUDIO_PARAM_SET_FAIL         = (QL_COMPONENT_AUDIO << 16) | 914,
	QL_AUDIO_OPEN_FILE_FAIL         = (QL_COMPONENT_AUDIO << 16) | 915,
	QL_AUDIO_RECORD_SYS_FAIL 		= (QL_COMPONENT_AUDIO << 16) | 916,
	QL_AUDIO_PLAYER_SYS_FAIL 		= (QL_COMPONENT_AUDIO << 16) | 917,
	QL_AUDIO_END_FAIL 				= (QL_COMPONENT_AUDIO << 16) | 918,
	QL_AUDIO_NOT_INIT				= (QL_COMPONENT_AUDIO << 16) | 919,

//codec
	QL_AUDIO_CODEC_WR_FAIL          = (QL_COMPONENT_AUDIO << 16) | 2000,
	QL_AUDIO_CODEC_RD_FAIL			= (QL_COMPONENT_AUDIO << 16) | 2001,
	QL_AUDIO_CODEC_INIT_FAIL		= (QL_COMPONENT_AUDIO << 16) | 2002,
}ql_audio_errcode_e;

typedef enum
{
	QL_AUDIO_RINGTONE_PLAY		    = (QL_COMPONENT_AUDIO << 16) | 4000,
}ql_audio_event_e;

typedef enum
{
	AUD_RECORD_ERROR = -1,
	AUD_RECORD_START = 0,   //audio recorder start record
	AUD_RECORD_DATA,		//record data comming
	AUD_RECORD_PAUSE,	    //record has been paused by call or pause api
	AUD_RECORD_CLOSE,		//record closed
	AUD_RECORD_RESUME,		//resume from pause
}enum_aud_record_state;

typedef enum
{
	AUD_PLAYER_ERROR = -1,
	AUD_PLAYER_START = 0,	//audio player start play
	AUD_PLAYER_PAUSE,		//audio player has been paused by call or pause api
	AUD_PLAYER_FINISHED,    //audio player finish current playing
	AUD_PLAYER_CLOSE,       //audio player closed
	AUD_PLAYER_RESUME,      //resume player from pause
}enum_aud_player_state;

typedef enum
{
    QL_AUDIO_PLAY_TYPE_NONE = 0,
    QL_AUDIO_PLAY_TYPE_LOCAL,
    QL_AUDIO_PLAY_TYPE_VOICE,
    QL_AUDIO_PLAY_TYPE_POC,
    QL_AUDIO_PLAY_TYPE_MAX
} AudPlayType_e;

typedef enum
{
    QL_OUTPUT_RECEIVER = 0,  ///< receiver
    QL_OUTPUT_HEADPHONE = 1, ///< headphone
    QL_OUTPUT_SPEAKER = 2,   ///< speaker

	QL_OUTPUT_MAX,
} AudOutputType_e;

typedef enum
{
    QL_INPUT_MAINMIC = 0, ///< main mic
    QL_INPUT_AUXMIC  = 1,  ///< auxilary mic
    QL_INPUT_DUALMIC = 2, ///< dual mic
    QL_INPUT_HPMIC_L = 3, ///< headphone mic left
    QL_INPUT_HPMIC_R = 4, ///< headphone mic right
} AudInputType_e;

typedef enum
{
    QL_AUDIO_FORMAT_UNKNOWN, ///< placeholder for unknown format
    QL_AUDIO_FORMAT_PCM,     ///< raw PCM data
    QL_AUDIO_FORMAT_WAVPCM,  ///< WAV, PCM inside
    QL_AUDIO_FORMAT_MP3,     ///< MP3
    QL_AUDIO_FORMAT_AMRNB,   ///< AMR-NB
    QL_AUDIO_FORMAT_AMRWB,   ///< AMR_WB

	QL_AUDIO_FORMAT_MAX
} AudStreamFormat_e;


typedef enum
{
	AMRNB_MODE_NONE = 0,

    AMRNB_MODE_475, 	///< 4.75 kbps
    AMRNB_MODE_515,     ///< 5.15 kbps
    AMRNB_MODE_590,     ///< 5.90 kbps
    AMRNB_MODE_670,     ///< 6.70 kbps
    AMRNB_MODE_740,     ///< 7.40 kbps
    AMRNB_MODE_795,     ///< 7.95 kbps
    AMRNB_MODE_1020,    ///< 10.2 kbps
    AMRNB_MODE_1220,    ///< 12.2 kbps

	AMRNB_MODE_MAX,
} amrnb_mode_e;

/**
 * \brief AMR-WB modes
 */
typedef enum
{
	AMRWB_MODE_NONE = 0,
		
    AMRWB_MODE_660, 	///< 6.60 kbps
    AMRWB_MODE_885,     ///< 8.85 kbps
    AMRWB_MODE_1265,    ///< 12.65 kbps
    AMRWB_MODE_1425,    ///< 14.25 kbps
    AMRWB_MODE_1585,    ///< 15.85 kbps
    AMRWB_MODE_1825,    ///< 18.25 kbps
    AMRWB_MODE_1985,    ///< 19.85 kbps
    AMRWB_MODE_2305,    ///< 23.05 kbps
    AMRWB_MODE_2385,    ///< 23.85 kbps

	AMRWB_MODE_MAX,
} amrwb_mode_e;

typedef enum
{
	QL_AUD_VOICECALL_NB	= 0,	//not volte mode
	QL_AUD_VOICECALL_WB = 5		//volte mode
}QL_AUD_CALLMODE_E;

typedef enum
{
    QL_AUDIO_STATUS_IDLE,     ///< playback not started
    QL_AUDIO_STATUS_RUNNING,  ///< playback is started
    QL_AUDIO_STATUS_PAUSE,    ///< playback is paused
    QL_AUDIO_STATUS_FINISHED, ///< playback is finished
} AudioStatus_e;

typedef enum
{
	QL_AUDIO_PLAYER_IDLE,	//audio player is in idle state

/*
	audio is playing file now, started 
	by ql_aud_play_file_start
*/	
	QL_AUDIO_IS_PLAYING_FILE,	  
	
/*
	audio is playing stream now, started 
	by ql_aud_play_stream_start
*/
	QL_AUDIO_IS_PLAYING_STREAM,

/*
	audio is playing stream now, started 
	by ql_pcm_open, ql_aud_pcm_open, and flag is write, include tts
*/
	QL_AUDIO_IS_PLAYING_PCM,

/*
	audio is playing DTMF ; not support now
*/
	QL_AUDIO_IS_PLAYING_DTMF,
}QL_PLAYING_FUNCTION_E;


typedef enum
{
	QL_AUDIO_RECORDER_IDLE,			  //audio recorder is in idle state

/*
	audio is recording file now, started 
	by ql_aud_record_file_start
*/	
	QL_AUDIO_IS_RECORDING_FILE,	 

/*
	audio is recording stream now, started 
	by ql_aud_record_stream_start
*/
	QL_AUDIO_IS_RECORDING_STREAM,

/*
	audio is recording pcm now, started 
	by ql_pcm_open, ql_aud_pcm_open, and flag is read
*/
	QL_AUDIO_IS_RECORDING_PCM,
}QL_RECORDING_FUNCTION_E;


typedef enum
{
    QL_REC_TYPE_NONE = 0,
    QL_REC_TYPE_MIC,		//uplink
    QL_REC_TYPE_VOICE,		//downlink
    QL_REC_TYPE_VOICE_DUAL,	//DUAL uplink and downlink
    QL_REC_TYPE_DEBUG_DUMP,	//invalid now
    QL_REC_TYPE_POC,		//used in POC mode

	QL_REC_TYPE_MAX,
} AudRecType_e;

typedef enum
{
	QL_PCM_NONE = 0,
	QL_PCM_LOCAL,
	QL_PCM_VOICE_CALL,
	QL_PCM_POC,

	QL_PCM_MAX,
}pcm_path_e;

typedef enum
{
	QL_POC_TYPE_MIN = -1,

	QL_POC_TYPE_HALF_DUPLEX,
	QL_POC_TYPE_FULL_DUPLEX,

	QL_POC_TYPE_MAX,
}QL_POC_TYPE_E;

typedef enum
{
	QL_POC_MODE_MIN = 0,

	QL_POC_MODE_REC,
	QL_POC_MODE_PLAY,

	QL_POC_MODE_MAX,
}QL_POC_MODE_E;


typedef enum
{
    QL_AUD_SPKPA_TYPE_CLASSAB,
    QL_AUD_SPKPA_TYPE_CLASSD,
    
    QL_AUD_SPKPA_TYPE_MAX
} QL_AUD_SPKPA_TYPE_E;

//ex codec
typedef enum
{
    QL_AIF_RX_DELAY_ALIGN,
    QL_AIF_RX_DELAY_1,
    QL_AIF_RX_DELAY_2,
    QL_AIF_RX_DELAY_3,
    QL_AIF_RX_DELAY_QTY
} QL_AIF_RX_DELAY_E;

typedef enum
{
    QL_AIF_RX_MODE_STEREO_STEREO,
    QL_AIF_RX_MODE_STEREO_MONO_FROM_L,
    QL_AIF_RX_MODE_QTY
} QL_AIF_RX_MODE_E;

typedef enum
{
    QL_SERIAL_MODE_I2S,
    QL_SERIAL_MODE_VOICE,
    QL_SERIAL_MODE_DAI,
    QL_SERIAL_MODE_QTY
} QL_SERIAL_MODE_E;

typedef enum
{
    QL_AIF_TX_DELAY_ALIGN,
    QL_AIF_TX_DELAY_1,
    QL_AIF_TX_DELAY_QTY
} QL_AIF_TX_DELAY_E;

typedef enum
{
    QL_AIF_TX_MODE_STEREO_STEREO,
    QL_AIF_TX_MODE_MONO_STEREO_CHAN_L,
    QL_AIF_TX_MODE_MONO_STEREO_DUPLI,
    QL_AIF_TX_MODE_QTY
} QL_AIF_TX_MODE_E;

typedef enum
{
    QL_CODEC_CLK_FREQ_12M = 12,
    QL_CODEC_CLK_FREQ_13M = 13,
    QL_CODEC_CLK_FREQ_24M = 24,
    QL_CODEC_CLK_FREQ_26M = 26,
    QL_CODEC_CLK_FREQ_MAX
} QL_CODEC_CLK_FREQ_E;

typedef enum
{
	QL_AUD_RING_NONE  =  0,    //no ring tone
	QL_AUD_RING_NOKIA,		   //invalid now
	QL_AUD_RING_DIAL_TONE,
	QL_AUD_RING_CUSTOMER_DEF,
		
	QL_AUD_RING_MAX,
}QL_AUD_RING_TYPE_E;

/**********************************************************************************************
								       Typedef
**********************************************************************************************/
typedef struct
{
	amrwb_mode_e amrwb_mode;
	int reserve[2];
}amrwb_param_t;

typedef struct
{
	amrnb_mode_e amrnb_mode;
	int reserve[2];
}amrnb_param_t;


typedef struct
{
	int channels;
	int samplerate;
	int len_size;

	amrwb_param_t amrwb_param;
	amrnb_param_t amrnb_param;
}ql_aud_config;

typedef void    *PCM_HANDLE_T ;

typedef struct {
	unsigned int channels;
	unsigned int samplerate;
	unsigned int periodcnt; // 20ms buffersize/per period, value:2~8

	amrwb_param_t amrwb_param;
	amrnb_param_t amrnb_param;
} QL_PCM_CONFIG_T;

//EXT PA
typedef void (*cb_on_speakerpa) (uint8 on);
typedef void (*cb_on_receiverpa) (uint8 on);

typedef struct
{
	bool spk_pa_start;
	bool rcv_pa_start;
	cb_on_speakerpa spk_pa_cb;
	cb_on_receiverpa rcv_pa_cb;
}ext_pa_info_t;

//ext codec
typedef struct
{
    int32_t id;
    uint32_t freq;
    int32_t clockMode;
} AUD_I2C_CFG_T;

typedef struct
{
    uint32_t regAddr;
    uint16_t val;
    uint16_t delay;
} AUD_CODEC_REG_T;

typedef struct
{
    QL_SERIAL_MODE_E mode;
    bool aifIsMaster;
    bool lsbFirst;
    bool polarity;
    QL_AIF_RX_DELAY_E rxDelay;
    QL_AIF_TX_DELAY_E txDelay;
    QL_AIF_RX_MODE_E rxMode;
    QL_AIF_TX_MODE_E txMode;
    uint32_t fs;
    uint32_t bckLrckRatio;
    bool invertBck;
    bool outputHalfCycleDelay;
    bool inputHalfCycleDelay;
    bool enableBckOutGating;
}AUD_AIF_SERIAL_CFG_T;

typedef struct
{
    uint32_t codecAddr;
    uint16_t codecIsMaster;
    uint16_t dataFormat;
    uint16_t reserved;
	uint16_t iic_data_width;
    uint16_t initFlag;
    uint16_t externalCodec;
}AUD_BASIC_CFG_T;

typedef struct
{
	AUD_BASIC_CFG_T	basicCfg;
    uint16_t reserved[24];
    AUD_I2C_CFG_T i2cCfg;
    AUD_AIF_SERIAL_CFG_T i2sAifcfg;
    AUD_CODEC_REG_T initRegCfg[100];
    AUD_CODEC_REG_T closeRegCfg[50];
    AUD_CODEC_REG_T sampleRegCfg[10][12];
    AUD_CODEC_REG_T inpathRegCfg[6][20];
    AUD_CODEC_REG_T outpathRegCfg[4][20];
} QL_HAL_CODEC_CFG_T;

typedef int(*cb_on_player)(char *p_data, int len, enum_aud_player_state state);
typedef int(*cb_on_record)(char *p_data, int len, enum_aud_record_state state);
typedef int(*cb_on_ring_tone)(bool start, void *ctx);

/**********************************************************************************************
					API (support local and voice call play, record)
**********************************************************************************************/

/*****************************************************************
* Function: ql_get_audio_state
*
* Description: get audio initial state
* 
* Parameters:
*
* Return:
* 	1: initial ok
*	0: have not initialed ok, or initial failed
*****************************************************************/
int ql_get_audio_state(void);

/*****************************************************************
* Function: ql_get_file_format
*
* Description: get format of file
* 
* Parameters:
* 	file_name	  		[in] file name
*
* Return:
* 	AUSTREAM_FORMAT_UNKNOWN : unknown format 
*	others : file format 
*****************************************************************/
AudStreamFormat_e ql_get_file_format(char *file_name);

/*****************************************************************
* Function: ql_aud_set_volume
*
* Description: set the play volume
* 
* Parameters:
* 	type	  		[in] play type: QL_AUDIO_PLAY_TYPE_LOCAL, QL_AUDIO_PLAY_TYPE_VOICE
*	volume          [in] play volume, range is 0~AUDIOHAL_SPK_VOL_11
*
* Return:
* 	0:         		successful
* 	others          ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_volume(AudPlayType_e type, AUDIOHAL_SPK_LEVEL_T volume);

/*****************************************************************
* Function: ql_aud_get_volume
*
* Description: get the play volume
* 
* Parameters:
* 	type	  		[in] play type: QL_AUDIO_PLAY_TYPE_LOCAL, QL_AUDIO_PLAY_TYPE_VOICE
*
* Return:
* 	volume:         play volume(0~AUDIOHAL_SPK_VOL_11)
*   others:			ql_audio_errcode_e
*****************************************************************/
AUDIOHAL_SPK_LEVEL_T ql_aud_get_volume(AudPlayType_e type);

/*****************************************************************
* Function: ql_aud_play_file_start
*
* Description: play file
* 
* Parameters:
* 	type	  		[in] play type: QL_AUDIO_PLAY_TYPE_LOCAL, QL_AUDIO_PLAY_TYPE_VOICE is valid
*	file_name       [in] file name, full path required
*	play_cb         [in] The callback function, will be called at the beginning and end of the play
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_play_file_start(char *file_name, AudPlayType_e type, cb_on_player play_cb);

/*****************************************************************
* Function: ql_aud_play_stream_start
*
* Description: play audio stream data, no more than 3K at a time
* 
* Parameters:
* 	type	  		[in] play type: QL_AUDIO_PLAY_TYPE_LOCAL, QL_AUDIO_PLAY_TYPE_VOICE is valid
*	format          [in] stream format: wav/mp3/amr/pcm
*	play_cb         [in] The callback function, will be called at the beginning and end of the play
*   buf             [in] Audio data address, cannot be released until the end of playback
*	size            [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_play_stream_start(AudStreamFormat_e format, const void *buf, unsigned size, AudPlayType_e type, cb_on_player play_cb);

/*****************************************************************
* Function: ql_aud_data_done
*
* Description: to tell module that no data need to be write, player will stop after cache empty，
			   used for stream media, include ql_aud_play_file_start, ql_pcm_write. after use this
			   api, new data cannot be write to kernel
* 
* Parameters:
* 	
* Return: none
*****************************************************************/
void ql_aud_data_done(void);

/*****************************************************************
* Function: ql_aud_player_pause
*
* Description: play pause, used for all play function
* 
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_pause(void);

/*****************************************************************
* Function: ql_aud_player_resume
*
* Description: play resume, play pause, used for all play function
* 
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_resume(void);

/*****************************************************************
* Function: ql_aud_player_stop
*
* Description: Stop playing, this function will release the playing 
  			   resource (due to the need to communicate with CP ,It will takes 
  			   50~70ms to execute this function), cannot use it when in interrupt, 
  			   and in critiacl
  			   
* Attention: this function can stop playing started by "ql_pcm_open", "ql_aud_pcm_open"
* Parameters:
* 	
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_player_stop(void);

/*****************************************************************
* Function: ql_aud_get_play_state
*
* Description: get current play state, if player has started, can 
			   call ql_aud_get_playing_func for detail
* 
* Parameters:
* 	
* Return:
* 	AudioStatus_e:  play state
*   others:         ql_audio_errcode_e
*****************************************************************/
AudioStatus_e ql_aud_get_play_state(void);

/*****************************************************************
* Function: ql_aud_get_playing_func
*
* Description:  get which function has called player
  if called ql_aud_play_file_start and has not called ql_aud_player_stop, 
    it will return QL_AUDIO_IS_PLAYING_FILE;
  if called ql_aud_play_stream_start and has not called ql_aud_player_stop, 
    it will return QL_AUDIO_IS_PLAYING_STREAM;
  if called ql_aud_pcm_open or ql_pcm_open, flag is write, and has not called ql_pcm_close,
    it will return QL_AUDIO_IS_PLAYING_PCM
  if no audio playing functions has been called, it will return QL_AUDIO_PLAYER_IDLE
*
* Parameters:
* 	
* Return:
* 	QL_PLAYING_FUNCTION_E
*****************************************************************/
QL_PLAYING_FUNCTION_E ql_aud_get_playing_func(void);

/*****************************************************************
* Function: ql_set_record_packet_size
*
* Description: Set the packet size read by the recording callback function each time
* 
* Parameters:
* 	packet_size	  	[in] packet size; for amrwb, one packet is 41 bytes; for amrnb, one packet is 21 bytes; for PCM, 
*			 		one packet is 320bytes in 8000HZ samprate, is 640 bytes in 16000HZ, is 1280bytes
*			 		in 16000HZ, 2 channels. No more than 1280 
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_record_packet_size(int packet_size);

/*****************************************************************
* Function: ql_aud_record_file_start
*
* Description: record to file
* 
* Parameters:
* 	type	  		[in] record type: QL_REC_TYPE_MIC , QL_REC_TYPE_VOICE
*	file_name       [in] file name, full path required
*	play_cb         [in] The callback function, will be called at the beginning and end of the record
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_file_start(char *file_name, ql_aud_config *config, AudRecType_e type,  cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_record_stream_start
*
* Description: record to memory,format is PCM
* 
* Parameters:
* 	type	  		[in] record type: QL_REC_TYPE_MIC , QL_REC_TYPE_VOICE
*	play_cb         [in] The callback function, will be called at the beginning, end of the record, and data comming 
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stream_start(ql_aud_config *config, AudRecType_e type, cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_record_stream_start_ex
*
* Description: record to memory, and format need be pointed
* 
* Parameters:
* 	type	  		[in] record type: QL_REC_TYPE_MIC , QL_REC_TYPE_VOICE
*	play_cb         [in] The callback function, will be called at the beginning, end of the record, and data comming 
*	config          [in] Recording configuration, default 8K sampling rate, single channel
*	format			[in] record format
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stream_start_ex(ql_aud_config *config, AudRecType_e type, AudStreamFormat_e format, cb_on_record record_cb);

/*****************************************************************
* Function: ql_aud_wait_play_finish
*
* Description: Wait for the playback to complete (blocks the current task until the playback is complete)
* 
* Parameters:
* 	timeout         [in] waiting timeout
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_wait_play_finish(int timeout);

/*****************************************************************
* Function: ql_aud_get_record_state
*
* Description: get current record state, can call ql_aud_get_recording_func for detail
* 
*
* Return:
	AudioStatus_e: state
	others:         ql_audio_errcode_e
*****************************************************************/
AudioStatus_e ql_aud_get_record_state(void);


/*****************************************************************
* Function: ql_aud_get_recording_func
*
* Description: get which function has called recorder
  if called ql_aud_record_file_start and has not called ql_aud_record_stop, 
    it will return QL_AUDIO_IS_RECORDING_FILE;
  if called ql_aud_record_stream_start and has not called ql_aud_player_stop, 
    it will return QL_AUDIO_IS_RECORDING_STREAM;
  if called ql_aud_pcm_open or ql_pcm_open, flag is read, and has not called ql_pcm_close,
    it will return QL_AUDIO_IS_RECORDING_PCM
  if no audio playing functions has been called, it will return QL_AUDIO_RECORDER_IDLE
*
* Parameters:
* 	
* Return:
* 	QL_RECORDING_FUNCTION_E
*****************************************************************/
QL_RECORDING_FUNCTION_E ql_aud_get_recording_func(void);

/*****************************************************************
* Function: ql_aud_recorder_pause
*
* Description: record pause, used for all record function
* 
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_recorder_pause(void);

/*****************************************************************
* Function: ql_aud_recorder_resume
*
* Description: resume record, used for all record function
* 
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_recorder_resume(void);

/*****************************************************************
* Function: ql_aud_record_stop
*
* Description: Stop recording, this function will release the recording 
  			   resource (due to the need to communicate with CP ,It will takes 
  			   50~70ms to execute this function)
*
* Attention: this function cannot stop recording started by "ql_pcm_open", "ql_aud_pcm_open" 
*
* Parameters:
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_record_stop(void);


/*****************************************************************
* Function: ql_play_file_start
*
* Description: play file locally
* 
* Parameters:
*	file_name       [in] file name, full path required
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_file_start(char *file_name);

/*****************************************************************
* Function: ql_play_mp3_stream_start
*
* Description: play audio stream data locally, no more than 3K at a time
* 
* Parameters:
*   mp3_buff        [in] Audio data address, cannot be released until the end of playback
*	mp3_size        [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_mp3_stream_start(unsigned char *mp3_buff, int mp3_size );

/*****************************************************************
* Function: ql_set_volume
*
* Description: set the volume level of local play
* 
* Parameters:
*	vol             [in] volume level
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_volume(AUDIOHAL_SPK_LEVEL_T vol);

/*****************************************************************
* Function: Avoid jamming caused by frequent volume adjustment during playback
*
* Description: set the volume level of local play, but it will not be set immediately,
			   when call this api and "delay" is valid, kernel will start a timer, and
			   the volume will be set when timer timeout. if this api called again when timer
			   is running, kernel will note the volume and timer will not be restart.
* 
* Parameters:
*	vol             [in] volume level
*   delay			[in] timer timeout value, should be larger than 0, Unit: ms
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_volume_ex(AUDIOHAL_SPK_LEVEL_T vol, uint delay);

/*****************************************************************
* Function: ql_get_volume
*
* Description:      Gets the volume level of local play
*
* Return:
* 	0~11:         	volume level
*   others:         ql_audio_errcode_e
*****************************************************************/
AUDIOHAL_SPK_LEVEL_T ql_get_volume(void);

/*****************************************************************
* Function: ql_aud_get_output_type
*
* Description: 		get output path, speaker, earphone, or receiver
*
* Return:
* 	AudOutputType_e
****************************************************************/
AudOutputType_e ql_aud_get_output_type(void);

/*****************************************************************
* Function: ql_aud_get_input_type
*
* Description: 		get input path, main mic, HPL, ....
*
* Return:
* 	AudInputType_e
****************************************************************/
AudInputType_e ql_aud_get_input_type(void);

/*****************************************************************
* Function: ql_aud_set_output_type
*
* Description: 		set output path to speaker, earphone, or receiver
* Parameters:
*	output_type     [in] output path, speaker, earphone, or receiver
* Return:
* 	ql_audio_errcode_e
****************************************************************/
ql_audio_errcode_e ql_aud_set_output_type(AudOutputType_e output_type);

/*****************************************************************
* Function: ql_aud_set_input_type
*
* Description: 		set input path, main mic, HPL, ....
* Parameters:
*	output_type     [in] input path, main mic, HPL, ....
* Return:
* 	ql_audio_errcode_e
****************************************************************/
ql_audio_errcode_e ql_aud_set_input_type(AudInputType_e input_type);

/*****************************************************************
* Function: ql_set_audio_path_receiver
*
* Description:	    set output mode to receiver, input type 
					to main mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_receiver(void);

/*****************************************************************
* Function: ql_set_audio_path_earphone
*
* Description: 	    set output mode to earphone, input type 
					to main HPL mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_earphone(void);

/*****************************************************************
* Function: ql_set_audio_path_speaker
*
* Description: 		set output mode to loud speaker, input type 
					to main mic
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_set_audio_path_speaker(void);


/*****************************************************************
* Function: ql_play_file_stop
*
* Description: stop play file
*
* Return:
* 	0:         		successful
*   others:         ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_play_file_stop(void);

/*****************************************************************
* Function: ql_aud_pcm_open
*
* Description: Enable local play/recording, format can be amr, wav, mp3, pcm
*
* Parameters:
*	config       [in] play/record configuration
*   flags        
	QL_PCM_BLOCK_FLAG		(0x01): block
	QL_PCM_NONBLOCK_FLAG	(0x02): not block
	QL_PCM_READ_FLAG    	(0x04): record
	QL_PCM_WRITE_FLAG		(0x08): play
*
* notice: in read mode, it will start record after ql_aud_pcm_open!!!
*
* Return:
* 	NULL:        failed
* 	others:      PCM handler
*****************************************************************/
PCM_HANDLE_T ql_aud_pcm_open(QL_PCM_CONFIG_T *config, AudStreamFormat_e format, unsigned int flags, pcm_path_e type);

/*****************************************************************
* Function: ql_pcm_open
*
* Description: Enable local play/recording, format is PCM
*
* Parameters:
*	config       [in] play/record configuration
*   flags        
	QL_PCM_BLOCK_FLAG		(0x01): block
	QL_PCM_NONBLOCK_FLAG	(0x02): not block
	QL_PCM_READ_FLAG    	(0x04): record
	QL_PCM_WRITE_FLAG		(0x08): play
*
* notice: in read mode, it will start record after ql_pcm_open!!!
*
* Return:
* 	NULL:        failed
* 	others:      PCM handler
*****************************************************************/
PCM_HANDLE_T ql_pcm_open(QL_PCM_CONFIG_T *config, unsigned int flags);

/*****************************************************************
* Function: ql_pcm_write
*
* Description: play data of pcm format locally, no more than 3K at a time
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open, or ql_aud_pcm_open
*   data         [in] PCM data address, cannot be released until the end of playback
*	count        [in] data size, no more than 3K at a time
*
* Return:
* 	>0:          actual write size
* 	others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_write(PCM_HANDLE_T handle, void *data, unsigned int count);

/*****************************************************************
* Function: ql_pcm_read
*
* Description: get recording data locally
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open, or ql_aud_pcm_open
*   data         [in] PCM data address, cannot be released until the end of playback
*	count        [in] data size
* Return:
* 	>0:          actual read size
* 	others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_read(PCM_HANDLE_T handle, void *data, unsigned int count);

/*****************************************************************
* Function: ql_pcm_close
*
* Description: close pcm record/play
*
* Parameters:
*	handle       [in] PCM handler, get by ql_pcm_open
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
int ql_pcm_close(PCM_HANDLE_T handle);

/*****************************************************************
* Function: ql_pcm_buffer_reset
*
* Description: reset the pcm record/play buffer
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open or ql_aud_pcm_open
* Return:
* 	=0:          success
* 	others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_pcm_buffer_reset(PCM_HANDLE_T handle);


/*****************************************************************
* Function: ql_pcm_buffer_used
*
* Description: get the size that buffer used
*
* Parameters:
*	handle       [in] PCM handler，get by ql_pcm_open or ql_aud_pcm_open
* Return:
* 	>=0:         used size
* 	<0:			 error
*****************************************************************/
int ql_pcm_buffer_used(PCM_HANDLE_T handle);


/*****************************************************************
* Function: ql_amr_stream_write
*
* Description: play amr stream locally, no more than 3K at a time
*
* Parameters:
*   data         [in] amr data address, cannot be released until the end of playback
*	count        [in] stream size, no more than 3K at a time
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_stream_write(void *data, unsigned int count);

/*****************************************************************
* Function: ql_amr_stream_close
*
* Description:   close amr play, and release play resource
*
* Return:
* 	0:         	 successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_stream_close(void);

/*****************************************************************
* Function: ql_amr_file_start
*
* Description: play amr file locally
*
* Parameters:
*	file_name    [in] file name, full path required
*	play_cb      [in] The callback function, will be called at the beginning and end of the play
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_start(char *file_name, cb_on_player aud_cb);

/*****************************************************************
* Function: ql_amr_file_pause
*
* Description:   pause or resume amr player
*
* Parameters:
*	pause        [in] 1 for pause, 0 for resume
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_pause(bool pause);

/*****************************************************************
* Function: ql_amr_file_end
*
* Description:   end amr player
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_amr_file_end(void);

/*****************************************************************
* Function: ql_aud_loop_back_start
*
* Description:   loop test input device and output device
*
* Parameters:
*	out_type     [in] output device
*   in_type 	 [in] input device
*   volume       [in] test volume
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_loop_back_start(AudOutputType_e out_type, AudInputType_e in_type, AUDIOHAL_SPK_LEVEL_T volume);

/*****************************************************************
* Function: ql_aud_loop_back_stop
*
* Description:   stop test loopback
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_loop_back_stop(void);


/*****************************************************************
* Function: ql_aud_set_pa_type
*
* Description:   set default PA type to AB or D, need restart 
*				 to take effect, only for intrenal PA
* Parameters:
*   pa_type	     [in] PA type,  AB or D
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_pa_type(QL_AUD_SPKPA_TYPE_E pa_type);

/*****************************************************************
* Function: ql_aud_get_pa_type
*
* Description:   get the default PA type, only for intrenal PA
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
QL_AUD_SPKPA_TYPE_E ql_aud_get_pa_type(void);

/*****************************************************************
* Function: ql_bind_speakerpa_cb
*
* Description:   set the external pa callback of speaker path,  only for 
				 external PA to avoid POP noise. when player start and stoped,and play 
				 path is speaker, kernal will call the callback to notify users to turn
				 on or turn off external pa		 
*
*****************************************************************/
void ql_bind_speakerpa_cb(cb_on_speakerpa cb);

/*****************************************************************
* Function: ql_bind_receiverpa_cb
*
* Description:   set the external pa callback of receiver path,  only for 
				 external PA to avoid POP noise. when player start and stoped, and play 
				 path is receiver, kernal will call the callback to notify users to turn
				 on or turn off external pa		 
*
*****************************************************************/
void ql_bind_receiverpa_cb(cb_on_receiverpa cb);

#ifdef CONFIG_QUEC_PROJECT_FEATURE_USB_AUDIO
/*****************************************************************
* Function: ql_aud_uac_enable
*
* Description:   to enable usb audio class 1.0, reset to be effective
*
*Parameters:
* enable : whether to enable uac, 1 for enable, 0 for disable
*
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_uac_enable(bool enable);

/*****************************************************************
* Function: ql_aud_uac_is_enabled
*
* Description:   whether the uac is enabled
*
* Return:
* 	0:           uac was disabled
*   1:           uac was enabled
*****************************************************************/
bool ql_aud_uac_is_enabled(void);
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EXT_CODEC

/*****************************************************************
* Function: ql_audio_iic_write
*
* Description:   write register data to codec
*
*Parameters:
*	dev_addr: iic slave addr, for alc5616 is 0x1B 
*	reg_addr: codec register addr
*	size: data size, for 16 bit register is 2, for 8bit register is 1
*	val: data to be write
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_audio_iic_write(uint8 dev_addr, uint16 reg_addr, uint8 size, uint16 val);

/*****************************************************************
* Function: ql_audio_iic_read
*
* Description:   read register data from codec
*
*Parameters:
*	dev_addr: iic slave addr, for alc5616 is 0x1B 
*	reg_addr: codec register addr
*	size: data size, for 16 bit register is 2, for 8bit register is 1
*	val: data to be read
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_audio_iic_read(uint8 dev_addr, uint16 reg_addr, uint8 size, uint16 *p_val);

/*****************************************************************
* Function: ql_aud_codec_write_nv
*
* Description:   write codec nv data, need reset to active
*
*Parameters:
*	nv_cfg: codec nv structure
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_codec_write_nv(QL_HAL_CODEC_CFG_T *nv_cfg);

/*****************************************************************
* Function: ql_aud_codec_read_nv
*
* Description:   read codec nv data
*
*Parameters:
*	nv_cfg: codec nv structure
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_codec_read_nv(QL_HAL_CODEC_CFG_T *nv_cfg);

/*****************************************************************
* Function: ql_aud_codec_clk_enable
*
* Description:   enable 26M codec clock
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_enable(void);

/*****************************************************************
* Function: ql_aud_codec_clk_enable_ex
*
* Description:   enable codec clock with Freq
*
*Parameters:
*	Freq: QL_CODEC_CLK_FREQ_E
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_enable_ex(QL_CODEC_CLK_FREQ_E Freq);

/*****************************************************************
* Function: ql_aud_codec_clk_disable
*
* Description:   disable codec clock
*
* Return: none
*****************************************************************/
void ql_aud_codec_clk_disable(void);

/*****************************************************************
* Function: ql_aud_start_poc_mode
*
* Description:   start POC , this mode use the audio param of voice call, like EQ, AGC, volume.
*				 this function can be used only after recorder and player created together
*
*Parameters:
*	poc_type: 	 half-duplex or full-duplex
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_start_poc_mode(QL_POC_TYPE_E poc_type);

/*****************************************************************
* Function: ql_aud_poc_switch
*
* Description:   switch POC function when in half_duplex mode;
*				 QL_POC_MODE_REC means only record, and QL_POC_MODE_PLAY
				 means only play
*
*Parameters:
*	mode: 		 QL_POC_MODE_REC or QL_POC_MODE_PLAY
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_poc_switch(QL_POC_MODE_E mode);


/*****************************************************************
* Function: ql_aud_stop_poc_mode
*
* Description:  stop poc mode	
*
* Atttention: users must call "ql_aud_stop_poc_mode" then call "ql_pcm_close", otherwise may leed to some errors
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_stop_poc_mode(void);

/*****************************************************************
* Function: ql_bind_ring_tone_cbbind
*
* Description: ring tone callback, when call is comming and released,
			the callback will be run if ring tone type was defined as 
		    customer-def	
*
*Parameters:
*	cb: 	 callbacK, cannot be NULL
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
#ifdef CONFIG_QL_OPEN_EXPORT_PKG
ql_audio_errcode_e ql_bind_ring_tone_cb(cb_on_ring_tone cb);
#endif

/*****************************************************************
* Function: ql_aud_get_ringtone_type
*
* Description:  get ring tone type when call is comming	
*
* Return:
	QL_AUD_RING_TYPE_E
*****************************************************************/
QL_AUD_RING_TYPE_E ql_aud_get_ringtone_type(void);

/*****************************************************************
* Function: ql_aud_set_ringtone_type
*
* Description: set ring tone type, to control the ring tone type when 
			   voice call is comming
*
*Parameters:
*type: 
	QL_AUD_RING_NONE : no ring tone
	QL_AUD_RING_NOKIA : invalid now, can use  QL_AUD_RING_CUSTOMER_DEF
	QL_AUD_RING_DIAL_TONE : DTMF tone, controled by system
	QL_AUD_RING_CUSTOMER_DEF : play ring tone by customer; can use ql_bind_ring_tone_cb
						       to bind callback, when call is comming or released, the 
						       callback will be called, users can send event to task to play 
						       music
	Attention: when use QL_AUD_RING_CUSTOMER_DEF, don't play music in callback directly!!!
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_ringtone_type(QL_AUD_RING_TYPE_E type);

/*****************************************************************
* Function: ql_aud_set_calib_real_time
*
* Description: In real-time mode, the audio channel / call mode of the audio parameters 
			   to be calibrated must be exactly the same as the audio channel used in the 
			   current playback / call, and it will take effect in real time after modification; 
			   There is no such requirement in the non real time mode, and the modification 
			   will not take effect in real time, only in the next play / call
*
* Attention: in real-time mode, users must modify the audio parameters when playing or calling;
*			   
* effection: 
1. for example, in real time calib mode, if current output path is headphone, users
   cannot setting the param belonging to handfree; if current call mode is volte(voice call wb),
   users cannot setting the param belonging to voice call nb
2. in real time calib mode, params will take effect after setting; if not, params
   will take effect in next calling or next playing

* attention:  it will apply to folowing function:
			  1. ql_aud_set_icmic_gain  2. ql_aud_set_icsidet_gain
*
*Parameters:
*	real_time: 	TRUE for real time, FALSE for not;
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_calib_real_time(bool real_time);

/*****************************************************************
* Function: ql_aud_set_icmic_gain
*
* Description: used for setting the uplink mic gain when calling, only 
			   effective in internal codec mode; 
* Attention:   if not need setting it in real time calib mode, use 
			   ql_aud_set_calib_real_time(0) before set the mic gain, see 
			   it in ql_aud_set_calib_real_time
*
*Parameters:
*	mode: 	QL_AUD_VOICECALL_NB, QL_AUD_VOICECALL_WB
*	type:   QL_OUTPUT_RECEIVER, QL_OUTPUT_HEADPHONE, QL_OUTPUT_SPEAKER
*	ana_gain: analog gain, rang is 0~7, see it in "QL_ICMIC_ANA_GAIN_E"
*	adc_gain: adc gain, range is 0~15, see it in "QL_ICMIC_ADC_GAIN_E"
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_icmic_gain
(
	QL_AUD_CALLMODE_E mode,
	AudOutputType_e type,
	QL_ICMIC_ANA_GAIN_E ana_gain,
	QL_ICMIC_ADC_GAIN_E adc_gain
);

/*****************************************************************
* Function: ql_aud_get_icmic_gain
*
* Description: used for getting the uplink mic gain when calling, only 
			   effective in internal codec mode
*
*Parameters:
*	mode: 	QL_AUD_VOICECALL_NB, QL_AUD_VOICECALL_WB
*	type:   QL_OUTPUT_RECEIVER, QL_OUTPUT_HEADPHONE, QL_OUTPUT_SPEAKER
*	ana_gain: analog gain, rang is 0~7, see it in "QL_ICMIC_ANA_GAIN_E"
*	adc_gain: adc gain, range is 0~15, see it in "QL_ICMIC_ADC_GAIN_E"
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_get_icmic_gain
(
	QL_AUD_CALLMODE_E mode,
	AudOutputType_e type,
	QL_ICMIC_ANA_GAIN_E *ana_gain,
	QL_ICMIC_ADC_GAIN_E *adc_gain
);

/*****************************************************************
* Function: ql_aud_set_icsidet_gain
*
* Description: used for setting the sidetone gain in internal code mode
*
*Parameters:
*	mode: 	QL_AUD_VOICECALL_NB, QL_AUD_VOICECALL_WB
*	type:   QL_OUTPUT_RECEIVER, QL_OUTPUT_HEADPHONE, QL_OUTPUT_SPEAKER
*	gain: 	side tone gain, range is 0~15, see it in "QL_ICSIDET_GAIN_E"
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_set_icsidet_gain
(
	QL_AUD_CALLMODE_E mode,
	AudOutputType_e type,
	QL_ICSIDET_GAIN_E gain
);

/*****************************************************************
* Function: ql_aud_get_icsidet_gain
*
* Description: used for getting the sidetone gain in internal code mode
*
* Attention:   if not need setting it in real time calib mode, use 
			   ql_aud_set_calib_real_time(0) before set the mic gain, see 
			   it in ql_aud_set_calib_real_time
*Parameters:
*	mode: 	QL_AUD_VOICECALL_NB, QL_AUD_VOICECALL_WB
*	type:   QL_OUTPUT_RECEIVER, QL_OUTPUT_HEADPHONE, QL_OUTPUT_SPEAKER
*	gain: 	side tone gain, range is 0~15, see it in "QL_ICSIDET_GAIN_E"
*
* Return:
* 	0:           successful
*   others:      ql_audio_errcode_e
*****************************************************************/
ql_audio_errcode_e ql_aud_get_icsidet_gain
(
	QL_AUD_CALLMODE_E mode,
	AudOutputType_e type,
	QL_ICSIDET_GAIN_E *sidet_gain
);

/**********************************************************************************
* Function: set the dac gain and algorithm gain corresponding to the volume

play_mode : for voice call -> QL_AUDIO_PLAY_TYPE_VOICE; 
            for local play -> QL_AUDIO_PLAY_TYPE_LOCAL

out_type:  headphone, speaker, handset

level:	  AUDIOHAL_SPK_MUTE ~ AUDIOHAL_SPK_VOL_11

dac gain: recommended to keep the default value, set it too large
		  will lead to distortion; range is 0~99; 0==>-26db  1==>-25.5db
		  2==>-25db 3==>-24.5db ..... 99==>23db 99==>23.5db

alg gain: recommended to keep the default value, set it too large
		  will lead to distortion; range is 0~15; 0==>MUTE  1==>-42db
		  2==>-39db 3==>-36db......15==>0db
		  
for default: the params can be adjusted by "AudioCalibrationTool", then the software
			 will generate a NV, and the NV can be merged to user's code. the value
			 in NV described above will be the default param;

Attention:
		  it must use ql_aud_set_calib_real_time(0) before call this api, beacuse the 
		  params cannot take effect right now, and it will take effect when next palying
		 
	 users can adjust or get the volume gains by "AudioCalibrationTool" as well
*************************************************************************************/
ql_audio_errcode_e  ql_aud_set_icvolume_level_gain
(
	AudPlayType_e			play_mode,
	AudOutputType_e 		out_type,
	AUDIOHAL_SPK_LEVEL_T	level,
	uint16 					dac_gain,
	uint16					alg_gain
);

/**********************************************************************************
* Function: get the dac gain and algorithm gain corresponding to the volume

play_mode : for voice call -> QL_AUDIO_PLAY_TYPE_VOICE; 
            for local play -> QL_AUDIO_PLAY_TYPE_LOCAL

out_type:  headphone, speaker, handset

level:	  AUDIOHAL_SPK_MUTE ~ AUDIOHAL_SPK_VOL_11

dac gain: recommended to keep the default value, set it too large
		  will lead to distortion; range is 0~99; 0==>-26db  1==>-25.5db
		  2==>-25db 3==>-24.5db ..... 99==>23db 99==>23.5db

alg gain: recommended to keep the default value, set it too large
		  will lead to distortion; range is 0~15; 0==>MUTE  1==>-42db
		  2==>-39db 3==>-36db......15==>0db
		  
for default: the params can be adjusted by "AudioCalibrationTool", then the software
			 will generate a NV, and the NV can be merged to user's code. the value
			 in NV described above will be the default param;
*************************************************************************************/
ql_audio_errcode_e	ql_aud_get_icvolume_level_gain
(
	AudPlayType_e			play_mode,
	AudOutputType_e 		out_type,
	AUDIOHAL_SPK_LEVEL_T	level,
	uint16					*dac_gain,
	uint16					*alg_gain
);

#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif
