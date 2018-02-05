/*******************************************************************************
 * Copyright (c) 2017 ,ENSENTEC
 * All rights reserved.
 * FileName e-stop.c
 * Data     2018-2-5 10:26:41
 * The Eclipse Public License is available at
 * http://www.ensentec.com
 * 
 * Contributors     tianqf@ensentec.com
 *******************************************************************************/
//两个IO口，一个用作急停，另一个IO口常高电平，不处于低电平不允许充电

#include "e-stop.h"
#include "public.h"

int AA, BB, CC, DD;

void E_STOP_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

    AA = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
    BB = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);
    
    rt_kprintf("\r\n~~ E4:%d ~~~~ E5:%d ~~\r\n", AA, BB);
}

int Check_estop(void)
{
    CC = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4);
    DD = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);
    rt_kprintf("\r\n~~ E4:%d ~~~~ E5:%d ~~\r\n", CC, DD);
    if(CC != AA)
    {
        g_Status.sdst_error1 = S_ERR_E_STOP;
    }
    else
    {
        g_Status.sdst_error1 = S_ERR_ST_NO;
    }
/*-------- E5 常高，变低才可以充电 --------*/ 
    if(DD == 1)
    {
        g_Status.sdst_error = S_ERR_IO_KEY;
    }
    else
    {
        g_Status.sdst_error = S_ERR_ST_NO;
    }
}