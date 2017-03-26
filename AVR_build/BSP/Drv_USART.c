// ==========================================================================================================
// Copyright (c) 2017 ChenChen <codingmanon@163.com>
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
// 本文定义了Atmega16下的USART发送/接收驱动
// 
// ---------------------------------------
// 包含：
// 
// ==========================================================================================================
#include "Drv_USART.h"


// ==========================================================================================================
// USART 初始化
// 
// 参数：parity         奇偶校验(USART_PARITY_NONE,USART_PARITY_ODD,USART_PARITY_EVEN)
//       char_size      数据位宽(字符长度)(USART_CHAR_SIZE_9:USART_CHAR_SIZE_5)
//       baud_rate      波特率
// 
// 说明：
// (1). 默认使用9位数据位宽、第9位是校验位，不需要手动写入、启动奇偶校验即可由硬件自动写入
// (2). 写UCSRC时、需要将URSEL位写1
// (3). 写UBRRH时、需要将URSEL位清0
// (4). 写UCSRA时、需要将UDRE位清0
// (5). 相同波特率下，倍速的误差比不倍速的小，因此这里启用倍速
// (6). 8MHz下、115200的误差为-3.5%(倍速)、Secure CRT可以收到数据，但逻辑分析仪表示收到的数据中有一半左右是错误的
//      而波特率更高的250000bps的误差为0，逻辑分析仪可以全部收对，也就是说误差太大就可能导致错误，这个需要测试
// 
// ==========================================================================================================
void Drv_USART_init(const uint8_t parity, const uint8_t char_size, const uint32_t baud_rate)
{
    uint8_t  mode         = USART_MODE_ASYNC;        // 异步模式
    uint8_t  stop_bit     = USART_STOP_BIT_2;        // 停止位的位数.USBS
    uint8_t  multi        = USART_MULTI_MCU_DIS;     // 多机通信模式使能位.MPCM
    uint8_t  double_speed = USART_DOUBLE_SPEED_EN;   // 倍速使能位.U2X
    uint8_t  polarity     = USART_POLARITY_RIASE;    // 时钟极性.UCPOL

    uint8_t  char_size_2  = (char_size >> 2) & 0x01;
    uint8_t  char_size_10 =  char_size & 0x03;
    uint8_t  div;
    uint16_t ubrr;

    if(USART_MODE_ASYNC == mode)
    {
        polarity     = 0;  // 异步模式下、时钟极性.UCPOL不可用、需要清0
        div          = (1 == double_speed) ? 8 : 16 ;
    }
    else
    {
        double_speed = 0;  // 同步模式下、倍速.U2X不可用、需要清0
        div          = 2;
    }

    UCSRA = (double_speed << U2X ) |  // 倍速使能位.U2X
            (multi        << MPCM) ;  // 多机通信模式使能位.MPCM

    UCSRC = ( 1              << URSEL) |  // 将URSEL位写1
            ( mode           << UMSEL) |  // 同步/异步模式.UMSEL
            ((parity & 0x03) << UPM0 ) |  // 奇偶校验.UPM[1:0]
            ( stop_bit       << USBS ) |  // 停止位的位数.USBS
            ( char_size_10   << UCSZ0) |  // 数据位宽.UCSZ[1:0]
            ( polarity       << UCPOL) ;  // 时钟极性.UCPOL

    // 先清除再或上具体的数值
    UCSRB &= ~(1           << UCSZ2);
    UCSRB |=  (char_size_2 << UCSZ2); // 数据位宽UCSZ2

    // 设置波特率
//  ubrr = (SYS_OSC_FREQUENCE / (div * baud_rate)) - 1;
    ubrr = (2 * SYS_OSC_FREQUENCE + div * baud_rate) / (2 * div * baud_rate) - 1;  // 在上一句的基础上+0.5、以实现四舍五入
    UBRRH = ((~(1 << URSEL)) & ((ubrr >> 8) & 0x000F)) & 0x00FF;  // 需要将URSEL位清0
    UBRRL = ubrr & 0x00FF;
}

// ==========================================================================================================
// USART 中断使能
// 
// 参数：int_mode = USART_INT_MODE_RXC,USART_INT_MODE_TXC,USART_INT_MODE_UDR
//       enable   = ENABLE,DISABLE
// 
// ==========================================================================================================
void Drv_USART_INT_Enable(const uint8_t int_mode, const uint8_t enable)
{
    if(USART_INT_MODE_RXC == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << RXCIE);
        }
        else
        {
            UCSRB |=  (1 << RXCIE);
        }
    }
    if(USART_INT_MODE_TXC == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << TXCIE);
        }
        else
        {
            UCSRB |=  (1 << TXCIE);
        }
    }
    if(USART_INT_MODE_UDR == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << UDRIE);
        }
        else
        {
            UCSRB |=  (1 << UDRIE);
        }
    }
}

// ==========================================================================================================
// USART 收发器的启动与禁止
// 
// 参数：transfer = 发送器或接收器(USART_TRANSFER_RX,USART_TRANSFER_TX)
//       enable   = ENABLE,DISABLE
// 
// 说明：
// (1). 禁止接收器、R-UDR将被刷新、且FE/DOR/PE标志将无效、同时RxD引脚恢复为通用IO
// (2). 禁止发送器、所有数据都已发送完毕后(T-UDR和发送移位寄存器)、发送器的硬件才会停止
// (3). 禁止发送器、TxD引脚将会恢复为为通用IO
// 
// ==========================================================================================================
void Drv_USART_transfer_start(const uint8_t transfer, const uint8_t enable)
{
    if(USART_TRANSFER_RX == transfer)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << RXEN);
        }
        else
        {
            UCSRB |=  (1 << RXEN);
        }
    }
    if(USART_TRANSFER_TX == transfer)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << TXEN);
        }
        else
        {
            UCSRB |=  (1 << TXEN);
        }
    }
}
