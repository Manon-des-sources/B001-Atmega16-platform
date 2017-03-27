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
// ���Ķ�����Atmega16�µ�USART����/��������
// 
// ==========================================================================================================
#ifndef __DRV_USART_H__
#define __DRV_USART_H__


#include <avr/io.h>
#include "sys_config.h"


typedef enum 
{
    // ����ʹ��
    USART_DOUBLE_SPEED_DIS = 0,
    USART_DOUBLE_SPEED_EN  = 1,

    // ���ͨ��ģʽʹ��
    USART_MULTI_MCU_DIS = 0,
    USART_MULTI_MCU_EN  = 1,

    // �շ���
    USART_TRANSFER_RX = 0,
    USART_TRANSFER_TX = 1,

    // ������ʹ��
    USART_RXEN_DIS = 0,
    USART_RXEN_EN  = 1,

    // ������ʹ��
    USART_TXEN_DIS = 0,
    USART_TXEN_EN  = 1,

    // �첽/ͬ��ģʽ
    USART_MODE_ASYNC = 0,
    USART_MODE_SYNC  = 1,

    // ��żУ��
    USART_PARITY_NONE = 0,
    USART_PARITY_EVEN = 2,
    USART_PARITY_ODD  = 3,

    // ֹͣλ��λ��
    USART_STOP_BIT_1 = 0,
    USART_STOP_BIT_2 = 1,

    // ʱ�Ӽ���(ͬ��ģʽ����Ч)
    USART_POLARITY_RIASE = 0,  // �������Ƴ����ݡ��½��ز�������
    USART_POLARITY_FALL  = 1,  // �½����Ƴ����ݡ������ز�������

    // ����λ��(�ַ�����)
    USART_CHAR_SIZE_5 = 0,
    USART_CHAR_SIZE_6 = 1,
    USART_CHAR_SIZE_7 = 2,
    USART_CHAR_SIZE_8 = 3,
    USART_CHAR_SIZE_9 = 7,

    // �ж�ģʽ
    USART_INT_MODE_RXC = 0,
    USART_INT_MODE_TXC = 1,
    USART_INT_MODE_UDR = 2,
} DRV_USART_OPTION;

void Drv_USART_init(const uint8_t parity, const uint8_t char_size, const uint32_t baud_rate);
void Drv_USART_INT_Enable(const uint8_t int_mode, const bool enable);
void Drv_USART_transfer_start(const uint8_t transfer, const bool enable);
void Drv_USART_multi_enable(const bool enable);



#endif  // #ifndef __DRV_USART_H__
