/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-29     misonyo      first implementation.
 */
/*
 * 程序清单： ADC 设备使用例程
 * 例程导出了 adc_sample 命令到控制终端
 * 命令调用格式：adc_sample
 * 程序功能：通过 ADC 设备采样电压值并转换为数值。
 *           示例代码参考电压为3.3V,转换位数为12位。
*/

#include <rtthread.h>
#include <rtdevice.h>
#include "SillySlave.h"
#include "canfestival.h"
#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     4           /* ADC 通道 */
#define ADC_DEV_CHANNEL1     1           /* ADC 通道 */
#define ADC_DEV_CHANNEL2     2           /* ADC 通道 */
#define ADC_DEV_CHANNEL3     3           /* ADC 通道 */
#define ADC_DEV_CHANNEL4     4           /* ADC 通道 */

#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

static int adc_vol_sample(int argc, char *argv[])
{
    rt_adc_device_t adc_dev;
    rt_uint32_t value, vol;
    rt_err_t ret = RT_EOK;

    /* 查找设备 */
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);

    /* 读取采样值 */
    value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    rt_kprintf("the value is :%d \n", value);

    /* 转换为对应电压值 */
    vol = value * REFER_VOLTAGE / CONVERT_BITS;
    rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);

    /* 关闭通道 */
    ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(adc_vol_sample, adc voltage convert sample);



void adc_thread(void)
{
    rt_adc_device_t adc_dev;
    rt_err_t ret = RT_EOK;
    /* 查找设备 */
    adc_dev = (rt_adc_device_t) rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL1);
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL2);
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL3);
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL4);

    while (1)
    {
        /* 读取采样值 */
        CHANEL1 = rt_adc_read(adc_dev, ADC_DEV_CHANNEL1);
        CHANEL2 = rt_adc_read(adc_dev, ADC_DEV_CHANNEL2);
        CHANEL3 = rt_adc_read(adc_dev, ADC_DEV_CHANNEL3);
        CHANEL4 = rt_adc_read(adc_dev, ADC_DEV_CHANNEL4);
        rt_kprintf("the CHANEL1 is :%d \n", CHANEL1);

        /* 转换为对应电压值 */
        CHANEL1 = CHANEL1 * REFER_VOLTAGE / CONVERT_BITS;
        CHANEL2 = CHANEL2 * REFER_VOLTAGE / CONVERT_BITS;
        CHANEL3 = CHANEL3 * REFER_VOLTAGE / CONVERT_BITS;
        CHANEL4 = CHANEL4 * REFER_VOLTAGE / CONVERT_BITS;
        rt_kprintf("the voltage is :%d.%02d \n", CHANEL1 / 100, CHANEL1 % 100);
        rt_thread_mdelay(10);
    }
}

uint16_t adc_init(void)
{

    rt_thread_t thread;
    rt_err_t ret = RT_EOK;

    thread = rt_thread_create("can_rx", adc_thread, RT_NULL, 1024, 25, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("create adc_thread  failed!\n");
    }



//    /* 关闭通道 */
//    ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);

    return ret;
}


