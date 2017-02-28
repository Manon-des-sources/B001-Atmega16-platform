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
// ���Ķ�����ϵͳ��ʾ�;���
// 
// --------------------------------------
// ������
// 
// ==========================================================================================================
#include "sys_warning.h"



// ==========================================================================================================
// ����ϵͳ��������Ϣ�;���
// 
// ������type    ��Ҫ���µ��¼�
//       msg     ��Ҫ���µ���Ϣ
//       index   ����š����¼���
// 
// ˵����
// (1). ʹ�ö�-�޸�-д�ķ�ʽ�����¼�
// (2). ʹ����sys_event_post()��ֱ��д�룬���ڶ�д֮ǰǿ�ƽ������¼�
// 
// ==========================================================================================================
void sys_update_event(const uint8_t type, const uint32_t msg, const uint8_t index)
{
    bool peek;
    U_MSG_EVENT_SYS sys;

    sys_event_unlock(type);
    peek = sys_event_data(type, &sys.data);

    if(TRUE == peek)
    {
        switch(msg)
        {
            // ���¼������� ----------
            case MSG_SYS_EVENT_LOCKED : sys.msg.event_locked = 1;
                                        sys.msg.event_index  = index;
                                        break;
            // �������ӳ� ----------
            case MSG_SYS_TASK_DELAYED : sys.msg.task_delayed = 1;
                                        sys.msg.task_index   = index;
                                        break;
            // ϵͳ״̬ --------
            case MSG_SYS_SLEEP  : sys.msg.sys_sleep  = 1;
                                  break;
            case MSG_SYS_WAKEUP : sys.msg.sys_wakeup = 1;
                                  break;
            case MSG_SYS_IDLE   : sys.msg.sys_ldle   = 1;
                                  break;
            case MSG_SYS_START  : sys.msg.sys_start  = 1;
                                  break;
            default : break;
        }
        sys_event_post(type, sys.data);
    }
}
