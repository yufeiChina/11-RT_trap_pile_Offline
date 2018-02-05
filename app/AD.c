#include "AD.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"										   
#include <stdio.h>
#include "supply_power.h"

#define ADCConver_size       1024		 // 10240个值采集需要700MS	 方波周期是1ms	  1ms采集14.6个数据
__IO uint16_t ADCConvertedValue[ADCConver_size];
__IO uint16_t AD_AVER_VALUE;
__IO uint16_t AD_VALUE_SUM = 0;
/****************ADC Functions**************************************/
//这里我们只检测0V以上的AD值 2015-4-30 14:02:08
#define  AD_VOLATAGE_HIGH          1834//2480      //舍弃采样到的高于2v的电压值


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
		* 如果我们采集不到正电压，肯定是出错了。所以这个时候我们要禁止充电。2015-5-12 12:13:38
		* 如果出现这种状况，我们认为把电压设置为0V 1834 .然后如果禁止1V一下充电，则会停止充电。
		*/
	else
		AD_AVER_VALUE  = 1834;

    //检测AD值判断是否小于2V，如果小于2V则进入错误状态
    //AD_VALUE_SUM = AD_AVER_VALUE >  AD_OUT_2V ? (AD_VALUE_SUM+1) : 0;
	//update2015-5-12 12:11:30 ztlchina 增加AD判断。
	//当我们把cp与pe短接起来之后， cp的电压为0v，这个时候应该禁止充电。我们这里为低于1v禁止充电。
	AD_VALUE_SUM = AD_AVER_VALUE >  AD_OUT_1V ? (AD_VALUE_SUM+1) : 0;
}


//ADC GPIO端口配置
void ADC_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);								
  /* Configure PA.00 (ADC Channel0) as analog input */ 
  /*PA0 作为模拟通道0输入引脚*/                         
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void DMA_init(void)
{
  DMA_InitTypeDef DMA_InitStructure;      //DMA初始化结构体声明 
  /* DMA1 channel1 configuration ---*/ 
  DMA_DeInit(DMA1_Channel1);												      //  DMA通道配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                     //  DMA对应的外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;             //  内存存储基地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                          //  DMA的转换模式为SRC模式，由外设搬移到内存
  DMA_InitStructure.DMA_BufferSize = ADCConver_size;		                      //  DMA缓存大小，1个,单位为DMA_MemoryDataSize
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	              //  接收一次数据后，设备地址禁止后移
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                      // 关闭接收一次数据后，目标内存地址后移
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     // 定义外设数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;             // DMA搬数据尺寸，HalfWord就是为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                   // 转换模式，循环缓存模式。
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                  	          // DMA优先级高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                              // M2M模式禁用
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);           
  
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);  //打开DMA中断 
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

// ADC模式配置
static void ADC_Mode_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;      //ADC初始化结构体声明
  
  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //  独立的转换模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;		                     //  开启扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    //  开启连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //  ADC外部开关，关闭状态
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //  对齐方式,ADC为12位中，右对齐方式
  ADC_InitStructure.ADC_NbrOfChannel = 1;	                         // 开启通道数，1个
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel0 configuration ADC通道组， 第0个通道 采样顺序1，转换时间 */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);	  //ADC命令，使能
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);        //开启ADC1 
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  //重新校准
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));        //等待重新校准完成
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);	 	               //开始校准
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	   //等待校准完成
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//连续转换开始，ADC通过DMA方式不断的更新RAM区。

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

//ADC初始化
void  ADC1_Init(void)  
{
  ADC_GPIO_Config();
  DMA_NVIC_Configuration();
  DMA_init();
  ADC_Mode_Config();
}  
