/*******************************************************************************
 * Copyright (c) 2017 ,ENSENTEC
 * All rights reserved.
 * 
 * Version          V1.0.0
 * The Eclipse Public License is available at
 * http://www.ensentec.com
 * 
 * Contributors     tianqf
 *******************************************************************************/
 
#ifndef _PUBLIC_H_INCLUDE_
#define _PUBLIC_H_INCLUDE_

#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "AD.h"

//#define SCREEN_OPEN_CPUCARD_CLOSE

#define AD_OUT_V_11P2V     	343		//ʵ���ⲿ��ѹ11.8V��Ϊ�˱���һ�£�����11P2V������꣬����һ��
#define AD_OUT_V_10P8		381		//�ⲿ��ѹ10.8V
#define AD_OUT_V_9P6          
#define AD_OUT_V_8P4       	769		//�ⲿ��ѹ8.4V
#define AD_OUT_V_8V			800	    //�ⲿ��ѹ8v
#define AD_OUT_V_7P5V       836     //�ⲿ��ѹ7.5V
#define AD_OUT_V_7V         900     //�ⲿ��ѹ7V
#define AD_OUT_2V			1567	//�ⲿ��ѹ2v   
#define AD_OUT_1V		    1700    //�ⲿ��ѹ1V
#define AD_OUT_0P5V			1767	//�ⲿ��ѹ0.5V

#define GUN_TO_WAIT   (AD_AVER_VALUE < AD_OUT_V_11P2V)      //   AD  > 11.2v   0 ~ 343	

#define GUN_TO_INPUT	 (AD_AVER_VALUE > AD_OUT_V_10P8 && AD_AVER_VALUE < AD_OUT_V_8V) //  10.8V  AD  8v     381 ~ 800 

#define GUN_TO_CONFIRM	 (AD_AVER_VALUE > AD_OUT_V_8V && AD_AVER_VALUE < AD_OUT_2V)  //8.4V  AD   2V    800 ~ 1567   
 

//2015-5-12 12:17:24 Ϊ��Ӧ��CP PE�̽ӵ�ë������������ѵ���1V��״̬����Ϊ����״̬�������״̬�����ǲ��ڽ��г��
//ֻ�ܰ�ǹ������ٴγ�硣
//70ms���һ�Ρ� �������������⵽14�� 70*14 = 980 �����1s�ĵ͵�ѹ�������ǽ��������״̬��
#define AD_ERROR_VOLTAGE_CHECK_TIME    14
#define GUN_TO_ERR	(AD_VALUE_SUM > AD_ERROR_VOLTAGE_CHECK_TIME ) //(0) //(AD_AVER_VALUE > AD_OUT_2V)

//#define GPIO_Pin14_Pin15   


//���ǹ״̬
typedef enum  
{
    SD_GUN_ST_OUT = 0,					//���ǹ�γ�       12V
    SD_GUN_ST_CNCTING,					//���ǹ������     9V
    SD_GUN_ST_CNCTOK,					//���ǹ����ȷ��ok  6V 
}S_GUN_STATUS;


//���״̬
typedef enum supplypower_status
{
    SPT_PILE_Init = 0,       //���׮��ʼ��
    SPT_PILE_Work,			 //���׮����
    SPT_PILE_Wait,			 //���׮����״̬
    SPT_PILE_Input,		     //���׮��ǹ
    SPT_PILE_Confirm,        //���׮ȷ��
    SPT_PILE_Over,			 //���׮�������
    SPT_PILE_Error           //���׮����
}SUPPLYPOWER_STATUS;


//�̵���״̬
typedef enum  
{
    RELAY_OFF= 0,
    RELAY_ON 
}RELAY_STATUS ;


//����״̬
typedef enum  
{
    S_ERR_ST_NO = 0,                 //���׮û�й���
    S_ERR_VOL_LESS,                  //���׮Ƿѹ         1
    S_ERR_VOL_BIGGER,                //���׮��ѹ         2
    S_ERR_CURRENT_OVER,              //���׮����         3
    S_ERR_DIANNENG,                  //���ܱ����         4
    S_ERR_CP_VOL_LESS,			     //ǹ�˵�ѹ��С       5
    S_ERR_CPU_CARD,                  //cpu������          6
    S_ERR_E_STOP,
    S_ERR_IO_KEY
}S_ERR_STATUS;


//�������Ƿ�����
typedef enum   {
    SVR_ALLOW_SUPPLY_NO = 0,	    	      //��������������
    SVR_ALLOW_SUPPLY_OK                       //������������
}SVR_SUPPLY_STATUS;


//��״̬
typedef enum   {
    LED_OFF = 0,	    //����
    LED_ON,             //����
    LED_FLASH,          //����˸
    LED_FLASH_HARRY,    //����
    LED_FLASH1,         //��һ�� 
    LED_FLASH2,         //������
    LED_FLASH3,         //������
    LED_FLASH4,         //���Ĵ�
    LED_FLASH5,          //�����
    LED_FLASH6
    
}LED_STATUS;


//����״̬
typedef enum   {
    GPRS_CONNECT = 0,	 //GPRS������
    MQTT_CONNECT,        //MQTT������
    CONNECT_OK           //���ӳɹ�
}CONNECT_STATUS;


//����״̬
typedef enum   {
    SWIPE_CARD_NO = 0,	  //δˢ��
    SWIPE_CARD_OK,        //ˢ���ɹ�
    SWIPE_CARD_ING        //ˢ�������
}SWIPE_CARD_STATUS;         


typedef enum    {
    SCREEN_PAGE_LOGO = 0,       //����LOGO
    SCREEN_PAGE_WELCOME,        //��ӭ����
    SCREEN_PAGE_WAIT,           //����
    SCREEN_PAGE_INPUT,          //��ǹδȷ��
    SCREEN_PAGE_WORK,           //�����
    SCREEN_PAGE_OVER,           //�������(�������)
    SCREEN_PAGE_PLEASE_SWIPE,   //��ˢ�� 
    SCREEN_PAGE_PLEASE_SWIPE_OR_SCAN,    //��ɨ�� ˢ�� 
    SCREEN_PAGE_Err             //����
    
}SCREEN_PAGE_CHOSE;

struct sd_status
{
    S_GUN_STATUS            gun_status;
    S_ERR_STATUS            sdst_error;
    S_ERR_STATUS            sdst_error1;
    RELAY_STATUS            relay_status;
    SUPPLYPOWER_STATUS      supplypower;
    SVR_SUPPLY_STATUS       svr_allow_suplly; 
    CONNECT_STATUS          connect;
    SWIPE_CARD_STATUS       swipe_card;
    SCREEN_PAGE_CHOSE       screen_page_chose;
    
    LED_STATUS              led_status_red;
    LED_STATUS              led_status_yellow;
    LED_STATUS              led_status_blue;
    LED_STATUS              led_status_green;
    
    unsigned char           clear_err_times;
    rt_tick_t               charging_begin_time;
    rt_tick_t               charging_time;
    rt_tick_t               read_begin_energy;
    rt_tick_t               heat_time;
    
    rt_uint16_t             PWM_Set;
};
extern struct sd_status g_Status;


void  sd_status_init(void);


#endif


