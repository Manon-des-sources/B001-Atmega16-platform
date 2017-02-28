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
// ---------------------------------------
// ���Ķ�����Atmega16�µĺ��ⷢ��/��������
// 
// ---------------------------------------
// ������
// 
// ==========================================================================================================
#include "Mod_IR.h"



// ==========================================================================================================
// ���ⷢ��
// ==========================================================================================================
#if USE_IR_SEND



// IRָ�
// IR���պͷ��͵�ָ���������
typedef enum 
{
    IR_COMMAND_01 = 0x01
}IR_COMMAND_SET;



// ==========================================================================================================
// ���ⷢ�ͳ�ʼ�� @ 8MHz
// 
// (1). ��ʱ��1����38KHz�ز�(1/3ռ�ձ�)
//      ����ΪT = ((1.0/8000000)*1000000)*8*(25+1) = 26us
//              = (1/8000000)*div*(N+1)(��λ����)
//            F = 8000000 / (div * (N + 1))(��λ��Hz)
//            N = 8000000 / ( freq * div ) - 1
//              = 8000000 / freq / div - 1
//      ռ�ձ�  = OCR1A / ICR1 = 9 / 27
// (2). �ز�����PD5(OC1A)���������
// (3). ��ʼ���ĺ�ʱ�ǿ��Խ��ܵģ�����ʹ�ó�����
// 
// ==========================================================================================================
void MOD_IR_send_init(void)
{
    uint16_t div;           // 16bit��ȵķ�Ƶ��
    uint16_t freq = 38000;  // 16bit��ȵ�Ƶ��
    uint8_t  duty = 33;     // 33%ռ�ձ�
    uint8_t  icr1;
    uint8_t  ocr1a;
    uint8_t  DIV  = T1_CLK_SOURCE_DIV_8;

    // --------
    // �������
    switch(DIV)
    {
        case T1_CLK_SOURCE_DIV_1:    div = 1;    break;
        case T1_CLK_SOURCE_DIV_8:    div = 8;    break;
        case T1_CLK_SOURCE_DIV_64:   div = 64;   break;
        case T1_CLK_SOURCE_DIV_256:  div = 256;  break;
        case T1_CLK_SOURCE_DIV_1024: div = 1024; break;
        default: return;
    }
    icr1  = ((SYS_OSC_FREQUENCE / freq) / div) - 1; // ��������8λ�ġ����ֲ����м���������16λ��32λ��
    ocr1a =  (icr1 * duty) / 100;

    // --------
    // ���ò���
    Drv_Timer1_init(T1_WGM_FAST_PWM_ICR1, COM_MODE_CLEAR, COM_MODE_NONE, DIV);
    Drv_Timer1_set_TCNT1_OCR1A_OCR1B_ICR1(0, ocr1a, 0, icr1);
    Drv_IO_mode_bit(DDRD, DDD5, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD5);
}

// ==========================================================================================================
// ���ⷢ��ֹͣ(ֹͣ����ز�)
// 
// (1). �رն�ʱ��1��ʱ��
// (2). OC1A��������͵�ƽ
// 
// ==========================================================================================================
void MOD_IR_send_stop(void)
{
    // --------------
    // ֹͣ��ʱ��ʱ��  
    Drv_Timer1_init(T1_WGM_FAST_PWM_ICR1, COM_MODE_CLEAR, COM_MODE_NONE, T1_CLK_SOURCE_NONE);
    Drv_IO_clr_bit(PORTD, PD5);
}





#else



void MOD_IR_send_init(void)
{
}
void MOD_IR_send_stop(void)
{
}



#endif  // #if USE_IR_SEND





// ==========================================================================================================
// �������
// ==========================================================================================================
#if USE_IR_RECEIVE





#else






#endif  // #if USE_IR_RECEIVE
