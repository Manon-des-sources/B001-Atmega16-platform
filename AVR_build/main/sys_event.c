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
// ---------------------------
// 本文定义了事件管理模块
// 
// 说明：
// (1).本文将事件(event)所带的参数称为消息(message)，以区分事件本身和事件的参数
//
// ---------------------------------------------------
// 这种简单的锁定、对实时任务会访问事件队列的情况无效
// 那时、需使用资源管理器与资源请求队列
// 
//    需要加锁的部分：
// 1、任务间通信的事件队列
// 2、多个任务共同使用的RAM块
// 3、多个任务共同使用的端口/硬件接口
// 
//    更安全的锁定机制-资源管理器与资源请求队列(32bit的队列.list，支持32个任务使用该资源)：
// 0、任务A访问资源S前，先做：if(ALLOWED | (.list | (1 << PRIORITY_S_A)))
//    ALLOWED一般为0、表示允许使用
// 1、任务A访问资源S前，先做：.list | (1 << PRIORITY_S_A)，设置任务A的使用请求标志
//    其中PRIORITY_S_A有用户分配，表示使用优先级
// 2、资源队列从.list的bit0开始检查、bit0对应优先级最高的请求
//    然后通知
// 
// ---------------------------
// 包含：
// 
// ==========================================================================================================
#include "sys_event.h"



// 事件队列
static T_EVENT_LIST sys_event_list[EVENT_MAX];


// 事件管理器
// (保存着每个任务独占的事件缓存的首地址，数组下标和任务队列的下标保持一致)
T_TASK_EVENT_BOX task_event_box[SYS_TASK_MAX];


// ==========================================================================================================
//      锁定事件队列中的元素
// 
// ==========================================================================================================
void sys_event_lock(uint8_t type)
{
    if(type < EVENT_MAX)
    {
        sys_event_list[type].lock = LOCKED;
    }
}

// ==========================================================================================================
//      解锁事件队列中的元素
// 
// ==========================================================================================================
void sys_event_unlock(uint8_t type)
{
    if(type < EVENT_MAX)
    {
        sys_event_list[type].lock = UNLOCKED;
    }
}

// ==========================================================================================================
//      检查是否有事件被锁定
// 
// 返回值：index  被锁定事件的事件号
// 
// 说明：
// (1). 从头开始查找，直到找到第一个被锁定的事件为止
// (2). 调度器在每次调度新任务前，都会检查所有事件，确保没有任何锁的存在
//      因为，每个任务退出后、必须解锁所有的锁
// 
// ==========================================================================================================
uint8_t sys_event_any_lock(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        if(LOCKED == sys_event_list[index].lock)
        {
            break;
        }
    }
    return index;
}

// ==========================================================================================================
//      解锁所有事件
// 
// ==========================================================================================================
void sys_event_unlock_all(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        sys_event_list[index].lock = UNLOCKED;
    }
}

// ==========================================================================================================
//      事件队列初始化、事件缓存管理器初始化
// 
// ==========================================================================================================
void sys_event_init(void)
{
    uint8_t index;

    for(index = 0; index < EVENT_MAX; index++)
    {
        sys_event_list[index].type = EVENT_MAX;
        sys_event_list[index].lock = UNLOCKED;
        sys_event_list[index].data = 0;
    }
    for(index = 0; index < SYS_TASK_MAX; index++)
    {
        task_event_box[index].number  = 0;
        task_event_box[index].pBuffer = NULL;
    }
}

// ==========================================================================================================
//      直接将每个任务产生的事件写入到事件队列
// 
// 返回值：Fin  TURE  = 写入成功
//              FALSE = 写入失败(事件被锁定、或事件是无效的事件)
// 
// ==========================================================================================================
bool sys_event_post(uint8_t type, uint32_t data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            sys_event_list[type].type = type;
            sys_event_list[type].data = data;
            sys_event_list[type].lock = UNLOCKED;
            Fin = TRUE;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      查询事件队列
// 
// 参数：  event    用于读出事件的type和data
// 
// 返回值：Fin      TURE  = 读到1个事件及其消息参数
//                  FALSE = 没有任何事件存在
// 
// 说明：
// (1). 由于总是从头开始查找，直到找到第一个有效的事件为止
//      所以在typedef enum { }EVENT中越靠前的事件、越会被优先查询到
// 
// ==========================================================================================================
bool sys_event_get(pT_EVENT_LIST event)
{
    bool Fin = FALSE;
    uint8_t index;

    event->type = EVENT_MAX;
    event->data = 0;

    for(index = 0; index < EVENT_MAX; index++)
    {
        if(UNLOCKED == sys_event_list[index].lock)
        {
            sys_event_list[index].lock = LOCKED;
            if(EVENT_MAX != sys_event_list[index].type)
            {
                Fin = TRUE;
                event->type = sys_event_list[index].type;
                event->data = sys_event_list[index].data;
                sys_event_list[index].type = EVENT_MAX;
                sys_event_list[index].data = 0;
            }
            sys_event_list[index].lock = UNLOCKED;
        }
        if(TRUE == Fin)
        {
            break;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      查看某个事件是否已经存在、要求参数也对应
// 
// 参数：  type     事件
//         data     事件的参数
// 
// 返回值：FALSE    该事件没有发生、或被锁定
//         TRUE     该事件已经发生，返回后将该事件从事件队列中清除
// 
// ==========================================================================================================
bool sys_event_peek(uint8_t type, uint32_t data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            if(sys_event_list[type].type == type)
            {
                if(sys_event_list[type].data == data)
                {
                    Fin = TRUE;
                    sys_event_list[type].type = EVENT_MAX;
                    sys_event_list[type].data = 0;
                }
            }
            sys_event_list[type].lock = UNLOCKED;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      查看某个事件是否已经存在、并取出事件的参数
// 
// 参数：  type     事件号
//        *data     取出该事件的参数
// 
// 返回值：FALSE    该事件没有发生、或被锁定
//         TRUE     该事件已经发生，返回后将该事件从事件队列中清除
// 
// ==========================================================================================================
bool sys_event_data(uint8_t type, uint32_t *data)
{
    bool Fin = FALSE;

    if(type < EVENT_MAX)
    {
        if(UNLOCKED == sys_event_list[type].lock)
        {
            sys_event_list[type].lock = LOCKED;
            if(sys_event_list[type].type == type)
            {
                Fin = TRUE;
                *data = sys_event_list[type].data;
                sys_event_list[type].type = EVENT_MAX;
                sys_event_list[type].data = 0;
            }
            sys_event_list[type].lock = UNLOCKED;
        }
    }
    return Fin;
}

// ==========================================================================================================
//      将任务独占的事件缓存注册到事件管理器
// 
// 参数：task       任务函数
//       buffer     该任务独占的事件缓存的首地址(每个任务独占1个buffer，不和其他任务共享、无访问冲突)
// 
// 说明：
// (1). 一般在任务初始化函数里面、去注册该任务独占的事件缓存
//      也就是说、这个函数一般在任务的初始化函数里面被调用
// 
// ==========================================================================================================
void sys_event_buffer_set(const p_void_funtion_void task, const pT_EVENT_LIST buffer)
{
    uint8_t task_index;

    task_index = sys_task_index(task);
    if(task_index < SYS_TASK_MAX)
    {
        task_event_box[task_index].pBuffer = buffer;
    }
}

// ==========================================================================================================
//      设置任务需要发送的事件的数量
// 
// 参数：task       任务函数
//       number     该任务独占的事件缓存里面需要发送的事件的数量
// 
// 说明：
// (1). 每个任务根据需要建立事件缓存，并在任务退出时将缓存地址发给事件管理模块即可
//      如果事件管理模块发现该任务需要发送的事件数量>0，它就会将这些事件送到事件队列，任务本身不需要自己去发送事件
// (2). 被delete的任务的缓存还在，所以依然可以在被delete后得以发送
// (3). 假如任务A的事件缓存(.buffer)有4个元素，但本次只发送2个事件、并设置.number = 2
//      那么就需要将这2个事件放在.buffer[0]和.buffer[1]，否则不能保证事件可以被发送
//      因为我们在sys_event_push()里面只查询.buffer的前2个元素(前.number个元素)，后面的不再查询
//      所以一般将.number设为_countof(event_buffer)、如果这个值不是很大的话
//      当然、如果已经确认将这2个事件放在了buffer[0]和buffer[1]，那么只需.number = 2即可避免额外的任务消耗
// 
// ==========================================================================================================
void sys_event_buffer_post(const p_void_funtion_void task, const uint8_t event_number)
{
    uint8_t task_index;

    task_index = sys_task_index(task);
    if(task_index < SYS_TASK_MAX)
    {
        task_event_box[task_index].number = event_number;
    }
}

// ==========================================================================================================
//      将所有任务的事件缓存中的事件保存到事件队列
// 
// 返回值：Fin  FALSE = 至少有1个事件还未写入事件队列
//              TRUE  = 所有事件都已成功地写入事件队列
// 
// 说明：
// (1). 每个任务独占1个buffer，不和其他任务共享、无访问冲突
// (2). 假如有>=2个事件必须同时成功地写入事件队列、才能保证用户任务执行正常
//      那么就必须在用户任务里面判断这>=2个事件同时有效
// 
// ==========================================================================================================
bool sys_event_push(void)
{
    bool Fin = TRUE;

    uint8_t post;       // 某个事件发送是否成功
    uint8_t task_index; // 任务号
    uint8_t msg_number; // 事件数量
    uint8_t msg_index;  // 事件序号
    T_EVENT_LIST event; // 事件及其参数

    for(task_index = 0; task_index < SYS_TASK_MAX; task_index++)
    {
        msg_number = task_event_box[task_index].number;
        if(msg_number > 0)
        {
            // --------
            // 发送事件
            for(msg_index = 0; msg_index < msg_number; msg_index++)
            {
                event.type = (task_event_box[task_index].pBuffer)[msg_index].type;
                if(EVENT_MAX != event.type)
                {
                    event.data = (task_event_box[task_index].pBuffer)[msg_index].data;
                    post = sys_event_post(event.type, event.data);
                    if(TRUE == post)  // 事件发送成功，则清除该事件
                    {
                        (task_event_box[task_index].pBuffer)[msg_index].type = EVENT_MAX;
                    }
                }
            }
            // ----------------------------------------------------------------------------------
            // 检查是否将全部事件都发送完毕，没发完就将未发送的部分移到前面，等待下次进来再次发送
            post = TRUE;
            for(msg_index = 0; msg_index < msg_number; msg_index++)
            {
                if(EVENT_MAX != (task_event_box[task_index].pBuffer)[msg_index].type)
                {
                    post = FALSE;
                    Fin  = FALSE;  // 至少有1个事件还未写入事件队列
                    break;
                }
            }
            if(TRUE == post)
            {
                task_event_box[task_index].number = 0;
            }
        }
    }
    return Fin;
}
