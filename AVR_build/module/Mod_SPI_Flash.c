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
// ------------------------------------------------
// �ϵ�10ms��ſ��Բ�дGD25Q32B�������ϵ����Ҫ��ʱ
// 
// ==========================================================================================================
#include "Mod_SPI_Flash.h"


#if USE_SPI_FLASH



volatile uint32_t temp2016;


static T_SPI_MGR_USER SPI_USER_Flash;

// ���ݻ���(1�ֽ�ָ�� + 3�ֽڵ�ַ + ����)
static unsigned char SPI_Flash_buffer[1 + 3 + SIZE_PAGE_GD25Q32B];

// ==========================================================================================================
// SPI Flash����
// 
// ==========================================================================================================
void task_SPI_Flash(void)
{
}

// ==========================================================================================================
// SPI Flash��ʼ��
// 
// ˵����
// (0). ��GD25Q32B������д�����������������������оƬ����Ҫȷ����״̬�Ĵ����Ƿ�����������һ��
// (1). GD25Q32B֧��ģʽ0��ģʽ3
// 
// ==========================================================================================================
void Mod_SPI_Flash_init(void)
{
#if 1

    temp2016 = Mod_SPI_MGR_user_add(&SPI_USER_Flash, SPI_Flash_buffer, _countof(SPI_Flash_buffer));

#else

    uint32_t rdid;
    uint16_t status;

    // ��ȡоƬ��������(�ͺ����)
    rdid = Mod_SPI_Flash_read_RDID();
    // д��������(��ͬоƬ��״̬�Ĵ�����һ����������д״̬�Ĵ���֮ǰҪȷ��оƬ�ͺ�)
    if(RDID_GD25Q32B == rdid)
    {
        status = Mod_SPI_Flash_read_status();
        if((status & MASK_PRO) != AREA_PRO)
        {
            status &= ~MASK_PRO;
            status |= AREA_PRO;
            Mod_SPI_Flash_write_status(status);
            delay_ms(20);   // ��ʱ2-15ms
            status = Mod_SPI_Flash_read_status();
            if((status & MASK_PRO) == AREA_PRO)
            {
                // д�����ɹ�
            }
            else
            {
                // д����ʧ��
            }
        }
    }

#endif
}
/*
// ==========================================================================================================
// дʹ��(WELλ)(��ʱ700us����)
// 
// ������enable     ENABLE  = дʹ��
//                  DISABLE = д��ֹ
// 
// ����ֵ��Fin      TRUE  = дʹ���Ѿ���Ч/д��ֹ�Ѿ���Ч
//                  FALSE = дʹ��δ��Ч/д��ֹ��û����Ч
// 
// ˵����
// (1). дʹ���Ƕ�д״̬�Ĵ���������/��̴洢�����Եģ�ָ���д�벻��Ҫʹ�ܾͿ���д��
// (2). дʹ��λWEL�� �ϵ�/д״̬�Ĵ���/ҳ���/��������/�����/��Ƭ����ָ�� �󱻸�λ
// (3). ����10��ʹ�ܶ��������ã��͸�������
// (4). дʹ�ܲ������Լ���λ����Ҫ�ֶ���ֹ
// (5). �����ԣ�д1��ENABLE/DISABLE֮���������SR��ENABLE/DISABLE���Ѿ���Ч
//      дENABLE/DISABLEָ����϶���SR����ʱ680us����
//      ����������������ġ����������ʱӦ�ñ�700us��һ���
// (6). ��(5)���ǿ���֪����дʹ��֮�󡢿�������ȡ��дʹ��
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
// ��RDID(GD25Q32B��RDID��һ��24bit��ֵ)
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
// ��״̬�Ĵ���
// 
// ˵����
// (1). �ڲ�����д�����ݣ���д״̬�Ĵ����⼸��ָ���ִ�й����У������Զ�״̬�Ĵ���
// 
// ==========================================================================================================
uint16_t Mod_SPI_Flash_read_status(void)
{
    uint8_t  temp;
    uint16_t data;

    // SR�ĵ�8λ
    Drv_SPI_clr_SS();
    Drv_SPI_write_byte(SPI_COMMAND_READ_STATUS_L);// ��SR��8λ��ָ��(���ȴ������Ƴ����)
    data = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);  // �ٴη���SCLK���Խ�������
    Drv_SPI_set_SS();

    // SR�ĸ�8λ
    Drv_SPI_clr_SS();
    Drv_SPI_write_byte(SPI_COMMAND_READ_STATUS_H);
    temp = Drv_SPI_read_byte(SPI_COMMAND_DUMMY);
    Drv_SPI_set_SS();

    data |= temp << 8;

    return data;
}

// ==========================================================================================================
// д״̬�Ĵ���(��ʱ2-15ms)
// 
// ˵����
// (1). ��ͬоƬ��״̬�Ĵ�����һ����������д״̬�Ĵ���֮ǰҪȷ��оƬ�ͺ�
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
//      delay_ms(20);   // (��ʱ2-15ms)֮��ſ���ȡ��дʹ�ܣ���Ȼд����ȥ
//      Mod_SPI_Flash_write_enable(DISABLE);
    }
}

// ==========================================================================================================
// ������
// 
// ������  pBuffer    ���ݶ�ȡ��pBufferָ����ڴ�
//         addr       ���ݵĵ�ַ
//         size       ���ݵ�����
// 
// ����ֵ��Fin        TRUE  = ��ȡ�ɹ�
//                    FALSE = ��ȡʧ��(��������æ״̬)
// 
// ˵����
// (1). ��ȡǰ���Flashæ��־WIP
// (2). SPI_COMMAND_READ(80MHzʱ��)��SPI_COMMAND_READ_FAST(120MHzʱ��)��ȡ128�ֽں�ʱ��ͬ��Ϊ18.4ms @ 8MHz/DIV=128=62.5KHz
//      ��������Ϊ62.5KHz����GD25Q32B��ʱ����˵̫����
//      ��������ʹ����ͨģʽ
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
//          Drv_SPI_write_byte(SPI_COMMAND_DUMMY);  // SPI_COMMAND_READ_FASTģʽ�¡�����Ҫ�෢һ��dummy�ֽڣ����͹�����GD25Q32����������
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
// дһҳ���ݵ�SPI Flash
// 
// ������  pBuffer    ����λ��pBufferָ����ڴ�
//         addr       ҳ���׵�ַ(һ����ĳһҳ���׵�ַ��������־�������)
// 
// ����ֵ��Fin        TRUE  = д����ɡ���һ���ɹ�
//                    FALSE = д��ʧ��(WIPæ��дʹ��ʧ�ܡ��������д����)
// 
// ˵����
// (1). ������д��ǰ���Flashæ��־WIP���Լ�дʹ��(WEL)
//      д��������Ǳ�д�뵽GD25Q32B��һ����СΪ256�ֽڵ�ҳ����
//      �����һ��SCLK�������������CS#����ִ��Page-Programָ����������������д��Flash��
//      д��Flash�����������WIP=1����ʱԼ0.4-2.4ms
//      ��������д�뻺��Ĺ��̡���WIP�޹ء�
// 
// (2). ��ַaddr����ĳһҳ���׵�ַ�����addr����ĳһҳ���׵�ַ�����Ǹ�ҳ�׵�ַ�͸�ҳ���һ����ַ֮�����ֵ��
//      ��ô���ݽ���addr����ʼд�롢������ҳ���һ����ַ�󡢻᷵�ظ�ҳ���׵�ַ����д��(ҳ����ֻ��256�ֽڣ����Ǿ���(��¼)
// 
// (3). д�����������д��������������ﱻ����Ϊһҳ�Ĵ�С���������һҳ�Ĵ�С����ô������(2)���������ľ��գ�
//      ֱ�����һ���ֽڱ�д�룬������ǡ�pBuffer�е����256�ֽڱ�д���ҳ
// 
// ==========================================================================================================
bool Mod_SPI_Flash_write_page(uint8_t *const pBuffer, const uint32_t addr)
{
    bool Fin = FALSE;
    bool temp;
    uint16_t index; // һҳ������65536�ֽ�
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
// ����
// 
// ������  type       ֧�����ֲ�������������(4K)[ʵ�� = SPI_COMMAND_ERASE_SECTOR](GD25Q32B������������ʱ=70-500ms)
//                                  �����(64K)[ʵ�� = SPI_COMMAND_ERASE_BLOCK64](GD25Q32B��64K�������ʱ=400-800ms)
//                                  ��Ƭ����[ʵ�� = SPI_COMMAND_ERASE_CHIP](GD25Q32B����Ƭ������ʱ=20-40s)
//         addr       ��Ҫ����������/���ڵĵ�ַ���ɡ���Ҫ���Ǹ�����/���ڵ��׵�ַ
// 
// ����ֵ��Fin        TRUE  = ����ָ���ѷ���
//                    FALSE = д��ʧ��(WIPæ��дʹ��ʧ�ܡ��������д����)
// 
// ˵����
// (1). �����ڲ���ǰ���Flashæ��־WIP���Լ�дʹ��(WEL)
// (2). ����ָ��������CS#��ִ�У�������WIP=1
// (3). ����������ʱ����һ��Ҫ������������ȫ��64��blockʱ����ʱ500ms��Ҳ������һ��blockδ�����ɹ��������ĳ�����ʱ1000ms����
//      ����ʱ200msҲ������ɹ���
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
// ���/�����ӳ�
// 
// ������             SPI_COMMAND_SUSPEND(2us����Ч)
//                    SPI_COMMAND_RESUME(200ns����Ч)
// 
// ����ֵ��Fin        TRUE  = ָ������Ч
//                    FALSE = ָ��δ��Ч
// 
// ˵����
// (1). SPI_COMMAND_SUSPEND������ҳ��̺�����/�����ָ������С���ָͣ���ִ�У�������ȥ��ȡ��������/������
//      ��ͣ��дSR�Ĵ���������security�Ĵ��������в���ָ���ҳ���ָ���������(��Ч�����ǲ�������Щ����)
// (2). SPI_COMMAND_SUSPENDָ��ֻ��ҳ��̺�����/�����ָ���������Ч(SR.WIP=1 && SR.SUS=0)
//      ָ����Ч��SR.WIP=0 && SR.SUS=1
// (3). SPI_COMMAND_RESUMEָ��ֻ��SR.WIP=0 && SR.SUS=1ʱ��Ч
//      ָ����Ч��SR.WIP=1 && SR.SUS=0
// 
// ==========================================================================================================
bool Mod_SPI_Flash_suspend(const uint8_t type)
{
    bool Fin = FALSE;
    uint16_t status;
    uint16_t bit_wip;   // 16bit��SR�Ĵ���
    uint16_t bit_sus;   // 16bit��SR�Ĵ���

    if((SPI_COMMAND_SUSPEND == type) || (SPI_COMMAND_RESUME == type))
    {
        switch(type)
        {
            case SPI_COMMAND_SUSPEND: bit_wip = _WIP_; bit_sus = 0;     break;
            case SPI_COMMAND_RESUME : bit_wip = 0;     bit_sus = _SUS_; break;
        }
        // ����ָ��
        status = Mod_SPI_Flash_read_status();
        if((bit_wip == (status & _WIP_)) && (bit_sus == (status & _SUS_)))
        {
            Drv_SPI_clr_SS();
            Drv_SPI_write_byte(type);
            Drv_SPI_set_SS();
            // ���ָ���Ƿ���Ч
            bit_wip = (_WIP_ == bit_wip) ? 0 : _WIP_;
            bit_sus = (_SUS_ == bit_sus) ? 0 : _SUS_;
            status = Mod_SPI_Flash_read_status();   // ��ȡһ�ε�ʱ����Ҫ > 2us����Ȼָ����ܻ�û��Ч
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
// ��������
// 
// ����ֵ��Fin        TRUE  = ����ָ���ѷ���
//                    FALSE = ��������ʧ��(WIPæ)
// 
// ˵����
// (1). ����������ǰ��Ҫ���Flashæ��־WIP
// (2). ���ߺ󽫺���д�롢��̡�����ָ�ֻ��Ӧ����ָ��(ƽʱCS#���߱���ֹ�󡢽����ڴ���ģʽ)
//      ����ָ����󡢻���0.1us����ʽ��������
// (3). �������ߺͻ��ѵ�����������δ���ԣ�����Ҫ�͹��ĵ�Ӧ�ó��ϡ�����PCB�ٽ�������ǰ��ĵ����Ƚϲ���
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
// �����߻������ģʽ�˳�
// 
// ����ֵ��Fin        TRUE  = ָ���ѷ���
//                    FALSE = WIPæ���˳�ʧ��
// 
// ˵����
// (1). ����������ǰ��Ҫ���Flashæ��־WIP
// (2). ���Ի�����������Ӹ�����ģʽ�˳�
//      ָ����󡢻���0.1us����ɻ���
// (3). ��Ҫ��ȡ����ID����Ҫ���Ѻ���SPI_COMMAND_READ_IDָ������3��dummy���ݡ�Ȼ����ܶ�������ID
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
