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
#include "stmflash.h"

struct sd_status g_Status;

void sd_status_init(void)
{
    g_Status.gun_status = SD_GUN_ST_OUT;
    g_Status.sdst_error =  S_ERR_ST_NO;
    g_Status.supplypower = SPT_PILE_Init;
//    g_Status.relay_status = RELAY_OFF;
//    g_Status.svr_allow_suplly = SVR_ALLOW_SUPPLY_NO;
//    g_Status.swipe_card = SWIPE_CARD_NO;
//    g_Status.connect = GPRS_CONNECT;
//    g_Status.screen_page_chose = SCREEN_PAGE_LOGO;
    
    g_Status.led_status_red = LED_OFF;
    g_Status.led_status_yellow = LED_OFF;
    g_Status.led_status_blue = LED_OFF;
    g_Status.led_status_green = LED_OFF;
//    g_Status.charging_begin_time = 0;
//    g_Status.charging_time = 0;
//    g_Status.clear_err_times = 0;
//    g_Status.read_begin_energy = 0;
    
    
    u16 change_flag;
    u16 currect1 = 0;
    u16 currect2 = 0;
    /* 去FLASH里读取修改密码标志位，如果没修改过，则设置密码为出厂密码 */
    STMFLASH_Read(FLASH_SAVE_ADDR+32,&change_flag ,sizeof(change_flag));
//    rt_kprintf("change_flag:%x \r\n", change_flag); 
    if(1 != change_flag) 
    {
        g_Status.PWM_Set = 266;   // 未更改过时，出厂密码
    } else {
        STMFLASH_Read(FLASH_SAVE_ADDR,&currect1 ,sizeof(currect1));
        STMFLASH_Read(FLASH_SAVE_ADDR+16,&currect2 ,sizeof(currect2)); 
        g_Status.PWM_Set = (currect1 << 16) | currect2; 
    } 
    rt_kprintf("\r\n*******************PWM Corresponding Current List*************************\r\n");  
    rt_kprintf("PWM Value:   100   116   133   150   166   183   200   216   233   250   266\r\n");
    rt_kprintf("Current(A):   6     7     8     9     10    11    12    13    14    15    16 \r\n");  
    rt_kprintf("\r\nNow PWM:[%d]\r\nChange PWM Please Enter：setPWM(Value)\r\n", g_Status.PWM_Set);  
     
}

void send_PWM(rt_uint16_t PWM)
{ 
    u16 change_flag = 1;
    u16 currect1 = 0;
    u16 currect2 = 0;
    
    if(PWM >= 1000){
        PWM = 1000;
    }
    
    /* 新 PWM 写到FLASH */
    currect1 = PWM >> 16;
    currect2 = PWM; 
     
    STMFLASH_Write(FLASH_SAVE_ADDR,&currect1,sizeof(currect1));
    rt_thread_delay(10);
    STMFLASH_Write(FLASH_SAVE_ADDR+16,&currect2,sizeof(currect2));
    rt_thread_delay(10);
    rt_kprintf("\r\nSet PWM:[%d] OK, Write FLASH OK  \r\n\r\n", PWM);
    STMFLASH_Write(FLASH_SAVE_ADDR+32,&change_flag,sizeof(change_flag));//标志改过
//    g_Status.PWM_Set = PWM;        
}
#ifdef RT_USING_FINSH

#include <finsh.h>
void setPWM(rt_uint16_t PWM)
{
    send_PWM(PWM);
}
FINSH_FUNCTION_EXPORT(setPWM, set pwm value); 
MSH_CMD_EXPORT(sendPWM, set pwm value);
#endif


