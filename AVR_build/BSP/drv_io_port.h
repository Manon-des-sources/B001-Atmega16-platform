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
// ==========================================================================================================
#ifndef __DRV_IO_PORT_H__
#define __DRV_IO_PORT_H__


#include <avr/io.h>
#include <avr/interrupt.h>

#include "sys_timer.h"
#include "sys_config.h"


// IO配置
// -------------------------------------------------------------
// DDxn | PORTxn | PUD | I/O | 上拉电阻 | 引脚状态
// -------------------------------------------------------------
//  0   |    0   |  X  |  I  |    OFF   | 高阻态
// -------------------------------------------------------------
//  0   |    1   |  0  |  I  |    ON    | 外部电路为L时输出电流
// -------------------------------------------------------------
//  0   |    1   |  1  |  I  |    OFF   | 高阻态
// -------------------------------------------------------------
//  1   |    0   |  X  |  O  |    OFF   | 输出L(吸收电流)
// -------------------------------------------------------------
//  1   |    1   |  X  |  O  |    OFF   | 输出H(输出电流)
// -------------------------------------------------------------
typedef enum 
{
	IO_INPUT   = 0,
	IO_OUTPUT  = 1,

	IO_PUD_ON  = 0,  // 控制上拉电阻的步骤：1.SFIOR.PUD = IO_PUD_ON;
	IO_PUD_OFF = 1,  //                     2.DDRxn     = IO_INPUT;
	IO_RES_ON  = 1,  //                     3.PORTxn    = IO_RES_ON or IO_RES_OFF
	IO_RES_OFF = 0
} DRV_IO_OPTION;

// IO操作
#define Drv_IO_toggle_bit(PORT,bit)     PORT ^=  (1 << bit)
#define Drv_IO_set_bit(PORT,bit)        PORT |=  (1 << bit)
#define Drv_IO_clr_bit(PORT,bit)        PORT &= ~(1 << bit)

// 这种读-修改写的方式好于：do{ DDRx &= ~(1 << bit); DDRx |= (mode << bit); }while(0)，因为DDRx的bit只翻转了1次，而do...的方式里面可能会翻转2次，这个翻转有时会带来麻烦
#define Drv_IO_mode_bit(DDRx,bit,mode)  DDRx  = (mode   ? (DDRx  |   (1 << bit))  : (DDRx  & (~(1 << bit))))
#define Drv_IO_PUD_enable(enable)       SFIOR = (enable ? (SFIOR & (~(1 << PUD))) : (SFIOR |   (1 << PUD)))

#define Drv_IO_write_port(PORT,data)    PORT  = data
#define Drv_IO_read_port(data,PIN)      data  = PIN


// 外部中断INTn操作
typedef enum 
{
    INTn_IO_INT0 = 0,
    INTn_IO_INT1 = 1,
    INTn_IO_INT2 = 2,

    INTn_LEVEL_LOW  = 0,
    INTn_EDGE_BOTH  = 1,
    INTn_EDGE_FALL  = 2,
    INTn_EDGE_RAISE = 3
} DRV_INTn_OPTION;

void Drv_IO_INTn_init(const uint8_t INTn_IO, const uint8_t edge_mode);
void Drv_IO_INTn_Enable(const uint8_t INTn_IO, const uint8_t enable);



#endif  // #ifndef __DRV_IO_PORT_H__
