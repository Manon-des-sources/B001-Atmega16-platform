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
// --------------------------------------
// 本文定义了系统提示和警告
// 
// --------------------------------------
// 包含：
// 
// ==========================================================================================================
#include "sys_warning.h"



// ==========================================================================================================
// 更新系统产生的消息和警告
// 
// 参数：type    需要更新的事件
//       msg     需要更新的消息
//       index   任务号、或事件号
// 
// 说明：
// (1). 使用读-修改-写的方式更新事件
// (2). 使用了sys_event_post()来直接写入，并在读写之前强制解锁该事件
// 
// ==========================================================================================================
void sys_update_event(const uint8_t type, const uint32_t msg, const uint8_t index)
{
    bool peek;
    U_MSG_EVENT_SYS sys;

    sys_event_unlock(type);
    peek = sys_event_data(type, &sys.data);

    if(TRUE == peek)
    {
        switch(msg)
        {
            // 有事件被锁定 ----------
            case MSG_SYS_EVENT_LOCKED : sys.msg.event_locked = 1;
                                        sys.msg.event_index  = index;
                                        break;
            // 有任务被延迟 ----------
            case MSG_SYS_TASK_DELAYED : sys.msg.task_delayed = 1;
                                        sys.msg.task_index   = index;
                                        break;
            // 系统状态 --------
            case MSG_SYS_SLEEP  : sys.msg.sys_sleep  = 1;
                                  break;
            case MSG_SYS_WAKEUP : sys.msg.sys_wakeup = 1;
                                  break;
            case MSG_SYS_IDLE   : sys.msg.sys_ldle   = 1;
                                  break;
            case MSG_SYS_START  : sys.msg.sys_start  = 1;
                                  break;
            default : break;
        }
        sys_event_post(type, sys.data);
    }
}
