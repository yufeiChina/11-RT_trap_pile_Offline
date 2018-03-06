/*******************************************************************************
 * Copyright (c) 2017 ,ENSENTEC
 * All rights reserved.
 * 
 * Version          V1.0.0
 * The Eclipse Public License is available at
 * http://www.ensentec.com
 * 
 * Contributors     tianqf
 *******************************************************************************/

#include "Public.h"

struct sd_status g_Status;

void sd_status_init(void)
{
    g_Status.gun_status = SD_GUN_ST_OUT;
    g_Status.sdst_error =  S_ERR_ST_NO;
    g_Status.supplypower = SPT_PILE_Init;
    g_Status.relay_status = RELAY_OFF;
    g_Status.svr_allow_suplly = SVR_ALLOW_SUPPLY_NO;
      
    g_Status.led_status_red = LED_OFF;
    g_Status.led_status_yellow = LED_OFF;
    g_Status.led_status_blue = LED_OFF;
    g_Status.led_status_green = LED_OFF;
    g_Status.charging_begin_time = 0;
    g_Status.charging_time = 0;
    g_Status.clear_err_times = 0;
    g_Status.read_begin_energy = 0;
}
