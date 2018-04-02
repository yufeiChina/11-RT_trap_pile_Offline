
/******************************************************************************
 *          |  PA.01: (TIM2_CH2)  |
 *
**********************************************************************************/
#include "PWM.h"

#define START_PWM_TIMER TIM_Cmd(TIM2, ENABLE)
#define STOP_PWM_TIMER  TIM_Cmd(TIM2, DISABLE)
/*
 * 函数名：TIM2_GPIO_Config
 * 描述  ：配置TIM2复用输出PWM时用到的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void TIM2_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM2 clock enable */
	//PCLK1经过2倍频后作为TIM2的时钟源等于36MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /*GPIOA Configuration: TIM2 channel 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,  GPIO_Pin_1);	 // turn off all led

}

/*
 * 函数名：TIM2_Mode_Config
 * 描述  ：配置TIM2输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void TIM2_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM信号电平跳变值 */
//	u16 CCR1_Val = 240;
    
//	u16 CCR1_Val = g_Status.m_PWMrate;          /*6666666666666666666666666666666666666*/
    
    //u16 CCR1_Val = g_SysConfig.m_PWMrate; //267;        
    //printf("PWM:%d.\r\n", g_SysConfig.m_PWMrate);
//	u16 CCR1_Val = 200;        

/* -----------------------------------------------------------------------
    TIM2 Configuration: generate 1 PWM signals 
    TIM2CLK = 36 MHz, Prescaler = 0x24, TIM2 counter clock = 1 MHz
    TIM2 ARR Register = 999 => TIM2 Frequency = TIM2 counter clock/(ARR + 1)
    TIM2 Frequency = 1 KHz.
    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR)* 100 = 26.7%
  ----------------------------------------------------------------------- */

  /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = 999;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 71;	    //设置预分频：72，即为1MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  
  //PWM = 1000 ---> 60A   
  
  
  //  TIM_OCInitStructure.TIM_Pulse = 533;//g_SysConfig.m_PWMrate;//CCR1_Val; //设置跳变值，当计数器计数到这个值时，电平发生跳变   /*6666666666666666666666666666666666666*/
//  TIM_OCInitStructure.TIM_Pulse = 100;         //6A
//  TIM_OCInitStructure.TIM_Pulse = 116;         //7A
//  TIM_OCInitStructure.TIM_Pulse = 133;         //8A
//  TIM_OCInitStructure.TIM_Pulse = 150;         //9A
//  TIM_OCInitStructure.TIM_Pulse = 166;         //10A
//  TIM_OCInitStructure.TIM_Pulse = 183;         //11A
//  TIM_OCInitStructure.TIM_Pulse = 200;         //12A
//  TIM_OCInitStructure.TIM_Pulse = 216;         //13A
//  TIM_OCInitStructure.TIM_Pulse = 233;         //14A
//  TIM_OCInitStructure.TIM_Pulse = 250;         //15A
  TIM_OCInitStructure.TIM_Pulse = 266;	     //16A
//  TIM_OCInitStructure.TIM_Pulse = 533;         //32A

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);	 //使能通道1

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // 使能TIM2重载寄存器ARR

  /* TIM2 enable counter */
  //TIM_Cmd(TIM2, ENABLE);                   //使能定时器2	
}

/*
 * 函数名：TIM2_Mode_Config
 * 描述  ：TIM2 输出PWM信号初始化，只要调用这个函数,
 * 通道1 就会有PWM信号输出
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */


void Start_PWM(void)
{
	TIM2_GPIO_Config();
	START_PWM_TIMER;	
}

void Stop_PWM(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

    STOP_PWM_TIMER;
	
	/*GPIOA Configuration: TIM2 channel 2 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,  GPIO_Pin_1);	 // turn off all led

//    //GPIO_SetBits(GPIOA,  GPIO_Pin_1);	 // turn off all led
//    TIM_ForcedOC1Config(TIM3, TIM_ForcedAction_Active);
}

void TIM2_PWM_Init(void)
{
	TIM2_GPIO_Config();
	TIM2_Mode_Config();
    Stop_PWM();
}
