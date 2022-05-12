/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-17     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "canfestival.h"
#include "CO_slave.h"
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#define LED1_PIN GET_PIN(C, 13)

#define RLY1_PIN GET_PIN(B, 3)
#define RLY2_PIN GET_PIN(B, 5)
#define RLY3_PIN GET_PIN(B, 7)
#define RLY4_PIN GET_PIN(B, 9)


void IO_Set(CAN_PORT ctrl, Message *RxMsg)
{
    Message TxMsg;
    //返回设置状态；
    if ((RxMsg->data[1] == 0x01) || (RxMsg->data[4] == 0x01))
        rt_pin_write(RLY1_PIN, PIN_HIGH);
    if ((RxMsg->data[1] == 0x02) || (RxMsg->data[4] == 0x02))
        rt_pin_write(RLY2_PIN, PIN_HIGH);
    if ((RxMsg->data[1] == 0x04) || (RxMsg->data[4] == 0x04))
        rt_pin_write(RLY3_PIN, PIN_HIGH);
    if ((RxMsg->data[1] == 0x08) || (RxMsg->data[4] == 0x08))
        rt_pin_write(RLY4_PIN, PIN_HIGH);

    rt_pin_write(RLY1_PIN, PIN_HIGH);
    TxMsg.cob_id = 0x280 + NODE_SLAVE; ///>使用PDO2tx
    TxMsg.rtr = 0;
    for (int ii = 0; ii < 8; ii++)
    {
        TxMsg.data[ii] = 0x00;
    }
    TxMsg.data[0] = 0x0F;
    TxMsg.data[1] = RxMsg->data[1];
    TxMsg.data[2] = 0x01;
    TxMsg.data[4] = RxMsg->data[4];
    TxMsg.data[5] = 0x01;
    TxMsg.len = 8;

    canSend((CAN_PORT *) 1, &TxMsg);
    canSend((CAN_PORT *) 2, &TxMsg);
}

int main(void)
{
    int count = 1;

    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

    rt_pin_mode(RLY1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(RLY2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(RLY3_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(RLY4_PIN, PIN_MODE_OUTPUT);

    while (count++)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
