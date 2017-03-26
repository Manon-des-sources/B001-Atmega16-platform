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
// -----------------------------
// ���ĵ�SPI�ӿڹ���������ģʽ��
// 
// ==========================================================================================================
#include "Mod_SPI_manager.h"




#if USE_SPI_MANAGER



volatile uint32_t temp2014 = 0;
volatile uint32_t temp2015 = 0;
volatile uint32_t temp2020 = 0;

volatile static T_SPI_MGR_CTRL  SPI_mgr_ctrl = { .step     = 0,
                                                 .end      = FALSE,
                                                 .count    = 0,
                                                 .number   = 0,
                                                 .pBuffer  = NULL,
                                                 .user     = 0,
                                                 .suspend  = 0,
                                                 .user_sus = 0
                                               };

// SPI�ӿ��û�������
static pT_SPI_MGR_USER SPI_mgr_box[4];

// SPI״̬������
static p_bool_funtion_void Mod_SPI_MGR_run;

// ���ڴ��״̬�Ĵ�����дָ��Լ���д����(��д���ݽ����ڶ�Ӧ�Ķ�дָ����棬��[com_SR2|data_SR2|com_SR1|data_SR1|com_SR0|data_SR0])
// Ҳ���ڷ��Ͷ�дָ��͵�ַ
// ������ż������С
static uint8_t SPI_MGR_buffer[6];


// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// ��һ���֣���ʼ��



// ==========================================================================================================
// SPI��������ʼ�� �Լ� SPI�ӿڳ�ʼ��
// 
// ==========================================================================================================
void Mod_SPI_MGR_init(void)
{
    uint8_t index;

    // SPI��������ʼ��
    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        SPI_mgr_box[index] = NULL;
    }

    // ��ʼ��SPI�ӿڣ�����ģʽ��mode-3��MSB first��128��Ƶ
    Drv_SPI_init(SPI_MODE_MSTR, SPI_SHIFT_MODE_00, SPI_SHIFT_FIRST_MSB, SPI_CLK_SOURCE_DIV_128);
    Drv_SPI_INT_Enable(ENABLE);

    // ���õ�һ��״̬
    Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);


    Drv_IO_mode_bit(DDRD, DDD1, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD1);
}

// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// �ڶ����֣��û��ṹ���û�����



// ==========================================================================================================
// �����û���Ϣ
// 
// ������  user         �û��ĵ�ַ
//         pBuffer      �û��Ļ���
//         size_buffer  ����Ĵ�С
//         size_SR      ״̬�Ĵ���SR�Ĵ�С
// ˵����
// (1). n_SR�Ĵ��ڡ�����Ϊ�е�SPI Flash��״̬�Ĵ�����1���ֽڡ�2���ֽڻ�3���ֽ�
// (2). ��ָ���ַ�����ݶ�����pBuffer����Ϊ�˷���SPI�жϵı��
//      �������û���˵��pBuffer�ǲ��ɼ���(������ʹ��)���û�ֻʹ��pData��nData����д����
// 
// ==========================================================================================================
void Mod_SPI_MGR_user_msg(const pT_SPI_MGR_USER user, uint8_t *const pBuffer, const uint32_t size_buffer)
{
    user->action  = 0;
    user->state   = 0;
    user->command = 0;
    user->addr    = 0;
    user->number  = 0;
    user->pBuffer = pBuffer;
    user->pData   = pBuffer + 4;
    user->nData   = size_buffer - 4;
}

// ==========================================================================================================
// ������ע��ӿ�
//               ---- ��bufferע�ᵽ�û��������û�ע�ᵽSPI������
// 
// ������  user         �û��ĵ�ַ
//         pBuffer      �û��Ļ���
//         size_buffer  ����Ĵ�С
//         size_SR      ״̬�Ĵ���SR�Ĵ�С
// 
// ����ֵ��status   0 = �û�ע��ɹ�
//                  1 = �û��Ѿ�ע�ᡢ�˴ν��ø��»���Ϣ
//                  2 = SPI����������
//                  3 = δ�ҵ��û�
//                  4 = 
//                  5 = 
//                  6 = ����������
//                  7 = ���治��Ϊ��
// 
// ˵����
// (1). ����û������ڡ���ע����û�
//      ����û��Ѿ����ڡ��͸����û���Ϣ
// 
// ==========================================================================================================
uint8_t Mod_SPI_MGR_user_add(const pT_SPI_MGR_USER user, uint8_t *const pBuffer, const uint32_t size_buffer)
{
    uint8_t status = 0;
    uint8_t index;

    if(NULL == user)     { status = 3; }
    if(NULL == pBuffer)  { status = 6; }
    if(0 == size_buffer) { status = 7; }

    if(0 == status)
    {
        for(index = 0; index < _countof(SPI_mgr_box); index++)
        {
            if(user == SPI_mgr_box[index]) { break; }
        }
        if(index < _countof(SPI_mgr_box))
        {
            // ����û��Ѿ����ڡ��͸����û���Ϣ
            Mod_SPI_MGR_user_msg(user, pBuffer, size_buffer);
            user->state = SPI_MGR_STATE_IDLE;
            status = 1;
        }
        else// ����û������ڡ���ע����û�
        {
            for(index = 0; index < _countof(SPI_mgr_box); index++)
            {
                if(NULL == SPI_mgr_box[index])
                {
                    SPI_mgr_box[index] = user;
                    Mod_SPI_MGR_user_msg(user, pBuffer, size_buffer);
                    user->state = SPI_MGR_STATE_IDLE;
                    status = 0;
                    break;
                }
            }
            if(index >= _countof(SPI_mgr_box))
            {
                status = 2;
            }
        }
    }
    return status;
}

// ==========================================================================================================
// ��ȡ�û��ڹ������еı��(λ��)
// 
// ������  user     �û��ĵ�ַ
// 
// ����ֵ��index    �û��ڹ������еı��(λ��)
//                 ( < _countof(SPI_mgr_box)�ı������Ч�ı��)
// 
// ==========================================================================================================
uint8_t Mod_SPI_MGR_user_index(pT_SPI_MGR_USER const user)
{
    uint8_t index;

    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        if(user == SPI_mgr_box[index]) { break; }
    }
    return index;
}

// ==========================================================================================================
// ����ʹ��SPI�ӿ�
// 
// ������  user     �û��ĵ�ַ
//         command  �����ݵ�ָ��
//         addr     24bit�Ĳ�����ַ
//         size     ������
// 
// ����ֵ��Fin      TRUE  = ��������ɹ�������������SPI_MGR_STATE_WORK��SPI_MGR_STATE_DONE״̬�»�ý��
//                  FALSE = ��������ʧ��(������̫��)
// 
// ˵����
// (1). �û�������Ĳ�����״̬��
//                              SPI_MGR_STATE_WORK = ����״̬����д��ָ���Ѿ���Ч
//                              SPI_MGR_STATE_DONE = ����״̬����д�����Ѿ��������
//                              SPI_MGR_STATE_BUSY = ����״̬�����������С�����׼������(�ȴ�ִ��)
//                              SPI_MGR_STATE_IDLE = ����״̬������δ����(δ����)
// (2). ��������ɹ����û����Զ�ȡ�Լ���״̬��Ϣ.state������SPI_MGR_STATE_WORK״̬��ȥ��ȡ���
// 
// ==========================================================================================================
bool Mod_SPI_MGR_application(pT_SPI_MGR_USER const user, const uint8_t action, const uint8_t command, const uint32_t addr, const uint32_t size)
{
    bool Fin = FALSE;
    uint8_t index;

    if(user->nData >= size)
    {
        index = Mod_SPI_MGR_user_index(user);
        if(index < _countof(SPI_mgr_box))
        {
            Fin = TRUE;
            SPI_mgr_box[index]->action  = action;       // ��������
            SPI_mgr_box[index]->command = command;      // ����ָ��
            SPI_mgr_box[index]->addr    = addr;         // ������ַ
            SPI_mgr_box[index]->number  = size;         // �û��õ���������(�ֽ�)
//          SPI_mgr_box[index]->number  = (SPI_MGR_ACT_EARASE_ADDR == action) ? 0 : size;  // ����ָ����������ݡ��������²���ʧ��
            SPI_mgr_box[index]->state   = SPI_MGR_STATE_READY;  // �û��Ѿ��������������SPI�ӿ�
        }
    }
    return Fin;
}


// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// �������֣�SPI��������״̬��



// ==========================================================================================================
// ״̬����0�������Ѿ��������û�
// 
// ����ֵ��  FALSE = ��������
//           TRUE  = ��������״̬����
// 
// ==========================================================================================================
bool Mod_SPI_MGR_start(void)
{
    bool Fin = FALSE;
    uint8_t index;

    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        temp2014 = index;
        if(NULL != SPI_mgr_box[index])
        {
            // ���û��Ѿ���
            if(SPI_MGR_STATE_READY == SPI_mgr_box[index]->state)
            {
                SPI_mgr_box[index]->state = SPI_MGR_STATE_BUSY;
                switch(SPI_mgr_box[index]->action)
                {
                    case SPI_MGR_ACT_READ_COM   : 
                    case SPI_MGR_ACT_READ_ADDR  : Mod_SPI_MGR_set_run(Mod_SPI_MGR_transfer);
                                                  Fin = TRUE;
                                                  break;
                    case SPI_MGR_ACT_WRIRE_COM  : 
                    case SPI_MGR_ACT_WRITE_ADDR :
                    case SPI_MGR_ACT_EARASE_ADDR: Mod_SPI_MGR_set_run(Mod_SPI_MGR_write_WEL);
                                                  Fin = TRUE;
                                                  break;
                    default : break;  // ������Ч�Ĳ���
                }
                // �Ѿ��õ�һ���û�����Ч���롢���ٲ��Һ�����û�������ֱ�ӵ�����һ��������
                if(TRUE == Fin)
                {
                    SPI_mgr_ctrl.user = index;
                    break;
                }
            }
        }
    }
    return Fin;
}

// ==========================================================================================================
// ״̬����1��дʹ��
// 
// ����ֵ��  FALSE = ��������
//           TRUE  = ��������״̬����
// 
// ˵����
// (0). ����������������SPI Flash��WELλ����SR[0]����������ֻ���SR[0]
// (1). �����������ж�����϶��ᷢ�ͽ�������֤SPI_mgr_ctrl.end = TRUE;
//      ��������case 1��case 2�ĵȴ����䲻�ᵼ��task_SPI_MGR()�е�while()��ѭ��
// (2). ͬʱ��������ɺ�case 2����ֻ���һ�ξ��˳������Բ�����ѭ��
//      Ҳ����˵������SPI�ӻ�û�ӡ���SPI���߶��ѣ�Ҳ���ᵼ����ѭ��
// 
// ==========================================================================================================
bool Mod_SPI_MGR_write_WEL(void)
{
    bool Fin = FALSE;
    uint16_t status = 0;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // ����ָ��(ͨ����д��ָ���дʹ�ܻ�������Ч��Ҳ������������SR���Ϳɵõ�SR.WEL=1)
                SPI_MGR_buffer[0] = SPI_COMMAND_WRITE_ENABLE;
                Mod_SPI_MGR_shift(1, SPI_MGR_buffer);
                SPI_mgr_ctrl.step = 1;
                Fin = TRUE;  // ����1�ֽں�ʱ140us���������½������ȴ��������
                break;
        case 1: // дʹ��ָ�����ɺ�ȥ��ȡSR.WEL
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    SPI_MGR_buffer[0] = SPI_COMMAND_READ_STATUS_L;
                    SPI_MGR_buffer[1] = 0;
                    Mod_SPI_MGR_shift(2, SPI_MGR_buffer);
                    SPI_mgr_ctrl.step = 2;
                }
                Fin = TRUE;  // ����2�ֽں�ʱ280us
                break;
        case 2: // ���дʹ��
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    status = SPI_MGR_buffer[1];
                    temp2014 = status;
                    if(_WEL_ == (status & _WEL_))
                    {
                        // ȥ��������
                        Mod_SPI_MGR_set_run(Mod_SPI_MGR_transfer);
                        Fin = TRUE;
                    }
                    else
                    {   // дʹ��δ��Ч���´�������SR�Ĵ���
                        SPI_mgr_ctrl.end  = TRUE;
                        SPI_mgr_ctrl.step = 1;
                        Fin = FALSE;
                    }
                }
                else
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// ״̬����2�����æ״̬
// 
// ����ֵ��  FALSE = ��������
//           TRUE  = ��������״̬����
// 
// ˵����
// (0). ����������������SPI Flash��WIPλ����SR[0]����������ֻ���SR[0]
// (1). �����������ж�����϶��ᷢ�ͽ�������֤SPI_mgr_ctrl.end = TRUE;
//      ��������case 1���ᵼ��task_SPI_MGR()�е�while()��ѭ��
// (2). ͬʱ�����ͽ�����case 1����ֻ���һ�ξ��˳������Բ�����ѭ��
//      Ҳ����˵������SPI�ӻ�û�ӡ���SPI���߶��ѣ�Ҳ���ᵼ����ѭ��
// 
// ==========================================================================================================
bool Mod_SPI_MGR_read_WIP(void)
{
    bool Fin = FALSE;
    uint8_t status = 0;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // ׼��
                SPI_MGR_buffer[0] = SPI_COMMAND_READ_STATUS_L;
                SPI_MGR_buffer[1] = 0;
                Mod_SPI_MGR_shift(2, SPI_MGR_buffer);
                SPI_mgr_ctrl.step = 1;
                Fin = TRUE;  // ����2�ֽں�ʱ280us
                break;
        case 1: // ���æ״̬
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    status = SPI_MGR_buffer[1];
                    temp2020 ++;
                    temp2014 = status;
                    if(_WIP_ == (status & _WIP_))
                    {
                        // æ�����´����½�����ȥ��SR�Ĵ���
                        SPI_mgr_ctrl.step = 0;
                        Fin = FALSE;
                    }
                    else
                    {   // ��дָ���Ѿ���Ч��ȥ��ѯ��һ���û�������
                        SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                        Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                        Fin = TRUE;
                    }
                }
                else
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// ״̬����3�����ݴ���
// 
// ����ֵ��  FALSE = ��������
//           TRUE  = ��������״̬����
// 
// ˵����
// (1). case 2�������»ص�cas 1�����á����ᵼ��task_SPI_MGR()һֱѭ�����˳�
//      ��Ϊcase 1���治�����½������
// (2). ���ʹ��8MHz & 2��Ƶ����ô����256�ֽ����ݡ���ʱԼ280us
//      case 0����Ϳ�������Fin = TRUE�����ȴ����ݴ������
// (3). �����������ж�����϶��ᷢ�ͽ�������֤SPI_mgr_ctrl.end = TRUE;
//      �������ﲻ�ᵼ��task_SPI_MGR()�е�while()��ѭ��
//      Ҳ����˵������SPI�ӻ�û�ӡ���SPI���߶��ѣ�Ҳ���ᵼ����ѭ��
// 
// ==========================================================================================================
bool Mod_SPI_MGR_transfer(void)
{
    bool Fin = FALSE;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // ȡ��ָ��͵�ַ
                switch(SPI_mgr_box[SPI_mgr_ctrl.user]->action)
                {
                    case SPI_MGR_ACT_READ_COM   : 
                    case SPI_MGR_ACT_WRIRE_COM  : SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3] = SPI_mgr_box[SPI_mgr_ctrl.user]->command;
                                                  Mod_SPI_MGR_shift(SPI_mgr_box[SPI_mgr_ctrl.user]->number + 1, &SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3]);
                                                  Fin = TRUE;   // ������Ĵ�����ָ������������󡢿��Խ����ȴ�
                                                  break;
                    case SPI_MGR_ACT_READ_ADDR  : 
                    case SPI_MGR_ACT_WRITE_ADDR :
                    case SPI_MGR_ACT_EARASE_ADDR: SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[0] = SPI_mgr_box[SPI_mgr_ctrl.user]->command;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[1] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr >> 16;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[2] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr >> 8;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr;
                                                  Mod_SPI_MGR_shift(SPI_mgr_box[SPI_mgr_ctrl.user]->number + 4, SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer);
                                                  Fin = FALSE;  // number�ܴ�ʱ������Ҫ����
                                                  break;
                    default                     : // ������Ч�Ĳ�������Ϊ������ȥ��ѯ��һ���û�������
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                  Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                  Fin = TRUE;
                                                  break;
                }
                SPI_mgr_ctrl.step = 1;
                break;
        case 1: // ���ݷ������
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    switch(SPI_mgr_box[SPI_mgr_ctrl.user]->action)
                    {
                        case SPI_MGR_ACT_READ_COM   : 
                        case SPI_MGR_ACT_READ_ADDR  : SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                      Fin = TRUE;
                                                      break;
                        case SPI_MGR_ACT_WRIRE_COM  : 
                        case SPI_MGR_ACT_WRITE_ADDR :
                        case SPI_MGR_ACT_EARASE_ADDR: SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_DONE;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_read_WIP);
                                                      Fin = FALSE;  // ��д��ʱ������Ҫ�����������æ�ź�
                                                      break;
                        default                     : // ������Ч�Ĳ�������Ϊ������ȥ��ѯ��һ���û�������
                                                      SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                      Fin = TRUE;
                                                      break;
                    }
                }
                else  // �ȴ����ݷ������
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// ���ò��������ݷ���
// 
// ������  number   ������(�ֽ�)
//         pBuffer  ���ݻ���ĵ�ַ
// 
// ==========================================================================================================
void Mod_SPI_MGR_shift(uint16_t number, uint8_t *const pBuffer)
{
    SPI_mgr_ctrl.end     = FALSE;
    SPI_mgr_ctrl.count   = 0;
    SPI_mgr_ctrl.number  = number;
    SPI_mgr_ctrl.pBuffer = pBuffer;
    Drv_SPI_clr_SS();
    SPDR = SPI_mgr_ctrl.pBuffer[0];
}

// ==========================================================================================================
// ������һ��״̬����
// 
// ������  p_bool_funtion_void      �������������е�״̬����
//         �����ͺ����ķ���ֵ��     TRUE  = ��������״̬����
//                                  FALSE = ��������
// 
// ==========================================================================================================
void Mod_SPI_MGR_set_run(p_bool_funtion_void const fun)
{
    SPI_mgr_ctrl.step = 0;
    Mod_SPI_MGR_run   = fun;
}

// ==========================================================================================================
// �����û���SPI��������
// 
// ˵����
// (1). ÿ1ms����һ�Ρ�������ý�ÿ�ε���ʱ�������500us֮�ڻ����
// (2). ��һ��������ʹ��ÿ�ο���ִ�ж��״̬��������Ȼ�����ݻ�Ϊ������
//      run  = FALSE����������
//             TRUE ����������״̬����
//                    ���һ��״̬����û�в���SPI�ӿڣ���ò�����ʱ����
//                    �Ϳ���ֱ�ӽ�����һ��״̬�����������½��뱾�ε��Ⱥ��������������е���һ����֧
//                    �������Ա������SPI�ӿڵĶ������ӳ�����1ms
// 
// ==========================================================================================================
void task_SPI_MGR(void)
{
    bool run = TRUE;
    
    Drv_IO_toggle_bit(PORTD, PD1);
    while(run)
    {
        run = Mod_SPI_MGR_run();
        temp2015 = run;
    }
}

// ==========================================================================================================
// SPI��������ж�
// 
// ˵����
// (1). ��ӻ��������ݡ�ֱ���������ݽ������
// (2). ʹ������ģʽ
//      ����ģʽ�£����SS��������Ϊ���롢SS�����յ��͵�ƽ���ͻᵼ��SPI�ӿ��Զ���Ϊ�ӻ�ģʽ
//      ͬʱSPIF��1���������ж�������Ҫ�ָ�����ģʽ��
//      if(SPI_MODE_SLAVE == (SPCR & (1 << MSTR)))
//      {
//          SPCR |= (SPI_MODE_MSTR << MSTR);
//      }
// (3). ��������������ж�����ʹ�����жϣ�if(SPI_mgr_ctrl.count < SPI_mgr_ctrl.number)
//      ���ԡ�һ������SPI_mgr_ctrl.count > SPI_mgr_ctrl.number��ʱ�̡�������ɴ��䣺SPI_mgr_ctrl.end = TRUE;
// 
// ==========================================================================================================
ISR(SPI_STC_vect)
{
    // һ�δ�����ɺ󡢻�ȡ��ӻ������õ�������
    SPI_mgr_ctrl.pBuffer[SPI_mgr_ctrl.count] = SPDR;

    // �����������ֱ���������ݷ������
    SPI_mgr_ctrl.count++;
    if(SPI_mgr_ctrl.count < SPI_mgr_ctrl.number)
    {
        SPDR = SPI_mgr_ctrl.pBuffer[SPI_mgr_ctrl.count];
    }
    else
    {
        Drv_SPI_set_SS();
        SPI_mgr_ctrl.end = TRUE;
    }
}




#endif  // #if USE_SPI_MANAGER
