#ifndef __AD_H_INCLUDE
#define __AD_H_INCLUDE
#include "stm32f10x.h"

extern __IO uint16_t AD_AVER_VALUE;
extern __IO uint16_t AD_VALUE_SUM;

#define ADC1_DR_Address    ((uint32_t)0x4001244C)  //ADC1 DR�Ĵ�������ַ, ����ַ=0x40012400�� DR�Ĵ���ƫ�Ƶ�ַ0x4c 

void  ADC1_Init(void);
void ADC_fiter(void);
void DMA_init(void);

#endif

