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
// --------------------------
// 本文记录了本代码的版本历史
// 
// --------------------------
// 包含：
// 
// ==========================================================================================================
#ifndef __VERSION_H__
#define __VERSION_H__



#define SYS_VERSION     "AVR_build-v1.2.0.20170209_alpha"


// ==========================================================================================================
// AVR_build-v1.2.0.20170209_alpha(branch = dev-USART)
// 
// 1、增加USART模块驱动



// ==========================================================================================================
// AVR_build-1019(v1.1.0.20170113_alpha)
// 
// 1、修改SPI读写管理器的第一个版本：只读SR[0]，不再将SR寄存器的字节数作为一个用户参数
//                                   这样可以减少读WEL和WIP的耗时
//                                   而目前遇到的SPI Flash们的WEL和WIP都在SR[0]里面



// ==========================================================================================================
// AVR_build-1018
// 
// 1、sys_task_add()中的参数init允许是NULL，用于方便那些不需要初始化的任务
// 2、建立SPI读写管理器的第一个版本：其它用户的读操作不打断当前用户的擦写操作、只有擦写指令生效后才可以响应其他用户的操作申请
//                                   也就是忙状态下不响应其它用户的读操作
//    管理器的状态机不会有死循环、哪怕SPI从机没接、或SPI总线断裂




// ==========================================================================================================
// AVR_build-1017
// 
// 1、增加Atmega16的SPI模块的操作函数
// 2、增加SPI Flash的操作函数(以GD25Q32为准)-基础型接口
//    这些接口耗时都太长、不适合用在时间触发的结构中，下一个版本将建立适合用在时间触发的结构中的接口




// ==========================================================================================================
// AVR_build-1016
// 
// 1、增加了1个抢占式任务、以建立混合式调度器，并调试ok
// 2、事件的参数data全部改为32bit
// 3、增加消息发送任务sys_event_push()、并在调度器中嵌入事件管理
//    消息队列的发送任务，是将消息缓存中的消息放入消息队列
//    以应对任务发送消息时发现消息被锁定(只有抢占式任务会因打断其他任务而出现这种情况)
//    任务函数使用sys_event_post_buffer()将消息送入缓存，并不真的发送到消息队列
//    由调度器里面的sys_event_push()在两个合作式任务进行切换的中间去完成消息放入消息队列的操作
//    当然也可以用sys_event_post()直接写入事件，但只应在初始化或没有冲突的情况下使用



// ==========================================================================================================
// AVR_build-1015
// 
// 1、给事件队列加锁、所有读写事件队列的函数都不再禁止中断、而是通过判断锁定状态来得知读写权限
// 2、数码管可按照指定进制显示
// 3、增加了EVENT_DIGITAL_FORMAT事件，用于控制数字的进制
// 4、建立了合作式调度器，并修改测试ok



// ==========================================================================================================
// AVR_build-1014
// 
// 1、修改了Mod_IR_send_init()里面载波频率不对的Bug
// 2、sys_timer_init()使用和Mod_IR_send_init()里面一样的初始化设置方式：自动计算频率参数
// 3、将sys_task_add()里面加入任务的初始化函数、作为一个参数，在任务添加的时候就初始化任务
//    这使得sys_init()只初始化与系统相关的地方，而任务相关的地方则由任务自己初始化



// ==========================================================================================================
// AVR_build-1013
// 
// 1、发现并修复了分时调度的一个bug(这个BUG会导致period=0时、任务延时为255)
//    受影响的版本：[AVR_build-1009, AVR_build-1012]
//    修改：
/*
void sys_task_add(uint8_t delay, uint8_t period, p_task_funtion task)
{
    uint8_t index = 0;

    for(index = 0; index < ARRAY_SIZE(sys_task_ctrl); index++)
    {
        if(NULL == (p_sys_task_ctrl + index)->task) { break; }
    }
    if(index >= ARRAY_SIZE(sys_task_ctrl)) { return; }

    (p_sys_task_ctrl + index)->delay  = delay;
    (p_sys_task_ctrl + index)->period = (period > 0) ? (period - 1) : 0; // 发现并修复bug的地方
    (p_sys_task_ctrl + index)->task   = task;
}
*/
// 2、重写了事件管理模块，增加了好几个接口，同时可读性更高。
// 3、完善了中断调度的前后台结构(由于增加了休眠接口(还未实现功能、只是增加了结构上的接口)，所以整体较为省电)。
//    这个结构中、所有任务都放入中断：
//                                  1、外部触发的实时任务
//                                  2、定时器中断替代CPU的主循环
//                                  3、CPU每次退出中断之后都直接进入休眠，可以有效地省电



#endif  // #ifndef __VERSION_H__
