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
// ���Ķ�����Atmega16�µ�SPI��д������
// 
// ==========================================================================================================
#ifndef __MOD_SPI_MANAGER_H__
#define __MOD_SPI_MANAGER_H__




#include <avr/interrupt.h>
#include "sys_config.h"
#include "Drv_SPI.h"
#include "Mod_SPI_Flash.h"



// �û�ʹ��SPI��״̬
typedef enum 
{
    SPI_MGR_STATE_WORK,  // ����״̬���û��Ĳ��������(��д��ָ���Ѿ���Ч)
    SPI_MGR_STATE_DONE,  // ����״̬���û��Ķ�д�����Ѿ��������
    SPI_MGR_STATE_BUSY,  // ����״̬���û��Ĳ���������
    SPI_MGR_STATE_READY, // ����״̬���û���׼������(�ȴ�ִ��)
    SPI_MGR_STATE_IDLE   // ����״̬���û�û�в�������
}SPI_MGR_STATE;

// �û�����Ĳ�������
typedef enum 
{
    SPI_MGR_ACT_READ_COM,   // ������ַ�Ķ�
    SPI_MGR_ACT_WRIRE_COM,  // ������ַ��д
    SPI_MGR_ACT_READ_ADDR,  // ����ַ�Ķ�
    SPI_MGR_ACT_WRITE_ADDR, // ����ַ��д
    SPI_MGR_ACT_EARASE_ADDR // ����ַ�Ĳ���
}SPI_MGR_ACT;


// SPI�ӿ��û���Ϣ
typedef struct 
{
    uint8_t  action;    // �û�����Ĳ���������
    uint8_t  state;     // �û�ʹ��SPI��״̬
    uint8_t  command;   // ����ָ��
    uint32_t addr;      // ������ַ
    uint16_t number;    // ����������(�ֽ�)
    uint8_t *pBuffer;   // ����(���� = ָ�� + ��ַ + ����)
    uint8_t *pData;     // �����ڻ����е�λ��
    uint16_t nData;     // �����з�������ݵĿռ��С(����ȥ�����е�ָ��͵�ַ)
}T_SPI_MGR_USER, *pT_SPI_MGR_USER;

// SPI���ӻ����ݽ������̿���
typedef struct 
{
    uint8_t  step;      // �������裺���ڿ���SPI����˳��
    uint8_t  end;       // ���ݴ����Ƿ����(��ɣ�TRUE��δ��ɣ�FALSE)
    uint16_t count;     // �Ѿ������������(�ֽ�)
    uint16_t number;    // ��Ҫ�����������(�ֽ�)
    uint8_t *pBuffer;   // ִ����Ҫ��������ݻ���
    uint8_t  user;      // ��ǰ�û��ı��

    uint8_t          suspend;  // �Ƿ������ͣ��д�Ĳ���
    pT_SPI_MGR_USER user_sus;  // ���ڱ��汻��ͣ���û��ı�ţ��Ա�ִ����Ҫ�����û�
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
