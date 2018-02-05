#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include <rtthread.h>

#define ON	1
#define OFF	0

#define LEDR(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_12);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_12)

#define LEDG(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_10);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_10)

#define LEDB(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_8)
					
#define LEDY(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_5)

rt_err_t Led_init(void);

#endif /* __LED_H */
