// ==========================================================================================================
// 
// COPYRIGHT
// ---------
// 
// This code is from the book:
// 
// PATTERNS FOR TIME-TRIGGERED EMBEDDED SYSTEMS by Michael J. Pont 
// [Pearson Education, 2001; ISBN: 0-201-33138-1].
// 
// This code is copyright (c) 2001 by Michael J. Pont.
// 
// See book for copyright details and other information.
// 
// ----------------------------------------------
// modified: 2016 ChenChen <codingmanon@163.com>
// 
// ----------------------------------------
// ���Ķ�����Atmega16��ʱģ����������ģ��
// 
// -------------
// ������
// (1). ������ʱ
// (2). ȫ��ʱ��
// (3). ϵͳʱ��(��Ϊ������ʹ��)
// 
// ==========================================================================================================
#include "sys_timer.h"


typedef struct 
{
    uint8_t number;         // ����ţ�����������������е�λ��
    uint8_t co_op;          // �������ͣ�1=����ʽ����0=��ռʽ����
    uint8_t run;            // ����״̬��(0)=׼���С�(>0)=������(>1)=�����������ӳ�
    uint8_t delay;          // ������ʱ����
    uint8_t period;         // �������м��
    p_void_funtion_void task;  // ������
}T_sys_task;

volatile T_sys_task sys_task_ctrl[SYS_TASK_MAX];


// ==========================================================================================================
//      ϵͳ������ȶ�ʱ������(�ж����ȼ�������2)
// 
// (1). ʹ��Timer0����1ms��ʱ��
//      ��ʱ���� T = ((1.0/8000000)*1000000)*64*(124+1) = 1000us = 1ms
//      ʹ�ý�С��OCR0=122�����Դ�PA1�õ�����ȷ��1ms����Ϊ�����ж�Ҳ����Ҫʮ��us
//      ������ô��жϲ����������жϺ���Ϊֹ��ʱ�����ȷΪ1.0ms
// 
// (2). t = ((1.0 / 8000000)) * div * (N + 1)(��λ����)
//      f = 8000000 / (div * (N + 1))
//      N = 8000000 / freq / div - 1
// 
// ==========================================================================================================
void sys_task_start(void)
{
    uint16_t div;          // 16bit��ȵķ�Ƶϵ��
    uint16_t freq = 1000;  // 16bit��ȵ�Ƶ�ʣ�1000Hz
    uint8_t  ocr0;
    uint8_t  DIV  = T0_CLK_SOURCE_DIV_64;

    // --------
    // �������
    switch(DIV)
    {
        case T0_CLK_SOURCE_DIV_1:    div = 1;    break;
        case T0_CLK_SOURCE_DIV_8:    div = 8;    break;
        case T0_CLK_SOURCE_DIV_64:   div = 64;   break;
        case T0_CLK_SOURCE_DIV_256:  div = 256;  break;
        case T0_CLK_SOURCE_DIV_1024: div = 1024; break;

        default: return;
    }
    ocr0 = SYS_OSC_FREQUENCE / freq / div - 1;  // ��������8λ�ġ����ֲ����м���������16λ��32λ��
    ocr0 = ocr0 - 2;                            // ��΢����һ��OCR0�����Դ�PA1�õ�����ȷ��1ms����Ϊ�����ж�Ҳ��Ҫʮ��us

    // --------
    // ���ò���
    Drv_Timer0_init(T0_WGM_CTC, COM_MODE_NONE, DIV);
    Drv_Timer0_set_TCNT0_OCR0(0, ocr0);
    Drv_Timer0_INT_Enable(INT_MODE_OCF, ENABLE);
}

// ==========================================================================================================
// ��ʼ��ϵͳ�������ģ��
// 
// ==========================================================================================================
void sys_task_init(void)
{
    uint8_t index = 0;

    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        sys_task_delete(index);
    }
}

// ==========================================================================================================
//      ��������������
// 
// ������
//      delay   ������ʱ����
//      period  �������м��
//      task    ��������
//      co_op   �������ͣ�1=����ʽ����0=��ռʽ����-(SYS_TASK_TYPE_CO_OP, SYS_TASK_TYPE_PRE_EM)
//      init    ����ĳ�ʼ������-(�����ٷŵ�sys_init()����ȥ��ʼ��)
// 
// ����ֵ��
//      index   ����ţ�>=SYS_TASK_MAX==SYS_TASK_MAX�����������Ч�������
// 
// ==========================================================================================================
uint8_t sys_task_add(const uint8_t delay, const uint8_t period, const p_void_funtion_void task, const uint8_t co_op, const p_void_funtion_void init)
{
    uint8_t index = 0;

    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        if(NULL == sys_task_ctrl[index].task) { break; }
    }
    if(index < SYS_TASK_MAX)
    {
        sys_task_ctrl[index].number = index;
        sys_task_ctrl[index].co_op  = co_op;
        sys_task_ctrl[index].run    = (delay == 0) ? 1      : 0;
        sys_task_ctrl[index].delay  = (delay == 0) ? period : delay;
        sys_task_ctrl[index].period = period;
        sys_task_ctrl[index].task   = task;
        // ����������������Ϻ�ų�ʼ��
        // ��Ϊ��ʼ��������Ҫ�õ�sys_task_index(),���漰��sys_task_ctrl[index].task��sys_task_ctrl[index].number
        if(NULL != init)
        {
            init();
        }
    }
    return index;
}

// ==========================================================================================================
// ɾ����������е�����
// 
// ==========================================================================================================
void sys_task_delete(const uint8_t index)
{
    if(index < SYS_TASK_MAX)
    {
        if(NULL != sys_task_ctrl[index].task)
        {
            sys_task_ctrl[index].number = SYS_TASK_MAX;
            sys_task_ctrl[index].co_op  = SYS_TASK_TYPE_CO_OP;
            sys_task_ctrl[index].run    = 0;
            sys_task_ctrl[index].delay  = 0;
            sys_task_ctrl[index].period = 0;
            sys_task_ctrl[index].task   = NULL;
        }
    }
}

// ==========================================================================================================
// ��ȡһ������������
// 
// ������
//      task    ��������
// 
// ����ֵ��
//      index   ����ţ�>=SYS_TASK_MAX�����������Ч�������
// 
// ==========================================================================================================
uint8_t sys_task_index(const p_void_funtion_void task)
{
    uint8_t index = SYS_TASK_MAX;

    if(NULL != task)
    {
        for(index = 0; index < SYS_TASK_MAX; index++)
        {
            if(sys_task_ctrl[index].task == task)   // ָ�������Ҫת��Ϊ(uint16_t)�����бȽ�
            {
                index = sys_task_ctrl[index].number;
                break;
            }
        }
    }
    return index;
}

// ==========================================================================================================
//      ������Ⱥ���
// 
// ˵����
// (1). ������������� ���� �Ѿ�����������
// (2). .run > 1��ʾ������֮ǰ�б��ӳ� >1������ʱ��
// 
// (3). ��ʱ�̵�����������ϵ��Ⱥ�����
//      �����й����лᱻˢ������״̬��ISR(TIMER0_COMP_vect)��ϣ���������������ÿ��ʱ�̵���ʱ��ִ��
//      ����ÿ�δ�ϵ�ʱ�̡����Ǹղ�����ִ�е�ʱ�̽���������ʱ�̵�����ʱ��
//      ��ʱ��ֻҪ�ղŵ�����û������ӽ�1ms����ô�����ʱ�̵���ǰ�����������ִ�������
//      ���ԡ���ʱ��ϲ���Ӱ��ղŵ�����Ҳ������ָղŵ�����ִ����Ϻ󡢱������������ִ�е����
// 
// (2). ��������
//      ��.period��ʼ��Ϊ0���ú�����ֻ�ᱻִ��1�Ρ�ִ����Ϻ��������ɾ��
//      sys_task_add(0, n, X, Xi)�ڳ�ʼ������X��X��������ִ�С���������һ��ʱ�̱�ִ�У�
//      1�������������С���ǰ����ǰ�滹�п���λ�ã�����X�ͻᱻ���ڵ�ǰ����֮ǰ����ô����X������һ��ʱ�̱�ִ��
//      2�������������С���ǰ����ǰ��û�п���λ�ã�����X�ͻᱻ���ڵ�ǰ����֮����ô����X���ڵ�ǰ��������ʱ�̱�ִ��
//      ��������X�����ʼ������Xi��������ִ��ʱ�䣬���Ա��뿼�������񲻻ᵼ�µ�ǰ����ʱ
//      һ�㣺
//      -->ִֻ��һ�ε����񡢾����ŵ������������֮ǰȥִ�У����ŵ����������
//      -->ִֻ��һ�ε����񡢾������ö�С�����ʼ��һ��Ϊ��
//      ���Բ���֧�ֵ������е��������ﱣ��
// 
// (3). ÿ��������Ҫ���˳�����ǰ�������Լ��������¼�����Դ
// 
// ==========================================================================================================
void sys_task_dispatch(void)
{
    uint8_t  index = 0;
    uint8_t  lock;

    bool event = FALSE;

    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        // --------
        // �������
        if(sys_task_ctrl[index].run > 0)
        {
            sys_task_ctrl[index].task();
            if(sys_task_ctrl[index].run > 1)
            {
                // �������������ӳ�
                sys_update_event(EVENT_SYS, MSG_SYS_TASK_DELAYED, index);
            }
            sys_task_ctrl[index].run = 0;
            if(0 == sys_task_ctrl[index].period)
            {
                sys_task_delete(index);
            }
        }
        // --------
        // �¼�����
        lock = sys_event_any_lock();
        if(lock < EVENT_MAX)
        {
            // ���¼���������
            sys_update_event(EVENT_SYS, MSG_SYS_EVENT_LOCKED, lock);
        }
        event = sys_event_push();
    }
    // --------------------------------------------------------
    // ��������ִ����ϡ��������¼�����д���¼����к󣬽�������
    if(TRUE == event)
    {
        task_sys_enter_sleep();
    }
}

// ==========================================================================================================
//      ϵͳ��ʱ��(Timer0)�ж�(�ж�����=1ms)
// 
// (1). ˢ������״̬
// (2). sys_task_add�н�.delay��ʼ��Ϊ0����ζ�ŵ�1�ν��� ������Ⱥ������ͻ�ִ���������
// (3). ��ռʽ����ֱ�����ж���ִ�У�����Ҫ������С
// 
// ==========================================================================================================
ISR(TIMER0_COMP_vect)
{
    uint8_t index = 0;

    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        if(NULL != sys_task_ctrl[index].task)
        {
            if(0 < sys_task_ctrl[index].delay)  // ������ʱ����
            {
                sys_task_ctrl[index].delay--;
            }
            if(0 == sys_task_ctrl[index].delay) // ����������
            {
                if(SYS_TASK_TYPE_CO_OP == sys_task_ctrl[index].co_op)
                {
                    if(sys_task_ctrl[index].run < SYS_TASK_RUN_MAX)
                    {
                        sys_task_ctrl[index].run++;
                    }
                }
                else
                {
                    sys_task_ctrl[index].task();
                    if(0 == sys_task_ctrl[index].period)
                    {
                        sys_task_ctrl[index].co_op = SYS_TASK_TYPE_CO_OP;
                        sys_task_ctrl[index].task  = NULL;
                    }
                }
                // �������е����񽫻ᱻɾ�����ڱ�ɾ��ǰ�������¼���
                if(sys_task_ctrl[index].period > 0)
                {
                    sys_task_ctrl[index].delay = sys_task_ctrl[index].period;
                }
            }
        }
    }
}

// ==========================================================================================================
//      ���뼶��ʱ
// 
// ˵����
// (1). ���ݲ�ͬ��ʱ��,ѡ��ͬ�Ĳ���,��֤�ڲ�ͬʱ���µ���ʱ����1ms
// 
// (2). �����������error������ʾ��Ҫ��error�滻��count��������count��ֵ���Եõ��µ���Ƶ�µ�1ms��ʱ��
// 
// (3). ����ʹ��PA0���Ų���ʱ��
// 
// ==========================================================================================================
void delay_ms(const uint16_t count)
{
    volatile uint16_t i,j;
    for(i = 0; i < count; i++)
    {
#if   SYS_OSC_FREQUENCE == 8000000UL
        for(j = 0; j < 430; j++) {}

#elif SYS_OSC_FREQUENCE == 4000000UL
        for(j = 0; j < error; j++) {}

#elif SYS_OSC_FREQUENCE == 2000000UL
        for(j = 0; j < error; j++) {}

#elif SYS_OSC_FREQUENCE == 1000000UL
        for(j = 0; j < error; j++) {}
#else
        error = 0;
#endif
    }
}


