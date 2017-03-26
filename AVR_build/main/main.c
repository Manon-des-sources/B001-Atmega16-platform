// ==========================================================================================================
// Copyright (c) 2016 ChenChen <codingmanon@163.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
// associated documentation files (the "Software"), to deal in the Software without restriction, including 
// without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject 
// to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// ----------------------------------------------------------------------------------------------------------
// 
// ==========================================================================================================
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Drv_WDT.h"
#include "Drv_noinit.h"
#include "Drv_IO_Port.h"
#include "Mod_SPI_Flash.h"
#include "Mod_SPI_manager.h"
#include "Mod_LED_Displayer.h"
#include "Mod_USART_manager.h"
#include "sys_init.h"
#include "sys_timer.h"
#include "sys_event.h"
#include "sys_config.h"


volatile uint32_t cout = 0;
volatile uint32_t second = 10000000;

T_EVENT_LIST event_32;
T_EVENT_LIST event_8;


void task_count_time(void);
T_EVENT_LIST event_buffer_task_count_time[2];
// ==========================================================================================================
// 任务事件缓存初始化
// 
// ==========================================================================================================
void task_count_time_event_buffer_init(void)
{
    uint8_t index;
    for(index = 0; index < _countof(event_buffer_task_count_time); index++)
    {
        event_buffer_task_count_time[index].lock = UNLOCKED;
        event_buffer_task_count_time[index].type = EVENT_MAX;
        event_buffer_task_count_time[index].data = 0;
    }
    // 将自己的事件缓存注册到事件管理器
    sys_event_buffer_set(task_count_time, event_buffer_task_count_time);
}

void task_count_time_init(void)
{
    // ----------
    // 硬件初始化
    Drv_IO_mode_bit(DDRD, DDD4, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD4);

    // --------------
    // 事件缓存初始化
    task_count_time_event_buffer_init();
}

void task_count_time(void)
{
    // 运行时刻标记、用来标记任务何时被调度
    Drv_IO_toggle_bit(PORTD, PD4);

    // ------------------------------------------
    // 消息查询

    // ------------------------------------------
    // 任务正文
    sys_event_unlock(EVENT_IR_SEND);
    if(++cout >= 250)
    {
        cout = 0;
        second++;
        // --------------------------------------
        // 组织事件、并通知事件管理模块
        event_buffer_task_count_time[0].type = EVENT_IR_SEND;
        event_buffer_task_count_time[0].data = second;
        event_buffer_task_count_time[1].type = EVENT_SEG_UPDATE;
        event_buffer_task_count_time[1].data = second;
        sys_event_buffer_post(task_count_time, _countof(event_buffer_task_count_time));

        sys_event_lock(EVENT_IR_SEND);
    }
}

void task02_init(void)
{
    Drv_IO_mode_bit(DDRD, DDD3, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD3);
}

void task02(void)
{
    // 运行时刻标记
    Drv_IO_toggle_bit(PORTD, PD3);
}

void task03_init(void)
{
    Drv_IO_mode_bit(DDRD, DDD2, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD2);
}

void task03(void)
{
    // 运行时刻标记
    Drv_IO_toggle_bit(PORTD, PD2);
}

// ==========================================================================================================
// main函数
// ==========================================================================================================
int main(void)
{
    // ----------
    // 关全局中断
    cli();
    // 系统初始化
    sys_init();
    // 开全局中断
    sei();

    // --------------
    // 任务队列初始化
    sys_task_init();
    // 注册3个任务
    sys_task_add(0, 4, task_count_time, SYS_TASK_TYPE_PRE_EM, task_count_time_init);
    sys_task_add(1, 4, task02, SYS_TASK_TYPE_CO_OP, task02_init);
    sys_task_add(2, 2, task_Mode_USART, SYS_TASK_TYPE_CO_OP, Mod_USART_init);
    // 任务添加完成后、再启动系统任务调度模块
    // 保证所有任务在同一步调上开始
    sys_task_start();

    // ---------------------------------------------------------------------
    // 所有任务都放入中断，CPU每次退出中断之后都直接进入休眠，可以有效地省电
    while(1)
    {
        sys_task_dispatch();
    }
    return 0;
}




















