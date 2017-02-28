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
// --------------------------------------
// 本文定义了系统提示和警告
// 
// --------------------------------------
// 包含：
// 
// ==========================================================================================================
#ifndef __SYS_WARNING_H__
#define __SYS_WARNING_H__



#include <stdint.h>
#include "sys_event.h"
#include "sys_config.h"


// EVENT_SYS的参数
typedef enum 
{
    MSG_SYS_TASK_DELAYED, // 有任务被延迟
    MSG_SYS_EVENT_LOCKED, // 有事件被锁定

    MSG_SYS_SLEEP,
    MSG_SYS_WAKEUP,
    MSG_SYS_IDLE,
    MSG_SYS_START
}MSG_EVENT_SYS;

// MSG_EVENT_SYS的结构(32bit)
typedef struct 
{                               // 数据放在一个或半个字节里面、调试的时候以十六进制格式查看会更方便
    uint8_t  event_index  : 8;  // bit[07:00]被锁定的事件号
    uint8_t  task_index   : 8;  // bit[15:08]被延迟的任务号
    uint8_t  event_locked : 1;  // bit[  :16]有事件被锁定
    uint8_t  task_delayed : 1;  // bit[  :17]有任务被延迟

    uint16_t reserved     : 10; // bit[27:18]用于将来扩展消息

    uint8_t  sys_sleep    : 1;  // bit[  :28]系统休眠
    uint8_t  sys_wakeup   : 1;  // bit[  :29]系统唤醒
    uint8_t  sys_ldle     : 1;  // bit[  :30]系统空闲
    uint8_t  sys_start    : 1;  // bit[  :31]系统开机
}T_MSG_EVENT_SYS, *pT_MSG_EVENT_SYS;

// MSG_EVENT_SYS的联合体结构、更适合在函数中进行操作
typedef union
{
    T_MSG_EVENT_SYS msg;
    uint32_t data;
}U_MSG_EVENT_SYS;

void sys_update_event(const uint8_t type, const uint32_t msg, const uint8_t index);



#endif // #ifndef __SYS_WARNING_H__
