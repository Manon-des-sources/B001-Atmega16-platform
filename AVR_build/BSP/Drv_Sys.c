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
// ------------------------------
// ���Ķ�����Atmega16ϵͳ���ò���
// 
// ------------------------------
// ������
// 
// ==========================================================================================================
#include "Drv_Sys.h"


// ==========================================================================================================
// д������У׼����
// 
// ==========================================================================================================
void Drv_sys_write_OSCCAL(void)
{
    uint8_t trim_data;

    switch(SYS_OSC_FREQUENCE)
    {
//      case 1000000UL: trim_data = OSC_TRIM_1M; break;   // 1MHz��У׼���ݻ����ϵ�ʱ�Զ����أ������ֶ�����
        case 2000000UL: trim_data = OSC_TRIM_2M; break;
        case 4000000UL: trim_data = OSC_TRIM_4M; break;
        case 8000000UL: trim_data = OSC_TRIM_8M; break;

        default: return;
    }
    OSCCAL = trim_data;
}

// ==========================================================================================================
// �������ݸ�ʽ(��Χ��[2,16])
// 
// ==========================================================================================================
void Drv_sys_set_digital_format(uint8_t format)
{
    if((format >= 2) && (format <= 16))
    {
        sys_event_post(EVENT_DIGITAL_FORMAT, format);
    }
}

// ==========================================================================================================
// sram_addr_overflow_check()
//
// ������addr       ��ַ
//
// ˵����
// (1). ��Щ�Ĵ�����������֮��ͻᱻ��0����������������������print()��%s�ȵط���Ҫ���е�ַ���
// (2). ��ַԽ�硢�͸���RAMSTART(iom16.h)
//
// ==========================================================================================================
uint16_t sram_addr_overflow_check(uint16_t addr)
{
    uint16_t Fin = addr;

    if((addr < RAMSTART) || (RAMEND < addr))
    {
        Fin = RAMSTART;
    }
    return Fin;
}

// ==========================================================================================================
// ��������
// 
// ==========================================================================================================
void task_sys_enter_sleep(void)
{
}
