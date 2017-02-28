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
// --------------------------
// 本文定义了Atmega16定时模块
// 
// ==========================================================================================================
#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__



#include <stdint.h>
#include <avr/interrupt.h>
#include "Drv_IO_Port.h"
#include "Drv_Sys.h"
#include "Drv_Timer.h"
#include "sys_event.h"
#include "sys_config.h"
#include "sys_warning.h"



#define SYS_TASK_MAX         4  // 最多支持10个任务
#define SYS_TASK_RUN_MAX    10  // 1个任务的就绪状态的最大值


typedef enum 
{
    SYS_TASK_TYPE_PRE_EM = 0,   // 抢占式任务
    SYS_TASK_TYPE_CO_OP  = 1    // 合作式任务
}SYS_TASK_TYPE;


void sys_task_init(void);
void sys_task_start(void);
void sys_task_dispatch(void);
void sys_task_delete(const uint8_t index);
uint8_t sys_task_add(const uint8_t delay, const uint8_t period, const p_void_funtion_void task, const uint8_t co_op, const p_void_funtion_void init);
uint8_t sys_task_index(const p_void_funtion_void task);

void delay_ms(const uint16_t count);



#endif // #ifndef __SYS_TIMER_H__
