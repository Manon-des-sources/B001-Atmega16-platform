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
// -----------------------
// ���Ķ�����buffer������
// 
// -----------------------
// ������
// 
// ==========================================================================================================
#include "general.h"

// ���ݶ���
typedef struct 
{
    unsigned char *data;  // ָ�����ݶ���Ļ���
    int size;             // ����buffer�Ĵ�С����size = sizeof(buffer.data)

    int write;  // дָ��
    int read;   // ��ָ��
    int num;    // ��д����ֽ�����buffer����ֹͣ���� 

    int max;    // ��д����ֽ�����buffer����Ȼ����   
    int over;   // ������
}T_BUFFER_MGR, *pT_BUFFER_MGR;


// ==========================================================================================================
// ����buffer������
// 
// ����ֵֵ��Fin     =0��û�����
//                  >0���������˶����ֽ�
// 
// ==========================================================================================================
int buffer_check_overflow(pT_BUFFER_MGR buffer)
{
    return buffer->overflow;
}

// ==========================================================================================================
// ��bufferд��1�����ݡ�buffer��ʱ������ͷ������д
// 
// ������buffer    ��dataд��buffer��������ݻ�������
//      data       ����
// 
// ����ֵ��         TRUE = ����д��ɹ���FALSE = buffer����
// 
// ==========================================================================================================
bool buffer_append(pT_BUFFER_MGR buffer, unsigned char data)
{
    int temp;

    if(buffer->num < buffer->size)
    {
        buffer->data[buffer->write] = data;
        add_within_range(&buffer->write�� buffer->size);
        buffer->num++;
        return TRUE;
    }
    buffer->max++;
    hold_max(&buffer->over, &buffer->max);
    return FALSE;
}

// ==========================================================================================================
// print_data_out() �ӻ���������1������
// 
// ������buffer     ��buffer��������ݻ�������ȡһ����ֵ
//      *data      ȡ������ֵ�ŵ�data����
// 
// ����ֵ��Fin      TRUE = ���ݶ����ɹ���FALSE = bufferΪ��
// 
// ==========================================================================================================
bool buffer_pop(pT_BUFFER_MGR buffer, unsigned char *data)
{
    bool Fin = FALSE;
    if(buffer->num > 0)
    {
       *data = buffer->data[buffer->read];
        add_within_range(&buffer->read, buffer->size);
        buffer->num--;
        buffer->max = 0;
        Fin = TRUE;
    }
    return Fin;
}
