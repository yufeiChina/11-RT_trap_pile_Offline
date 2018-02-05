#ifndef _ENERGY_METER_H_INCLUDE_
#define _ENERGY_METER_H_INCLUDE_

#include <rtthread.h>
#include "stm32f10x.h"

struct dianneng
{
    u32  dianya;	  				//��ѹV = data  / 10000 V         0.0001V
    u32  dianliu;					//����A = data / 10000		  A   0.0001A
    u32  yougonggonglv;		        //�й����� w = data / 10000     W  0.0001w
    u32  dangqiandianneng;          //�й�����kwh = data / 10000	 0.1wh
    u32  begindianneng;
    u32  gonglvyinshu;				//�������� data / 1000
    u32  systicks;					//��¼�ɼ���ʱ��
};

rt_err_t energy_meter_init(void);

extern struct dianneng g_dianneng_temp;
#endif
