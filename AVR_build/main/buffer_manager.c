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
// 本文定义了buffer管理方法
// 
// -----------------------
// 包含：
// 
// ==========================================================================================================
#include "general.h"

// 数据对象
typedef struct 
{
    unsigned char *data;  // 指向数据对象的缓存
    int size;             // 数据buffer的大小，即size = sizeof(buffer.data)

    int write;  // 写指针
    int read;   // 读指针
    int num;    // 已写入的字节数、buffer满则停止递增 

    int max;    // 已写入的字节数、buffer满仍然递增   
    int over;   // 溢出检测
}T_BUFFER_MGR, *pT_BUFFER_MGR;


// ==========================================================================================================
// 数据buffer溢出检测
// 
// 返回值值：Fin     =0：没有溢出
//                  >0：最多溢出了多少字节
// 
// ==========================================================================================================
int buffer_check_overflow(pT_BUFFER_MGR buffer)
{
    return buffer->overflow;
}

// ==========================================================================================================
// 向buffer写入1个数据、buffer满时不会卷回头部覆盖写
// 
// 参数：buffer    将data写到buffer对象的数据缓存里面
//      data       数据
// 
// 返回值：         TRUE = 数据写入成功、FALSE = buffer已满
// 
// ==========================================================================================================
bool buffer_append(pT_BUFFER_MGR buffer, unsigned char data)
{
    int temp;

    if(buffer->num < buffer->size)
    {
        buffer->data[buffer->write] = data;
        add_within_range(&buffer->write， buffer->size);
        buffer->num++;
        return TRUE;
    }
    buffer->max++;
    hold_max(&buffer->over, &buffer->max);
    return FALSE;
}

// ==========================================================================================================
// print_data_out() 从缓冲区读出1个数据
// 
// 参数：buffer     从buffer对象的数据缓存里面取一个数值
//      *data      取出的数值放到data里面
// 
// 返回值：Fin      TRUE = 数据读出成功、FALSE = buffer为空
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
