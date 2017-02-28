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
// ------------------------------------------------
// 上电10ms后才可以擦写GD25Q32B，所以上电后需要延时
// 
// ==========================================================================================================
#include "Mod_SPI_Flash.h"


#if USE_SPI_FLASH



volatile uint32_t temp2016;


static T_SPI_MGR_USER SPI_USER_Flash;

// 数据缓存(1字节指令 + 3字节地址 + 数据)
static unsigned char SPI_Flash_buffer[1 + 3 + SIZE_PAGE_GD25Q32B];

// ==========================================================================================================
// SPI Flash测试
// 
// ==========================================================================================================
void task_SPI_Flash(void)
{
}

// ==========================================================================================================
// SPI Flash初始化
// 
// 说明：
// (0). 对GD25Q32B进行了写保护操作，如果更换了其他芯片、需要确认其状态寄存器是否和这里的设置一致
// (1). GD25Q32B支持模式0和模式3
// 
// ==========================================================================================================
void Mod_SPI_Flash_init(void)
{
#if 1

    temp2016 = Mod_SPI_MGR_user_add(&SPI_USER_Flash, SPI_Flash_buffer, _countof(SPI_Flash_buffer));

#else

    uint32_t rdid;
    uint16_t status;

    // 读取芯片特征数据(型号相关)
    rdid = Mod_SPI_Flash_read_RDID();
    // 写保护设置(不同芯片的状态寄存器不一样，所以在写状态寄存器之前要确定芯片型号)
    if(RDID_GD25Q32B == rdid)
    {
        status = Mod_SPI_Flash_read_status();
        if((status & MASK_PRO) != AREA_PRO)
        {
            status &= ~MASK_PRO;
            status |= AREA_PRO;
            Mod_SPI_Flash_write_status(status);
            delay_ms(20);   // 耗时2-15ms
            status = Mod_SPI_Flash_read_status();
            if((status & MASK_PRO) == AREA_PRO)
            {
                // 写保护成功
            }
            else
            {
                // 写保护失败
            }
        }
    }

#endif
}
/*
// ==========================================================================================================
// 写使能(WEL位)(耗时700us左右)
// 
// 参数：enable     ENABLE  = 写使能
//                  DISABLE = 写禁止
// 
// 返回值：Fin      TRUE  = 写使能已经生效/写禁止已经生效
//                  FALSE = 写使能未生效/写禁止还没有生效
// 
// 说明：
// (1). 写使能是对写状态寄存器、擦除/编程存储器而言的，指令的写入不需要使能就可以写入
// (2). 写使能位WEL在 上电/写状态寄存器/页编程/扇区擦除/块擦除/整片擦除指令 后被复位
// (3). 连续10次使能都不起作用，就给出警告
// (4). 写使能并不会自己复位，需要手动禁止
// (5). 经测试：写1次ENABLE/DISABLE之后、立马读出SR，ENABLE/DISABLE就已经生效
//      写ENABLE/DISABLE指令、加上读出SR，耗时680us左右
//      加上其他代码的消耗、这个函数耗时应该比700us多一点儿
// (6). 从(5)我们可以知道、写使能之后、可以立马取消写使能
// 
// ==========================================================================================================
bool Mod_SPI_Flash_write_enable(const uint8_t enable)
{
    bool Fin = FALSE;

    uint8_t  command;
    uint8_t  index;
    uint16_t status = 0;

    command = (ENABLE == enable) ? SPI_COMMAND_WRITE_ENABLE : SPI_COMMAND_WRITE_DISABLE ;
    for(index = 0; index < 10; index++)
    {
        Drv_SPI_clr_SS();
        Drv_SPI_write_byte(command);
        Drv_SPI_set_SS();
        status = Mod_SPI_Flash_read_status();
        command = (ENABLE == enable) ? _WEL_ : 0 ;
        if(command == (status & _WEL_))
        {
            Fin = TRUE;
            break;
        }
    }
    return Fin;
}

// ==========================================================================================================
// 读RDID(GD25Q32B的RDID是一个24bit数值)
// 
// ==========================================================================================================
uint32_t Mod_SPI_Flash_read_RDID(void)
{
    uint8_t  index;
    uint8_t  temp;
    uint32_t data = 0;

    Drv_SPI_clr_SS();
    Drv_SPI_write_byte(SPI_COMMAND_READ_RDID);
    for(index = 0; index < 3; index++)
    {
        temp = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);
        data = (data << 8) | temp;
    }
    Drv_SPI_set_SS();

    return data;
}

// ==========================================================================================================
// 读状态寄存器
// 
// 说明：
// (1). 在擦除、写入数据，或写状态寄存器这几个指令的执行过程中，都可以读状态寄存器
// 
// ==========================================================================================================
uint16_t Mod_SPI_Flash_read_status(void)
{
    uint8_t  temp;
    uint16_t data;

    // SR的低8位
    Drv_SPI_clr_SS();
    Drv_SPI_write_byte(SPI_COMMAND_READ_STATUS_L);// 读SR低8位的指令(并等待数据移出完毕)
    data = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);  // 再次发出SCLK、以交换数据
    Drv_SPI_set_SS();

    // SR的高8位
    Drv_SPI_clr_SS();
    Drv_SPI_write_byte(SPI_COMMAND_READ_STATUS_H);
    temp = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);
    Drv_SPI_set_SS();

    data |= temp << 8;

    return data;
}

// ==========================================================================================================
// 写状态寄存器(耗时2-15ms)
// 
// 说明：
// (1). 不同芯片的状态寄存器不一样，所以在写状态寄存器之前要确定芯片型号
// 
// ==========================================================================================================
void Mod_SPI_Flash_write_status(const uint16_t data)
{
    bool temp;

    temp = Mod_SPI_Flash_write_enable(ENABLE);
    if(TRUE == temp)
    {
        Drv_SPI_clr_SS();
        Drv_SPI_write_byte(SPI_COMMAND_WRITE_STATUS);
        Drv_SPI_write_byte(data & 0x00FF);
        Drv_SPI_write_byte(data >> 8);
        Drv_SPI_set_SS();
//      delay_ms(20);   // (耗时2-15ms)之后才可以取消写使能，不然写不进去
//      Mod_SPI_Flash_write_enable(DISABLE);
    }
}

// ==========================================================================================================
// 读数据
// 
// 参数：  pBuffer    数据读取到pBuffer指向的内存
//         addr       数据的地址
//         size       数据的数量
// 
// 返回值：Fin        TRUE  = 读取成功
//                    FALSE = 读取失败(器件处于忙状态)
// 
// 说明：
// (1). 读取前检查Flash忙标志WIP
// (2). SPI_COMMAND_READ(80MHz时钟)和SPI_COMMAND_READ_FAST(120MHz时钟)读取128字节耗时相同、为18.4ms @ 8MHz/DIV=128=62.5KHz
//      可能是因为62.5KHz对于GD25Q32B的时钟来说太慢了
//      所以这里使用普通模式
// 
// ==========================================================================================================
bool Mod_SPI_Flash_read_bytes(uint8_t *const pBuffer, const uint32_t addr, const uint32_t size)
{
    bool Fin = FALSE;
    uint32_t index;
    uint16_t status;

    status = Mod_SPI_Flash_read_status();
    if(0 == (status & _WIP_))
    {
        if((size > 0) && (NULL != pBuffer))
        {
            Fin = TRUE;
            Drv_SPI_clr_SS();
            Drv_SPI_write_byte(SPI_COMMAND_READ);
//          Drv_SPI_write_byte(SPI_COMMAND_READ_FAST);
            Drv_SPI_write_byte((addr >> 16) & 0x000000FF);
            Drv_SPI_write_byte((addr >>  8) & 0x000000FF);
            Drv_SPI_write_byte((addr >>  0) & 0x000000FF);
//          Drv_SPI_write_byte(SPI_COMMAND_DUMMY);  // SPI_COMMAND_READ_FAST模式下、主机要多发一个dummy字节，发送过程中GD25Q32不发出数据
            for(index = 0; index < size; index++)
            {
                pBuffer[index] = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);
            }
            Drv_SPI_set_SS();
        }
    }
    return Fin;
}

// ==========================================================================================================
// 写一页数据到SPI Flash
// 
// 参数：  pBuffer    数据位于pBuffer指向的内存
//         addr       页的首地址(一般是某一页的首地址，否则出现卷烧现象)
// 
// 返回值：Fin        TRUE  = 写入完成、不一定成功
//                    FALSE = 写入失败(WIP忙、写使能失败、或该区域被写保护)
// 
// 说明：
// (1). 这里在写入前检查Flash忙标志WIP、以及写使能(WEL)
//      写入的数据是被写入到GD25Q32B的一个大小为256字节的页缓存
//      在最后一个SCLK脉冲结束后、拉高CS#、才执行Page-Program指令、将缓存里面的数据写入Flash，
//      写入Flash的这个过程中WIP=1、耗时约0.4-2.4ms
//      而将数据写入缓存的过程、与WIP无关。
// 
// (2). 地址addr不是某一页的首地址：如果addr不是某一页的首地址、而是该页首地址和该页最后一个地址之间的数值，
//      那么数据将从addr处开始写入、超过该页最后一个地址后、会返回该页的首地址继续写入(页缓存只有256字节，这是卷烧(烧录)
// 
// (3). 写入的数据量：写入的数据量在这里被设置为一页的大小，如果超过一页的大小、那么将出现(2)中所描述的卷烧，
//      直到最后一个字节被写入，结果就是、pBuffer中的最后256字节被写入该页
// 
// ==========================================================================================================
bool Mod_SPI_Flash_write_page(uint8_t *const pBuffer, const uint32_t addr)
{
    bool Fin = FALSE;
    bool temp;
    uint16_t index; // 一页不超过65536字节
    uint16_t status;

    status = Mod_SPI_Flash_read_status();
    if(0 == (status & _WIP_))
    {
        temp = Mod_SPI_Flash_write_enable(ENABLE);
        if((TRUE == temp) && (NULL != pBuffer))
        {
            Fin = TRUE;
            Drv_SPI_clr_SS();
            Drv_SPI_write_byte(SPI_COMMAND_PROGRAM_PAGE);
            Drv_SPI_write_byte((addr >> 16) & 0x000000FF);
            Drv_SPI_write_byte((addr >>  8) & 0x000000FF);
            Drv_SPI_write_byte((addr >>  0) & 0x000000FF);
            for(index = 0; index < SIZE_PAGE_GD25Q32B; index++)
            {
                Drv_SPI_write_byte(pBuffer[index]);
            }
            Drv_SPI_set_SS();
        }
    }
    return Fin;
}

// ==========================================================================================================
// 擦除
// 
// 参数：  type       支持三种擦除：扇区擦除(4K)[实参 = SPI_COMMAND_ERASE_SECTOR](GD25Q32B的扇区擦除耗时=70-500ms)
//                                  块擦除(64K)[实参 = SPI_COMMAND_ERASE_BLOCK64](GD25Q32B的64K块擦除耗时=400-800ms)
//                                  整片擦除[实参 = SPI_COMMAND_ERASE_CHIP](GD25Q32B的整片擦除耗时=20-40s)
//         addr       需要擦除的扇区/块内的地址即可、不要求是该扇区/块内的首地址
// 
// 返回值：Fin        TRUE  = 擦除指令已发送
//                    FALSE = 写入失败(WIP忙、写使能失败、或该区域被写保护)
// 
// 说明：
// (1). 这里在擦除前检查Flash忙标志WIP、以及写使能(WEL)
// (2). 擦除指令在拉高CS#后被执行，过程中WIP=1
// (3). 连续擦除的时间间隔一定要长、连续擦除全部64个block时、延时500ms、也发现有一个block未擦除成功，后来改成了延时1000ms才行
//      但有时200ms也会擦除成功。
// 
// ==========================================================================================================
bool Mod_SPI_Flash_erase(const uint8_t type, const uint32_t addr)
{
    bool Fin = FALSE;
    bool temp;
    uint16_t status;

    status = Mod_SPI_Flash_read_status();
    if(0 == (status & _WIP_))
    {
        temp = Mod_SPI_Flash_write_enable(ENABLE);
        if(TRUE == temp)
        {
            Fin = TRUE;
            Drv_SPI_clr_SS();
            switch(type)
            {
                case SPI_COMMAND_ERASE_SECTOR  : Drv_SPI_write_byte(SPI_COMMAND_ERASE_SECTOR);
                                                 Drv_SPI_write_byte((addr >> 16) & 0x000000FF);
                                                 Drv_SPI_write_byte((addr >>  8) & 0x000000FF);
                                                 Drv_SPI_write_byte((addr >>  0) & 0x000000FF);
                                                 break;
                case SPI_COMMAND_ERASE_BLOCK64 : Drv_SPI_write_byte(SPI_COMMAND_ERASE_BLOCK64);
                                                 Drv_SPI_write_byte((addr >> 16) & 0x000000FF);
                                                 Drv_SPI_write_byte((addr >>  8) & 0x000000FF);
                                                 Drv_SPI_write_byte((addr >>  0) & 0x000000FF);
                                                 break;
                case SPI_COMMAND_ERASE_CHIP    : Drv_SPI_write_byte(SPI_COMMAND_ERASE_CHIP);
                                                 break;
                default : break;
            }
            Drv_SPI_set_SS();
        }
    }
    return Fin;
}

// ==========================================================================================================
// 编程/擦除延迟
// 
// 参数：             SPI_COMMAND_SUSPEND(2us后生效)
//                    SPI_COMMAND_RESUME(200ns后生效)
// 
// 返回值：Fin        TRUE  = 指令已生效
//                    FALSE = 指令未生效
// 
// 说明：
// (1). SPI_COMMAND_SUSPEND用于在页编程和扇区/块擦除指令过程中、暂停指令的执行，并允许去读取其他扇区/块数据
//      暂停后、写SR寄存器、擦除security寄存器、所有擦除指令和页编程指令都不被允许(无效？还是不能有这些操作)
// (2). SPI_COMMAND_SUSPEND指令只在页编程和扇区/块擦除指令过程中有效(SR.WIP=1 && SR.SUS=0)
//      指令生效后SR.WIP=0 && SR.SUS=1
// (3). SPI_COMMAND_RESUME指令只在SR.WIP=0 && SR.SUS=1时有效
//      指令生效后SR.WIP=1 && SR.SUS=0
// 
// ==========================================================================================================
bool Mod_SPI_Flash_suspend(const uint8_t type)
{
    bool Fin = FALSE;
    uint16_t status;
    uint16_t bit_wip;   // 16bit的SR寄存器
    uint16_t bit_sus;   // 16bit的SR寄存器

    if((SPI_COMMAND_SUSPEND == type) || (SPI_COMMAND_RESUME == type))
    {
        switch(type)
        {
            case SPI_COMMAND_SUSPEND: bit_wip = _WIP_; bit_sus = 0;     break;
            case SPI_COMMAND_RESUME : bit_wip = 0;     bit_sus = _SUS_; break;
        }
        // 发送指令
        status = Mod_SPI_Flash_read_status();
        if((bit_wip == (status & _WIP_)) && (bit_sus == (status & _SUS_)))
        {
            Drv_SPI_clr_SS();
            Drv_SPI_write_byte(type);
            Drv_SPI_set_SS();
            // 检查指令是否生效
            bit_wip = (_WIP_ == bit_wip) ? 0 : _WIP_;
            bit_sus = (_SUS_ == bit_sus) ? 0 : _SUS_;
            status = Mod_SPI_Flash_read_status();   // 读取一次的时间需要 > 2us、不然指令可能还没生效
            if((bit_wip == (status & _WIP_)) && (bit_sus == (status & _SUS_)))
            {
                Fin = TRUE;
            }
        }
    }
    return Fin;
}
*/
// ==========================================================================================================
// 进入休眠
// 
// 返回值：Fin        TRUE  = 休眠指令已发出
//                    FALSE = 进入休眠失败(WIP忙)
// 
// 说明：
// (1). 这里在休眠前需要检查Flash忙标志WIP
// (2). 休眠后将忽略写入、编程、擦除指令，只响应唤醒指令(平时CS#拉高被禁止后、仅处于待机模式)
//      休眠指令发出后、会在0.1us后正式进入休眠
// (3). 进入休眠和唤醒的两个函数还未测试，有需要低功耗的应用场合、有了PCB再进行休眠前后的电流比较测试
// 
// ==========================================================================================================
// bool Mod_SPI_Flash_enter_sleep(void)
// {
//     bool Fin = FALSE;
//     uint16_t status;
// 
//     status = Mod_SPI_Flash_read_status();
//     if(0 == (status & _WIP_))
//     {
//         Fin = TRUE;
//         Drv_SPI_clr_SS();
//         Drv_SPI_write_byte(SPI_COMMAND_SLEEP);
//         Drv_SPI_set_SS();
//     }
//     return Fin;
// }
// 
// ==========================================================================================================
// 从休眠或高性能模式退出
// 
// 返回值：Fin        TRUE  = 指令已发出
//                    FALSE = WIP忙、退出失败
// 
// 说明：
// (1). 这里在休眠前需要检查Flash忙标志WIP
// (2). 可以唤醒器件、或从高性能模式退出
//      指令发出后、会在0.1us后完成唤醒
// (3). 想要读取器件ID，需要唤醒后、再SPI_COMMAND_READ_ID指令后面跟3个dummy数据、然后才能读到器件ID
// 
// ==========================================================================================================
// bool Mod_SPI_Flash_release(void)
// {
//     bool Fin = FALSE;
//     uint16_t status;
// 
//     status = Mod_SPI_Flash_read_status();
//     if(0 == (status & _WIP_))
//     {
//         Fin = TRUE;
//         Drv_SPI_clr_SS();
//         Drv_SPI_write_byte(SPI_COMMAND_READ_ID);
//         Drv_SPI_set_SS();
//     }
//     return Fin;
// }
// 




#else   // #ifdef USE_SPI_FLASH



void Mod_SPI_Flash_init(void)
{
}
void task_SPI_Flash(void)
{
}



#endif  // #ifdef USE_SPI_FLASH
