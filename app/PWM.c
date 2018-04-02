
/******************************************************************************
 *          |  PA.01: (TIM2_CH2)  |
 *
**********************************************************************************/
#include "PWM.h"

#define START_PWM_TIMER TIM_Cmd(TIM2, ENABLE)
#define STOP_PWM_TIMER  TIM_Cmd(TIM2, DISABLE)
/*
 * ��������TIM2_GPIO_Config
 * ����  ������TIM2�������PWMʱ�õ���I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void TIM2_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM2 clock enable */
	//PCLK1����2��Ƶ����ΪTIM2��ʱ��Դ����36MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /*GPIOA Configuration: TIM2 channel 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,  GPIO_Pin_1);	 // turn off all led

}

/*
 * ��������TIM2_Mode_Config
 * ����  ������TIM2�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void TIM2_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM�źŵ�ƽ����ֵ */
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
  TIM_TimeBaseStructure.TIM_Period = 999;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 71;	    //����Ԥ��Ƶ��72����Ϊ1MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//����ʱ�ӷ�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  
  //PWM = 1000 ---> 60A   
  
  
  //  TIM_OCInitStructure.TIM_Pulse = 533;//g_SysConfig.m_PWMrate;//CCR1_Val; //��������ֵ�������������������ֵʱ����ƽ��������   /*6666666666666666666666666666666666666*/
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

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��1

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // ʹ��TIM2���ؼĴ���ARR

  /* TIM2 enable counter */
  //TIM_Cmd(TIM2, ENABLE);                   //ʹ�ܶ�ʱ��2	
}

/*
 * ��������TIM2_Mode_Config
 * ����  ��TIM2 ���PWM�źų�ʼ����ֻҪ�����������,
 * ͨ��1 �ͻ���PWM�ź����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		    // �����������
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
