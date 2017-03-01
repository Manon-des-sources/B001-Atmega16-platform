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
// ---------------------------------------------
// ���Ķ�����Atmega16�µ�print����(��printf����)
// 
// ---------------------------------------------
// ������
// 
// ==========================================================================================================
#include "Mod_USART_print.h"



#define PRINT_RING_BUFFER_MAX   64

typedef struct 
{
    uint8_t in;     // дָ��
    uint8_t out;    // ��ָ��
    uint8_t num;    // ��д���������
}T_PRINT_CTRL;

T_PRINT_CTRL print_ctrl = { .in  = 0,
                            .out = 0,
                            .num = 0
                          };
static uint8_t print_ring_buffer[PRINT_RING_BUFFER_MAX];
static uint8_t data_part[10] = {0};  // ���ڴ�Ų�ֺ��32λʮ������


// ==========================================================================================================
// print_ctrl_add() ��дָ���Լ��ƶ�����
// 
// ������p          ��дָ���ֵ
// 
// ==========================================================================================================
uint8_t print_ctrl_add(uint8_t p)
{
    return (PRINT_RING_BUFFER_MAX == (p + 1)) ? 0 : (p + 1);
}

// ==========================================================================================================
// print_data_in()  �򻺳���д��1������
// 
// ������data       ����ֵ
// 
// ����ֵ��Fin      TRUE = ����д��ɹ���FALSE = buffer����
// 
// ==========================================================================================================
bool print_data_in(uint8_t data)
{
    bool Fin = TRUE;

    if(print_ctrl.num < (PRINT_RING_BUFFER_MAX + 1))
    {
        print_ring_buffer[print_ctrl.in] = data;
        print_ctrl.in = print_ctrl_add(print_ctrl.in);
        print_ctrl.num++;
    }
    else
    {
        Fin = FALSE;
    }
    return Fin;
}

// ==========================================================================================================
// print_data_out() �ӻ���������1������
// 
// ������*data      ���ڽ�������ֵ
// 
// ����ֵ��Fin      TRUE = ���ݶ����ɹ���FALSE = bufferΪ��
// 
// ==========================================================================================================
bool print_data_out(uint8_t *data)
{
    bool Fin = TRUE;

    if(print_ctrl.num > 0)
    {
       *data = print_ring_buffer[print_ctrl.out];
        print_ctrl.out = print_ctrl_add(print_ctrl.out);
        print_ctrl.num--;
    }
    else
    {
        Fin = FALSE;
    }
    return Fin;
}

// ==========================================================================================================
// print_buffer_data() ���unsigned int���������ݵ�ÿһλ������buffer
// 
// ������data       ��������
// 
// ==========================================================================================================
void print_buffer_udata(uint32_t data)
{
    uint8_t  index = 0;

    do
    {
        data_part[index] = data % 10;
        data -= data_part[index];  // ����������Խ��Խ�󡢵��³��������ѭ������������Ҫ����ת��Ϊ����
        data /= 10;
        index++;
    }while(0 != data);

    do
    {
        index--;
        print_data_in(data_part[index] + '0');
    }while(0 < index);
}

// ==========================================================================================================
// print_buffer_data() ���int���������ݵ�ÿһλ������buffer
// 
// ������data       ��������
// 
// ==========================================================================================================
void print_buffer_data(int32_t data)
{
    uint32_t temp  = data;

    if(0 > data)
    {
        temp = -data;
        print_data_in('-');
    }

    print_buffer_udata(temp);
}

// ==========================================================================================================
// print_buffer_char() ��֯�ַ����ݵ�����buffer
// 
// ������data       �ַ�����
// 
// ==========================================================================================================
void print_buffer_char(char data)
{
    print_data_in(data);
}

// ==========================================================================================================
// print_buffer_string() ��֯�ַ���������buffer
// 
// ������data       �ַ�����ַ
// 
// ==========================================================================================================
void print_buffer_string(char *data)
{
    while('\0' != *data)
    {
        print_data_in((uint8_t)*data);
        data++;
    }
}

// ==========================================================================================================
// print_buffer_hex() ��֯������ʮ�����Ƹ�ʽ����������buffer
//
// ������data       ����
//
// ==========================================================================================================
void print_buffer_hex(uint32_t data)
{
    uint8_t index = 0;
    uint8_t temp;

    do
    {
        temp = (data & 0x0F);
        temp = ((0 <= temp) && (temp <= 9)) ? (temp + '0') : (temp + 'a' - 10);
        data_part[index] = temp;
        data >>= 4;  // ���������������ܱ�֤���ѭ���ܹ��˳�(��������ʱ���λ��1)
        index++;
    }while(0 != data);

    do
    {
        index--;
        print_data_in(data_part[index]);
    }while(0 < index);
}

// ==========================================================================================================
// print()
// 
// ������fmt        ��ʽ�����ַ���
// 
// ˵����
// (1). ֧�ָ�ʽ��%c    char  �ַ�
//                %s    char *�ַ���
//                %d    16λsigned int
//                %l    32λsigned long int
//                %u    16λunsigned int
//                %U    32λunsigned long int
//                %x    16λint��ʮ�����Ƹ�ʽ(�����ᱣ������λ)
//                %X    32λlong int��ʮ�����Ƹ�ʽ
// (2). �ո�����%��d/./x֮�䣬���������print("a = % d\n",a);
// (3). ��Ҫ���ж��л���ռʽ������ʹ�����print��������Ϊ���ǲ�������ģ����޸�ȫ�ֵ�buffer
// 
// ==========================================================================================================
void print(const char fmt[], ...)
{
    bool     transfer = FALSE;
    uint8_t *format   = (uint8_t *)fmt;
    uint16_t addr     = 0;
    va_list  ap       = NULL;

    va_start(ap, fmt);

    while('\0' != *format)
    {
        transfer = TRUE;

        if('%' == *format)
        {
            format++;
            switch(*format)
            {
                case 'c':print_buffer_char((char)va_arg(ap, int));
                         break;
                case 's':addr = sram_addr_overflow_check(va_arg(ap, int));
                         print_buffer_string((char *)addr);
                         break;
                case 'd':print_buffer_data(va_arg(ap, int));
                         break;
                case 'l':print_buffer_data(va_arg(ap, long));
                         break;
                case 'u':print_buffer_udata((uint32_t)(va_arg(ap, int)) & 0x0000FFFF);  // ȥ������λ(����intת��Ϊ�޷���long��ʱ�򡢸�16λȫ�Ƿ���λFFFF)
                         break;
                case 'U':print_buffer_udata(va_arg(ap, long));
                         break;
                case 'x':print_buffer_hex(va_arg(ap, int));  // ������16λ���ÿ��Կ�������λ
                         break;
                case 'X':print_buffer_hex(va_arg(ap, long));
                         break;
                default: va_arg(ap, int);  // ����������������ĸ�ʽ���ò��ԣ����������������ľ���ֵû�б�������
                         print_buffer_char(*format);
                         break;
            }
        }
        else
        {
            print_buffer_char(*format);
        }
        format++;
    }
    // �������ݷ���(������������ڷ��ͣ��������������Ӱ�����ڷ��͵Ĳ���)
    if(TRUE == transfer)
    {
        Mod_USART_transfer(print_data_out);
    }
    va_end(ap);
}






























