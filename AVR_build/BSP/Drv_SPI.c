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
// ---------------------------------
// ���Ķ�����Atmega16�µ�SPI��ʼ��
// 
// ==========================================================================================================
#include "Drv_SPI.h"


// ==========================================================================================================
// SPI��ʼ��
// 
// ������device_mode    ����ģʽ(������SPI_MODE_MSTR, �ӻ���SPI_MODE_SLAVE) 
//       shift_mode     ����ģʽ(����/��λģʽ)
//       shift_first    �����Ƴ�MSB(����)����LSB(����)
//       clk_source     SCLKƵ��
// 
// ˵����
// (1). ��MSTR=1 && SS����Ϊ����IOʱ�����SS���ű����ͣ��ᵼ��MSTR=0 && SPIF=1��ʹ�û�����ɴӻ�ģʽ
// 
// ==========================================================================================================
void Drv_SPI_init(const uint8_t device_mode, const uint8_t shift_mode, const uint8_t shift_first, const uint8_t clk_source)
{
    uint8_t spr10;
    uint8_t spi2x;
    uint8_t cpol;
    uint8_t cpha;

    switch(shift_mode)
    {
        case SPI_SHIFT_MODE_00: cpol = 0; cpha = 0;
                                break;
        case SPI_SHIFT_MODE_01: cpol = 0; cpha = 1;
                                break;
        case SPI_SHIFT_MODE_02: cpol = 1; cpha = 0;
                                break;
        case SPI_SHIFT_MODE_03: cpol = 1; cpha = 1;
                                break;
        default : return;
    }
    switch(clk_source)
    {
        case SPI_CLK_SOURCE_DIV_2:   spr10 = 0; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_4:   spr10 = 0; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_8:   spr10 = 1; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_16:  spr10 = 1; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_32:  spr10 = 2; spi2x = 1;
                                     break;
        case SPI_CLK_SOURCE_DIV_64:  spr10 = 2; spi2x = 0;
                                     break;
        case SPI_CLK_SOURCE_DIV_128: spr10 = 3; spi2x = 0;
                                     break;
        default : return;
    }

    // IO����
    if(SPI_MODE_MSTR == device_mode)
    {
        Drv_IO_set_bit(PORTB, PB4);
        Drv_IO_mode_bit(DDRB, PB4, IO_OUTPUT); // SS���(����)���Խ�ֹSPI�ӻ�
        Drv_IO_clr_bit(PORTB, PB5);
        Drv_IO_mode_bit(DDRB, PB5, IO_OUTPUT); // MOSI���(����)
        Drv_IO_clr_bit(PORTB, PB6);
        Drv_IO_mode_bit(DDRB, PB6, IO_INPUT);  // MISO����(����)
        Drv_IO_clr_bit(PORTB, PB7);
        Drv_IO_mode_bit(DDRB, PB7, IO_OUTPUT); // SCLK���(����)
    }
    else
    {
        Drv_IO_set_bit(PORTB, PB4);
        Drv_IO_mode_bit(DDRB, PB4, IO_INPUT);  // SS����(����|���PUDʹ�ܡ���������)
        Drv_IO_clr_bit(PORTB, PB5);
        Drv_IO_mode_bit(DDRB, PB5, IO_INPUT);  // MOSI����(����)
        Drv_IO_clr_bit(PORTB, PB6);
        Drv_IO_mode_bit(DDRB, PB6, IO_OUTPUT); // MISO���(����)
        Drv_IO_clr_bit(PORTB, PB7);
        Drv_IO_mode_bit(DDRB, PB7, IO_INPUT);  // SCLK����(����)
    }

    // ��ʼ��
    SPCR = ( 1 << SPE ) |
           ((shift_first & 0x01) << DORD) | // �ȷ���MSB��LSB
           ((device_mode & 0x01) << MSTR) | // ����ģʽ
           ((cpol        & 0x01) << CPOL) | // ʱ�Ӽ���
           ((cpha        & 0x01) << CPHA) | // ǰ����
           ((spr10       & 0x03) << SPR0) ; // ʱ��Ƶ��

    SPSR &= ~(1     << SPI2X);
    SPSR |=  (spi2x << SPI2X);              // ʱ�ӱ�Ƶ
}

// ==========================================================================================================
// SPI��������ж�ʹ��
// 
// ==========================================================================================================
void Drv_SPI_INT_Enable(const uint8_t enable)
{
    if(DISABLE == enable)
    {
        SPCR &= ~(1 << SPIE);
    }
    else
    {
        SPCR |=  (1 << SPIE);
    }
    Drv_SPI_clr_SPIF();
}

// ==========================================================================================================
// ����жϱ�־λSPIF
// 
// ˵����
// (1). SPIF�ڽ���ISRʱ�Զ���0�����ȶ�SPSR���ڷ���SPDR������0
// (2). ����һ������ֵ��Ϊ�˱������������δ����Ż�������Ϊ�Ż������ܻ���Ϊ��ζ�����ⲿ���κ�Ӱ��
// 
// ==========================================================================================================
uint8_t Drv_SPI_clr_SPIF(void)
{
    uint8_t temp;

    temp  = SPSR;
    temp += SPDR;

    return temp;
}

// ==========================================================================================================
// ������д��SPDR�����ȴ�д�����
// 
// ˵����
// (1). ʹ����������������ֹSPI�жϣ������жϻ��Զ���0 SPIF������������Զ�Ȳ���SPIF=0��ʱ��
// 
// ==========================================================================================================
void Drv_SPI_write_byte(const uint8_t data)
{
    Drv_SPI_clr_SPIF();
    SPDR = data;
    while(0 == (SPSR & (1 << SPIF)))
    {}
}

// ==========================================================================================================
// ��ȡSPDR�е�����
// 
// ˵����
// (1). ���������������SPI_COMMAND_DUMMYȥ����SCLK����ӻ���������
// 
// ==========================================================================================================
uint8_t Drv_SPI_read_byte(const uint8_t dummy)
{
    uint8_t data;

    Drv_SPI_write_byte(dummy);
    data = SPDR;

    return data;
}
