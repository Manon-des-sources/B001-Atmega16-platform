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
// ---------------------------------
// 本文定义了Atmega16下的IIC初始化等
// 
// ==========================================================================================================
#include "Drv_IIC.h"



// ==========================================================================================================
// IIC初始化
// 
// 参数：clk_source     SCL频率
//       clk_quadruple  SCL频率翻4的幂次方倍(IIC_TWPS_00, IIC_TWPS_01, IIC_TWPS_02, IIC_TWPS_03)
// 
// 说明：
// (1). SCL  =  SYS_OSC_FREQUENCE / (16 + (2 * TWBR * (4^TWPS)))
//      TWBR = (SYS_OSC_FREQUENCE / SCL - 16) / 2 / (4^TWPS)
// 
// ==========================================================================================================
void Drv_IIC_init(const uint8_t clk_source, const uint8_t twps_quadruple)
{
    uint8_t twps_power = 0;

    switch(twps_quadruple)
    {
        case IIC_TWPS_00: twps_power = 1;  break;
        case IIC_TWPS_01: twps_power = 4;  break;
        case IIC_TWPS_02: twps_power = 16; break;
        case IIC_TWPS_03: twps_power = 64; break;
        default         : twps_power = 1;  break;
    }
    if(SYS_OSC_FREQUENCE / clk_source >= 16)
    {
        TWBR = (SYS_OSC_FREQUENCE / clk_source - 16) / 2 / twps_power;
        TWSR = (TWSR & 0xFC) | (twps_quadruple & 0x03);
    }
/*
    else
    {
        print("warning: IIC_SCL is can't bigger than CPU_clock/16(%l)\n", SYS_OSC_FREQUENCE/16);
    }
*/
}

// ==========================================================================================================
// 设置从机地址
// 
// 参数：addr       从机地址
//       gc_enable  是否响应广播地址(general call address)(ENABLE, DISABLE)
// 
// ==========================================================================================================
void Drv_IIC_set_slave_addr(const uint8_t addr, const uint8_t gc_enable)
{
    TWAR = (addr & 0xFE) | (gc_enable & 0x01);
}

// ==========================================================================================================
// IIC操作完成中断使能
// 
// ==========================================================================================================
void Drv_IIC_INT_Enable(const uint8_t enable)
{
    if(DISABLE == enable)
    {
        TWCR &= ~(1 << TWIE);
    }
    else
    {
        TWCR |=  (1 << TWIE);
    }
}

// ==========================================================================================================
// 输出IIC操作
// 
// 参数：action     要输出的动作
// 
// ==========================================================================================================
void Drv_IIC_action(const uint8_t action)
{
    switch(action)
    {
        case START    : TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN ); break;
        case STOP     : TWCR = (1 << TWINT) | (1 << TWEN ) | (1 << TWSTO); break;
        case TRANSFER : TWCR = (1 << TWINT) | (1 << TWEN );                break;

        default : break;
    }
}

// ==========================================================================================================
// 输出起始信号
// 
// ==========================================================================================================
void Drv_IIC_start(void)
{
    Drv_IIC_action(START);
}

// ==========================================================================================================
// 输出结束信号
// 
// ==========================================================================================================
void Drv_IIC_stop(void)
{
    Drv_IIC_action(STOP);
}

// ==========================================================================================================
// 启动传输
// 
// ==========================================================================================================
void Drv_IIC_transfer(void)
{
    Drv_IIC_action(TRANSFER);
}

// ==========================================================================================================
// 需要发送的SLA + W/R
// 
// 参数：addr       从机地址
//       direction  读/写
// 
// ==========================================================================================================
void Drv_IIC_set_addr(const uint8_t addr, const uint8_t direction)
{
    TWDR = (addr & 0xFE) | (direction & 0x01);
}

// ==========================================================================================================
// 需要发送的数据
// 
// 参数：data       数据
// 
// ==========================================================================================================
void Drv_IIC_set_data(const uint8_t data)
{
    TWDR = data;
}

// ==========================================================================================================
// 读IIC状态寄存器
// 
// 返回值：TWSR[7:3]
// 
// ==========================================================================================================
uint8_t Drv_IIC_read_status(void)
{
    return TWSR & 0xF8;
}

// ==========================================================================================================
// 等待TWINT置位(手动写1来置0、执行中断时不会自动置0)
// 
// 说明：
// (1). TWINT置位后、表示当前动作已经完成
// (2). 使能了中断的话、TWINT = 1将进入中断
// 
// ==========================================================================================================
void Drv_IIC_wait_finish(void)
{
    while(!(TWCR & (1 << TWINT))) {}
}

// ==========================================================================================================
// 等待TWINT置位后、读IIC状态寄存器
// 
// 返回值：TWSR[7:3]
// 
// ==========================================================================================================
uint8_t Drv_IIC_wait_finish_and_read_status(void)
{
    while(!(TWCR & (1 << TWINT))) {}
    return TWSR & 0xF8;
}

// ==========================================================================================================
// IIC中断
// 
// 说明：
// (1). TWINT需要手动写1来置0、执行中断时不会自动置0
// 
// ==========================================================================================================
ISR(TWI_vect)
{
    TWCR = 1 << TWINT;
}
