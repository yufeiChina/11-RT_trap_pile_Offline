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

//���ǹ��⺯��
void Check_Gun_Status()
{
    //ǹ��״̬
    if(GUN_TO_WAIT)
    { 
        rt_thread_delay(20);
		if(GUN_TO_WAIT)
		{
			g_Status.gun_status = SD_GUN_ST_OUT; 
		}
    }
    else if(GUN_TO_INPUT)
    {
        rt_thread_delay(20);
		if(GUN_TO_INPUT)
		{
			g_Status.gun_status = SD_GUN_ST_CNCTING;
		}
    }
    else if(GUN_TO_CONFIRM)
    {
        rt_thread_delay(20);
		if(GUN_TO_CONFIRM)
		{
			g_Status.gun_status = SD_GUN_ST_CNCTOK;
		}
    } 
    else if(GUN_TO_ERR)
    { 
        rt_thread_delay(20);
		if(GUN_TO_ERR)
		{
			g_Status.sdst_error = S_ERR_CP_VOL_LESS;
		}
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

//���׮״̬���
void SupplyPower_entry(void *s)
{    
    close_LN();
    power_gate_init();  //�̵�����ʼ�� GPIOD
	rt_thread_delay(10);
    power_gate_init();  //�̵�����ʼ�� GPIOD
//    E_STOP_init(); 
    while(1)
    {    
        Check_Gun_Status(); 
        
        switch (g_Status.supplypower)
        {
            case SPT_PILE_Init:  //��ʼ��
                if (g_Status.gun_status == SD_GUN_ST_OUT)   
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Wait;
                    Stop_PWM(); 
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTING)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Input,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Input;
                    Start_PWM(); 
                }
                else if (g_Status.gun_status == SD_GUN_ST_CNCTOK)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Init->Confirm,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Confirm;
                    Start_PWM(); 
                }
                break;
                
            case SPT_PILE_Wait:   //����
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
                
            case SPT_PILE_Input:   //��ǹ
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

            case SPT_PILE_Confirm:  //ȷ��
                       
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
					rt_kprintf("\r\n\tSPLPOWER(CP):Confirm->Work,AD:%u.\r\n\r\n", AD_AVER_VALUE);
					g_Status.supplypower = SPT_PILE_Work; 
					open_powergate(); 
					Start_PWM(); 
                }
                break;

            case SPT_PILE_Work:  //���  
                if (g_Status.gun_status == SD_GUN_ST_OUT)
                {
                    rt_kprintf("\r\n\tSPLPOWER(CP):Work->Wait,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Over;
                    close_powergate(); 
                    Stop_PWM();
                }
                break;
				
            case SPT_PILE_Over:     //����
                { 
                    rt_kprintf("\r\n\tSPLPOWER(CP):Over->Init,AD:%u.\r\n\r\n", AD_AVER_VALUE);
                    g_Status.supplypower = SPT_PILE_Init; 
                    close_powergate(); 
                    Stop_PWM();
                }
                break;
				
            default:
                {
                    g_Status.supplypower = SPT_PILE_Wait;
					close_powergate(); 
                    Stop_PWM();
                    break;
                }
        }
        rt_thread_delay(500);
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
    
    result = rt_thread_init(&SupplyPower_thread,      //�߳�������
                            "SupplyPower",                        //�߳�����
                            SupplyPower_entry,                    //�̵߳����
                            RT_NULL,                        //����
                            &SupplyPower_stack[0],                //�̵߳�ջ
                            sizeof(SupplyPower_stack),            //ջ�Ĵ�С
                            21,                             //���ȼ�     21 ~ 32
                            30);                            //ʱ��Ƭ     30: 30ms
    if (result == RT_EOK)
        rt_thread_startup(&SupplyPower_thread);       //�����߳�
    else 
    {
        return RT_ERROR;
    }

    return RT_EOK;
}
