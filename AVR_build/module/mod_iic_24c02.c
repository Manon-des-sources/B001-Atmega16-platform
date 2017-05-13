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
// -----------------------------------------------
// 本文定义了Atmega16下的24C02操作接口和操作状态机
// 
// ==========================================================================================================
#include "Mod_IIC_24C02.h"


// ==========================================================================================================
// 24C02操作状态机：
// 
// 读操作一个状态函数列表
// 写操作一个状态函数列表
// 
// 执行时只需要将状态函数列表遍历一遍即可
// 需要新的操作时、在建立一个对应的状态函数列表即可
// 
// ==========================================================================================================

typedef struct 
{
    uint8_t index;               // 状态函数遍历时使用
    uint8_t number;              // 状态函数列表的长度
    p_void_funtion_void p_list;  // 执行状态函数列表中的一个元素
} T_MOD_IIC_24C02;


// 读操作的状态函数列表
static p_void_funtion_void read_list[] = 
{
    Drv_IIC_start,
    Drv_IIC_stop,
};

// 写操作的状态函数列表
static p_void_funtion_void write_list[] = 
{
    Drv_IIC_start,
    Drv_IIC_stop,
};
