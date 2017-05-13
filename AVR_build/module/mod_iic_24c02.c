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
// -----------------------------------------------
// ���Ķ�����Atmega16�µ�24C02�����ӿںͲ���״̬��
// 
// ==========================================================================================================
#include "Mod_IIC_24C02.h"


// ==========================================================================================================
// 24C02����״̬����
// 
// ������һ��״̬�����б�
// д����һ��״̬�����б�
// 
// ִ��ʱֻ��Ҫ��״̬�����б����һ�鼴��
// ��Ҫ�µĲ���ʱ���ڽ���һ����Ӧ��״̬�����б���
// 
// ==========================================================================================================

typedef struct 
{
    uint8_t index;               // ״̬��������ʱʹ��
    uint8_t number;              // ״̬�����б�ĳ���
    p_void_funtion_void p_list;  // ִ��״̬�����б��е�һ��Ԫ��
} T_MOD_IIC_24C02;


// ��������״̬�����б�
static p_void_funtion_void read_list[] = 
{
    Drv_IIC_start,
    Drv_IIC_stop,
};
