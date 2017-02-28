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
// 本文定义了Atmega16下的SPI初始化
// 
// ==========================================================================================================
#include "Drv_SPI.h"


// ==========================================================================================================
// SPI初始化
// 
// 参数：device_mode    主从模式(主机：SPI_MODE_MSTR, 从机：SPI_MODE_SLAVE) 
//       shift_mode     传输模式(锁存/移位模式)
//       shift_first    首先移出MSB(左移)还是LSB(右移)
//       clk_source     SCLK频率
// 
// 说明：
// (1). 当MSTR=1 && SS引脚为输入IO时，如果SS引脚被拉低，会导致MSTR=0 && SPIF=1，使得机器变成从机模式
// 
// ==========================================================================================================
void Drv_SPI_init(const uint8_t device_mode, const uint8_t shift_mode, const uint8_t shift_first, const uint8_t clk_source)
{
    uint8_t spr10;
    uint8_t spi2x;
    uint8_t cpol;
    uint8_t cpha;

    switch(shift_mode)
    {
        case SPI_SHIFT_MODE_00: cpol = 0; cpha = 0;
                                break;
        case SPI_SHIFT_MODE_01: cpol = 0; cpha = 1;
                                break;
        case SPI_SHIFT_MODE_02: cpol = 1; cpha = 0;
                                break;
        case SPI_SHIFT_MODE_03: cpol = 1; cpha = 1;
                                break;
        default : return;
    }
    switch(clk_source)
    {
        case SPI_CLK_SOURCE_DIV_2:   spr10 = 0; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_4:   spr10 = 0; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_8:   spr10 = 1; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_16:  spr10 = 1; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_32:  spr10 = 2; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_64:  spr10 = 2; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_128: spr10 = 3; spi2x = 0;
                                     break;
        default : return;
    }

    // IO设置
    if(SPI_MODE_MSTR == device_mode)
    {
        Drv_IO_set_bit(PORTB, PB4);
        Drv_IO_mode_bit(DDRB, PB4, IO_OUTPUT); // SS输出(拉高)、以禁止SPI从机
        Drv_IO_clr_bit(PORTB, PB5);
        Drv_IO_mode_bit(DDRB, PB5, IO_OUTPUT); // MOSI输出(拉低)
        Drv_IO_clr_bit(PORTB, PB6);
        Drv_IO_mode_bit(DDRB, PB6, IO_INPUT);  // MISO输入(拉低)
        Drv_IO_clr_bit(PORTB, PB7);
        Drv_IO_mode_bit(DDRB, PB7, IO_OUTPUT); // SCLK输出(拉低)
    }
    else
    {
        Drv_IO_set_bit(PORTB, PB4);
        Drv_IO_mode_bit(DDRB, PB4, IO_INPUT);  // SS输入(拉高|如果PUD使能、将打开上拉)
        Drv_IO_clr_bit(PORTB, PB5);
        Drv_IO_mode_bit(DDRB, PB5, IO_INPUT);  // MOSI输入(拉低)
        Drv_IO_clr_bit(PORTB, PB6);
        Drv_IO_mode_bit(DDRB, PB6, IO_OUTPUT); // MISO输出(拉低)
        Drv_IO_clr_bit(PORTB, PB7);
        Drv_IO_mode_bit(DDRB, PB7, IO_INPUT);  // SCLK输入(拉低)
    }

    // 初始化
    SPCR = ( 1 << SPE ) |
           ((shift_first & 0x01) << DORD) | // 先发送MSB或LSB
           ((device_mode & 0x01) << MSTR) | // 主从模式
           ((cpol        & 0x01) << CPOL) | // 时钟极性
           ((cpha        & 0x01) << CPHA) | // 前后沿
           ((spr10       & 0x03) << SPR0) ; // 时钟频率

    SPSR &= ~(1     << SPI2X);
    SPSR |=  (spi2x << SPI2X);              // 时钟倍频
}

// ==========================================================================================================
// SPI传输完成中断使能
// 
// ==========================================================================================================
void Drv_SPI_INT_Enable(const uint8_t enable)
{
    if(DISABLE == enable)
    {
        SPCR &= ~(1 << SPIE);
    }
    else
    {
        SPCR |=  (1 << SPIE);
    }
    Drv_SPI_clr_SPIF();
}

// ==========================================================================================================
// 清除中断标志位SPIF
// 
// 说明：
// (1). SPIF在进入ISR时自动清0，或先读SPSR、在访问SPDR，可清0
// (2). 增加一个返回值是为了避免编译器将这段代码优化掉，因为优化器可能会认为这段段码对外部无任何影响
// 
// ==========================================================================================================
uint8_t Drv_SPI_clr_SPIF(void)
{
    uint8_t temp;

    temp  = SPSR;
    temp += SPDR;

    return temp;
}

// ==========================================================================================================
// 将数据写入SPDR、并等待写入结束
// 
// 说明：
// (1). 使用这个函数、必须禁止SPI中断，否则中断会自动清0 SPIF，导致这里永远等不到SPIF=0的时刻
// 
// ==========================================================================================================
void Drv_SPI_write_byte(const uint8_t data)
{
    Drv_SPI_clr_SPIF();
    SPDR = data;
    while(0 == (SPSR & (1 << SPIF)))
    {}
}

// ==========================================================================================================
// 读取SPDR中的数据
// 
// 说明：
// (1). 发送无意义的数据SPI_COMMAND_DUMMY去启动SCLK、与从机交换数据
// 
// ==========================================================================================================
uint8_t Drv_SPI_read_byte(const uint8_t dummy)
{
    uint8_t data;

    Drv_SPI_write_byte(dummy);
    data = SPDR;

    return data;
}
