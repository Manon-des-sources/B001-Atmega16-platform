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
// ���Ķ������¼�����ģ��
// 
// ˵����
// (1).���Ľ��¼�(event)�����Ĳ�����Ϊ��Ϣ(message)���������¼�������¼��Ĳ���
// 
// ==========================================================================================================
#ifndef __SYS_EVENT_H__
#define __SYS_EVENT_H__



#include <avr/interrupt.h>
#include "sys_config.h"
#include "sys_timer.h"


// �¼�(�¼�������tpye,Ϊ8bit)(�¼��Ĳ���data,Ϊ32bit)
//     (����Ĭ�ϵ�EVENT_MAX�����ٻ���Ҫһ���¼�������sys_event_list[EVENT_MAX]���sys_event_list[0])
typedef enum 
{
    EVENT_SYS,
    EVENT_KEY,
    EVENT_IR_RECIEVE,
    EVENT_IR_SEND,
    EVENT_RTC,
    EVENT_DIGITAL_FORMAT,// ���ݽ��Ƹ�ʽ����Χ��[2,16]����
    EVENT_SEG_UPDATE,    // ����Ϊ32bit���¼�(����������һ������������sys_event_int32[]��Ԫ�ظ���Ϊ0)
    EVENT_MAX
}EVENT;


// �¼����еĽṹ(type[7bit],lock[1bit],data[32bit])
typedef struct 
{
    uint8_t  type :7 ;  // �¼����͡�������������и��£�EVENT_SEG_UPDATE
    uint8_t  lock :1 ;  // ������־
    uint32_t data;      // �¼�������������ܵ����ݣ�1265214
}T_EVENT_LIST, *pT_EVENT_LIST;

// �¼��������Ľṹ(�����ռ���¼�����Ӧ�����ֽṹ��T_EVENT_LIST task_event_buffer[])
typedef struct 
{
          uint8_t number;   // �����е��¼�����
    pT_EVENT_LIST pBuffer;  // �¼�����ĵ�ַ
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
