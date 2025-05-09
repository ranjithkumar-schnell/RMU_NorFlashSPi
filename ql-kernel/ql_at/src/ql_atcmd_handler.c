/**  @file
  ql_atcmd_handler.c

  @brief
  This file is used to define at command callback functions for different Quectel OPEN SDK Project.

*/

/*================================================================
  Copyright (c) 2021 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


/*===========================================================================
 * include files
============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osi_mem.h"
#include "at_engine.h"
#include "at_command.h"
#include "at_response.h"
#include "atr_config.h"
#include "ql_app_feature_config.h"
#include "ql_log.h"
#include "quec_at_engine.h"
#include "ql_atcmd_handler.h"
#include "ql_at_cmd_table.h"



/*************************************************************************
*  							Macro
**************************************************************************/
#define ql_at_handler_log(msg, ...)  QL_LOG(QL_LOG_LEVEL_INFO, "QATHDLR", msg, ##__VA_ARGS__)


/*************************************************************************
*  							Enum
**************************************************************************/


/*************************************************************************
*  							Struct
**************************************************************************/
 
 
/*************************************************************************
*	                        Variable
**************************************************************************/
const char ql_ver_date[] = __DATE__;
const char ql_ver_time[] = __TIME__;
const char ql_ver_authors[] = "Authors: QCT";


/*************************************************************************
*	                        Function
**************************************************************************/
const atCmdDesc_t *ql_atcmd_search_desc(const char *name, size_t length)
{
    ql_at_handler_log("AT CMD search %s (%d)", name, length);

    if (length == 0)
        return NULL;

    if (length <= MAX_WORD_LENGTH && length >= MIN_WORD_LENGTH)
    {
        unsigned key = ql_atCmdNameHash(name, length);
        if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
            unsigned idx = ql_gAtCmdNameHashIndex[key];
            if (idx < TOTAL_KEYWORDS)
            {
                int cmp = strncasecmp(name, ql_gAtCmdTable[idx].name, length);
                ql_at_handler_log("AT CMD found key=%d index=%d cmp=%d", key, idx, cmp);
                return (cmp == 0) ? &ql_gAtCmdTable[idx] : NULL;
            }
        }
    }
    return NULL;
}

#ifndef QL_CORE_VERSION
#define QL_CORE_VERSION "QuecOPENSDK"
#endif
void ql_exec_ver_cmd(atCommand_t *cmd)
{
    char *verInfo = NULL;
	int verinfo_len;
	
	verinfo_len = strlen(QL_CORE_VERSION)+strlen(ql_ver_date)+strlen(ql_ver_time)+strlen(ql_ver_authors)+20;
	verInfo = (char *)malloc(verinfo_len);
	if (NULL == verInfo)
	{
		quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_ERROR, CMD_RC_ERROR);
		return;
	}

	memset(verInfo, 0, verinfo_len);
    /* get ver information */
    sprintf(verInfo, "VERSION: %s\r\n%s %s\r\n%s", QL_CORE_VERSION, ql_ver_date, ql_ver_time, ql_ver_authors);

    /*return the result code and response to the terminal */
	quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, verInfo, 1);

	free(verInfo);
	return;
}

void ql_exec_atdemo_cmd(atCommand_t *cmd)
{
	if (cmd->type == AT_CMD_SET)
    {
        //how to extract parameter, please refer to at_param.h, eg:
        //atParamUintInList: to extract uint parameter, and check in list
        //atParamUintInRange: to extract uint parameter, and check range
        //atParamStr: to extract string parameter
        //......
        quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        quec_atResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK, "atdemo read resp", 1);
    }
    else if (cmd->type == AT_CMD_TEST)
    {
        quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_OK, CMD_RC_OK);
    }
    else
    {
        quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_CME_ERROR, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

