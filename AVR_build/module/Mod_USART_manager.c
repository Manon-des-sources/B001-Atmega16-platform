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
// ----------------------------------------------
// ���Ķ�����Atmega16�µ�USART����/�������ݹ�����
// 
// ----------------------------------------------
// ������
// 
// ==========================================================================================================
#include "Mod_USART_manager.h"


typedef struct 
{
    uint8_t  count;   // �������Ѿ����͵�������
    uint8_t  number;  // �ܵ�������
    uint8_t *data;    // ����buffer
    uint8_t  end;     // �������״̬
    p_bool_funtion_uint8_t fun;  // ִ���û�ָ�������ݷ��ͺ���
}T_USART_transfer_CTRL;

volatile T_USART_transfer_CTRL transfer_ctrl = { .count  = 0,
                                                 .number = 0,
                                                 .data   = NULL,
                                                 .end    = TRUE,
                                                 .fun    = NULL
                                                };

char buffer_usart[] = "s123456";

T_FLOAT_INT32_UNION value_check = { .float32 = 10.45 };


// ==========================================================================================================
// USART ��������ʼ��
// 
// ������mode           ͬ��/�첽ģʽ(USART_MODE_ASYNC��USART_MODE_SYNC)
//       parity         ��żУ��(USART_PARITY_ODD��USART_PARITY_EVEN)
//       stop_bit       ֹͣλ��λ��(USART_STOP_BIT_1��USART_STOP_BIT_2)
//       char_size      ����λ��(�ַ�����)(USART_CHAR_SIZE_9:USART_CHAR_SIZE_5)
//       baud_rate      12λ�Ĳ�����
// 
// ˵����
// (1). Ĭ��ʹ��9λ����λ����9λ��У��λ������Ҫ�ֶ�д�롢������żУ�鼴����Ӳ���Զ�д��
// (2). дUCSRCʱ����Ҫ��URSELλд1
// (3). дUBRRHʱ����Ҫ��URSELλ��0
// 
// ==========================================================================================================
void Mod_USART_init(void)
{
    // USART�ӿڳ�ʼ��
    Drv_USART_init(USART_PARITY_ODD, USART_CHAR_SIZE_8, 115200);

    // ������IO��ʼ��
    Drv_IO_set_bit(PORTD,PD1);
    Drv_IO_mode_bit(DDRD,PD1,IO_OUTPUT);
}

// ==========================================================================================================
// USART �������ݷ���
// 
// ������fun        ���ݶ�ȡ����
// 
// ˵����
// (1). 
// 
// ==========================================================================================================
void Mod_USART_transfer(p_bool_funtion_uint8_t const fun)
{
    if(NULL != fun)
    {
        transfer_ctrl.end = FALSE;
        transfer_ctrl.fun = fun;
        Drv_USART_transfer_start(USART_TRANSFER_TX, ENABLE);
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, ENABLE);
    }
}

char a1 = ' ';
char a2 = ' ';
signed char a9 = 0xC0;

         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
unsigned long int a20[] = { 100 };

// ==========================================================================================================
// USART ����ģ��
// 
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 0;

    switch(step)
    {
        case 0: print("[%c],(%d),|%l|\n\r",a9, a9, a9, a9);
                step = 20;
                break;        
        case 1: print("%U%c%c", a20[0], a1, a2);
                a20[0]--;
                a1    = (0 == (a20[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a20[0] % 10)) ? '\n' : ' ';
                step += ((0xFFFFFFFF - 100) == a20[0]);
                break;       
        case 2: print("%u\r\n",a20[0]);
                step = 20;
                break; 

        default:break;
    }
}

// ==========================================================================================================
// USART ���ͻ���Ĵ���Ϊ���ж�
// 
// ˵����
// (0). UDRE��ʼ��ʱ����1������ֻ��Ҫ����UDR�жϣ��Ϳ���ֱ�Ӽ�������жϣ������ж�����ȥд����
// (1). ���Լ�����T-UDR��д����һ�����ݵȴ����ͣ������TXC�ж��еȴ�������ϲ�д����Ҫ������
// (2). ����ͨ��дT-UDR����0 UDRE��־λ�������˳��жϺ󽫼��������µ��жϣ������������д��󡢾�Ӧ��ֹ����ж�
// (3). ���һ������д����������ݷ�������жϣ�ȥ�ȴ����ķ������
// (4). ������UDRE(дUDR���)����UDRE����Ϊ1
// 
// ==========================================================================================================
ISR(USART_UDRE_vect)
{
//    UDR = transfer_ctrl.data[transfer_ctrl.count++];
    if(FALSE == transfer_ctrl.fun((uint8_t *)&UDR))
    {
        transfer_ctrl.end = TRUE;
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, DISABLE);
        Drv_USART_transfer_start(USART_TRANSFER_TX, DISABLE);
    }
}

// ==========================================================================================================
// USART ��������ж�
// 
// ˵����
// (1). ���ݷ��������Ѿ�������Ӳ�����Զ���������(�ͷ�����)
// (2). ��UDRE�ж��м�������Ƿ�д�꣬д�������������ɱ�־���ȵ����﷢�ͽ����󣬾Ϳ���ȷ���������ݷ��ͽ���
// 
// ==========================================================================================================
ISR(USART_TXC_vect)
{
/*
    if(transfer_ctrl.count >= transfer_ctrl.number)
    {
        transfer_ctrl.end = 1;
        Drv_USART_INT_Enable(USART_INT_MODE_TXC, DISABLE);
        Drv_USART_transfer_start(USART_TRANSFER_TX, DISABLE);
    }
    else
    {
        UDR = transfer_ctrl.data[print_ctrl.count++];
    }
*/
}

// ==========================================================================================================
// USART ��������ж�
// 
// ˵����
// (1). ���Զ�������
// 
// ==========================================================================================================
ISR(USART_RXC_vect)
{
}
