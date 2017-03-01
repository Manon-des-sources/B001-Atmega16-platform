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
#ifndef __MOD_USART_PRINT__
#define __MOD_USART_PRINT__



#include <stdarg.h>  // �����ɱ����������Ҫ�ļ�����
#include "sys_config.h"
#include "Drv_Sys.h"
#include "Drv_USART.h"
#include "Mod_USART_manager.h"


// ==========================================================================================================
// print_data_out() �ӻ���������1������
// 
// ������*data      ���ڽ�������ֵ
// 
// ����ֵ��Fin      TRUE = ���ݶ����ɹ���FALSE = bufferΪ��
// 
// ==========================================================================================================
bool print_data_out(uint8_t *data);

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
void print(const char *fmt, ...);



#endif  // #ifndef __MOD_USART_PRINT__
