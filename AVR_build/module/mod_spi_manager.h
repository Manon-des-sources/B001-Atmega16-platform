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
// -----------------------------------
// 本文定义了Atmega16下的SPI读写管理器
// 
// ==========================================================================================================
#ifndef __MOD_SPI_MANAGER_H__
#define __MOD_SPI_MANAGER_H__




#include <avr/interrupt.h>
#include "sys_config.h"
#include "Drv_SPI.h"
#include "Mod_SPI_Flash.h"



// 用户使用SPI的状态
typedef enum 
{
    SPI_MGR_STATE_WORK,  // 操作状态：用户的操作已完成(读写擦指令已经生效)
    SPI_MGR_STATE_DONE,  // 操作状态：用户的读写数据已经传输完毕
    SPI_MGR_STATE_BUSY,  // 操作状态：用户的操作进行中
    SPI_MGR_STATE_READY, // 操作状态：用户已准备就绪(等待执行)
    SPI_MGR_STATE_IDLE   // 操作状态：用户没有操作请求
}SPI_MGR_STATE;

// 用户申请的操作类型
typedef enum 
{
    SPI_MGR_ACT_READ_COM,   // 不带地址的读
    SPI_MGR_ACT_WRIRE_COM,  // 不带地址的写
    SPI_MGR_ACT_READ_ADDR,  // 带地址的读
    SPI_MGR_ACT_WRITE_ADDR, // 带地址的写
    SPI_MGR_ACT_EARASE_ADDR // 带地址的擦除
}SPI_MGR_ACT;


// SPI接口用户信息
typedef struct 
{
    uint8_t  action;    // 用户申请的操作的类型
    uint8_t  state;     // 用户使用SPI的状态
    uint8_t  command;   // 操作指令
    uint32_t addr;      // 操作地址
    uint16_t number;    // 操作数据量(字节)
    uint8_t *pBuffer;   // 缓存(内容 = 指令 + 地址 + 数据)
    uint8_t *pData;     // 数据在缓存中的位置
    uint16_t nData;     // 缓存中分配给数据的空间大小(即除去缓存中的指令和地址)
}T_SPI_MGR_USER, *pT_SPI_MGR_USER;

// SPI主从机数据交换过程控制
typedef struct 
{
    uint8_t  step;      // 操作步骤：用于控制SPI操作顺序
    uint8_t  end;       // 数据传输是否完成(完成：TRUE、未完成：FALSE)
    uint16_t count;     // 已经传输的数据量(字节)
    uint16_t number;    // 需要传输的数据量(字节)
    uint8_t *pBuffer;   // 执行需要传输的数据缓存
    uint8_t  user;      // 当前用户的编号

    uint8_t          suspend;  // 是否存在暂停擦写的操作
    pT_SPI_MGR_USER user_sus;  // 用于保存被暂停的用户的编号，以便执行需要读的用户
}T_SPI_MGR_CTRL;


void Mod_SPI_MGR_init(void);

uint8_t Mod_SPI_MGR_user_add(const pT_SPI_MGR_USER user, uint8_t *const pBuffer, const uint32_t size_buffer);
uint8_t Mod_SPI_MGR_user_index(pT_SPI_MGR_USER const user);
bool Mod_SPI_MGR_application(pT_SPI_MGR_USER const user, const uint8_t action, const uint8_t command, const uint32_t addr, const uint32_t size);

bool Mod_SPI_MGR_start(void);
bool Mod_SPI_MGR_transfer(void);
bool Mod_SPI_MGR_read_WIP(void);
bool Mod_SPI_MGR_write_WEL(void);
void Mod_SPI_MGR_shift(uint16_t number, uint8_t *const pBuffer);
void Mod_SPI_MGR_set_run(p_bool_funtion_void const fun);
void task_SPI_MGR(void);



#endif  // #ifndef __MOD_SPI_MANAGER_H__
