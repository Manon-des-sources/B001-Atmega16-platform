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
// ���Ķ����˻�е��������
// 
// --------------------------------------
// ������
// 
// ==========================================================================================================
#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__



#include <stdint.h>
#include "sys_event.h"
#include "sys_config.h"


// EVENT_KEY�Ĳ���(����״̬)
typedef enum 
{
    MSG_KEY_DOWN,
    MSG_KEY_UP,
    MSG_KEY_SHORT,
    MSG_KEY_LONG,
    MSG_KEY_HOLD,
    MSG_KEY_DOUBLE,
    MSG_KEY_TWO     // ��������ͬʱ���¡��õ�һ���µļ�ֵ(ͬʱ���µ����������ļ�ֵҲ����Ҫ��0)
}MSG_EVENT_KEY;

// MSG_EVENT_KEY�Ľṹ(32bit)(��ֵ=0)
typedef struct 
{                             // ���ݷ���һ�������ֽ����桢���Ե�ʱ����ʮ�����Ƹ�ʽ�鿴�������
    uint32_t key_index : 24;  // bit[23:00]�����ֵ(֧��2^24����ֵ)

    uint8_t  reserved  :  1;  // bit[  :24]������Ϊ��8�ְ���״̬

    uint8_t  key_two    : 1;  // bit[  :25]��������ͬʱ���¡��õ�һ���µļ�ֵ(ͬʱ���µ����������ļ�ֵҲ����Ҫ��0)
    uint8_t  key_double : 1;  // bit[  :26]˫��
    uint8_t  key_hold   : 1;  // bit[  :27]����
    uint8_t  key_long   : 1;  // bit[  :28]����
    uint8_t  key_short  : 1;  // bit[  :29]�̰�
    uint8_t  key_down   : 1;  // bit[  :30]����
    uint8_t  key_up     : 1;  // bit[  :31]����
}T_MSG_EVENT_KEY, *pT_MSG_EVENT_KEY;

// MSG_EVENT_KEY��������ṹ�����ʺ��ں����н��в���
typedef union
{
    T_MSG_EVENT_SYS msg;
    uint32_t data;
}U_MSG_EVENT_KEY;


void task_key_scan(void);



#endif  // #ifndef __DRV_KEY_H__
