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
// --------------------
// 本文用来记录一些笔记
// 
// --------------------
// 包含：
// 
// (1). 编码风格
// (2). 一些定义的出处
// 
// ==========================================================================================================
#ifndef __SYS_NOTES_H__
#define __SYS_NOTES_H__



// ==========================================================================================================
// 一些编码风格
// 
// (0). 命名分为三类：
//      1、BSP驱动：    Drv_Timer.c
//      2、外设驱动：   task_Mod_LED_display.c
//      3、具体应用：   task_App_Tetris.c
//      4、通信协议类： task_Con_modbus
// 
// (1). system.c中只包含一个头文件：system.h，其余需要的头文件都放到system.h里面去包含。
//      其余C文件依次示例，main.c和config.h这一对除外。
// 
// (2). #endif的后面需要注释来自哪一个#if或#ifndef。
//      如system.h的示例：#endif // #ifndef __SYSTEM_H__
// 
//      #if后面写明后面的条件是定义在哪个文件，如：“#if USE_SEG_LED_DISPLAY  // #include "config.h"”
// 
// (3). 结构体不使用指针操作，如：event->data = sys_event_int8[index].data;
//      event是指针、所以是event->data、sys_event_int8是结构体、所以是sys_event_int8[index].data
//      不推荐使用(p_sys_event_int8 + index)->data的方式操作结构体sys_event_int8，除非不觉得多余
// 
// ----------------------------------------------------------------------------------------------------------


// ==========================================================================================================
// 一些定义的出处
// 
// _SFR_IO8()   定义于sfr_defs.h
// 
// 
// ----------------------------------------------------------------------------------------------------------



#endif	// #define __SYS_NOTES_H__
