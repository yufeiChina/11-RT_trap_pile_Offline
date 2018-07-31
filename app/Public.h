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

#define AD_OUT_V_11P2V     	343		//实测外部电压11.8V，为了保持一致，还用11P2V来定义宏，下面一致
#define AD_OUT_V_10P8		381		//外部电压10.8V
#define AD_OUT_V_9P6          
#define AD_OUT_V_8P4       	769		//外部电压8.4V
#define AD_OUT_V_8V			800	    //外部电压8v
#define AD_OUT_V_7P5V       836     //外部电压7.5V
#define AD_OUT_V_7V         900     //外部电压7V
#define AD_OUT_2V			1567	//外部电压2v   
#define AD_OUT_1V		    1700    //外部电压1V
#define AD_OUT_0P5V			1767	//外部电压0.5V

#define GUN_TO_WAIT   (AD_AVER_VALUE < AD_OUT_V_11P2V)      //   AD  > 11.2v   0 ~ 343	

#define GUN_TO_INPUT	 (AD_AVER_VALUE > AD_OUT_V_10P8 && AD_AVER_VALUE < AD_OUT_V_8V) //  10.8V  AD  8v     381 ~ 800 

#define GUN_TO_CONFIRM	 (AD_AVER_VALUE > AD_OUT_V_8V && AD_AVER_VALUE < AD_OUT_2V)  //8.4V  AD   2V    800 ~ 1567   
 

//2015-5-12 12:17:24 为了应对CP PE短接的毛病，我们这里把低于1V的状态定义为错误状态。进入该状态后，我们不在进行充电
//只能拔枪后才能再次充电。
//70ms检测一次。 我们如果连续检测到14次 70*14 = 980 大概是1s的低电压故障我们进入错误处理状态。
#define AD_ERROR_VOLTAGE_CHECK_TIME    14
#define GUN_TO_ERR	(AD_VALUE_SUM > AD_ERROR_VOLTAGE_CHECK_TIME ) //(0) //(AD_AVER_VALUE > AD_OUT_2V)

//#define GPIO_Pin14_Pin15   


//充电枪状态
typedef enum  
{
    SD_GUN_ST_OUT = 0,					//充电枪拔出       12V
    SD_GUN_ST_CNCTING,					//充电枪连接中     9V
    SD_GUN_ST_CNCTOK,					//充电枪连接确认ok  6V 
}S_GUN_STATUS;


//充电状态
typedef enum supplypower_status
{
    SPT_PILE_Init = 0,       //充电桩初始化
    SPT_PILE_Work,			 //充电桩工作
    SPT_PILE_Wait,			 //充电桩待机状态
    SPT_PILE_Input,		     //充电桩插枪
    SPT_PILE_Confirm,        //充电桩确认
    SPT_PILE_Over,			 //充电桩结束充电
    SPT_PILE_Error           //充电桩故障
}SUPPLYPOWER_STATUS;


//继电器状态
typedef enum  
{
    RELAY_OFF= 0,
    RELAY_ON 
}RELAY_STATUS ;


//错误状态
typedef enum  
{
    S_ERR_ST_NO = 0,                 //充电桩没有故障
    S_ERR_VOL_LESS,                  //充电桩欠压         1
    S_ERR_VOL_BIGGER,                //充电桩过压         2
    S_ERR_CURRENT_OVER,              //充电桩过流         3
    S_ERR_DIANNENG,                  //电能表故障         4
    S_ERR_CP_VOL_LESS,			     //枪端电压过小       5
    S_ERR_CPU_CARD,                  //cpu卡故障          6
    S_ERR_E_STOP,
    S_ERR_IO_KEY
}S_ERR_STATUS;


//服务器是否允许
typedef enum   {
    SVR_ALLOW_SUPPLY_NO = 0,	    	      //服务器不允许充电
    SVR_ALLOW_SUPPLY_OK                       //服务器允许充电
}SVR_SUPPLY_STATUS;


//灯状态
typedef enum   {
    LED_OFF = 0,	    //常灭
    LED_ON,             //常亮
    LED_FLASH,          //慢闪烁
    LED_FLASH_HARRY,    //快闪
    LED_FLASH1,         //闪一次 
    LED_FLASH2,         //闪两次
    LED_FLASH3,         //闪三次
    LED_FLASH4,         //闪四次
    LED_FLASH5,          //闪五次
    LED_FLASH6
    
}LED_STATUS;


//连接状态
typedef enum   {
    GPRS_CONNECT = 0,	 //GPRS连接中
    MQTT_CONNECT,        //MQTT连接中
    CONNECT_OK           //连接成功
}CONNECT_STATUS;


//连接状态
typedef enum   {
    SWIPE_CARD_NO = 0,	  //未刷卡
    SWIPE_CARD_OK,        //刷卡成功
    SWIPE_CARD_ING        //刷卡充电中
}SWIPE_CARD_STATUS;         


typedef enum    {
    SCREEN_PAGE_LOGO = 0,       //开机LOGO
    SCREEN_PAGE_WELCOME,        //欢迎界面
    SCREEN_PAGE_WAIT,           //待机
    SCREEN_PAGE_INPUT,          //插枪未确认
    SCREEN_PAGE_WORK,           //充电中
    SCREEN_PAGE_OVER,           //结束充电(结算界面)
    SCREEN_PAGE_PLEASE_SWIPE,   //请刷卡 
    SCREEN_PAGE_PLEASE_SWIPE_OR_SCAN,    //请扫码 刷卡 
    SCREEN_PAGE_Err             //错误
    
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


