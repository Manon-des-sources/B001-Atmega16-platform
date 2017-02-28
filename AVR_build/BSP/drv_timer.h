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
// 本文定义了Atmega16定时器0/1/2 驱动模块
// 
// ==========================================================================================================
#ifndef __DRV_TIMER_H_
#define __DRV_TIMER_H_



#include <avr/interrupt.h>
#include <avr/io.h>
#include "sys_config.h"

// --------------
// 定时器中断模式
typedef enum 
{
    INT_MODE_TOV = 0,
    INT_MODE_OCF = 1,
    INT_MODE_ICF = 2,
    INT_MODE_OCF1A = 3,
    INT_MODE_OCF1B = 4
} TIMER_INT_MODE;

// 定时器比较匹配引脚输出模式
typedef enum 
{
    COM_MODE_NONE   = 0,
    COM_MODE_TOGGLE = 1,
    COM_MODE_CLEAR  = 2,
    COM_MODE_SET    = 3
} TIMER_COM_MODE;


// ----------------------------------------------------------------------------------------------
// 定时器0(这类定义占用flash空间，所以如果空间不足，就屏蔽这里，并在调用处用具体数值替换这些定义)
typedef enum 
{
    T0_WGM_NOMAL     = 0,
    T0_WGM_PHASE_PWM = 1,
    T0_WGM_CTC       = 2,
    T0_WGM_FAST_PWM  = 3,

    T0_CLK_SOURCE_NONE     = 0,
    T0_CLK_SOURCE_DIV_1    = 1,
    T0_CLK_SOURCE_DIV_8    = 2,
    T0_CLK_SOURCE_DIV_64   = 3,
    T0_CLK_SOURCE_DIV_256  = 4,
    T0_CLK_SOURCE_DIV_1024 = 5,
    T0_CLK_SOURCE_T0_FALL  = 6,
    T0_CLK_SOURCE_T0_RAISE = 7
} TIMER0_MODE;

void Drv_Timer0_init(const uint8_t wave_mode, const uint8_t com_mode, const uint8_t clk_source);
void Drv_Timer0_INT_Enable(const uint8_t int_mode, const uint8_t enable);
void Drv_Timer0_set_TCNT0_OCR0(const uint8_t tcnt0, const uint8_t ocr0);


// ----------------------------------------------------------------------------------------------
// 定时器1(这类定义占用flash空间，所以如果空间不足，就屏蔽这里，并在调用处用具体数值替换这些定义)
typedef enum 
{
    T1_WGM_NOMAL           = 0,
    T1_WGM_8_PHASE_PWM     = 1,
    T1_WGM_9_PHASE_PWM     = 2,
    T1_WGM_10_PHASE_PWM    = 3,
    T1_WGM_CTC             = 4,
    T1_WGM_8_FAST_PWM      = 5,
    T1_WGM_9_FAST_PWM      = 6,
    T1_WGM_10_FAST_PWM     = 7,
    T1_WGM_PHASE_FRQ_PWM_ICR1  = 8,
    T1_WGM_PHASE_FRQ_PWM_OCR1A = 9,
    T1_WGM_PHASE_PWM_ICR1    = 10,
    T1_WGM_PHASE_PWM_OCR1A   = 11,
    T1_WGM_CTC_ICR1          = 12,
    T1_WGM_SERVED            = 13,
    T1_WGM_FAST_PWM_ICR1     = 14,
    T1_WGM_FAST_PWM_OCR1A    = 15,

    T1_CLK_SOURCE_NONE     = 0,
    T1_CLK_SOURCE_DIV_1    = 1,
    T1_CLK_SOURCE_DIV_8    = 2,
    T1_CLK_SOURCE_DIV_64   = 3,
    T1_CLK_SOURCE_DIV_256  = 4,
    T1_CLK_SOURCE_DIV_1024 = 5,
    T1_CLK_SOURCE_T1_FALL  = 6,
    T1_CLK_SOURCE_T1_RAISE = 7
} TIMER1_MODE;

typedef enum 
{
    T1_ICP_FALL_EDGE       = 0,
    T1_ICP_RAISE_EDGE      = 1
} TIMER1_ICP;

void Drv_Timer1_init(const uint8_t com_mode, const uint8_t OCM1A_mode, const uint8_t OCM1B_mode, const uint8_t clk_source);
void Drv_Timer1_init_for_ICP(const uint8_t flit_enable, const uint8_t edge);
void Drv_Timer1_INT_Enable(const uint8_t int_mode, const uint8_t enable);
void Drv_Timer1_set_TCNT1_OCR1A_OCR1B_ICR1(const uint16_t tcnt1, const uint16_t ocr1a, const uint16_t ocr1b, const uint16_t icr1);


// ----------------------------------------------------------------------------------------------
// 定时器2(这类定义占用flash空间，所以如果空间不足，就屏蔽这里，并在调用处用具体数值替换这些定义)
typedef enum 
{
    T2_WGM_NOMAL     = 0,
    T2_WGM_PHASE_PWM = 1,
    T2_WGM_CTC       = 2,
    T2_WGM_FAST_PWM  = 3,

    T2_CLK_SOURCE_NONE     = 0,
    T2_CLK_SOURCE_DIV_1    = 1,
    T2_CLK_SOURCE_DIV_8    = 2,
    T2_CLK_SOURCE_DIV_32   = 3,
    T2_CLK_SOURCE_DIV_64   = 4,
    T2_CLK_SOURCE_DIV_128  = 5,
    T2_CLK_SOURCE_DIV_256  = 6,
    T2_CLK_SOURCE_DIV_1024 = 7
} TIMER2_MODE;

void Drv_Timer2_init(const uint8_t wave_mode, const uint8_t com_mode, const uint8_t clk_source);
void Drv_Timer2_INT_Enable(const uint8_t int_mode, const uint8_t enable);
void Drv_Timer2_FOC2_enable(const uint8_t enable);
void Drv_Timer2_set_TCNT2_OCR2(const uint8_t tcnt2, const uint8_t ocr2);




#endif  // #ifndef __DRV_TIMER_H_
