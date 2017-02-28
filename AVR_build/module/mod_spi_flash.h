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
// ���Ķ�����Atmega16�µ�SPI Flash��д����
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



// ��GD25Q32Bָ�Ϊ��׼����
typedef enum 
{
    SPI_COMMAND_DUMMY         = 0x00,   // ��Ч���ݣ���������SCLK����

    SPI_COMMAND_WRITE_ENABLE  = 0x06,
    SPI_COMMAND_WRITE_DISABLE = 0x04,
    SPI_COMMAND_READ_STATUS_L = 0x05,   // ״̬�Ĵ����ĵ�8λ
    SPI_COMMAND_READ_STATUS_H = 0x35,   // ״̬�Ĵ����ĸ�8λ
    SPI_COMMAND_WRITE_STATUS  = 0x01,

    SPI_COMMAND_READ          = 0x03,   //  80MHzʱ��
//  SPI_COMMAND_READ_FAST     = 0x0B,   // 120MHzʱ��
    SPI_COMMAND_PROGRAM_PAGE  = 0x02,
    SPI_COMMAND_ERASE_SECTOR  = 0x20,
//  SPI_COMMAND_ERASE_BLOCK32 = 0x52,   // �����(32K)
    SPI_COMMAND_ERASE_BLOCK64 = 0xD8,   // �����(64K)
    SPI_COMMAND_ERASE_CHIP    = 0xC7,   // ��0x60

    SPI_COMMAND_SUSPEND        = 0x75,  // ���/�����ӳ�
    SPI_COMMAND_RESUME         = 0X7A,  // ���/�����ָ�

    SPI_COMMAND_READ_RDID      = 0x9F,  // �����̱�ʶ(GD25Q32B = 0xC84016)
//  SPI_COMMAND_READ_ID_MANU   = 0x90,  // ������ID������ID(GD25Q32B = 0xC815)(ͬʱҲ�ǻ���ָ��)
//  SPI_COMMAND_READ_ID        = 0xAB,  // ����ID(GD25Q32B = 15)(ͬʱҲ�ǻ���ָ����˳�������ģʽ��ָ��)
//  SPI_COMMAND_SLEEP          = 0XB9,  // ����

//  SPI_COMMAND_HP_MODE        = 0xA3,  // ������ģʽ
//  SPI_COMMAND_RESET_CON_READ = 0xFF,  // ������������λ
//  SPI_COMMAND_ERASE_SECURITY = 0x44,  // ����security�Ĵ���
//  SPI_COMMAND_WRITE_SECURITY = 0x42,  // д��security�Ĵ���
//  SPI_COMMAND_READ_SECURITY  = 0x48,  // ��ȡsecurity�Ĵ���

//  SPI_COMMAND_DUAL_READ_OUT = 0x3B,
//  SPI_COMMAND_DUAL_READ_IO  = 0xBB,

//  SPI_COMMAND_QUAD_READ_OUT  = 0x6B,
//  SPI_COMMAND_QUAD_READ_IO   = 0xEB,
//  SPI_COMMAND_QUAD_READ_WORD = 0xE7,
//  SPI_COMMAND_QUAD_PROGRAM_PAGE = 0x32,
}FLASH_COMMAND_SET;


// GD25Q32B��״̬�Ĵ������(��������оƬ����Ҫȷ�ϲ��޸���Щλ)
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

    MASK_PRO = CMP | BP4 | BP3 | BP2 | BP1 | BP0,   // д��ر���λ
//  AREA_PRO = CMP | BP2 | BP1,                     // ������2MB����Addr[00000000H,001FFFFFH] == Block[1,31]
    AREA_PRO = 0x0000,                              // ��д����
    RDID_GD25Q32B = 0xC84016,                       // ȷ�����SPI Flash���ͺ���GD25Q32B����������RDID
    SIZE_PAGE_GD25Q32B = 256                        // һҳ�Ĵ�СΪ256�ֽ�
}SR_GD25Q32B;

// ����дʹ��ָ�����Ҫ���WELλ�Ƿ��Ѿ���Ч
// ��ͬоƬ��״̬�Ĵ������ܲ�һ��(GD25Q32B/C��GD25Q16C��GD25Q80C��GD25Q128C��WELλ����һ����)
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

// ģ�����
void task_SPI_Flash(void);



#endif  // #ifndef __MOD_SPI_FLASH_H__
