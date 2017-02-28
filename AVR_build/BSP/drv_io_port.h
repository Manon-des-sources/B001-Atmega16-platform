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
// ���Ķ�����Atmega16�µ�IO���ò���
// 
// ==========================================================================================================
#ifndef __DRV_IO_PORT_H__
#define __DRV_IO_PORT_H__


#include <avr/io.h>
#include <avr/interrupt.h>

#include "sys_timer.h"
#include "sys_config.h"


// IO����
// -------------------------------------------------------------
// DDxn | PORTxn | PUD | I/O | �������� | ����״̬
// -------------------------------------------------------------
//  0   |    0   |  X  |  I  |    OFF   | ����̬
// -------------------------------------------------------------
//  0   |    1   |  0  |  I  |    ON    | �ⲿ��·ΪLʱ�������
// -------------------------------------------------------------
//  0   |    1   |  1  |  I  |    OFF   | ����̬
// -------------------------------------------------------------
//  1   |    0   |  X  |  O  |    OFF   | ���L(���յ���)
// -------------------------------------------------------------
//  1   |    1   |  X  |  O  |    OFF   | ���H(�������)
// -------------------------------------------------------------
typedef enum 
{
	IO_INPUT   = 0,
	IO_OUTPUT  = 1,

	IO_PUD_ON  = 0,  // ������������Ĳ��裺1.SFIOR.PUD = IO_PUD_ON;
	IO_PUD_OFF = 1,  //                     2.DDRxn     = IO_INPUT;
	IO_RES_ON  = 1,  //                     3.PORTxn    = IO_RES_ON or IO_RES_OFF
	IO_RES_OFF = 0
} DRV_IO_OPTION;

// IO����
#define Drv_IO_toggle_bit(PORT,bit)     PORT ^=  (1 << bit)
#define Drv_IO_set_bit(PORT,bit)        PORT |=  (1 << bit)
#define Drv_IO_clr_bit(PORT,bit)        PORT &= ~(1 << bit)

// ���ֶ�-�޸�д�ķ�ʽ���ڣ�do{ DDRx &= ~(1 << bit); DDRx |= (mode << bit); }while(0)����ΪDDRx��bitֻ��ת��1�Σ���do...�ķ�ʽ������ܻᷭת2�Σ������ת��ʱ������鷳
#define Drv_IO_mode_bit(DDRx,bit,mode)  DDRx  = (mode   ? (DDRx  |   (1 << bit))  : (DDRx  & (~(1 << bit))))
#define Drv_IO_PUD_enable(enable)       SFIOR = (enable ? (SFIOR & (~(1 << PUD))) : (SFIOR |   (1 << PUD)))

#define Drv_IO_write_port(PORT,data)    PORT  = data
#define Drv_IO_read_port(data,PIN)      data  = PIN


// �ⲿ�ж�INTn����
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
