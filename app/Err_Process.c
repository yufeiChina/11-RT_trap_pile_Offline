/*******************************************************************************
 * Copyright (c) 2017 ,ENSENTEC
 * All rights reserved.
 * File Name : Err_Process.c
 * Version          V1.0.0
 * The Eclipse Public License is available at
 * http://www.ensentec.com
 * 
 * Contributors     tianqf
 *******************************************************************************/

#include "Public.h"
#include "supply_power.h"
#include "Err_Process.h"
#include "PWM.h"

int updata_err;

void Error_process(void *s)
{
    updata_err = 0;
    while(1)
    {
#ifdef E_STOP_AND_IO        
        if (g_Status.sdst_error1 != S_ERR_ST_NO)
        {
            switch(g_Status.sdst_error1)
            {
            case S_ERR_E_STOP:
                {
                    g_Status.led_status_red = LED_FLASH1;
                    rt_kprintf("error S_ERR_E_STOP \r\n"); 
                }
            default:
                {
                    g_Status.led_status_red = LED_ON;
                    rt_kprintf("Error S_ERR_IO_KEY \r\n"); 
                    break;
                }
            }
        }
        else        //没有故障就灭灯
        {
            g_Status.led_status_red = LED_OFF;
        }
#endif  /* E_STOP_AND_IO   */    
        
        /* 错误故障判断 */
        if (g_Status.sdst_error != S_ERR_ST_NO)
        {
            switch(g_Status.sdst_error)
            {
            case S_ERR_VOL_LESS:
                {
                    g_Status.led_status_red = LED_FLASH1;
                }
                break;
            case S_ERR_VOL_BIGGER:
                {
                    g_Status.led_status_red = LED_FLASH2;
                }
                break;
            case S_ERR_CURRENT_OVER:
                {
                    g_Status.led_status_red = LED_FLASH3;
                }
                break;
            case S_ERR_DIANNENG:
                {
                    g_Status.led_status_red = LED_FLASH4;
                }
                break;
            case S_ERR_CP_VOL_LESS:
                {
                    g_Status.led_status_red = LED_FLASH5;
                }
                break;
            case S_ERR_CPU_CARD:
                {
                    g_Status.led_status_red = LED_FLASH6;
                }
#ifdef E_STOP_AND_IO                   
            case S_ERR_E_STOP:
                {
                    g_Status.led_status_red = LED_FLASH1;
                    rt_kprintf("error S_ERR_E_STOP \r\n"); 
                }
            default:
                {
                    g_Status.led_status_red = LED_ON;
                    rt_kprintf("Error S_ERR_IO_KEY \r\n"); 
                    
                    break;
                }
#else
            default:   
                  break;
#endif  /* E_STOP_AND_IO   */                   
            }
        }
        else        //没有故障就灭灯
        {
            g_Status.led_status_red = LED_OFF;
        }
        
        /* 错误故障处理 */
        if (g_Status.sdst_error != S_ERR_ST_NO || g_Status.sdst_error1 != S_ERR_ST_NO)    
        { 
            g_Status.relay_status = RELAY_OFF;
            g_Status.swipe_card = SWIPE_CARD_NO;
            g_Status.svr_allow_suplly = SVR_ALLOW_SUPPLY_NO;
            if(!updata_err)
            {
               rt_kprintf("error Update \r\n");
//               Trap_Ring_in(&m_trapinfo); 
            }
            updata_err++;
            
            Stop_PWM();
            close_powergate();
        }
        
        /* 错误故障退出 */
        if (g_Status.sdst_error != S_ERR_ST_NO || g_Status.sdst_error1 != S_ERR_ST_NO)
        {
            switch(g_Status.sdst_error)
            {
            case S_ERR_VOL_LESS:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                else if (g_Status.clear_err_times == 4)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_VOL_BIGGER:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                else if (g_Status.clear_err_times == 4)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_CURRENT_OVER:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                else if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_DIANNENG:
                if (g_Status.clear_err_times)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_CP_VOL_LESS:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_CPU_CARD:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_E_STOP:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            case S_ERR_IO_KEY:
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    g_Status.sdst_error = S_ERR_ST_NO;
                    updata_err = 0;
                }
                break;
            default:
                break;
            }
        }
     rt_thread_sleep(800);
    }
}

static rt_uint8_t Error_stack[ 1024 ];
static struct rt_thread Error_thread;

rt_err_t Error_init(void)
{
    rt_err_t result;
    
    result = rt_thread_init(&Error_thread,      //线程描述符
                            "Error",                        //线程名称
                            Error_process,                    //线程的入口
                            RT_NULL,                        //参数
                            &Error_stack[0],                //线程的栈
                            sizeof(Error_stack),            //栈的大小
                            21,                             //优先级     21 ~ 32
                            30);                            //时间片     30: 30ms
    if (result == RT_EOK)
        rt_thread_startup(&Error_thread);       //启动线程
    else 
    {
        return RT_ERROR;
    }

    return RT_EOK;
}

