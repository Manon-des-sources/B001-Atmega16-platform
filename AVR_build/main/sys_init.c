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
// ���Ķ����˱���Ŀ�ĳ�ʼ��ģ��
// 
// ----------------------------
// ������
// 
// ==========================================================================================================
#include "sys_init.h"


// ==========================================================================================================
//  α�ж�BADISR_vect
// .���ڲ���δ�����жϺ������ж�
//  û�а���interrupt.h���ļ��е�ISR()������Ϊδ�����ISR(),����ISR()���Ա�BADISR_vect���α�жϲ���
// 
// .û����������Ļ�,��û�ж����жϺ������жϵ���ʱ,ϵͳ��ִ�е�����ĳ���(�縴λ) |<-------------- ��Ҫ�����������
// 
// ==========================================================================================================
ISR(BADISR_vect)
{
    PORTA ^= (1 << PA7);
}

// ==========================================================================================================
//      ��鸴λԴ
// (1). �����λ��Ϣ,�ȴ���¼��һ�θ�λ
// ==========================================================================================================
void get_reset_source(void)
{
    // �ϵ縴λ
    if( MCUCSR &   (1 << PORF) )
    {
        MCUCSR &= ~(1 << PORF);
    }
    // �ⲿ��λ
    if( MCUCSR &   (1 << EXTRF) )
    {
        MCUCSR &= ~(1 << EXTRF);
    }
    // BOD���縴λ
    if( MCUCSR &   (1 << BORF) )
    {
        MCUCSR &= ~(1 << BORF);
    }
    // WDT��λ
    if( MCUCSR &   (1 << WDRF) )
    {
        MCUCSR &= ~(1 << WDRF);
    }
    // Jtagָ��AVR_RESET��λ
    if( MCUCSR &   (1 << JTRF) )
    {
        MCUCSR &= ~(1 << JTRF);
    }
}

// ==========================================================================================================
// ϵͳ��ʼ��
// 
// (1). ֻ��ʼ����ϵͳ��صĵط�
// (2). ������صĵط��������Լ����ʼ��
// 
// ==========================================================================================================
void sys_init(void)
{
    // -------
    //ʱ��У׼
    Drv_sys_write_OSCCAL();

    // --------
	// IO��ʼ��
	// PORTA��ʼ��Ϊ����͵�ƽ
	DDRA  = 0xFF;
    PORTA = 0x00;
	// PORTB��ʼ��Ϊ����ߵ�ƽ(�����˵��Ƶ�LED)
    DDRB  = 0xFF;
    PORTB = 0xFF;
	// PORTC��ʼ��Ϊ����͵�ƽ
	DDRC  = 0xFF;
    PORTC = 0x00;
	// PORTD��ʼ��Ϊ����͵�ƽ
	DDRD  = 0xFF;
    PORTD = 0x00;   // PD3�Ѿ�����Ƶĺ���LED�Ͽ�


    // PA7��ʼ��Ϊ���0(��BADISR_vectʹ��)
    Drv_IO_mode_bit(DDRA, DDA7, IO_OUTPUT);
    Drv_IO_clr_bit(PORTA, PA7);


    // ϵͳ��Ϣ/�¼�����
    sys_event_init();
    // �������ݸ�ʽ
    Drv_sys_set_digital_format(10);

    // ����ϵͳ������־(�������������EVENT_MAX���滻ΪEVENT_SYS��sys_update_event(EVENT_SYS, ..)�ſ��Թ���)
    sys_event_post(EVENT_SYS, 0);
    sys_update_event(EVENT_SYS, MSG_SYS_START, 0);

    // SPI Flash���������ϵ�10ms�����֮��ſ�����������
    delay_ms(100);

    // ��ʾ����汾��Ϣ
//  printf("%s\r\n",SYS_VERSION);
}




