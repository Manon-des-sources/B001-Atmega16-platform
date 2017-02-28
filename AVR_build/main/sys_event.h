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
// ---------------------------
// 本文定义了事件管理模块
// 
// 说明：
// (1).本文将事件(event)所带的参数称为消息(message)，以区分事件本身和事件的参数
// 
// ==========================================================================================================
#ifndef __SYS_EVENT_H__
#define __SYS_EVENT_H__



#include <avr/interrupt.h>
#include "sys_config.h"
#include "sys_timer.h"


// 事件(事件的类型tpye,为8bit)(事件的参数data,为32bit)
//     (除了默认的EVENT_MAX，至少还需要一个事件，避免sys_event_list[EVENT_MAX]变成sys_event_list[0])
typedef enum 
{
    EVENT_SYS,
    EVENT_KEY,
    EVENT_IR_RECIEVE,
    EVENT_IR_SEND,
    EVENT_RTC,
    EVENT_DIGITAL_FORMAT,// 数据进制格式、范围：[2,16]进制
    EVENT_SEG_UPDATE,    // 参数为32bit的事件(必须至少有一个、避免数组sys_event_int32[]的元素个数为0)
    EVENT_MAX
}EVENT;


// 事件队列的结构(type[7bit],lock[1bit],data[32bit])
typedef struct 
{
    uint8_t  type :7 ;  // 事件类型、如数码管数据有更新：EVENT_SEG_UPDATE
    uint8_t  lock :1 ;  // 加锁标志
    uint32_t data;      // 事件参数、如数码管的数据：1265214
}T_EVENT_LIST, *pT_EVENT_LIST;

// 事件管理器的结构(任务独占的事件缓存应是这种结构：T_EVENT_LIST task_event_buffer[])
typedef struct 
{
          uint8_t number;   // 缓存中的事件数量
    pT_EVENT_LIST pBuffer;  // 事件缓存的地址
}T_TASK_EVENT_BOX;


void sys_event_lock(uint8_t type);
void sys_event_unlock(uint8_t type);
void sys_event_unlock_all(void);
uint8_t sys_event_any_lock(void);

void sys_event_init(void);
void sys_event_buffer_set(const p_void_funtion_void task, const pT_EVENT_LIST buffer);
void sys_event_buffer_post(const p_void_funtion_void task, const uint8_t event_number);
bool sys_event_push(void);
bool sys_event_post(uint8_t type, uint32_t data);

bool sys_event_get(pT_EVENT_LIST event);
bool sys_event_peek(uint8_t type, uint32_t data);
bool sys_event_data(uint8_t type, uint32_t *data);



#endif	// #ifndef __SYS_EVENT_H__
