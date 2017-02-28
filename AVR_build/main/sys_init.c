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
// ----------------------------
// 本文定义了本项目的初始化模块
// 
// ----------------------------
// 包含：
// 
// ==========================================================================================================
#include "sys_init.h"


// ==========================================================================================================
//  伪中断BADISR_vect
// .用于捕获未定义中断函数的中断
//  没有包含interrupt.h的文件中的ISR()都被视为未定义的ISR(),这种ISR()可以被BADISR_vect这个伪中断捕获
// 
// .没有这个函数的话,当没有定义中断函数的中断到来时,系统会执行到错误的程序(如复位) |<-------------- 需要测试这个东西
// 
// ==========================================================================================================
ISR(BADISR_vect)
{
    PORTA ^= (1 << PA7);
}

// ==========================================================================================================
//      检查复位源
// (1). 清除复位信息,等待记录下一次复位
// ==========================================================================================================
void get_reset_source(void)
{
    // 上电复位
    if( MCUCSR &   (1 << PORF) )
    {
        MCUCSR &= ~(1 << PORF);
    }
    // 外部复位
    if( MCUCSR &   (1 << EXTRF) )
    {
        MCUCSR &= ~(1 << EXTRF);
    }
    // BOD掉电复位
    if( MCUCSR &   (1 << BORF) )
    {
        MCUCSR &= ~(1 << BORF);
    }
    // WDT复位
    if( MCUCSR &   (1 << WDRF) )
    {
        MCUCSR &= ~(1 << WDRF);
    }
    // Jtag指令AVR_RESET复位
    if( MCUCSR &   (1 << JTRF) )
    {
        MCUCSR &= ~(1 << JTRF);
    }
}

// ==========================================================================================================
// 系统初始化
// 
// (1). 只初始化与系统相关的地方
// (2). 任务相关的地方、任务自己会初始化
// 
// ==========================================================================================================
void sys_init(void)
{
    // -------
    //时钟校准
    Drv_sys_write_OSCCAL();

    // --------
	// IO初始化
	// PORTA初始化为输出低电平
	DDRA  = 0xFF;
    PORTA = 0x00;
	// PORTB初始化为输出高电平(连接了低推的LED)
    DDRB  = 0xFF;
    PORTB = 0xFF;
	// PORTC初始化为输出低电平
	DDRC  = 0xFF;
    PORTC = 0x00;
	// PORTD初始化为输出低电平
	DDRD  = 0xFF;
    PORTD = 0x00;   // PD3已经与低推的红外LED断开


    // PA7初始化为输出0(给BADISR_vect使用)
    Drv_IO_mode_bit(DDRA, DDA7, IO_OUTPUT);
    Drv_IO_clr_bit(PORTA, PA7);


    // 系统消息/事件队列
    sys_event_init();
    // 设置数据格式
    Drv_sys_set_digital_format(10);

    // 设置系统启动标志(必须设置先清除EVENT_MAX、替换为EVENT_SYS，sys_update_event(EVENT_SYS, ..)才可以工作)
    sys_event_post(EVENT_SYS, 0);
    sys_update_event(EVENT_SYS, MSG_SYS_START, 0);

    // SPI Flash等器件在上电10ms或更久之后才可以正常操作
    delay_ms(100);

    // 显示输出版本信息
//  printf("%s\r\n",SYS_VERSION);
}




