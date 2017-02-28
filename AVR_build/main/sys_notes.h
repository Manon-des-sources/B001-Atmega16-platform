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
// ����������¼һЩ�ʼ�
// 
// --------------------
// ������
// 
// (1). ������
// (2). һЩ����ĳ���
// 
// ==========================================================================================================
#ifndef __SYS_NOTES_H__
#define __SYS_NOTES_H__



// ==========================================================================================================
// һЩ������
// 
// (0). ������Ϊ���ࣺ
//      1��BSP������    Drv_Timer.c
//      2������������   task_Mod_LED_display.c
//      3������Ӧ�ã�   task_App_Tetris.c
//      4��ͨ��Э���ࣺ task_Con_modbus
// 
// (1). system.c��ֻ����һ��ͷ�ļ���system.h��������Ҫ��ͷ�ļ����ŵ�system.h����ȥ������
//      ����C�ļ�����ʾ����main.c��config.h��һ�Գ��⡣
// 
// (2). #endif�ĺ�����Ҫע��������һ��#if��#ifndef��
//      ��system.h��ʾ����#endif // #ifndef __SYSTEM_H__
// 
//      #if����д������������Ƕ������ĸ��ļ����磺��#if USE_SEG_LED_DISPLAY  // #include "config.h"��
// 
// (3). �ṹ�岻ʹ��ָ��������磺event->data = sys_event_int8[index].data;
//      event��ָ�롢������event->data��sys_event_int8�ǽṹ�塢������sys_event_int8[index].data
//      ���Ƽ�ʹ��(p_sys_event_int8 + index)->data�ķ�ʽ�����ṹ��sys_event_int8�����ǲ����ö���
// 
// ----------------------------------------------------------------------------------------------------------


// ==========================================================================================================
// һЩ����ĳ���
// 
// _SFR_IO8()   ������sfr_defs.h
// 
// 
// ----------------------------------------------------------------------------------------------------------



#endif	// #define __SYS_NOTES_H__
