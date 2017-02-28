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
// ---------------------------------------------------
// ���ּ򵥵���������ʵʱ���������¼����е������Ч
// ��ʱ����ʹ����Դ����������Դ�������
// 
//    ��Ҫ�����Ĳ��֣�
// 1�������ͨ�ŵ��¼�����
// 2���������ͬʹ�õ�RAM��
// 3���������ͬʹ�õĶ˿�/Ӳ���ӿ�
// 
//    ����ȫ����������-��Դ����������Դ�������(32bit�Ķ���.list��֧��32������ʹ�ø���Դ)��
// 0������A������ԴSǰ��������if(ALLOWED | (.list | (1 << PRIORITY_S_A)))
//    ALLOWEDһ��Ϊ0����ʾ����ʹ��
// 1������A������ԴSǰ��������.list | (1 << PRIORITY_S_A)����������A��ʹ�������־
//    ����PRIORITY_S_A���û����䣬��ʾʹ�����ȼ�
// 2����Դ���д�.list��bit0��ʼ��顢bit0��Ӧ���ȼ���ߵ�����
//    Ȼ��֪ͨ
// 
// ---------------------------
// ������
// 
// ==========================================================================================================
#include "sys_event.h"



// �¼�����
static T_EVENT_LIST sys_event_list[EVENT_MAX];


// �¼�������
// (������ÿ�������ռ���¼�������׵�ַ�������±��������е��±걣��һ��)
T_TASK_EVENT_BOX task_event_box[SYS_TASK_MAX];


// ==========================================================================================================
//      �����¼������е�Ԫ��
// 
// ==========================================================================================================
void sys_event_lock(uint8_t type)
{
    if(type < EVENT_MAX)
    {
        sys_event_list[type].lock = LOCKED;
    }
}

// ==========================================================================================================
//      �����¼������е�Ԫ��
// 
// ==========================================================================================================
void sys_event_unlock(uint8_t type)
{
    if(type < EVENT_MAX)
    {
        sys_event_list[type].lock = UNLOCKED;
    }
}

// ==========================================================================================================
//      ����Ƿ����¼�������
// 
// ����ֵ��index  �������¼����¼���
// 
// ˵����
// (1). ��ͷ��ʼ���ң�ֱ���ҵ���һ�����������¼�Ϊֹ
// (2). ��������ÿ�ε���������ǰ�������������¼���ȷ��û���κ����Ĵ���
//      ��Ϊ��ÿ�������˳��󡢱���������е���
// 
// ==========================================================================================================
uint8_t sys_event_any_lock(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        if(LOCKED == sys_event_list[index].lock)
        {
            break;
        }
    }
    return index;
}

// ==========================================================================================================
//      ���������¼�
// 
// ==========================================================================================================
void sys_event_unlock_all(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        sys_event_list[index].lock = UNLOCKED;
    }
}

// ==========================================================================================================
//      �¼����г�ʼ�����¼������������ʼ��
// 
// ==========================================================================================================
void sys_event_init(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        sys_event_list[index].type = EVENT_MAX;
        sys_event_list[index].lock = UNLOCKED;
        sys_event_list[index].data = 0;
    }
    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        task_event_box[index].number  = 0;
        task_event_box[index].pBuffer = NULL;
    }
}

// ==========================================================================================================
//      ֱ�ӽ�ÿ������������¼�д�뵽�¼�����
// 
// ����ֵ��Fin  TURE  = д��ɹ�
//              FALSE = д��ʧ��(�¼������������¼�����Ч���¼�)
// 
// ==========================================================================================================
bool sys_event_post(uint8_t type, uint32_t data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            sys_event_list[type].type = type;
            sys_event_list[type].data = data;
            sys_event_list[type].lock = UNLOCKED;
            Fin = TRUE;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      ��ѯ�¼�����
// 
// ������  event    ���ڶ����¼���type��data
// 
// ����ֵ��Fin      TURE  = ����1���¼�������Ϣ����
//                  FALSE = û���κ��¼�����
// 
// ˵����
// (1). �������Ǵ�ͷ��ʼ���ң�ֱ���ҵ���һ����Ч���¼�Ϊֹ
//      ������typedef enum { }EVENT��Խ��ǰ���¼���Խ�ᱻ���Ȳ�ѯ��
// 
// ==========================================================================================================
bool sys_event_get(pT_EVENT_LIST event)
{
    bool Fin = FALSE;
    uint8_t index;

    event->type = EVENT_MAX;
    event->data = 0;

    for(index = 0; index < EVENT_MAX; index++)
    {
        if(UNLOCKED == sys_event_list[index].lock)
        {
            sys_event_list[index].lock = LOCKED;
            if(EVENT_MAX != sys_event_list[index].type)
            {
                Fin = TRUE;
                event->type = sys_event_list[index].type;
                event->data = sys_event_list[index].data;
                sys_event_list[index].type = EVENT_MAX;
                sys_event_list[index].data = 0;
            }
            sys_event_list[index].lock = UNLOCKED;
        }
        if(TRUE == Fin)
        {
            break;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      �鿴ĳ���¼��Ƿ��Ѿ����ڡ�Ҫ�����Ҳ��Ӧ
// 
// ������  type     �¼�
//         data     �¼��Ĳ���
// 
// ����ֵ��FALSE    ���¼�û�з�����������
//         TRUE     ���¼��Ѿ����������غ󽫸��¼����¼����������
// 
// ==========================================================================================================
bool sys_event_peek(uint8_t type, uint32_t data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            if(sys_event_list[type].type == type)
            {
                if(sys_event_list[type].data == data)
                {
                    Fin = TRUE;
                    sys_event_list[type].type = EVENT_MAX;
                    sys_event_list[type].data = 0;
                }
            }
            sys_event_list[type].lock = UNLOCKED;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      �鿴ĳ���¼��Ƿ��Ѿ����ڡ���ȡ���¼��Ĳ���
// 
// ������  type     �¼���
//        *data     ȡ�����¼��Ĳ���
// 
// ����ֵ��FALSE    ���¼�û�з�����������
//         TRUE     ���¼��Ѿ����������غ󽫸��¼����¼����������
// 
// ==========================================================================================================
bool sys_event_data(uint8_t type, uint32_t *data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            if(sys_event_list[type].type == type)
            {
                Fin = TRUE;
                *data = sys_event_list[type].data;
                sys_event_list[type].type = EVENT_MAX;
                sys_event_list[type].data = 0;
            }
            sys_event_list[type].lock = UNLOCKED;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      �������ռ���¼�����ע�ᵽ�¼�������
// 
// ������task       ������
//       buffer     �������ռ���¼�������׵�ַ(ÿ�������ռ1��buffer�����������������޷��ʳ�ͻ)
// 
// ˵����
// (1). һ���������ʼ���������桢ȥע��������ռ���¼�����
//      Ҳ����˵���������һ��������ĳ�ʼ���������汻����
// 
// ==========================================================================================================
void sys_event_buffer_set(const p_void_funtion_void task, const pT_EVENT_LIST buffer)
{
    uint8_t task_index;

    task_index = sys_task_index(task);
    if(task_index < SYS_TASK_MAX)
    {
        task_event_box[task_index].pBuffer = buffer;
    }
}

// ==========================================================================================================
//      ����������Ҫ���͵��¼�������
// 
// ������task       ������
//       number     �������ռ���¼�����������Ҫ���͵��¼�������
// 
// ˵����
// (1). ÿ�����������Ҫ�����¼����棬���������˳�ʱ�������ַ�����¼�����ģ�鼴��
//      ����¼�����ģ�鷢�ָ�������Ҫ���͵��¼�����>0�����ͻὫ��Щ�¼��͵��¼����У���������Ҫ�Լ�ȥ�����¼�
// (2). ��delete������Ļ��滹�ڣ�������Ȼ�����ڱ�delete����Է���
// (3). ��������A���¼�����(.buffer)��4��Ԫ�أ�������ֻ����2���¼���������.number = 2
//      ��ô����Ҫ����2���¼�����.buffer[0]��.buffer[1]�������ܱ�֤�¼����Ա�����
//      ��Ϊ������sys_event_push()����ֻ��ѯ.buffer��ǰ2��Ԫ��(ǰ.number��Ԫ��)������Ĳ��ٲ�ѯ
//      ����һ�㽫.number��Ϊ_countof(event_buffer)��������ֵ���Ǻܴ�Ļ�
//      ��Ȼ������Ѿ�ȷ�Ͻ���2���¼�������buffer[0]��buffer[1]����ôֻ��.number = 2���ɱ���������������
// 
// ==========================================================================================================
void sys_event_buffer_post(const p_void_funtion_void task, const uint8_t event_number)
{
    uint8_t task_index;

    task_index = sys_task_index(task);
    if(task_index < SYS_TASK_MAX)
    {
        task_event_box[task_index].number = event_number;
    }
}

// ==========================================================================================================
//      ������������¼������е��¼����浽�¼�����
// 
// ����ֵ��Fin  FALSE = ������1���¼���δд���¼�����
//              TRUE  = �����¼����ѳɹ���д���¼�����
// 
// ˵����
// (1). ÿ�������ռ1��buffer�����������������޷��ʳ�ͻ
// (2). ������>=2���¼�����ͬʱ�ɹ���д���¼����С����ܱ�֤�û�����ִ������
//      ��ô�ͱ������û����������ж���>=2���¼�ͬʱ��Ч
// 
// ==========================================================================================================
bool sys_event_push(void)
{
    bool Fin = TRUE;

    uint8_t post;       // ĳ���¼������Ƿ�ɹ�
    uint8_t task_index; // �����
    uint8_t msg_number; // �¼�����
    uint8_t msg_index;  // �¼����
    T_EVENT_LIST event; // �¼��������

    for(task_index = 0; task_index < SYS_TASK_MAX; task_index++)
    {
        msg_number = task_event_box[task_index].number;
        if(msg_number > 0)
        {
            // --------
            // �����¼�
            for(msg_index = 0; msg_index < msg_number; msg_index++)
            {
                event.type = (task_event_box[task_index].pBuffer)[msg_index].type;
                if(EVENT_MAX != event.type)
                {
                    event.data = (task_event_box[task_index].pBuffer)[msg_index].data;
                    post = sys_event_post(event.type, event.data);
                    if(TRUE == post)  // �¼����ͳɹ�����������¼�
                    {
                        (task_event_box[task_index].pBuffer)[msg_index].type = EVENT_MAX;
                    }
                }
            }
            // ----------------------------------------------------------------------------------
            // ����Ƿ�ȫ���¼���������ϣ�û����ͽ�δ���͵Ĳ����Ƶ�ǰ�棬�ȴ��´ν����ٴη���
            post = TRUE;
            for(msg_index = 0; msg_index < msg_number; msg_index++)
            {
                if(EVENT_MAX != (task_event_box[task_index].pBuffer)[msg_index].type)
                {
                    post = FALSE;
                    Fin  = FALSE;  // ������1���¼���δд���¼�����
                    break;
                }
            }
            if(TRUE == post)
            {
                task_event_box[task_index].number = 0;
            }
        }
    }
    return Fin;
}
