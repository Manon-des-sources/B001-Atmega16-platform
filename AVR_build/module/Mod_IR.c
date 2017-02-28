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
// ---------------------------------------
// 本文定义了Atmega16下的红外发送/接收驱动
// 
// ---------------------------------------
// 包含：
// 
// ==========================================================================================================
#include "Mod_IR.h"



// ==========================================================================================================
// 红外发送
// ==========================================================================================================
#if USE_IR_SEND



// IR指令集
// IR接收和发送的指令都放在这里
typedef enum 
{
    IR_COMMAND_01 = 0x01
}IR_COMMAND_SET;



// ==========================================================================================================
// 红外发送初始化 @ 8MHz
// 
// (1). 定时器1生成38KHz载波(1/3占空比)
//      周期为T = ((1.0/8000000)*1000000)*8*(25+1) = 26us
//              = (1/8000000)*div*(N+1)(单位：秒)
//            F = 8000000 / (div * (N + 1))(单位：Hz)
//            N = 8000000 / ( freq * div ) - 1
//              = 8000000 / freq / div - 1
//      占空比  = OCR1A / ICR1 = 9 / 27
// (2). 载波将在PD5(OC1A)引脚上输出
// (3). 初始化的耗时是可以接受的，所以使用除法吧
// 
// ==========================================================================================================
void MOD_IR_send_init(void)
{
    uint16_t div;           // 16bit宽度的分频比
    uint16_t freq = 38000;  // 16bit宽度的频率
    uint8_t  duty = 33;     // 33%占空比
    uint8_t  icr1;
    uint8_t  ocr1a;
    uint8_t  DIV  = T1_CLK_SOURCE_DIV_8;

    // --------
    // 计算参数
    switch(DIV)
    {
        case T1_CLK_SOURCE_DIV_1:    div = 1;    break;
        case T1_CLK_SOURCE_DIV_8:    div = 8;    break;
        case T1_CLK_SOURCE_DIV_64:   div = 64;   break;
        case T1_CLK_SOURCE_DIV_256:  div = 256;  break;
        case T1_CLK_SOURCE_DIV_1024: div = 1024; break;
        default: return;
    }
    icr1  = ((SYS_OSC_FREQUENCE / freq) / div) - 1; // 运算结果是8位的、但分步的中间结果可能是16位或32位的
    ocr1a =  (icr1 * duty) / 100;

    // --------
    // 设置参数
    Drv_Timer1_init(T1_WGM_FAST_PWM_ICR1, COM_MODE_CLEAR, COM_MODE_NONE, DIV);
    Drv_Timer1_set_TCNT1_OCR1A_OCR1B_ICR1(0, ocr1a, 0, icr1);
    Drv_IO_mode_bit(DDRD, DDD5, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD5);
}

// ==========================================================================================================
// 红外发送停止(停止输出载波)
// 
// (1). 关闭定时器1的时钟
// (2). OC1A引脚输出低电平
// 
// ==========================================================================================================
void MOD_IR_send_stop(void)
{
    // --------------
    // 停止定时器时钟  
    Drv_Timer1_init(T1_WGM_FAST_PWM_ICR1, COM_MODE_CLEAR, COM_MODE_NONE, T1_CLK_SOURCE_NONE);
    Drv_IO_clr_bit(PORTD, PD5);
}





#else



void MOD_IR_send_init(void)
{
}
void MOD_IR_send_stop(void)
{
}



#endif  // #if USE_IR_SEND





// ==========================================================================================================
// 红外接收
// ==========================================================================================================
#if USE_IR_RECEIVE





#else






#endif  // #if USE_IR_RECEIVE
