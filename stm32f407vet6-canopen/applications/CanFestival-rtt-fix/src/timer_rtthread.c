#include <stdbool.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "canfestival.h"
#include "timers.h"

static rt_mutex_t canfstvl_mutex = RT_NULL;
static rt_sem_t canfstvl_timer_sem = RT_NULL;
static rt_device_t canfstvl_timer_dev=RT_NULL;
static rt_hwtimerval_t last_timer_val;

void EnterMutex(void)
{
    rt_mutex_take(canfstvl_mutex, RT_WAITING_FOREVER);
}

void LeaveMutex(void)
{
    rt_mutex_release(canfstvl_mutex);
}

void setTimer(TIMEVAL value)
{
    // Jian: set a small interval to prevent a bug related with modifying timer
    // configurations (HB producer interval eg.)
    if (value < 10) {
        value = 10;
    }

    rt_hwtimerval_t val;
    
    val.sec = value / 1000000;
    val.usec = value % 1000000;
    last_timer_val.sec = 0;
    last_timer_val.usec = 0;
    rt_device_write(canfstvl_timer_dev, 0, &val, sizeof(val));
}

TIMEVAL getElapsedTime(void)
{
    rt_hwtimerval_t val;
    
    rt_device_read(canfstvl_timer_dev, 0, &val, sizeof(val));

    return (val.sec - last_timer_val.sec) * 1000000 + (val.usec - last_timer_val.usec);
}

static void canopen_timer_thread_entry(void* parameter)
{    
    while(1)
    {
        rt_sem_take(canfstvl_timer_sem, RT_WAITING_FOREVER);
        EnterMutex();
        TimeDispatch();
        LeaveMutex();
    }
}


static rt_err_t timer_timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_device_read(canfstvl_timer_dev, 0, &last_timer_val, sizeof(last_timer_val));
    rt_sem_release(canfstvl_timer_sem);
    
    return RT_EOK;
}


void TimerInit(void)
{
    rt_err_t err;
    rt_hwtimer_mode_t mode;
    int freq = 1000000;

    canfstvl_mutex = rt_mutex_create("canfstvl",RT_IPC_FLAG_FIFO);
    canfstvl_timer_sem = rt_sem_create("canfstvl", 0, RT_IPC_FLAG_FIFO);

    canfstvl_timer_dev = rt_device_find(CANFESTIVAL_TIMER_DEVICE_NAME);
    if (canfstvl_timer_dev == RT_NULL) {
        rt_kprintf("Can't find %s device!\n", CANFESTIVAL_TIMER_DEVICE_NAME);
        return;
    }

    err = rt_device_open(canfstvl_timer_dev, RT_DEVICE_OFLAG_RDWR);
    if (err != RT_EOK) {
        rt_kprintf("CanFestival open timer Failed! err=%d\n", err);
        return;
    }

    rt_device_set_rx_indicate(canfstvl_timer_dev, timer_timeout_cb);

    err = rt_device_control(canfstvl_timer_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    if (err != RT_EOK) {
        rt_kprintf("Set Freq=%dhz Failed\n", freq);
    }

    mode = HWTIMER_MODE_ONESHOT;
    err = rt_device_control(canfstvl_timer_dev, HWTIMER_CTRL_MODE_SET, &mode);
    rt_device_read(canfstvl_timer_dev, 0, &last_timer_val, sizeof(last_timer_val));
}

void StartTimerLoop(TimerCallback_t init_callback) 
{
    rt_thread_t tid;

    tid = rt_thread_create("cf_timer",
                           canopen_timer_thread_entry, RT_NULL,
                           1024, CANFESTIVAL_TIMER_THREAD_PRIO, 20);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
    }
    else {
        rt_kprintf("CanFestival start timer thread Failed!\n");
        return;
    }

    EnterMutex();
    init_callback(NULL, 0);
    LeaveMutex();
}


