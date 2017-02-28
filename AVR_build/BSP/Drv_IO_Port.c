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
// --------------------------------
// ������
// 
// (1). IO�Ĳ���(hͷ�ļ�)
// (2). �ⲿ�жϳ�ʼ����ʹ��
// 
// ==========================================================================================================
#include "Drv_IO_Port.h"



// ==========================================================================================================
// �ⲿ�жϳ�ʼ��
// 
// ������INTn_IO    �ⲿ�жϺ�(INTn_IO_INT0��INTn_IO_INT1��INTn_IO_INT2)
//       edge_mode  �жϱ���ģʽ(INTn_LEVEL_LOW��INTn_EDGE_BOTH��INTn_EDGE_FALL��INTn_EDGE_RAISE)
// 
// ˵����
// (1). ʹ��INT[2:0]��ͨ����������Ϊ����ģʽ
//      �����Ӧ���������ģʽ����ô�����ŵ�ƽ�仯ʱ���ᴥ���ⲿ�жϣ����Ϊ�������
// 
// δ������⣺
// (1). INTn_LEVEL_LOWģʽ�¡�DDRxn=PORTxn=0������оƬ�޷�����,������DDRxn=0��PORTxn=1��
//      ��PORTxn=1ʱ��INTn���ų��ֵ͵�ƽ��������оƬֹͣ����/������
//     Drv_IO_mode_bit(DDRD, DDD2, IO_INPUT);
//     Drv_IO_set_bit(PORTD, PD2);
// 
// ==========================================================================================================
void Drv_IO_INTn_init(const uint8_t INTn_IO, const uint8_t edge_mode)
{
    switch(INTn_IO)
    {
        case INTn_IO_INT0:  // �ⲿ�ж�0
                            MCUCR &= ~(0x03 << 0);
                            MCUCR |= ((edge_mode & 0x03) << 0);
                            break;
        case INTn_IO_INT1:  // �ⲿ�ж�1
                            MCUCR &= ~(0x03 << 2);
                            MCUCR |= ((edge_mode & 0x03) << 2);
                            break;
        case INTn_IO_INT2:  // �ⲿ�ж�2
                            GICR &= ~(1 << INT2);
                            if(INTn_EDGE_FALL  == edge_mode) { MCUCSR &= ~(1 << ISC2); }
                            if(INTn_EDGE_RAISE == edge_mode) { MCUCSR |=  (1 << ISC2); }
                            break;
        default : break;
    }
}

// ==========================================================================================================
// �ⲿ�ж�ʹ��
// 
// ������INTn_IO    �ⲿ�жϺ�(INTn_IO_INT0��INTn_IO_INT1��INTn_IO_INT2)
//       enable     ʹ�ܻ�ʹ��(ENABLE��DISABLE)
// 
// ==========================================================================================================
void Drv_IO_INTn_Enable(const uint8_t INTn_IO, const uint8_t enable)
{
    switch(INTn_IO)
    {
        case INTn_IO_INT0:  // �ⲿ�ж�0
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
        case INTn_IO_INT1:  // �ⲿ�ж�1
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
        case INTn_IO_INT2:  // �ⲿ�ж�2
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
// INT0 �жϷ������
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
// INT1 �жϷ������
// 
// ==========================================================================================================
ISR(INT1_vect)
{
}

// ==========================================================================================================
// INT2 �жϷ������
// 
// ==========================================================================================================
ISR(INT2_vect)
{
}
