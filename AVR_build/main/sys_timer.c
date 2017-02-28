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
// 本文定义了Atmega16定时模块和任务调度模块
// 
// -------------
// 包含：
// (1). 毫秒延时
// (2). 全局时间
// (3). 系统时标(作为调度器使用)
// 
// ==========================================================================================================
#include "sys_timer.h"


typedef struct 
{
    uint8_t number;         // 任务号：该任务在任务队列中的位置
    uint8_t co_op;          // 任务类型：1=合作式任务，0=抢占式任务
    uint8_t run;            // 任务状态：(0)=准备中、(>0)=就绪、(>1)=任务曾经被延迟
    uint8_t delay;          // 任务延时计数
    uint8_t period;         // 任务运行间隔
    p_void_funtion_void task;  // 任务函数
}T_sys_task;

T_sys_task sys_task_ctrl[SYS_TASK_MAX];


// ==========================================================================================================
//      系统任务调度定时器启动(中断优先级倒数第2)
// 
// (1). 使用Timer0产生1ms的时标
//      定时周期 T = ((1.0/8000000)*1000000)*64*(124+1) = 1000us = 1ms
//      使用较小的OCR0=122、可以从PA1得到更精确的1ms，因为进入中断也是需要十几us
//      这可以让从中断产生到进入中断函数为止的时间更精确为1.0ms
// 
// (2). t = ((1.0 / 8000000)) * div * (N + 1)(单位：秒)
//      f = 8000000 / (div * (N + 1))
//      N = 8000000 / freq / div - 1
// 
// ==========================================================================================================
void sys_task_start(void)
{
    uint16_t div;          // 16bit宽度的分频系数
    uint16_t freq = 1000;  // 16bit宽度的频率：1000Hz
    uint8_t  ocr0;
    uint8_t  DIV  = T0_CLK_SOURCE_DIV_64;

    // --------
    // 计算参数
    switch(DIV)
    {
        case T0_CLK_SOURCE_DIV_1:    div = 1;    break;
        case T0_CLK_SOURCE_DIV_8:    div = 8;    break;
        case T0_CLK_SOURCE_DIV_64:   div = 64;   break;
        case T0_CLK_SOURCE_DIV_256:  div = 256;  break;
        case T0_CLK_SOURCE_DIV_1024: div = 1024; break;

        default: return;
    }
    ocr0 = SYS_OSC_FREQUENCE / freq / div - 1;  // 运算结果是8位的、但分步的中间结果可能是16位或32位的
    ocr0 = ocr0 - 2;                            // 稍微减少一点OCR0、可以从PA1得到更精确的1ms，因为进入中断也需要十几us

    // --------
    // 设置参数
    Drv_Timer0_init(T0_WGM_CTC, COM_MODE_NONE, DIV);
    Drv_Timer0_set_TCNT0_OCR0(0, ocr0);
    Drv_Timer0_INT_Enable(INT_MODE_OCF, ENABLE);
}

// ==========================================================================================================
// 初始化系统任务调度模块
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
//      添加任务到任务队列
// 
// 参数：
//      delay   任务延时计数
//      period  任务运行间隔
//      task    任务函数名
//      co_op   任务类型：1=合作式任务，0=抢占式任务-(SYS_TASK_TYPE_CO_OP, SYS_TASK_TYPE_PRE_EM)
//      init    任务的初始化函数-(任务不再放到sys_init()里面去初始化)
// 
// 返回值：
//      index   任务号，>=SYS_TASK_MAX==SYS_TASK_MAX的任务号是无效的任务号
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
        // 必须在任务设置完毕后才初始化
        // 因为初始化里面需要用到sys_task_index(),这涉及到sys_task_ctrl[index].task、sys_task_ctrl[index].number
        if(NULL != init)
        {
            init();
        }
    }
    return index;
}

// ==========================================================================================================
// 删除任务队列中的任务
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
// 获取一个任务的任务号
// 
// 参数：
//      task    任务函数名
// 
// 返回值：
//      index   任务号，>=SYS_TASK_MAX的任务号是无效的任务号
// 
// ==========================================================================================================
uint8_t sys_task_index(const p_void_funtion_void task)
{
    uint8_t index = SYS_TASK_MAX;

    if(NULL != task)
    {
        for(index = 0; index < SYS_TASK_MAX; index++)
        {
            if(sys_task_ctrl[index].task == task)   // 指针可能需要转换为(uint16_t)来进行比较
            {
                index = sys_task_ctrl[index].number;
                break;
            }
        }
    }
    return index;
}

// ==========================================================================================================
//      任务调度函数
// 
// 说明：
// (1). 运行任务队列中 所有 已经就绪的任务
// (2). .run > 1表示该任务之前有被延迟 >1个调度时刻
// 
// (3). 新时刻到来会立即打断调度函数：
//      在运行过程中会被刷新任务状态的ISR(TIMER0_COMP_vect)打断，但由于任务是在每次时刻到来时被执行
//      所以每次打断的时刻、都是刚才任务被执行的时刻结束、而新时刻到来的时刻
//      此时、只要刚才的任务没超过或接近1ms，那么在这个时刻到来前、该任务早就执行完毕了
//      所以、此时打断并不影响刚才的任务，也不会出现刚才的任务执行完毕后、被加载在其后面执行的情况
// 
// (2). 单次任务：
//      将.period初始化为0，该函数就只会被执行1次、执行完毕后会立即被删除
//      sys_task_add(0, n, X, Xi)在初始化结束X后，X会立即被执行、或者在下一个时刻被执行：
//      1、如果任务队列中、当前任务前面还有空余位置，任务X就会被放在当前任务之前，那么任务X会在下一个时刻被执行
//      2、如果任务队列中、当前任务前面没有空余位置，任务X就会被放在当前任务之后，那么任务X会在当前任务所在时刻被执行
//      由于任务X及其初始化函数Xi都会增加执行时间，所以必须考虑新任务不会导致当前任务超时
//      一般：
//      -->只执行一次的任务、尽量放到启动任务队列之前去执行，不放到任务队列中
//      -->只执行一次的任务、尽量做得短小，其初始化一般为空
//      可以不用支持单次运行的任务，这里保留
// 
// (3). 每个任务都需要在退出任务前、解锁自己锁定的事件等资源
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
        // 任务调度
        if(sys_task_ctrl[index].run > 0)
        {
            sys_task_ctrl[index].task();
            if(sys_task_ctrl[index].run > 1)
            {
                // 该任务曾经被延迟
                sys_update_event(EVENT_SYS, MSG_SYS_TASK_DELAYED, index);
            }
            sys_task_ctrl[index].run = 0;
            if(0 == sys_task_ctrl[index].period)
            {
                sys_task_delete(index);
            }
        }
        // --------
        // 事件管理
        lock = sys_event_any_lock();
        if(lock < EVENT_MAX)
        {
            // 有事件被锁定了
            sys_update_event(EVENT_SYS, MSG_SYS_EVENT_LOCKED, lock);
        }
        event = sys_event_push();
    }
    // --------------------------------------------------------
    // 所有任务都执行完毕、且所有事件都已写入事件队列后，进入休眠
    if(TRUE == event)
    {
        task_sys_enter_sleep();
    }
}

// ==========================================================================================================
//      系统定时器(Timer0)中断(中断周期=1ms)
// 
// (1). 刷新任务状态
// (2). sys_task_add中将.delay初始化为0，意味着第1次进入 任务调度函数、就会执行这个任务
// (3). 抢占式任务将直接在中断中执行，所以要求尽量短小
// 
// ==========================================================================================================
ISR(TIMER0_COMP_vect)
{
    uint8_t index = 0;

    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        if(NULL != sys_task_ctrl[index].task)
        {
            if(0 < sys_task_ctrl[index].delay)  // 任务延时计数
            {
                sys_task_ctrl[index].delay--;
            }
            if(0 == sys_task_ctrl[index].delay) // 任务就绪检查
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
                // 单次运行的任务将会被删除、在被删除前不再重新计数
                if(sys_task_ctrl[index].period > 0)
                {
                    sys_task_ctrl[index].delay = sys_task_ctrl[index].period;
                }
            }
        }
    }
}

// ==========================================================================================================
//      毫秒级延时
// 
// 说明：
// (1). 根据不同的时钟,选择不同的参数,保证在不同时钟下的延时都是1ms
// 
// (2). 如果编译器将error报错，表示需要将error替换成count，并调试count的值，以得到新的主频下的1ms延时。
// 
// (3). 可以使用PA0引脚测试时间
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


