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
#include "Mod_Key.h"


T_EVENT_LIST event_buffer_task_key[1];

// ==========================================================================================================
// ������Ϣ�����ʼ��
// 
// ������void
// 
// ˵����
// (1). 
// 
// ==========================================================================================================
void Mod_Key_event_buffer_init(void)
{
    // 
    // ���Լ�����Ϣ����ע�ᵽ��Ϣ������
    sys_event_buffer_set(task_key_scan, event_buffer_task_key);
}

// ==========================================================================================================
// �������¼����浽��Ϣ���桢�ȴ�����
// 
// ������void
// 
// ˵����
// (1). 
// 
// ==========================================================================================================
void Mod_Key_event_buffer_post(void)
{
    sys_event_buffer_post(task_key_scan, _countof(event_buffer_task_key));
}

// ==========================================================================================================
// ����ɨ������
// 
// ������void
// 
// ˵����
// (1). 
// 
// ==========================================================================================================
void task_key_scan(void)
{
}
