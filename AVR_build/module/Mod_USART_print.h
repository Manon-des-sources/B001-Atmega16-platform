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
#ifndef __MOD_USART_PRINT__
#define __MOD_USART_PRINT__



#include <stdarg.h>  // 包含可变参数函数需要的几个宏
#include "sys_config.h"
#include "Drv_Sys.h"
#include "Drv_USART.h"
#include "Mod_USART_manager.h"


// ==========================================================================================================
// print_data_out() 从缓冲区读出1个数据
// 
// 参数：*data      用于接收数据值
// 
// 返回值：Fin      TRUE = 数据读出成功、FALSE = buffer为空
// 
// ==========================================================================================================
bool print_data_out(uint8_t *data);

// ==========================================================================================================
// print()
// 
// 参数：fmt        格式控制字符串
// 
// 说明：
// (1). 支持格式：%c    char  字符
//                %s    char *字符串
//                %d    16位signed int
//                %l    32位signed long int
//                %u    16位unsigned int
//                %U    32位unsigned long int
//                %x    16位int的十六进制格式(负数会保留符号位)
//                %X    32位long int的十六进制格式
// (2). 空格不能在%和d/./x之间，错误举例：print("a = % d\n",a);
// (3). 不要在中断中或抢占式任务中使用这个print函数，因为它是不可重入的，会修改全局的buffer
// 
// ==========================================================================================================
void print(const char *fmt, ...);



#endif  // #ifndef __MOD_USART_PRINT__
