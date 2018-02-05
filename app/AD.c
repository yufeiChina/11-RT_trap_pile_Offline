#include "AD.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"										   
#include <stdio.h>
#include "supply_power.h"

#define ADCConver_size       1024		 // 10240��ֵ�ɼ���Ҫ700MS	 ����������1ms	  1ms�ɼ�14.6������
__IO uint16_t ADCConvertedValue[ADCConver_size];
__IO uint16_t AD_AVER_VALUE;
__IO uint16_t AD_VALUE_SUM = 0;
/****************ADC Functions**************************************/
//��������ֻ���0V���ϵ�ADֵ 2015-4-30 14:02:08
#define  AD_VOLATAGE_HIGH          1834//2480      //�����������ĸ���2v�ĵ�ѹֵ


void ADC_fiter(void)
{		
    int i = 0; u32 sum = 0; int j = 0;
	
    for (i = 0; i < ADCConver_size; i++)    {
        if (ADCConvertedValue[i] < AD_VOLATAGE_HIGH) {
            sum += ADCConvertedValue[i];
            j++;
        }
    }
    if (j != 0)
        AD_AVER_VALUE = sum / j;
		/*
		* ������ǲɼ���������ѹ���϶��ǳ����ˡ��������ʱ������Ҫ��ֹ��硣2015-5-12 12:13:38
		* �����������״����������Ϊ�ѵ�ѹ����Ϊ0V 1834 .Ȼ�������ֹ1Vһ�³�磬���ֹͣ��硣
		*/
	else
		AD_AVER_VALUE  = 1834;

    //���ADֵ�ж��Ƿ�С��2V�����С��2V��������״̬
    //AD_VALUE_SUM = AD_AVER_VALUE >  AD_OUT_2V ? (AD_VALUE_SUM+1) : 0;
	//update2015-5-12 12:11:30 ztlchina ����AD�жϡ�
	//�����ǰ�cp��pe�̽�����֮�� cp�ĵ�ѹΪ0v�����ʱ��Ӧ�ý�ֹ��硣��������Ϊ����1v��ֹ��硣
	AD_VALUE_SUM = AD_AVER_VALUE >  AD_OUT_1V ? (AD_VALUE_SUM+1) : 0;
}


//ADC GPIO�˿�����
void ADC_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);								
  /* Configure PA.00 (ADC Channel0) as analog input */ 
  /*PA0 ��Ϊģ��ͨ��0��������*/                         
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void DMA_init(void)
{
  DMA_InitTypeDef DMA_InitStructure;      //DMA��ʼ���ṹ������ 
  /* DMA1 channel1 configuration ---*/ 
  DMA_DeInit(DMA1_Channel1);												      //  DMAͨ������
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                     //  DMA��Ӧ���������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;             //  �ڴ�洢����ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                          //  DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
  DMA_InitStructure.DMA_BufferSize = ADCConver_size;		                      //  DMA�����С��1��,��λΪDMA_MemoryDataSize
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	              //  ����һ�����ݺ��豸��ַ��ֹ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                      // �رս���һ�����ݺ�Ŀ���ڴ��ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     // �����������ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;             // DMA�����ݳߴ磬HalfWord����Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                   // ת��ģʽ��ѭ������ģʽ��
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                  	          // DMA���ȼ���
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                              // M2Mģʽ����
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);           
  
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);  //��DMA�ж� 
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

// ADCģʽ����
static void ADC_Mode_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;      //ADC��ʼ���ṹ������
  
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //  ������ת��ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;		                     //  ����ɨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    //  ��������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //  ADC�ⲿ���أ��ر�״̬
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //  ���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ
  ADC_InitStructure.ADC_NbrOfChannel = 1;	                         // ����ͨ������1��
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel0 configuration ADCͨ���飬 ��0��ͨ�� ����˳��1��ת��ʱ�� */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	  //ADC���ʹ��
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);        //����ADC1 
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  //����У׼
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));        //�ȴ�����У׼���
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);	 	               //��ʼУ׼
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   //�ȴ�У׼���
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM����

}
void DMA_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable DMA channel1 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//ADC��ʼ��
void  ADC1_Init(void)  
{
  ADC_GPIO_Config();
  DMA_NVIC_Configuration();
  DMA_init();
  ADC_Mode_Config();
}  
