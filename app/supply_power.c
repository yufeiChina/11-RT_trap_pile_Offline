/**
  * @file   supply_power.c
  * @brief  Charge Pile Supply power.
  * @author ztlchina@foxmail.com
  * @par   Copyright (c):
  *          ztlchina@foxmail.com. Ensentec.Inc
  *         All Rights Reserved
  * @date   2017:11:16 
  * @note   mattersneeding attention
  * @version <version  number>
  */ 

#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "supply_power.h"
#include "energy_meter.h"
#include "Public.h"
#include "PWM.h"
#include "e-stop.h"

static rt_uint8_t SupplyPower_stack[ 1024 ];
static struct rt_thread SupplyPower_thread;

//充电枪检测函数
void Check_Gun_Status()
{
    //枪的状态
    if(GUN_TO_WAIT)
    {
        g_Status.gun_status = SD_GUN_ST_OUT;
        g_Status.swipe_card = SWIPE_CARD_NO;
        g_Status.svr_allow_suplly = SVR_ALLOW_SUPPLY_NO;
    }
    else if(GUN_TO_INPUT)
    {
        g_Status.gun_status = SD_GUN_ST_CNCTING;
    }
    else if(GUN_TO_CONFIRM)
    {
        g_Status.gun_status = SD_GUN_ST_CNCTOK;
    } 
    else if(GUN_TO_ERR)
    {
        rt_kprintf("error error error error error error error error error error error error error error error error S_ERR_CP_VOL_LESS\r\n");
        g_Status.sdst_error = S_ERR_CP_VOL_LESS;
    }
}

void close_LN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_5);	
}

//充电桩状态检测
void SupplyPower_entry(void *s)
{    
//    close_LN();
    power_gate_init();  //继电器初始化 GPIOD
//    E_STOP_init(); 
    while(1)
    {    
        Check_Gun_Status();
        //充电桩的状态
//        Check_estop();
        
        switch (g_Status.supplypower)
        {
            case SPT_PILE_Init:  //初始化
                if (g_Status.gun_status == SD_GUN_ST_OUT)   
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Wait;
                    Stop_PWM();
                    
                    g_Status.charging_begin_time = 0;
                    g_Status.charging_time = 0;
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTING)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Input,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Input;
                    Start_PWM();
                    
                    g_Status.charging_begin_time = 0;
                    g_Status.charging_time = 0;
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTOK)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Confirm,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Confirm;
                    Start_PWM();
                    
                    g_Status.charging_begin_time = 0;
                    g_Status.charging_time = 0;
                }
                break;
                
            case SPT_PILE_Wait:   //待机
                if (g_Status.gun_status == SD_GUN_ST_CNCTING)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Wait->Input,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Input;
                    Start_PWM();
                    
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTOK)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Wait->Confirm,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Confirm;
                    Start_PWM();
                    
                }
                break;
                
            case SPT_PILE_Input:   //插枪
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Input->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Wait;
                    Stop_PWM();
                    
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTOK)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Input->Confirm,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Confirm;
                    Start_PWM();
                    
                }
                break;

            case SPT_PILE_Confirm:  //确认
                       
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Confirm->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Wait;
                    Stop_PWM();
                    
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTING)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Confirm->Input,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Input;
                    Start_PWM();
                    
                }
                else 
                {
                    if (g_Status.sdst_error == S_ERR_ST_NO && g_Status.sdst_error1 == S_ERR_ST_NO)
                    {
                        rt_kprintf("\r\n\tSPLPOWER(CP):Confirm->Work,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                        g_Status.supplypower = SPT_PILE_Work;
                        g_Status.charging_begin_time = rt_tick_get();     //获取开始充电的当前时间
                        
                        open_powergate();
                        g_Status.relay_status = RELAY_ON;
                        Start_PWM();
                    }
                }
                break;

            case SPT_PILE_Work:  //充电 
                if (g_Status.relay_status != RELAY_ON)
                {
                    rt_kprintf("\r\n\tg_Status.relay_status != RELAY_ON \r\n\r\n");
                    g_Status.supplypower = SPT_PILE_Over;
                    Stop_PWM();
                    close_powergate();
                    g_Status.relay_status = RELAY_OFF;
                }
                
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Work->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Over;
                    Stop_PWM();
                    close_powergate();
                    g_Status.relay_status = RELAY_OFF;
                }
                break;
            case SPT_PILE_Over:     //结束
                {
                    rt_thread_sleep(1000);
                    rt_kprintf("\r\n\tSPLPOWER(CP):Over->Init,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Init;
                    g_Status.svr_allow_suplly = SVR_ALLOW_SUPPLY_NO;
                    g_Status.read_begin_energy = 0;
                }
                break;
            default:
                {
                    g_Status.supplypower = SPT_PILE_Wait;
                    break;
                }
        }
        rt_thread_sleep(500);
    }
}


void power_gate_init(void)
{
#ifdef GPIO_Pin14_Pin15   
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_14);	
    GPIO_SetBits(GPIOA, GPIO_Pin_15);    
#else   
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
#endif
}

#ifdef GPIO_Pin14_Pin15
void open_powergate(void)
{ 
    GPIO_ResetBits(GPIOA, GPIO_Pin_14);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);	
	rt_kprintf("Relay(AC):Open.\r\n");
}
void close_powergate(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_14);	
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
	rt_kprintf("Relay(AC):Close.\r\n");		  
}

#else
void open_powergate(void)
{ 
	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);	
	rt_kprintf("Relay(AC):Open.\r\n");
}
void close_powergate(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
	rt_kprintf("Relay(AC):Close.\r\n");		  
}
#endif

rt_err_t SupplyPower_init(void)
{
    rt_err_t result;
    
    result = rt_thread_init(&SupplyPower_thread,      //线程描述符
                            "SupplyPower",                        //线程名称
                            SupplyPower_entry,                    //线程的入口
                            RT_NULL,                        //参数
                            &SupplyPower_stack[0],                //线程的栈
                            sizeof(SupplyPower_stack),            //栈的大小
                            21,                             //优先级     21 ~ 32
                            30);                            //时间片     30: 30ms
    if (result == RT_EOK)
        rt_thread_startup(&SupplyPower_thread);       //启动线程
    else 
    {
        return RT_ERROR;
    }

    return RT_EOK;
}
