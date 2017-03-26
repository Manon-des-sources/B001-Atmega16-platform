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
// 本文定义了Atmega16下的USART发送/接收数据管理器
//
// ----------------------------------------------
// 包含：
//
// ==========================================================================================================
#include "Mod_USART_manager.h"


typedef struct
{
    uint8_t  count;   // 发送器已经发送的数据量
    uint8_t  number;  // 总的数据量
    uint8_t *data;    // 数据buffer
    uint8_t  end;     // 发送完成状态
    p_bool_funtion_uint8_t_p fun;  // 执行用户指定的数据发送函数
}T_USART_transfer_CTRL;

T_USART_transfer_CTRL transfer_ctrl = { .count  = 0,
                                        .number = 0,
                                        .data   = NULL,
                                        .end    = TRUE,
                                        .fun    = NULL
                                       };

char buffer_usart[] = "s123456";

T_FLOAT_INT32_UNION value_check = { .float32 = 10.45 };


// ==========================================================================================================
// USART 管理器初始化
//
// 参数：mode           同步/异步模式(USART_MODE_ASYNC、USART_MODE_SYNC)
//       parity         奇偶校验(USART_PARITY_ODD、USART_PARITY_EVEN)
//       stop_bit       停止位的位数(USART_STOP_BIT_1、USART_STOP_BIT_2)
//       char_size      数据位宽(字符长度)(USART_CHAR_SIZE_9:USART_CHAR_SIZE_5)
//       baud_rate      12位的波特率
//
// 说明：
// (1). 默认使用9位数据位宽、第9位是校验位，不需要手动写入、启动奇偶校验即可由硬件自动写入
// (2). 写UCSRC时、需要将URSEL位写1
// (3). 写UBRRH时、需要将URSEL位清0
//
// ==========================================================================================================
void Mod_USART_init(void)
{
    // USART接口初始化
    Drv_USART_init(USART_PARITY_ODD, USART_CHAR_SIZE_8, 115200);

    // 发送器IO初始化
    Drv_IO_set_bit(PORTD,PD1);
    Drv_IO_mode_bit(DDRD,PD1,IO_OUTPUT);
}

// ==========================================================================================================
// USART 启动数据发送
//
// 参数：fun        数据读取函数
//
// 说明：
// (1).
//
// ==========================================================================================================
void Mod_USART_transfer(p_bool_funtion_uint8_t_p fun)
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

         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
unsigned long int a20[] = { 100 };


// ==========================================================================================================
// USART 测试模块
//
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 1;

    switch(step)
    {
        case 0: print("%U,%U,%U,%U,%U,%U,%U,%U,%U\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], a20[0]);
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
// USART 发送缓冲寄存器为空中断
//
// 说明：
// (0). UDRE初始化时就是1，所以只需要启动UDR中断，就可以直接激发这个中断，进入中断里面去写数据
// (1). 可以继续像T-UDR中写入下一个数据等待发送，这比在TXC中断中等待发送完毕才写数据要方便快捷
// (2). 必须通过写T-UDR来清0 UDRE标志位、否则退出中断后将继续产生新的中断，因此最后的数据写完后、就应禁止这个中断
// (3). 最后一个数据写完后，启动数据发送完成中断，去等待最后的发送完成
// (4). 最后不清除UDRE(写UDR清除)，让UDRE保持为1
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
// USART 发送完成中断
//
// 说明：
// (1). 数据发送任务已经结束、硬件会自动拉高总线(释放总线)
// (2). 在UDRE中断中检测数据是否写完，写完后设置数据完成标志，等到这里发送结束后，就可以确认所有数据发送结束
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
// USART 接收完成中断
//
// 说明：
// (1). 可以读数据了
//
// ==========================================================================================================
ISR(USART_RXC_vect)
{
}
