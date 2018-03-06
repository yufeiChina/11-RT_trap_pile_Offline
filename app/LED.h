#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include <rtthread.h>

#define ON	1
#define OFF	0

#ifdef GPIO_Pin14_Pin15 
 
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
#else  
                    
#define LEDR(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_4)

#define LEDG(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define LEDB(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_6);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_6)
					
#define LEDY(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#endif
                    
rt_err_t Led_init(void);

#endif /* __LED_H */
