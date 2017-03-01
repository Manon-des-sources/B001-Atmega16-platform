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
#ifndef __MOD_USART_MANAGER_H__
#define __MOD_USART_MANAGER_H__



#include <avr/interrupt.h>
#include "sys_config.h"
#include "Drv_USART.h"
#include "Drv_IO_Port.h"
#include "Mod_USART_print.h"


void Mod_USART_init(void);
void Mod_USART_transfer(p_bool_funtion_uint8_t_p fun);
void task_Mode_USART(void);



#endif  // #ifndef __MOD_USART_MANAGER_H__
