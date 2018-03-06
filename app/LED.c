#include "led.h"
#include <rtthread.h>
#include "supply_power.h"
#include "Public.h"

void LED_GPIO_Config(void)
{
#ifdef GPIO_Pin14_Pin15 
    
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12);	 // turn off all led
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_5);	 // turn off yellow led
    
#else
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);	 // turn off all led
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_5);	 // turn off yellow led
    
#endif
}

void LED_ALL_OFF(void)
{
	LEDR(OFF);
	LEDG(OFF);
	LEDB(OFF);
    LEDY(OFF);
}

void LED_ALL_ON(void)
{
    LEDR(ON);
    LEDG(ON);
    LEDB(ON);
}

void LEDR_turn(void)
{
	GPIOA->ODR ^= GPIO_Pin_4;
}

void LEDB_turn(void)
{
	GPIOA->ODR ^= GPIO_Pin_6;
}

void LEDG_turn(void)
{
	GPIOA->ODR ^= GPIO_Pin_5;
}
void LEDY_turn(void)
{
	GPIOB->ODR ^= GPIO_Pin_5;
}


void Set_led_status(void)
{
    
    //红灯状态
//    if()      //各种故障状态
//    {
//        
//    }
    
    
    //绿灯状态
    if(g_Status.relay_status == RELAY_ON)
    {
        g_Status.led_status_green = LED_ON;
    }
//    else if()     //刷卡闪烁
//    {
        
//    }
    else
    {
        g_Status.led_status_green = LED_OFF;
    }
    
    
    //蓝灯状态
    if(g_Status.connect == GPRS_CONNECT)
    {
//        g_Status.led_status_blue = LED_FLASH_HARRY;
        g_Status.led_status_blue = LED_OFF;
    }
    else if(g_Status.connect == MQTT_CONNECT)
    {
//        g_Status.led_status_blue = LED_FLASH;
        g_Status.led_status_blue = LED_OFF;
    }
    else
    {
//        g_Status.led_status_blue = LED_ON;
        g_Status.led_status_blue = LED_OFF;
    }
    
    
    //黄灯状态
    if( g_Status.gun_status == SD_GUN_ST_OUT)
    {
        g_Status.led_status_yellow = LED_OFF;
    }
    else if( g_Status.gun_status == SD_GUN_ST_CNCTING)
    {
        g_Status.led_status_yellow = LED_FLASH;
    }
    else if( g_Status.gun_status == SD_GUN_ST_CNCTOK)
    {
        g_Status.led_status_yellow = LED_ON;
    }
    
}
rt_uint8_t TrapTick[4] = {0};


void Flash_time(rt_uint8_t times)
{
    TrapTick[0]++;
    static int flashtimes = 0;
    if(times != flashtimes)
    {
        if(TrapTick[0] <= 5)
        {
            LEDR(ON);
        }                
        else if(TrapTick[0] <= 10)
        {
            LEDR(OFF);
        }
        else
        {
            TrapTick[0] = 0;
            flashtimes++;
        }
    } 
    else
    {
        if(TrapTick[0] == 20)
        {
            TrapTick[0] = 0;
            flashtimes = 0;
        }
    }
    
}

void Led_process(void *s)
{
    
    LED_GPIO_Config();
    LED_ALL_OFF();
    
    while(1)
    {
        Set_led_status();
        switch(g_Status.led_status_red)
        {
            case LED_OFF: 
            {
                LEDR(OFF);
                break;
            }
            case LED_ON: 
            {
                LEDR(ON);
                break;
            }
            case LED_FLASH1: 
            {
                Flash_time(1);
                break;
            }
            case LED_FLASH2: 
            {
                Flash_time(2);
                break;
            }
            case LED_FLASH3: 
            {
                Flash_time(3);
                break;
            }
            case LED_FLASH4: 
            {
                Flash_time(4);
                break;
            }
            case LED_FLASH5: 
            {
                Flash_time(5);
                break;
            }
            case LED_FLASH6: 
            {
                Flash_time(6);
                break;
            }
            default:
                break;
        }
        
        switch(g_Status.led_status_green)
        {
            case LED_OFF:
            {
                LEDG(OFF);
                break;
            }
            case LED_ON:
            {
                LEDG(ON);
                break;
            }
            default:
                break;
        }
        
        switch(g_Status.led_status_blue)
        {
            case LED_OFF:
            {
                LEDB(OFF);
                break;
            }
            case LED_ON:
            {
                LEDB(ON);
                break;
            }
            case LED_FLASH:
            {
                TrapTick[2]++;
                if(TrapTick[2] == 5)
                {
                    LEDB_turn();
                    TrapTick[2] = 0;
                }
                break;
            }
            case LED_FLASH_HARRY:
            {
                LEDB_turn();
                break;
            }
            default:
                break;
        }
        
        switch(g_Status.led_status_yellow)
        {
            case LED_OFF:
            {
                LEDY(OFF);
                break;
            }
            case LED_ON:
            {
                LEDY(ON);
                break;
            }
            case LED_FLASH:
            {
                TrapTick[3]++;
                if(TrapTick[3] == 5)
                {
                    LEDY_turn();
                    TrapTick[3] = 0;
                }
                break;
            }
            default:
                break;
        }

        rt_thread_sleep(100);
    }
}


static rt_uint8_t Led_stack[ 1024 ];
static struct rt_thread Led_thread;

rt_err_t Led_init(void)
{
    rt_err_t result;
    
    result = rt_thread_init(&Led_thread,      //线程描述符
                            "Led",                        //线程名称
                            Led_process,                    //线程的入口
                            RT_NULL,                        //参数
                            &Led_stack[0],                //线程的栈
                            sizeof(Led_stack),            //栈的大小
                            28,                             //优先级     21 ~ 32
                            30);                            //时间片     30: 30ms
    if (result == RT_EOK)
        rt_thread_startup(&Led_thread);       //启动线程
    else 
    {
        return RT_ERROR;
    }

    return RT_EOK;
}



