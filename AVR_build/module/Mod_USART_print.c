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
// 本文定义了Atmega16下的print函数(仿printf函数)
// 
// ---------------------------------------------
// 包含：
// 
// ==========================================================================================================
#include "Mod_USART_print.h"



#define PRINT_RING_BUFFER_MAX   64

typedef struct 
{
    uint8_t in;     // 写指针
    uint8_t out;    // 读指针
    uint8_t num;    // 已写入的数据量
}T_PRINT_CTRL;

T_PRINT_CTRL print_ctrl = { .in  = 0,
                            .out = 0,
                            .num = 0
                          };
static uint8_t print_ring_buffer[PRINT_RING_BUFFER_MAX];
static uint8_t data_part[10] = {0};  // 用于存放拆分后的32位十进制数


// ==========================================================================================================
// print_ctrl_add() 读写指针自加移动控制
// 
// 参数：p          读写指针的值
// 
// ==========================================================================================================
uint8_t print_ctrl_add(uint8_t p)
{
    return (PRINT_RING_BUFFER_MAX == (p + 1)) ? 0 : (p + 1);
}

// ==========================================================================================================
// print_data_in()  向缓冲区写入1个数据
// 
// 参数：data       数据值
// 
// 返回值：Fin      TRUE = 数据写入成功、FALSE = buffer已满
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
// print_data_out() 从缓冲区读出1个数据
// 
// 参数：*data      用于接收数据值
// 
// 返回值：Fin      TRUE = 数据读出成功、FALSE = buffer为空
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
// print_buffer_data() 拆分unsigned int型整数数据的每一位到数据buffer
// 
// 参数：data       整数数据
// 
// ==========================================================================================================
void print_buffer_udata(uint32_t data)
{
    uint8_t  index = 0;

    do
    {
        data_part[index] = data % 10;
        data -= data_part[index];  // 负数在这里越减越大、导致出不了这个循环，所以上面要将其转化为正数
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
// print_buffer_data() 拆分int型整数数据的每一位到数据buffer
// 
// 参数：data       整数数据
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
// print_buffer_char() 组织字符数据到数据buffer
// 
// 参数：data       字符数据
// 
// ==========================================================================================================
void print_buffer_char(char data)
{
    print_data_in(data);
}

// ==========================================================================================================
// print_buffer_string() 组织字符串到数据buffer
// 
// 参数：data       字符串地址
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
// print_buffer_hex() 组织数据以十六进制格式解析到数据buffer
//
// 参数：data       数据
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
        data >>= 4;  // 必须是正数、才能保证这个循环能够退出(负数右移时最高位补1)
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
// 参数：fmt        格式控制字符串
// 
// 说明：
// (1). 支持格式：%c    字符
//                %s    字符串
//                %d    16位signed int
//                %l    32位signed long int
//                %u    16位unsigned int
//                %U    32位unsigned long int
//                %x    16位int的十六进制格式
//                %X    32位long int的十六进制格式
// (2). 空格不能在%和d/./x之间，错误举例：print("a = % d\n",a);
// (3). 不要在中断中或抢占式任务中使用这个print函数，因为它是不可重入的，会修改全局的buffer
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
                case 'u':print_buffer_udata((uint32_t)(va_arg(ap, int)) & 0x0000FFFF);  // 去掉符号位(负数int转化为无符号long的时候、高16位全是符号位FFFF)
                         break;
                case 'U':print_buffer_udata(va_arg(ap, long));
                         break;
                case 'x':print_buffer_hex(va_arg(ap, int));  // 保留高16位正好可以看到符号位
                         break;
                case 'X':print_buffer_hex(va_arg(ap, long));
                         break;
                default: va_arg(ap, int);  // 跳过这个参数、它的格式设置不对，结果就是这个参数的具体值没有被存下来
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
    // 触发数据发送(如果发送器正在发送，这个触发并不会影响正在发送的操作)
    if(TRUE == transfer)
    {
        Mod_USART_transfer(print_data_out);
    }
    va_end(ap);
}






























