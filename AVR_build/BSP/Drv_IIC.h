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
#ifndef __DRV_IIC_H__
#define __DRV_IIC_H__



#include <avr/io.h>
#include <avr/interrupt.h>
#include "sys_config.h"

typedef enum 
{
    // IIC������ģʽ����˭�ȷ���START�źž����ģ�IICӲ�����Զ���������Ƿ�æ����æ����START�źţ���Ȼ�Ͳ���(���������ߡ����ܳ�Ϊ����)
    IIC_MODE_SLAVE = 0,
    IIC_MODE_MSTR  = 1,

    // TWPS[1:0]����ֵ����TWBRһ�������SCLƵ�ʡ���Drv_IIC_init()��clk_source����
    IIC_TWPS_00 = 0,
    IIC_TWPS_01 = 1,
    IIC_TWPS_02 = 2,
    IIC_TWPS_03 = 3,
} DRV_IIC_SETTNG;

typedef enum 
{
    START = 0,  // ��ʼ�ź�
    STOP,       // �����ź�
    TRANSFER,   // ��������
} DRV_IIC_ACTION;

void Drv_IIC_init(const uint8_t clk_source, const uint8_t twps_quadruple);
void Drv_IIC_set_slave_addr(const uint8_t addr, const uint8_t gc_enable);
void Drv_IIC_INT_Enable(const uint8_t enable);

void Drv_IIC_action(const uint8_t action);
void Drv_IIC_start(void);
void Drv_IIC_stop(void);
void Drv_IIC_transfer(void);

void Drv_IIC_set_addr(const uint8_t addr, const uint8_t direction);
void Drv_IIC_set_data(const uint8_t data);

uint8_t Drv_IIC_read_status(void);
void Drv_IIC_wait_finish(void);
uint8_t Drv_IIC_wait_finish_and_read_status(void);



#endif  // #ifndef __DRV_IIC_H__
