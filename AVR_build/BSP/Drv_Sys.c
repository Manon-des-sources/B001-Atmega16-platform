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
// 本文定义了Atmega16系统配置操作
// 
// ------------------------------
// 包含：
// 
// ==========================================================================================================
#include "Drv_Sys.h"


// ==========================================================================================================
// 写入振荡器校准数据
// 
// ==========================================================================================================
void Drv_sys_write_OSCCAL(void)
{
    uint8_t trim_data;

    switch(SYS_OSC_FREQUENCE)
    {
//      case 1000000UL: trim_data = OSC_TRIM_1M; break;   // 1MHz的校准数据会在上电时自动加载，无需手动加载
        case 2000000UL: trim_data = OSC_TRIM_2M; break;
        case 4000000UL: trim_data = OSC_TRIM_4M; break;
        case 8000000UL: trim_data = OSC_TRIM_8M; break;

        default: return;
    }
    OSCCAL = trim_data;
}

// ==========================================================================================================
// 设置数据格式(范围：[2,16])
// 
// ==========================================================================================================
void Drv_sys_set_digital_format(const uint8_t format)
{
    if((format >= 2) && (format <= 16))
    {
        sys_event_post(EVENT_DIGITAL_FORMAT, format);
    }
}

// ==========================================================================================================
// sram_addr_overflow_check()
//
// 参数：addr       地址
//
// 说明：
// (1). 有些寄存器读出数据之后就会被清0或者引发其他隐患，所以print()的%s等地方想要进行地址检查
// (2). 地址越界、就赋予RAMSTART(iom16.h)
//
// ==========================================================================================================
uint16_t sram_addr_overflow_check(const uint16_t addr)
{
    uint16_t Fin = addr;

    if((addr < RAMSTART) || (RAMEND < addr))
    {
        Fin = RAMSTART;
    }
    return Fin;
}

// ==========================================================================================================
// 进入休眠
// 
// ==========================================================================================================
void task_sys_enter_sleep(void)
{
}
