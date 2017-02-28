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
// 本文定义了Atmega16下的SPI Flash读写驱动
// 
// ==========================================================================================================
#ifndef __MOD_SPI_FLASH_H__
#define __MOD_SPI_FLASH_H__




#include <avr/io.h>
#include <avr/interrupt.h>

#include "sys_config.h"
#include "Drv_SPI.h"
#include "Mod_SPI_manager.h"



#if USE_SPI_FLASH



// 以GD25Q32B指令集为基准建立
typedef enum 
{
    SPI_COMMAND_DUMMY         = 0x00,   // 无效数据，用于启动SCLK发送

    SPI_COMMAND_WRITE_ENABLE  = 0x06,
    SPI_COMMAND_WRITE_DISABLE = 0x04,
    SPI_COMMAND_READ_STATUS_L = 0x05,   // 状态寄存器的低8位
    SPI_COMMAND_READ_STATUS_H = 0x35,   // 状态寄存器的高8位
    SPI_COMMAND_WRITE_STATUS  = 0x01,

    SPI_COMMAND_READ          = 0x03,   //  80MHz时钟
//  SPI_COMMAND_READ_FAST     = 0x0B,   // 120MHz时钟
    SPI_COMMAND_PROGRAM_PAGE  = 0x02,
    SPI_COMMAND_ERASE_SECTOR  = 0x20,
//  SPI_COMMAND_ERASE_BLOCK32 = 0x52,   // 块擦除(32K)
    SPI_COMMAND_ERASE_BLOCK64 = 0xD8,   // 块擦除(64K)
    SPI_COMMAND_ERASE_CHIP    = 0xC7,   // 或0x60

    SPI_COMMAND_SUSPEND        = 0x75,  // 编程/擦除延迟
    SPI_COMMAND_RESUME         = 0X7A,  // 编程/擦除恢复

    SPI_COMMAND_READ_RDID      = 0x9F,  // 生产商标识(GD25Q32B = 0xC84016)
//  SPI_COMMAND_READ_ID_MANU   = 0x90,  // 生产商ID和器件ID(GD25Q32B = 0xC815)(同时也是唤醒指令)
//  SPI_COMMAND_READ_ID        = 0xAB,  // 器件ID(GD25Q32B = 15)(同时也是唤醒指令、和退出高性能模式的指令)
//  SPI_COMMAND_SLEEP          = 0XB9,  // 休眠

//  SPI_COMMAND_HP_MODE        = 0xA3,  // 高性能模式
//  SPI_COMMAND_RESET_CON_READ = 0xFF,  // 连续读操作复位
//  SPI_COMMAND_ERASE_SECURITY = 0x44,  // 擦除security寄存器
//  SPI_COMMAND_WRITE_SECURITY = 0x42,  // 写入security寄存器
//  SPI_COMMAND_READ_SECURITY  = 0x48,  // 读取security寄存器

//  SPI_COMMAND_DUAL_READ_OUT = 0x3B,
//  SPI_COMMAND_DUAL_READ_IO  = 0xBB,

//  SPI_COMMAND_QUAD_READ_OUT  = 0x6B,
//  SPI_COMMAND_QUAD_READ_IO   = 0xEB,
//  SPI_COMMAND_QUAD_READ_WORD = 0xE7,
//  SPI_COMMAND_QUAD_PROGRAM_PAGE = 0x32,
}FLASH_COMMAND_SET;


// GD25Q32B的状态寄存器相关(更换其他芯片后、需要确认并修改这些位)
typedef enum 
{
    SUS_GD25Q32B = (1 << 15),
    CMP = (1 << 14),
    BP4 = (1 <<  6),
    BP3 = (1 <<  5),
    BP2 = (1 <<  4),
    BP1 = (1 <<  3),
    BP0 = (1 <<  2),
    WEL_GD25Q32B = (1 << 1),
    WIP_GD25Q32B = (1 << 0),

    MASK_PRO = CMP | BP4 | BP3 | BP2 | BP1 | BP0,   // 写相关保护位
//  AREA_PRO = CMP | BP2 | BP1,                     // 保护低2MB区域：Addr[00000000H,001FFFFFH] == Block[1,31]
    AREA_PRO = 0x0000,                              // 不写保护
    RDID_GD25Q32B = 0xC84016,                       // 确认这个SPI Flash的型号是GD25Q32B的特征数据RDID
    SIZE_PAGE_GD25Q32B = 256                        // 一页的大小为256字节
}SR_GD25Q32B;

// 发送写使能指令后、需要检查WEL位是否已经生效
// 不同芯片的状态寄存器可能不一样(GD25Q32B/C、GD25Q16C、GD25Q80C、GD25Q128C的WEL位都是一样的)
#define _WEL_   WEL_GD25Q32B
#define _WIP_   WIP_GD25Q32B
#define _SUS_   SUS_GD25Q32B



#endif  // #if USE_SPI_FLASH


void Mod_SPI_Flash_init(void);
bool Mod_SPI_Flash_write_enable(const uint8_t enable);
void Mod_SPI_Flash_write_status(const uint16_t data);
uint16_t Mod_SPI_Flash_read_status(void);
uint32_t Mod_SPI_Flash_read_RDID(void);

bool Mod_SPI_Flash_read_bytes(uint8_t *const pBuffer, const uint32_t addr, const uint32_t size);
bool Mod_SPI_Flash_write_page(uint8_t *const pBuffer, const uint32_t addr);
bool Mod_SPI_Flash_erase(const uint8_t type, const uint32_t addr);
bool Mod_SPI_Flash_suspend(const uint8_t type);

// 模块测试
void task_SPI_Flash(void);



#endif  // #ifndef __MOD_SPI_FLASH_H__
