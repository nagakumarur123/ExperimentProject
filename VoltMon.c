/**********************************************************************************************************************
 * \file VoltMon.c
 * @naga kumar - This file generated just for learning purposes and experiments
 **********************************************************************************************************************/

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "IfxEvadc_Adc.h"



/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define VAREF_VOLTS     5.0f
#define ADC_MAX_COUNTS  4095.0f   /* 12-bit resolution */

static IfxEvadc_Adc         evadcReg;
static IfxEvadc_Adc_Group   adcGroup0;
static IfxEvadc_Adc_Channel g_adcChannel0;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

void init_ADC_VoltageRead(void)
{
    /* Step1: Initialize EVADC module */
    IfxEvadc_Adc_Config adcConfig;

    IfxEvadc_Adc_initModuleConfig(&adcConfig, &MODULE_EVADC);
    IfxEvadc_Adc_initModule(&evadcReg, &adcConfig);

    /* Step2: Initialize Group 0 */
    IfxEvadc_Adc_GroupConfig groupConfig;
    IfxEvadc_Adc_initGroupConfig(&groupConfig, &evadcReg);
    groupConfig.groupId = IfxEvadc_GroupId_0;
    groupConfig.master  = IfxEvadc_GroupId_0;

    /* Enable background scan source for software-triggered reads */
    groupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;
    groupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
    groupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxEvadc_GatingMode_always;

    IfxEvadc_Adc_initGroup(&adcGroup0, &groupConfig);

    /* Step3: Initialize Channel 0 on Group 0 */
    IfxEvadc_Adc_ChannelConfig channelConfig;
    IfxEvadc_Adc_initChannelConfig(&channelConfig, &adcGroup0);
    channelConfig.channelId = IfxEvadc_ChannelId_0;
    channelConfig.resultRegister = IfxEvadc_ResultReg_0;

    /* Add channel to background scan */
    channelConfig.backgroundChannel.enabled = TRUE;

    IfxEvadc_Adc_initChannel(&g_adcChannel0, &channelConfig);

    /* Let's start background scan */
    IfxEvadc_Adc_startBackgroundScan(&evadcReg);
}


float read_Voltage(void)
{
    Ifx_EVADC_G_RES result;
    uint16 rawValue = 0;
    float ActVoltage = 0.0;


    /* Wait until a valid conversion result is available */
    do {
        result = IfxEvadc_Adc_getResult(&g_adcChannel0);
    } while (!result.B.VF);

    rawValue = (uint16)result.B.RESULT;
    ActVoltage = ((float)rawValue / ADC_MAX_COUNTS) * VAREF_VOLTS;

    return (ActVoltage);
}


