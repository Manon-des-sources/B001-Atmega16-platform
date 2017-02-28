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
// 本文定义了机械按键驱动
// 
// --------------------------------------
// 包含：
// 
// ==========================================================================================================
#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__



#include <stdint.h>
#include "sys_event.h"
#include "sys_config.h"


// EVENT_KEY的参数(按键状态)
typedef enum 
{
    MSG_KEY_DOWN,
    MSG_KEY_UP,
    MSG_KEY_SHORT,
    MSG_KEY_LONG,
    MSG_KEY_HOLD,
    MSG_KEY_DOUBLE,
    MSG_KEY_TWO     // 两个按键同时按下、得到一个新的键值(同时按下的两个按键的键值也许需要清0)
}MSG_EVENT_KEY;

// MSG_EVENT_KEY的结构(32bit)(初值=0)
typedef struct 
{                             // 数据放在一个或半个字节里面、调试的时候以十六进制格式查看会更方便
    uint32_t key_index : 24;  // bit[23:00]具体键值(支持2^24个键值)

    uint8_t  reserved  :  1;  // bit[  :24]将来作为第8种按键状态

    uint8_t  key_two    : 1;  // bit[  :25]两个按键同时按下、得到一个新的键值(同时按下的两个按键的键值也许需要清0)
    uint8_t  key_double : 1;  // bit[  :26]双击
    uint8_t  key_hold   : 1;  // bit[  :27]保持
    uint8_t  key_long   : 1;  // bit[  :28]长按
    uint8_t  key_short  : 1;  // bit[  :29]短按
    uint8_t  key_down   : 1;  // bit[  :30]按下
    uint8_t  key_up     : 1;  // bit[  :31]松手
}T_MSG_EVENT_KEY, *pT_MSG_EVENT_KEY;

// MSG_EVENT_KEY的联合体结构、更适合在函数中进行操作
typedef union
{
    T_MSG_EVENT_SYS msg;
    uint32_t data;
}U_MSG_EVENT_KEY;


void task_key_scan(void);



#endif  // #ifndef __DRV_KEY_H__
