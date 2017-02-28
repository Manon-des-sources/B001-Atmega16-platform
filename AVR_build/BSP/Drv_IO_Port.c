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
// --------------------------------
// 本文定义了Atmega16下的IO常用操作
// 
// --------------------------------
// 包含：
// 
// (1). IO的操作(h头文件)
// (2). 外部中断初始化和使能
// 
// ==========================================================================================================
#include "Drv_IO_Port.h"



// ==========================================================================================================
// 外部中断初始化
// 
// 参数：INTn_IO    外部中断号(INTn_IO_INT0、INTn_IO_INT1、INTn_IO_INT2)
//       edge_mode  中断边沿模式(INTn_LEVEL_LOW、INTn_EDGE_BOTH、INTn_EDGE_FALL、INTn_EDGE_RAISE)
// 
// 说明：
// (1). 使能INT[2:0]后、通常设置引脚为输入模式
//      如果对应引脚是输出模式，那么当引脚电平变化时、会触发外部中断，这称为软件触发
// 
// 未解决问题：
// (1). INTn_LEVEL_LOW模式下、DDRxn=PORTxn=0将导致芯片无法运行,必须是DDRxn=0、PORTxn=1。
//      在PORTxn=1时，INTn引脚出现低电平、将导致芯片停止运行/重启。
//     Drv_IO_mode_bit(DDRD, DDD2, IO_INPUT);
//     Drv_IO_set_bit(PORTD, PD2);
// 
// ==========================================================================================================
void Drv_IO_INTn_init(const uint8_t INTn_IO, const uint8_t edge_mode)
{
    switch(INTn_IO)
    {
        case INTn_IO_INT0:  // 外部中断0
                            MCUCR &= ~(0x03 << 0);
                            MCUCR |= ((edge_mode & 0x03) << 0);
                            break;
        case INTn_IO_INT1:  // 外部中断1
                            MCUCR &= ~(0x03 << 2);
                            MCUCR |= ((edge_mode & 0x03) << 2);
                            break;
        case INTn_IO_INT2:  // 外部中断2
                            GICR &= ~(1 << INT2);
                            if(INTn_EDGE_FALL  == edge_mode) { MCUCSR &= ~(1 << ISC2); }
                            if(INTn_EDGE_RAISE == edge_mode) { MCUCSR |=  (1 << ISC2); }
                            break;
        default : break;
    }
}

// ==========================================================================================================
// 外部中断使能
// 
// 参数：INTn_IO    外部中断号(INTn_IO_INT0、INTn_IO_INT1、INTn_IO_INT2)
//       enable     使能或不使能(ENABLE、DISABLE)
// 
// ==========================================================================================================
void Drv_IO_INTn_Enable(const uint8_t INTn_IO, const uint8_t enable)
{
    switch(INTn_IO)
    {
        case INTn_IO_INT0:  // 外部中断0
                            if(DISABLE == enable)
                            {
                                GICR &= ~(1 << INT0);
                            }
                            else
                            {
                                GICR |=  (1 << INT0);
                            }
                            GIFR |= (1 << INTF0);
                            break;
        case INTn_IO_INT1:  // 外部中断1
                            if(DISABLE == enable)
                            {
                                GICR &= ~(1 << INT1);
                            }
                            else
                            {
                                GICR |=  (1 << INT1);
                            }
                            GIFR |= (1 << INTF1);
                            break;
        case INTn_IO_INT2:  // 外部中断2
                            if(DISABLE == enable)
                            {
                                GICR &= ~(1 << INT2);
                            }
                            else
                            {
                                GICR |=  (1 << INT2);
                            }
                            GIFR |= (1 << INTF2);
                            break;
        default : break;
    }
}

// ==========================================================================================================
// INT0 中断服务程序
// 
// ==========================================================================================================
ISR(INT0_vect)
{
    sei();
    Drv_IO_toggle_bit(PORTD, PD7);
    delay_ms(500);
    cli();
}

// ==========================================================================================================
// INT1 中断服务程序
// 
// ==========================================================================================================
ISR(INT1_vect)
{
}

// ==========================================================================================================
// INT2 中断服务程序
// 
// ==========================================================================================================
ISR(INT2_vect)
{
}
