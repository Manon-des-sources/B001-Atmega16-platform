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
// ���Ķ�����Atmega16�µ�IIC��ʼ����
// 
// ==========================================================================================================
#include "Drv_IIC.h"



// ==========================================================================================================
// IIC��ʼ��
// 
// ������clk_source     SCLƵ��
//       clk_quadruple  SCLƵ�ʷ�4���ݴη���(IIC_TWPS_00, IIC_TWPS_01, IIC_TWPS_02, IIC_TWPS_03)
// 
// ˵����
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
// ���ôӻ���ַ
// 
// ������addr       �ӻ���ַ
//       gc_enable  �Ƿ���Ӧ�㲥��ַ(general call address)(ENABLE, DISABLE)
// 
// ==========================================================================================================
void Drv_IIC_set_slave_addr(const uint8_t addr, const uint8_t gc_enable)
{
    TWAR = (addr & 0xFE) | (gc_enable & 0x01);
}

// ==========================================================================================================
// IIC��������ж�ʹ��
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
// ���IIC����
// 
// ������action     Ҫ����Ķ���
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
// �����ʼ�ź�
// 
// ==========================================================================================================
void Drv_IIC_start(void)
{
    Drv_IIC_action(START);
}

// ==========================================================================================================
// ��������ź�
// 
// ==========================================================================================================
void Drv_IIC_stop(void)
{
    Drv_IIC_action(STOP);
}

// ==========================================================================================================
// ��������
// 
// ==========================================================================================================
void Drv_IIC_transfer(void)
{
    Drv_IIC_action(TRANSFER);
}

// ==========================================================================================================
// ��Ҫ���͵�SLA + W/R
// 
// ������addr       �ӻ���ַ
//       direction  ��/д
// 
// ==========================================================================================================
void Drv_IIC_set_addr(const uint8_t addr, const uint8_t direction)
{
    TWDR = (addr & 0xFE) | (direction & 0x01);
}

// ==========================================================================================================
// ��Ҫ���͵�����
// 
// ������data       ����
// 
// ==========================================================================================================
void Drv_IIC_set_data(const uint8_t data)
{
    TWDR = data;
}

// ==========================================================================================================
// ��IIC״̬�Ĵ���
// 
// ����ֵ��TWSR[7:3]
// 
// ==========================================================================================================
uint8_t Drv_IIC_read_status(void)
{
    return TWSR & 0xF8;
}

// ==========================================================================================================
// �ȴ�TWINT��λ(�ֶ�д1����0��ִ���ж�ʱ�����Զ���0)
// 
// ˵����
// (1). TWINT��λ�󡢱�ʾ��ǰ�����Ѿ����
// (2). ʹ�����жϵĻ���TWINT = 1�������ж�
// 
// ==========================================================================================================
void Drv_IIC_wait_finish(void)
{
    while(!(TWCR & (1 << TWINT))) {}
}

// ==========================================================================================================
// �ȴ�TWINT��λ�󡢶�IIC״̬�Ĵ���
// 
// ����ֵ��TWSR[7:3]
// 
// ==========================================================================================================
uint8_t Drv_IIC_wait_finish_and_read_status(void)
{
    while(!(TWCR & (1 << TWINT))) {}
    return TWSR & 0xF8;
}

// ==========================================================================================================
// IIC�ж�
// 
// ˵����
// (1). TWINT��Ҫ�ֶ�д1����0��ִ���ж�ʱ�����Զ���0
// 
// ==========================================================================================================
ISR(TWI_vect)
{
    TWCR = 1 << TWINT;
}
