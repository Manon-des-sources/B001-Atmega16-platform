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
// ���Ķ����˱���Ŀ��ʹ�õ���ȫ�ֺ궨��
// 
// ------------------------------------
// ������
// ��AVR-GCC��صĶ���
// ��Atmega16��صĶ���
// ������ƽ̨�޹صĶ���
// 
// ==========================================================================================================
#ifndef __CONFIG_H__
#define __CONFIG_H__






// ==========================================================================================================
// ����ƽ̨��Ϣ
// ==========================================================================================================
// ��������AVR Studio 4.19 + avr-toolchain-installer-3.4.1.1195-win32.win32.x86
// 
// оƬ�ͺţ�ATmega16
// оƬ��Ƶ��
#define    SYS_OSC_FREQUENCE    8000000UL   // (�����ͨ����ȡоƬ�Ĵ�����ֵ���������Ƶ)

// ��оƬ��OSCCALУ׼���ݣ�
typedef enum 
{
    OSC_TRIM_1M = 0xA7,
    OSC_TRIM_2M = 0xA7,
    OSC_TRIM_4M = 0x9F,
    OSC_TRIM_8M = 0xA2
} E_OSC_TRIM_DATA;



// ==========================================================================================================
// ȫ����������
// (1). AVR GCCֻ֧��32bit�ĸ�����
// (2). ָ��Ϊ16bit
// (3). 8bit��AVR�о���ʹ��int8_t��uint8_t,���ǿ���ֱ��֧�ֻ���ָ��,�ڵ�����ָ�����������
// ==========================================================================================================
#ifndef __STDINT_H_  // ���Ͳ����Ѿ���stdint.h�ж���(stdint.h��������AVR Libc��inttypes.h)

typedef   signed char          int8_t;
typedef   signed int          int16_t;
typedef   signed long         int32_t;
typedef   signed long long    int64_t;
typedef unsigned char         uint8_t;
typedef unsigned int         uint16_t;
typedef unsigned long        uint32_t;
typedef unsigned long long   uint64_t;

#endif  // #ifndef __STDINT_H_

typedef          float     float_32_t;  // avr gccֻ֧��float_32_t
typedef          double    float_64_t;

#ifndef bool
typedef unsigned char      bool;
#endif

typedef void(*p_void_funtion_void)(void);
typedef bool(*p_bool_funtion_void)(void);
typedef bool(*p_bool_funtion_uint8_t_p)(uint8_t *);

typedef union 
{
    float    float32;
    uint32_t int32;
}T_FLOAT_INT32_UNION;

// ==========================================================================================================
// ȫ�ֺ궨��
// ==========================================================================================================
// 1�ֽڶ���(��ֲ������ƽ̨ʱ�����������Ҫ�޸ĳ�__align(2)��__align(4))
//#define __ALIGN    __align(1)

// ��������
#define MIN(a,b)   ( (a < b) ? (a) : (b) )
#define MAX(a,b)   ( (a > b) ? (a) : (b) )
#define ABS(a,b)   ( (a > 0) ? (a) : (-a) )

// ����Ԫ�ظ���
#ifndef _countof
#define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif

// �߼�ֵ
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

// ʹ��/��ֹ
#ifndef ENABLE
#define ENABLE  1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif

// ��
#ifndef NULL
#define NULL    0
#endif

// ��
#ifndef LOCKED
#define LOCKED      1
#endif
#ifndef UNLOCKED
#define UNLOCKED    0
#endif

// λ�����(Ч�ʺ�ֱ��ʹ��δ������ͬ)
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
#define LED0 GET_BIT_FIELD_8(PORTB).bit0    // ʹ��ʾ��



// ==========================================================================================================
// ģ�鹦�ܿ���
// ==========================================================================================================
#define USE_NOINIT           1  // ʹ��noinit����
#define USE_SPI_FLASH        0  // SPI Flash��д
#define USE_SPI_MANAGER      0  // SPI��д��д������
#define USE_SEG_LED_DISPLAY  0  // ʹ��LED�����
#define USE_IR_SEND          0  // ���ⷢ��
#define USE_IR_RECEIVE       0  // �������


// ���⹦�ܼ��











#endif  // #ifndef __CONFIG_H__







// ==========================================================================================================
// AVR GCC Notes��
// (0). AVR Libc(avr-libc-user-manual.pdf)��.bit_is_set(sfr,bit)
//                                          .bit_is_clear(sfr,bit)
//                                          .֧�ֶ�Flash�洢���ķ���(AVR��Ƭ��ԭ����GCC���ʵ��.P111)
//                                          .֧�ֶ�EEPROM�ķ���(AVR��Ƭ��ԭ����GCC���ʵ��.P112)
//                                          .֧�ֶ�WDT�Ĳ���(AVR��Ƭ��ԭ����GCC���ʵ��.P114)
//                                          .������ѧ��(AVR��Ƭ��ԭ����GCC���ʵ��.P115)
// 
// (1). ��Ҫʹ��volatile���εĵط���.Ӳ���Ĵ���(��PINA)
//                                  .�ж���ʹ�õ���ȫ�ֱ���
//                                  .�ж���ʹ�õ��ı�static���εľֲ�����
//                                  .���߳��б����������ı���
// 
// (2). �жϣ�
//           ����  �� ʹ�����"ISR(�жϺ�1-21,�ж�����)"�������жϷ�����,��ISR(TIMER0_OVF_vect,ISR_BOLCK){}
//           ����  ��.ISR_BLOCK   = �����жϺ��ֹȫ���ж�,��ֹ���ж�Ƕ��(����ʱ��ָ�����ԡ���Ĭ��ΪISR_BLOCK)
//                   .ISR_NOBLOCK = �����жϺ��ȫ���ж�,�����ж�Ƕ��
//                   .ISR_NAKE    = �������������ж��ֳ������ͻָ�����,�û��Լ������ͻָ�,��ʹ��reti()����
//                   .ISR_ALIASOF(�жϺ�) = ���жϺ������ӵ���һ���жϺ���(GCC4.2����)
//           ���ȼ��� �ж�������ַ(�жϺ�)Խ��,���ȼ�Խ��
//           ˵��  ��.�жϺ������ܱ�������������
//                   .cli() = �ر�ȫ���ж�
//                   .sei() = ����ȫ���ж�
//                   .ʹ��ISR(BADISR_vect){}����δ�����жϺ������ж�,����ϵͳ��ִ�е�����ĳ���(�縴λ)
//                   .�������interrupt.h�ſ���ʹ��ISR()�������жϲ���������Ϊ�ж��Ǳ������Լ�ʵ�ֵ�
//                    �ɰ�GCC-AVRʹ��signal.h�е�SIGNAL(SIG_INTERRUPT0)
//                    ICC-AVR��д���ǣ�#pragma interruput_handler TIM1_OVF: 6
// 
// (3). ����ǿ��ת����.�޷��������з�����һ���������ʱ��Ҫǿ��ת�����������������ʽת�����ܵ���Ī��������
//                    .�����Ĳ����ͷ���ֵ������ǳ�����Ҫǿ��ת�����������������ʽת�����ܵ���Ī��������
//                    .�޷���unsinged����Ҫ�Ӻ�׺"U"
//                    .ʹ�ð�λȡ��"~"������"<<"����unsigned char��unsigned shortʱ,Ӧ���������ǿ��ת��Ϊunsigned char��unsigned short
// 
// (4). �����������ĺ�ʹ��do{}while(0)��#define SWAP(a,b) do{int temp; temp = a; a = b; b = temp;}while(0)
//                                        Ŀ���������ú��ʱ����Լ���һ���ֺ�":"
//                                        ����if
//                                              ...
//                                              SWAP(a,b);
//                                              ...
//                                            else
//                                              ...
//                                        �������Ϊ#define SWAP(a,b) {int temp; temp = a; a = b; b = temp;}
//                                        ��ôif��else֮����һ���ֺ�":"����if��else������
// 
// ==========================================================================================================
// 
// ==========================================================================================================
// AVR GCC Ƕ����ķ�ʽ��
// (1). asm("nop");
//      asm("sei");    ��ȫ���ж�
//      asm("cli");    ��ȫ���ж�
//      asm("sleep");  ����
// 
// (2). asm volatile( "not\n\t"
//                    "not\n\t"
//                    "not\n\t"
//                  ::); // ����ð�ſ���ʡ��
// ==========================================================================================================
// 
// ==========================================================================================================
// AVR GCC�Ĵ����(AVR��Ƭ��ԭ����GCC���ʵ��.P117)��
// (1). .text�Σ�.text = .init[9:0] + .fini[9:0] + ֻ������
//               .init[9:0]�Σ�.�����˴�оƬ��λ������main������һʱ���ڵĲ���,��ʵ������������,
//                             .ִ��˳�� = init0 -> init9
//               .fini[9:0]�Σ�.�������˳�main������(�����exit()����ʱ)��ʼִ�еĴ���
//                             .ִ��˳�� = fini9 -> fini0
//
// (2). .data�Σ�.�����ж���ġ���ֵ!=0��ȫ�ֱ����;�̬����
//
// (3). .bss�Σ�.��ֵ=0��û�г�ֵ��ȫ�ֱ����;�̬����,��.init4�׶α���0
//              .���Խ�����������.bss��.noinit��,����εı�����.init4�׶β��ᱻ��0
//              .��Ƭ�������縴λ����Щ������ֵ��Ȼ����,���Լ���ʹ��
//              .noinit�εĶ��巽����int bar __attribute__ ((section (".noinit")));
//              .����ָ����ֵ,��Ϊ��Щ��������.bss��
//
// (4). .eeprom�Σ�.�洢��EEPROM��
//                 .���巽����uint8_t eep_buffer[10] __attribute__ ((section(".eeprom"))) = {1,2,3};
//                            #include <avr/eeprom.h>
//                            uint8_t eep_buffer[10] EEMEM = {1,2,3};
// 
// ==========================================================================================================
// 


























