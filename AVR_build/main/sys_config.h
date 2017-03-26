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
// ----------------------------------------------------------------------------------------------------------
// 本文定义了本项目所使用到的全局宏定义
// 
// ------------------------------------
// 包含：
// 与AVR-GCC相关的定义
// 与Atmega16相关的定义
// 其他与平台无关的定义
// 
// ==========================================================================================================
#ifndef __CONFIG_H__
#define __CONFIG_H__






// ==========================================================================================================
// 基本平台信息
// ==========================================================================================================
// 编译器：AVR Studio 4.19 + avr-toolchain-installer-3.4.1.1195-win32.win32.x86
// 
// 芯片型号：ATmega16
// 芯片主频：
#define    SYS_OSC_FREQUENCE    8000000UL   // (最好是通过读取芯片寄存器的值来计算出主频)

// 本芯片的OSCCAL校准数据：
typedef enum 
{
    OSC_TRIM_1M = 0xA7,
    OSC_TRIM_2M = 0xA7,
    OSC_TRIM_4M = 0x9F,
    OSC_TRIM_8M = 0xA2
} E_OSC_TRIM_DATA;



// ==========================================================================================================
// 全局数据类型
// (1). AVR GCC只支持32bit的浮点数
// (2). 指针为16bit
// (3). 8bit的AVR中尽量使用int8_t和uint8_t,他们可以直接支持机器指令,在单周期指令中完成运算
// ==========================================================================================================
#ifndef __STDINT_H_  // 整型部分已经在stdint.h中定义(stdint.h被包含在AVR Libc的inttypes.h)

typedef   signed char         int8_t;
typedef   signed int         int16_t;
typedef   signed long        int32_t;
typedef   signed long long   int64_t;
typedef unsigned char        uint8_t;
typedef unsigned int        uint16_t;
typedef unsigned long       uint32_t;
typedef unsigned long long  uint64_t;

#endif  // #ifndef __STDINT_H_

typedef          float      float_32_t;  // avr gcc只支持float_32_t
typedef          double     float_64_t;

#ifndef bool
typedef unsigned char       bool;
#endif

#ifndef size_t
typedef unsigned int        size_t;
#endif

typedef void(*p_void_funtion_void)(void);
typedef bool(*p_bool_funtion_void)(void);
typedef bool(*p_bool_funtion_uint8_t)(uint8_t *);

typedef union 
{
    float    float32;
    uint32_t int32;
}T_FLOAT_INT32_UNION;

// ==========================================================================================================
// 全局宏定义
// ==========================================================================================================
// 1字节对齐(移植到其他平台时，这里可能需要修改成__align(2)或__align(4))
//#define __ALIGN    __align(1)

// 算术操作
#define MIN(a,b)   ( (a < b) ? (a) : (b) )
#define MAX(a,b)   ( (a > b) ? (a) : (b) )
#define ABS(a,b)   ( (a > 0) ? (a) : (-a) )

// 数组元素个数
#ifndef _countof
#define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif

// 结构体成员的偏移
#define offset_of_st(st, member) (size_t)(&((st*)0)->member)

// 逻辑值
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

// 使能/禁止
#ifndef ENABLE
#define ENABLE  1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif

// 空
#ifndef NULL
#define NULL    0
#endif

// 锁
#ifndef LOCKED
#define LOCKED      1
#endif
#ifndef UNLOCKED
#define UNLOCKED    0
#endif

// 位域操作(效率和直接使用未操作相同)
typedef struct _bit_struct_8
{
    unsigned char bit0 :1 ;
    unsigned char bit1 :1 ;
    unsigned char bit2 :1 ;
    unsigned char bit3 :1 ;
    unsigned char bit4 :1 ;
    unsigned char bit5 :1 ;
    unsigned char bit6 :1 ;
    unsigned char bit7 :1 ;
}bit_field_8;
#define GET_BIT_FIELD_8(a) (*(volatile bit_field_8 *)(&a))
#define LED0 GET_BIT_FIELD_8(PORTB).bit0    // 使用示例



// ==========================================================================================================
// 模块功能开关
// ==========================================================================================================
#define USE_NOINIT           1  // 使用noinit特性
#define USE_SPI_FLASH        0  // SPI Flash读写
#define USE_SPI_MANAGER      0  // SPI读写读写管理器
#define USE_SEG_LED_DISPLAY  0  // 使用LED数码管
#define USE_IR_SEND          0  // 红外发送
#define USE_IR_RECEIVE       0  // 红外接收


// 互斥功能检查











#endif  // #ifndef __CONFIG_H__







// ==========================================================================================================
// AVR GCC Notes：
// (0). AVR Libc(avr-libc-user-manual.pdf)：.bit_is_set(sfr,bit)
//                                          .bit_is_clear(sfr,bit)
//                                          .支持对Flash存储区的访问(AVR单片机原理与GCC编程实践.P111)
//                                          .支持对EEPROM的访问(AVR单片机原理与GCC编程实践.P112)
//                                          .支持对WDT的操作(AVR单片机原理与GCC编程实践.P114)
//                                          .带有数学库(AVR单片机原理与GCC编程实践.P115)
// 
// (1). 需要使用volatile修饰的地方：.硬件寄存器(如PINA)
//                                  .中断中使用到的全局变量
//                                  .中断中使用到的被static修饰的局部变量
//                                  .多线程中被多个任务共享的变量
// 
// (2). 中断：
//           定义  ： 使用语句"ISR(中断号1-21,中断属性)"来定义中断服务函数,如ISR(TIMER0_OVF_vect,ISR_BOLCK){}
//           属性  ：.ISR_BLOCK   = 进入中断后禁止全局中断,禁止了中断嵌套(定义时不指定属性、则默认为ISR_BLOCK)
//                   .ISR_NOBLOCK = 进入中断后打开全局中断,允许中断嵌套
//                   .ISR_NAKE    = 编译器不生成中断现场保护和恢复代码,用户自己保护和恢复,并使用reti()返回
//                   .ISR_ALIASOF(中断号) = 将中断函数连接到另一个中断函数(GCC4.2以上)
//           优先级： 中断向量地址(中断号)越低,优先级越高
//           说明  ：.中断函数不能被其他函数调用
//                   .cli() = 关闭全局中断
//                   .sei() = 开启全局中断
//                   .使用ISR(BADISR_vect){}捕获未定义中断函数的中断,否则系统会执行到错误的程序(如复位)
//                   .必须包含interrupt.h才可以使用ISR()、否则中断不会来，因为中断是编译器自己实现的
//                    旧版GCC-AVR使用signal.h中的SIGNAL(SIG_INTERRUPT0)
//                    ICC-AVR的写法是：#pragma interruput_handler TIM1_OVF: 6
// 
// (3). 数据强制转换：.无符号数和有符号数一起参与运算时，要强制转换，否则编译器的隐式转换可能导致莫名的问题
//                    .函数的参数和返回值如果不是常数，要强制转换，否则编译器的隐式转换可能导致莫名的问题
//                    .无符号unsinged常量要加后缀"U"
//                    .使用按位取反"~"和左移"<<"操作unsigned char和unsigned short时,应立即将结果强制转换为unsigned char和unsigned short
// 
// (4). 包含多条语句的宏使用do{}while(0)：#define SWAP(a,b) do{int temp; temp = a; a = b; b = temp;}while(0)
//                                        目的是在引用宏的时候可以加上一个分号":"
//                                        对于if
//                                              ...
//                                              SWAP(a,b);
//                                              ...
//                                            else
//                                              ...
//                                        如果定义为#define SWAP(a,b) {int temp; temp = a; a = b; b = temp;}
//                                        那么if和else之间会多一个分号":"导致if和else被分离
// 
// ==========================================================================================================
// 
// ==========================================================================================================
// AVR GCC 嵌入汇编的方式：
// (1). asm("nop");
//      asm("sei");    开全局中断
//      asm("cli");    关全局中断
//      asm("sleep");  休眠
// 
// (2). asm volatile( "not\n\t"
//                    "not\n\t"
//                    "not\n\t"
//                  ::); // 两个冒号可以省略
// ==========================================================================================================
// 
// ==========================================================================================================
// AVR GCC的代码段(AVR单片机原理与GCC编程实践.P117)：
// (1). .text段：.text = .init[9:0] + .fini[9:0] + 只读代码
//               .init[9:0]段：.定义了从芯片复位到进入main函数这一时段内的操作,其实就是启动代码,
//                             .执行顺序 = init0 -> init9
//               .fini[9:0]段：.定义了退出main函数后(或调用exit()函数时)开始执行的代码
//                             .执行顺序 = fini9 -> fini0
//
// (2). .data段：.程序中定义的、初值!=0的全局变量和静态变量
//
// (3). .bss段：.初值=0或没有初值的全局变量和静态变量,在.init4阶段被清0
//              .可以将变量定义在.bss的.noinit段,这个段的变量在.init4阶段不会被清0
//              .单片机不掉电复位后这些变量的值依然存在,可以继续使用
//              .noinit段的定义方法：int bar __attribute__ ((section (".noinit")));
//              .不能指定初值,因为这些变量是在.bss段
//
// (4). .eeprom段：.存储在EEPROM中
//                 .定义方法：uint8_t eep_buffer[10] __attribute__ ((section(".eeprom"))) = {1,2,3};
//                            #include <avr/eeprom.h>
//                            uint8_t eep_buffer[10] EEMEM = {1,2,3};
// 
// ==========================================================================================================
// 


























