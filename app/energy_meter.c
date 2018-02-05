/*
*FileName:energy_meter.c
*Author  :ztlchina@foxmail.com
*Date    :2017-11-13 00:13:06
*Desc    :���ܱ�ģ��
*/
#include <rtthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "board.h"
#include "energy_meter.h"
#include "ensentec_lib.h"
#include "Public.h"

#define MeterDBG rt_kprintf
#define MeterInf rt_kprintf
#define MeterErr rt_kprintf

#define Meter_Add    0x01
#define Meter_CMD   {0x01, 0x03, 0x00, 0x48, 0x00, 0x06, 0x45, 0xDE}
//#define Meter_CMD   {0x01, 0x03, 0x00, 0x00, 0x00, 0x12, 0xC5, 0xC7}

#define  DIANLIU_ALLOW_MAX         160         
#define  DIANYA_ALLOW_MAX          2500
#define  DIANYA_ALLOW_MIN          1800


#define Meter_DEVICE_NAME      "uart2"
#define Meter_buff_size_max    64
typedef struct {
    rt_device_t   dev;           //�豸������
    struct rt_semaphore  r_tx;          //�������ź���
    unsigned char buff[Meter_buff_size_max];
    unsigned char buff_i;  
    unsigned char r_sta;         //����״̬��
    unsigned char guoliu_times;
    unsigned char guoya_times;
    unsigned char qianya_times;
    unsigned char errtime;
    void (*err_process)(int errcode);
    void (*sucess_process)(unsigned char *card, u8 cardlen);
}Meter_DEV ;
Meter_DEV meterdev;

struct dianneng g_dianneng_temp;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t Meter_stack[ 1024 ];
static struct rt_thread Meter_thread;
/*
 ���ڽ��յĻص�����
 ���������ź���
 ����һ�����յ�������, �͵��øú���.
*/
static rt_err_t meter_rx_ind(rt_device_t dev, rt_size_t size)
{
    //RT_ASSERT(dev != RT_NULL);

    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&(meterdev.r_tx));
    //rt_kprintf("%s:%d:%d\r\n", __FUNCTION__, size, meterdev.r_tx.value);

    return RT_EOK;
}

/*
* �ȴ��������ܱ�
* ���ȴ�ʱ��maxtime,
* ����ֵ:
*    0: ��ȡ��ʱ;
*    1: ��ȡ�ɹ�
*   -1: ��ȡʧ��,CRC����.
*/
int memter_waitRead(int maxtime)
{
    rt_tick_t  timeout_tick    = 0;
    unsigned char ch = 0;
    unsigned char len = 0;
    rt_err_t ret;
   
    RT_ASSERT(meterdev.dev);

    timeout_tick = rt_tick_get() + maxtime; //rt_tick_get ��ȡ��ǰtick.

    /* ��ս��ղ���.,������׼�� */
    meterdev.r_sta = 0;
    meterdev.buff_i = 0;
    memset(meterdev.buff, 0, Meter_buff_size_max);

    while(1)
    {
        if (rt_tick_get() > timeout_tick)
        {
            return 0;       //��ȡ��ʱ
        }

        /* �ȴ��ź���(�ź����ڻص������б�release.) */
        if ((ret = rt_sem_take(&meterdev.r_tx, 1000)) != RT_EOK)
        { 
            meterdev.errtime++;
            rt_kprintf("\tmeterdev take sem error %d\r\n", ret);
            continue;
        }
        /* read one character from device */
        while (rt_device_read(meterdev.dev, 0, &ch, 1) == 1)
        {
            /*
            ���ǵ�Э���ʽ��
            01:   ͷ
            03:   ����
            24:   ����
            xx  xx  xx  xx  //��ѹ
            xx  xx  xx  xx  //����
            ...
            ...
            xx  xx  xx  xx //..   �ܳ���0x24
            CRC  CRC
            */
            switch (meterdev.r_sta)
            {
                case 0:     //Find Head
                    if (ch == Meter_Add)
                    {
                        meterdev.buff[meterdev.buff_i++] = ch;
                        meterdev.r_sta = 1;
                    }
                    break;
                case 1:    
                    meterdev.buff[meterdev.buff_i++] = ch;  
                    if (ch == 0x03)     //03: ��������
                    {
                        meterdev.r_sta = 2;
                    }
                    else 
                    {
                        meterdev.r_sta = 0;
                    }
                    break;
                case 2:    //GetData
                    meterdev.buff[meterdev.buff_i++] = ch;
                    if (ch == 0x18 )
                    {
                        len = ch;
                        meterdev.r_sta = 3;
                    }
                    else 
                    {
                        meterdev.r_sta =0;
                    }
                    break;
                case 3:
                    meterdev.buff[meterdev.buff_i++] = ch;
                    if (meterdev.buff_i >= (len + 5))      //Э������ܳ���Ϊ
                    {
                        //Check crc
                        if (CRC16(meterdev.buff, meterdev.buff_i) == 0)
                        {
                            g_dianneng_temp.dianya 		       = (((u32)meterdev.buff[ 3]) << 24) |
                                (((u32)meterdev.buff[ 4]) << 16) |
                                (((u32)meterdev.buff[ 5]) <<  8) |
                                (((u32)meterdev.buff[ 6]))       ;//��ѹ *10000 V
                            g_dianneng_temp.dianliu 		       = (((u32)meterdev.buff[ 7]) << 24) |
                                (((u32)meterdev.buff[ 8]) << 16) |
                                (((u32)meterdev.buff[ 9]) <<  8) |
                                (((u32)meterdev.buff[ 10]))       ;//����		  A
                            g_dianneng_temp.yougonggonglv       = (((u32)meterdev.buff[ 11]) << 24) |
                                (((u32)meterdev.buff[ 12]) << 16) |
                                (((u32)meterdev.buff[  13]) <<  8) |
                                (((u32)meterdev.buff[ 14]))       ;//�й����� *10000 V
                            g_dianneng_temp.dangqiandianneng = (((u32)meterdev.buff[ 15]) << 24) |
                                (((u32)meterdev.buff[ 16]) << 16) |
                                (((u32)meterdev.buff[ 17]) <<  8) |
                                (((u32)meterdev.buff[ 18]))       ;//�й����� *10000 V
                            g_dianneng_temp.gonglvyinshu        = (((u32)meterdev.buff[ 19]) << 24) |
                                (((u32)meterdev.buff[ 20]) << 16) |
                                (((u32)meterdev.buff[ 21]) <<  8) |
                                (((u32)meterdev.buff[ 22]))       ;//�������� *10000 V
                  
                            g_dianneng_temp.systicks = rt_tick_get() / 1000;
                            g_dianneng_temp.dianya = g_dianneng_temp.dianya / 1000;
                            g_dianneng_temp.dianliu = g_dianneng_temp.dianliu / 1000;
                            g_dianneng_temp.yougonggonglv = g_dianneng_temp.yougonggonglv / 100;
                            g_dianneng_temp.dangqiandianneng = g_dianneng_temp.dangqiandianneng / 100;
                            g_dianneng_temp.gonglvyinshu = g_dianneng_temp.gonglvyinshu ;
                            if(!g_Status.read_begin_energy)
                            {
                                g_dianneng_temp.begindianneng = g_dianneng_temp.dangqiandianneng;
                                rt_kprintf("66666666666666666666666666666666_dianneng_temp.dangqiandianneng  = %d     g_dianneng_temp.begindianneng = %d  ", g_dianneng_temp.dangqiandianneng, g_dianneng_temp.begindianneng); 
                                g_Status.read_begin_energy = 1;
                            }
                            
                            return 1;
                        }
                        else
                        {
                            return -1;
                        }
                    }
                    break;
            }
        }
    }
}


void meter_process(void *s)
{
    int ret = 0;
    int len = 0;

    unsigned char cmdbuff[] = Meter_CMD;
    meterdev.qianya_times = 0;
    meterdev.guoya_times = 0;
    meterdev.guoliu_times = 0;
    
    while (1)
    {
         /* ���ͼ���ָ�� */
        len = rt_device_write(meterdev.dev, 0, cmdbuff, sizeof(cmdbuff));
        
       // MeterErr("\r\nEnergyMeter rt_device_write %d.\r\n", len);
        ret = memter_waitRead(2000);
        
        if(ret == 0)
        {
            MeterErr("\r\nEnergyMeter rt_device_write %d.\r\n", len);
        }
        else if (ret == -1)
        {
            ;
        }
        else if (ret == 1)
        { 
            meterdev.errtime = 0;
            rt_kprintf("Get Energy:V:%lu, I:%lu, P::%lu, W:%lu, Y:%lu, T:%lu.\r\n", 
                        g_dianneng_temp.dianya, g_dianneng_temp.dianliu, g_dianneng_temp.yougonggonglv,
                        g_dianneng_temp.dangqiandianneng, g_dianneng_temp.gonglvyinshu, g_dianneng_temp.systicks);
            
            g_Status.clear_err_times++;
            if( g_dianneng_temp.dianya <= DIANYA_ALLOW_MIN)    //Ƿѹ����
            {
                g_Status.clear_err_times = 0;
                meterdev.qianya_times++;
                if(meterdev.qianya_times >= 7)
                {
                    rt_kprintf("error error error error error error error error error error error error error error error error S_ERR_VOL_LESS\r\n");
                    g_Status.sdst_error = S_ERR_VOL_LESS;
                }
            }
            else
            {
                meterdev.qianya_times = 0;
            }
            
            if( g_dianneng_temp.dianya >= DIANYA_ALLOW_MAX)    //��ѹ����
            {
                g_Status.clear_err_times = 0;
                meterdev.guoya_times++;
                if(meterdev.guoya_times >= 7)
                {
                    rt_kprintf("error error error error error error error error error error error error error error error error S_ERR_VOL_BIGGER\r\n");
                    g_Status.sdst_error = S_ERR_VOL_BIGGER;
                }
            }
            else
            {
                meterdev.guoya_times = 0;
            }
            if(g_Status.gun_status != SD_GUN_ST_OUT)
            {
                if( g_dianneng_temp.dianliu >= DIANLIU_ALLOW_MAX)    //��������
                {
                    g_Status.clear_err_times = 0;
                    meterdev.guoliu_times++;
                    if(meterdev.guoliu_times >= 3)
                    {
                        rt_kprintf("error error error error error error error error error error error error error error error error S_ERR_CURRENT_OVER\r\n");
                        g_Status.sdst_error = S_ERR_CURRENT_OVER;
                    }
                }
                else
                {
                    meterdev.guoliu_times = 0;
                }
            }
        }
        
        if(meterdev.errtime >= 15)
        {
            rt_kprintf("error error error error error error error error error error error error error error error error S_ERR_DIANNENG\r\n");
            g_Status.sdst_error = S_ERR_DIANNENG;       //���ܱ����
        }
        
            
        rt_thread_sleep(700);
    }
}

rt_err_t energy_meter_init(void)
{
    rt_err_t result;
    memset(&meterdev, 0, sizeof(Meter_DEV));

    /* ��ʼ���ź��� */
    if (RT_EOK !=  rt_sem_init(&(meterdev.r_tx), "MeterSem", 0, RT_IPC_FLAG_FIFO))
    {
        MeterErr("EnergyMeter Creat Meter Sem error.\r\n");
        return RT_ERROR;
    }
    MeterInf("EnergyMeter Creat Meter Sem = %d \r\n", meterdev.r_tx.value);
    /* ��ʼ������ */
    if (meterdev.dev == RT_NULL)
    {
        meterdev.dev = rt_device_find(Meter_DEVICE_NAME);
        RT_ASSERT(meterdev.dev);

        
        
        /* �򿪴��� */
        if (RT_EOK == rt_device_open(meterdev.dev, 
                                        (RT_DEVICE_OFLAG_RDWR       //��д��ʽ��
                                        | RT_DEVICE_FLAG_STREAM     //��ʽģʽ
                                        | RT_DEVICE_FLAG_INT_RX     //�ж�ʽ����
                                        | RT_DEVICE_FLAG_INT_TX)))  //�ж�ʽ����
        {
            MeterInf("EnergyMeter serial open success.\r\n");
            //return RT_EOK;
        }
        else
        {
            MeterErr("EnergyMeter serial open failure.\r\n");
            return RT_ERROR;
        }
        
        /* ���ô��ڵĻص����� */
        rt_device_set_rx_indicate(meterdev.dev, meter_rx_ind);

    }

    result = rt_thread_init(&Meter_thread,      //�߳�������
                            "meter",                        //�߳�����
                            meter_process,                    //�̵߳����
                            RT_NULL,                        //����
                            &Meter_stack[0],                //�̵߳�ջ
                            sizeof(Meter_stack),            //ջ�Ĵ�С
                            25,                             //���ȼ�     21 ~ 32
                            30);                            //ʱ��Ƭ     30: 30ms
    if (result == RT_EOK)
        rt_thread_startup(&Meter_thread);       //�����߳�
    else 
    {
        return RT_ERROR;
    }

    return RT_EOK;
}
