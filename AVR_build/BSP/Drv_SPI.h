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
#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__


#include <avr/io.h>

#include "sys_config.h"
#include "Drv_IO_Port.h"


// SPI的传输模式(CPOL：时钟极性)(CPHA：采样边沿)
// ---------------------------------------------------
// mode | CPOL | CPHA |      前沿      |      后沿
// ---------------------------------------------------
//  0   |  0   |  0   |  采样锁存(↑)  |  移出    (↓)
// ---------------------------------------------------
//  1   |  0   |  1   |  移出    (↑)  |  采样锁存(↓)
// ---------------------------------------------------
//  2   |  1   |  0   |  采样锁存(↓)  |  移出    (↑)
// ---------------------------------------------------
//  3   |  1   |  1   |  移出    (↓)  |  采样锁存(↑)
// ---------------------------------------------------
typedef enum 
{
    SPI_SHIFT_FIRST_MSB = 0,
    SPI_SHIFT_FIRST_LSB = 1,

    SPI_MODE_SLAVE = 0,
    SPI_MODE_MSTR  = 1,

    SPI_SHIFT_MODE_00 = 0,  // 传输模式(见上表)
    SPI_SHIFT_MODE_01 = 1,
    SPI_SHIFT_MODE_02 = 2,
    SPI_SHIFT_MODE_03 = 3,

    SPI_CLK_SOURCE_DIV_2   = 0,
    SPI_CLK_SOURCE_DIV_4   = 1,
    SPI_CLK_SOURCE_DIV_8   = 2,
    SPI_CLK_SOURCE_DIV_16  = 3,
    SPI_CLK_SOURCE_DIV_32  = 4,
    SPI_CLK_SOURCE_DIV_64  = 5,
    SPI_CLK_SOURCE_DIV_128 = 6
} DRV_SPI_SETTINHG;


#define Drv_SPI_set_SS()    Drv_IO_set_bit(PORTB, PB4)
#define Drv_SPI_clr_SS()    Drv_IO_clr_bit(PORTB, PB4)

void Drv_SPI_init(const uint8_t device_mode, const uint8_t shift_mode, const uint8_t shift_first, const uint8_t clk_source);
void Drv_SPI_INT_Enable(const uint8_t enable);

uint8_t Drv_SPI_clr_SPIF(void);
void Drv_SPI_write_byte(const uint8_t data);
uint8_t Drv_SPI_read_byte(const uint8_t dummy);



#endif  // #ifndef __DRV_SPI_H__
