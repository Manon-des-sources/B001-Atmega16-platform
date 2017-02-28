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
// ----------------------------
// 本文定义了本项目的初始化模块
// 
// ==========================================================================================================
#ifndef __SYSTEM_H__
#define __SYSTEM_H__



#include <avr/io.h>
#include <avr/interrupt.h>

#include "Drv_Sys.h"
#include "Drv_Timer.h"
#include "Drv_IO_Port.h"

#include "Mod_IR.h"
#include "Mod_LED_Displayer.h"
#include "Mod_USART_manager.h"

#include "sys_timer.h"
#include "sys_event.h"
#include "sys_config.h"
#include "sys_version.h"

void sys_init(void);



#endif // #ifndef __SYSTEM_H__
