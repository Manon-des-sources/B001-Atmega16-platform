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
// ���Ķ�����Atmega16���Ź��Ĳ�������
// 
// ----------------------------------
// ������
// ����ʹ��wdt.h�е�wdt_enable(value)��wdt_disable()
// Ҳ����ʹ�ñ��ļ������WDT_enable(uint8_t count)��WDT_disable(void)
// ��λ���Ź�һ����wdt_reset()
// 
// ==========================================================================================================
#include "Drv_WDT.h"


// ==========================================================================================================
// ʹ�ܿ��Ź�
//
// ������count = ���Ź����ʱ��@5V,��ѡ��WDTO_15MS   0
//                                       WDTO_30MS   1
//                                       WDTO_60MS   2
//                                       WDTO_120MS  3
//                                       WDTO_250MS  4
//                                       WDTO_500MS  5
//                                       WDTO_1S     6
//                                       WDTO_2S     7
// ������ʱ��͵�ѹ�й�,�������ʱ����ƫ��,��ž��ǵ�ѹ����5V
// ==========================================================================================================
void WDT_enable(const uint8_t count)
{
    wdt_reset();
    WDTCR |= (1 << WDTOE);
    WDTCR  = (1 << WDE) | (0x07 & count);
}

// ==========================================================================================================
// �رտ��Ź�
// �����������Ҫ�ر��ж�,���ⱻ���,�����ر�ʧ��
// ==========================================================================================================
void WDT_disable(void)
{
    cli();
    wdt_reset();
    WDTCR |= (1 << WDTOE) | (1 << WDE);
    WDTCR  = 0x00;
    sei();
}
