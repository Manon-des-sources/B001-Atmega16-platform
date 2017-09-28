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
// ----------------------
// ���Ķ�����һЩ���õķ���
// 
// ----------------------
// ������
// 
// ==========================================================================================================
#include "general.h"




// ==========================================================================================================
// ���μ�1
// 
// ����: p             ���ݶ�дָ��
//       range         ��д������
// 
// ==========================================================================================================
void add_within_range(int *p, int range)
{
    if((*p + 1) >= range) *P  = 0;
    else                  *P += 1;
}


// ==========================================================================================================
// һ�������ֵ
// 
// ����: max           ��ǰ����ֵ 
//       bigger        �����ֵ 
// 
// ==========================================================================================================
void hold_max(int *max, int *bigger)
{
    if(*bigger > *max)  *max = *bigger;
}
