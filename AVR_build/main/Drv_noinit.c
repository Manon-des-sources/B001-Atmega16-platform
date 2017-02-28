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
// ---------------------------------------
// ���Ķ�����Atmega16��.noinit����Ӧ��ʾ��
// 
// ---------------------------------------
// ������
// ��.noinit�ζ���һ��v_noinit,оƬ��λ��,���������ֵ���ᱻ��0��
// ʹ��������������Ը�λ
// ������Կ��Ź���λ
// 
// ==========================================================================================================
#include "Drv_noinit.h"


#if USE_NOINIT  // #include "config.h"


volatile uint8_t v_noinit __attribute__ ((section(".noinit")));

// ==========================================================================================================
// ͨ���۲�PORTB�ϵ�8λLED��״̬��֪��λ����
// ==========================================================================================================
uint8_t _noinit_using(void)
{
    return ++v_noinit;
}

#else


uint8_t _noinit_using(void)
{
}


#endif
