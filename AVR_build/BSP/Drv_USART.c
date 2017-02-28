// ==========================================================================================================
// Copyright (c) 2017 ChenChen <codingmanon@163.com>
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
// ---------------------------------------
// ���Ķ�����Atmega16�µ�USART����/��������
// 
// ---------------------------------------
// ������
// 
// ==========================================================================================================
#include "Drv_USART.h"



// ==========================================================================================================
// USART ��ʼ��
// 
// ������parity         ��żУ��(USART_PARITY_NONE,USART_PARITY_ODD,USART_PARITY_EVEN)
//       char_size      ����λ��(�ַ�����)(USART_CHAR_SIZE_9:USART_CHAR_SIZE_5)
//       baud_rate      ������
// 
// ˵����
// (1). Ĭ��ʹ��9λ����λ����9λ��У��λ������Ҫ�ֶ�д�롢������żУ�鼴����Ӳ���Զ�д��
// (2). дUCSRCʱ����Ҫ��URSELλд1
// (3). дUBRRHʱ����Ҫ��URSELλ��0
// (4). дUCSRAʱ����Ҫ��UDREλ��0
// (5). ��ͬ�������£����ٵ����Ȳ����ٵ�С������������ñ���
// (6). 8MHz�¡�115200�����Ϊ-3.5%(����)��Secure CRT�����յ����ݣ����߼������Ǳ�ʾ�յ�����������һ�������Ǵ����
//      �������ʸ��ߵ�250000bps�����Ϊ0���߼������ǿ���ȫ���նԣ�Ҳ����˵���̫��Ϳ��ܵ��´��������Ҫ����
// 
// ==========================================================================================================
void Drv_USART_init(const uint8_t parity, const uint8_t char_size, const uint32_t baud_rate)
{
    uint8_t  mode         = USART_MODE_ASYNC;        // �첽ģʽ
    uint8_t  stop_bit     = USART_STOP_BIT_2;        // ֹͣλ��λ��.USBS
    uint8_t  multi        = USART_MULTI_MCU_DIS;     // ���ͨ��ģʽʹ��λ.MPCM
    uint8_t  double_speed = USART_DOUBLE_SPEED_EN;   // ����ʹ��λ.U2X
    uint8_t  polarity     = USART_POLARITY_RIASE;    // ʱ�Ӽ���.UCPOL

    uint8_t  char_size_2  = (char_size >> 2) & 0x01;
    uint8_t  char_size_10 =  char_size & 0x03;
    uint8_t  div;
    uint16_t ubrr;

    if(USART_MODE_ASYNC == mode)
    {
        polarity     = 0;  // �첽ģʽ�¡�ʱ�Ӽ���.UCPOL�����á���Ҫ��0
        div          = (1 == double_speed) ? 8 : 16 ;
    }
    else
    {
        double_speed = 0;  // ͬ��ģʽ�¡�����.U2X�����á���Ҫ��0
        div          = 2;
    }

    UCSRA = (double_speed << U2X ) |  // ����ʹ��λ.U2X
            (multi        << MPCM) ;  // ���ͨ��ģʽʹ��λ.MPCM

    UCSRC = ( 1              << URSEL) |  // ��URSELλд1
            ( mode           << UMSEL) |  // ͬ��/�첽ģʽ.UMSEL
            ((parity & 0x03) << UPM0 ) |  // ��żУ��.UPM[1:0]
            ( stop_bit       << USBS ) |  // ֹͣλ��λ��.USBS
            ( char_size_10   << UCSZ0) |  // ����λ��.UCSZ[1:0]
            ( polarity       << UCPOL) ;  // ʱ�Ӽ���.UCPOL

    // ������ٻ��Ͼ������ֵ
    UCSRB &= ~(1           << UCSZ2);
    UCSRB |=  (char_size_2 << UCSZ2); // ����λ��UCSZ2

    // ���ò�����
//  ubrr = (SYS_OSC_FREQUENCE / (div * baud_rate)) - 1;
    ubrr = (2 * SYS_OSC_FREQUENCE + div * baud_rate) / (2 * div * baud_rate) - 1;  // ����һ��Ļ�����+0.5����ʵ����������
    UBRRH = ((~(1 << URSEL)) & ((ubrr >> 8) & 0x000F)) & 0x00FF;  // ��Ҫ��URSELλ��0
    UBRRL = ubrr & 0x00FF;
}

// ==========================================================================================================
// USART �ж�ʹ��
// 
// ������int_mode = USART_INT_MODE_RXC,USART_INT_MODE_TXC,USART_INT_MODE_UDR
//       enable   = ENABLE,DISABLE
// 
// ==========================================================================================================
void Drv_USART_INT_Enable(const uint8_t int_mode, const uint8_t enable)
{
    if(USART_INT_MODE_RXC == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << RXCIE);
        }
        else
        {
            UCSRB |=  (1 << RXCIE);
        }
    }
    if(USART_INT_MODE_TXC == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << TXCIE);
        }
        else
        {
            UCSRB |=  (1 << TXCIE);
        }
    }
    if(USART_INT_MODE_UDR == int_mode)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << UDRIE);
        }
        else
        {
            UCSRB |=  (1 << UDRIE);
        }
    }
}

// ==========================================================================================================
// USART �շ������������ֹ
// 
// ������transfer = �������������(USART_TRANSFER_RX,USART_TRANSFER_TX)
//       enable   = ENABLE,DISABLE
// 
// ˵����
// (1). ��ֹ��������R-UDR����ˢ�¡���FE/DOR/PE��־����Ч��ͬʱRxD���Żָ�Ϊͨ��IO
// (2). ��ֹ���������������ݶ��ѷ�����Ϻ�(T-UDR�ͷ�����λ�Ĵ���)����������Ӳ���Ż�ֹͣ
// (3). ��ֹ��������TxD���Ž���ָ�ΪΪͨ��IO
// 
// ==========================================================================================================
void Drv_USART_transfer_start(const uint8_t transfer, const uint8_t enable)
{
    if(USART_TRANSFER_RX == transfer)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << RXEN);
        }
        else
        {
            UCSRB |=  (1 << RXEN);
        }
    }
    if(USART_TRANSFER_TX == transfer)
    {
        if(DISABLE == enable)
        {
            UCSRB &= ~(1 << TXEN);
        }
        else
        {
            UCSRB |=  (1 << TXEN);
        }
    }
}
