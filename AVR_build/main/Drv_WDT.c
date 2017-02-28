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
// ----------------------------------
// 本文定义了Atmega16看门狗的操作函数
// 
// ----------------------------------
// 包含：
// 可以使用wdt.h中的wdt_enable(value)和wdt_disable()
// 也可以使用本文件定义的WDT_enable(uint8_t count)和WDT_disable(void)
// 复位看门狗一律用wdt_reset()
// 
// ==========================================================================================================
#include "Drv_WDT.h"


// ==========================================================================================================
// 使能看门狗
//
// 参数：count = 看门狗溢出时间@5V,可选：WDTO_15MS   0
//                                       WDTO_30MS   1
//                                       WDTO_60MS   2
//                                       WDTO_120MS  3
//                                       WDTO_250MS  4
//                                       WDTO_500MS  5
//                                       WDTO_1S     6
//                                       WDTO_2S     7
// 这个溢出时间和电压有关,所以如果时间有偏差,大概就是电压不是5V
// ==========================================================================================================
void WDT_enable(const uint8_t count)
{
    wdt_reset();
    WDTCR |= (1 << WDTOE);
    WDTCR  = (1 << WDE) | (0x07 & count);
}

// ==========================================================================================================
// 关闭看门狗
// 这个过程中需要关闭中断,避免被打断,否则会关闭失败
// ==========================================================================================================
void WDT_disable(void)
{
    cli();
    wdt_reset();
    WDTCR |= (1 << WDTOE) | (1 << WDE);
    WDTCR  = 0x00;
    sei();
}
