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
// ���Ķ�����Atmega16�µ�8��LED���������
// ���ж�Ӧ��������·��http://blog.csdn.net/manon_des_source/article/details/51783675
// 
// --------------------------------------
// ������
// 
// ==========================================================================================================
#include "Mod_LED_Displayer.h"



#if USE_SEG_LED_DISPLAY  // #include "config.h"



// ����(���� == �ߵ�ƽ����)
static const uint8_t segment_code[17]=
{
    0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,  // 0 - 9
    0x77,0x7c,0x39,0x5e,0x79,0x71,                      // A - F(��֮�������õ�ĳ1λ����ܵ���ֵ)
    0x00                                                // ȫ��Ϩ��(��֮�����Ϩ��ǰѡ�е�����ܵ���ʾ)
};

// λ��(�͵�ƽʹ��)(ʹ��74HC138ѡͨλѡ)
// �ֱ�ʹ�ܵ�0�ŵ���7�������(��֮����(&)��ʹ��)
static const uint8_t segment_index[8]= { 0,1,2,3,4,5,6,7 };

typedef struct 
{
    uint8_t *seg_index; // λѡ�˿ڵ�����
    uint8_t *seg_code;  // ��ѡ�˿ڵ�����
    uint8_t  index;     // ʹ�ܵ�index�����

    uint8_t  format;    // ���ݵ���ʾ��ʽ����Χ��[2,16]
    uint8_t  set_format;// ��Ҫ�����µ���ʾ��ʽ
    uint8_t  set_data;  // ���µ���ʾ����
    uint8_t  data_part[_countof(segment_index)];  // �͸�[0:7]�������ȥ��ʾ��8������
    uint8_t  data_index;// ���data��data_part[]ʱ����Ϊdata_part���±�ʹ��
    uint32_t data;      // ������ʾ�����ݣ���������ֵ�data_part[]ȥ����
    uint32_t data_copy; // ���ݱ��ݡ�����formatʱ��Ҫʹ��
}T_SEG_LED_DISPLAY_CTRL,*pT_SEG_LED_DISPLAY_CTRL;

static T_SEG_LED_DISPLAY_CTRL LED_display_ctrl = {  .seg_index = (uint8_t *)(&PORTA),
                                                    .seg_code  = (uint8_t *)(&PORTB),
                                                    .index = 0,

                                                    .format     = 10,   // Ĭ��ʹ��ʮ���Ƹ�ʽ��ʾ
                                                    .set_format = FALSE,
                                                    .set_data   = FALSE,
                                                    .data_part  = { 0,0,0,0,0,0,0,0 },
                                                    .data_index = 0,
                                                    .data       = 0,
                                                    .data_copy  = 0
                                                  };
static pT_SEG_LED_DISPLAY_CTRL p_LED_display_ctrl = &LED_display_ctrl;


// ==========================================================================================================
// LED�����Ӳ����ʼ��
// 
// ==========================================================================================================
void Mod_LED_display_init(void)
{
    // ----------
    // Ӳ����ʼ��
    // �����λѡʹ��(74HC138оƬʹ��)������ߵ�ƽ
    DDRC  |= (IO_OUTPUT << DDC7);
    PORTC |= (IO_OUTPUT << PC7 );
    // ��ѡ���ƣ�PORTB��ʼ��Ϊ������͵�ƽ
    DDRB  = 0xFF;
    PORTB = 0x00;
    // λѡ���ƣ�PORTA[2:0]��ʼ��Ϊ������͵�ƽ(ѡ�е�0�������)
    DDRA  |=   (IO_OUTPUT << DDA0) |(IO_OUTPUT << DDA1) | (IO_OUTPUT << DDA2);
    PORTA &= ~((1 << PA0 ) |(1 << PA1 ) | (1 << PA2 ));

    Drv_IO_mode_bit(DDRD, DDD3, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD3);
    
    // --------------
    // ��Ϣ�����ʼ��
}

// ==========================================================================================================
//   ���8λ���ݵ�data_part[](�ӵ�0������λ�ÿ�ʼ����)
// 
// ˵����
// (1). ÿ��ֻ���1λ�������г���8λ�Ĳ��ֻ��Զ����ضϣ����Զ����Ƹ�ʽʱֻ���ȡ��8λ
// (2). ʵ���ϡ�ÿ��data����ʱ������Ҫ���¼���8λ����p_LED_display_ctrl->data_part�ڵ�ֵ
// (3). ���data��3λ��(��120)����ֻ��Ҫ����3�Σ�Ҳ�ͻ����3�ν�Ϊ��ʱ�ļ���
//      data=0��Ͳ��ٽ��к�ʱ�ļ��㣬����ֱ�Ӹ�����p_LED_display_ctrl->data_part�ж�Ӧ��Ԫ�ظ�0ֵ��
// (4). ���dataΪ0��û�б仯���Ͳ���Ҫ����
// 
// ==========================================================================================================
bool Mod_LED_display_set_data(const uint8_t format)
{
    uint8_t temp = 0;

    if(p_LED_display_ctrl->data_index < _countof(segment_index))
    {
        if(0 == p_LED_display_ctrl->data) { temp = 0; }
        else                              { temp = p_LED_display_ctrl->data % format; }

        p_LED_display_ctrl->data_part[_countof(segment_index) - 1 - p_LED_display_ctrl->data_index] = temp;

        if(0 != p_LED_display_ctrl->data) { p_LED_display_ctrl->data = p_LED_display_ctrl->data / format; }

        p_LED_display_ctrl->data_index++;
        return TRUE;
    }
    return FALSE;
}

// ==========================================================================================================
// LED�������ʾ���ݵ�ˢ��
// 
// ==========================================================================================================
void Mod_LED_display_update(void)
{
    // Ϩ��ǰ�����
    *p_LED_display_ctrl->seg_code = segment_code[_countof(segment_code) - 1];

    // �л�����1�������
    p_LED_display_ctrl->index++;
    if(p_LED_display_ctrl->index > (_countof(segment_index) - 1))
    {
        p_LED_display_ctrl->index = 0;
    }
    // �޸�λѡ���޸���ʾ
    *p_LED_display_ctrl->seg_index |= segment_index[_countof(segment_index) - 1];
    *p_LED_display_ctrl->seg_index &= segment_index[p_LED_display_ctrl->index];
    *p_LED_display_ctrl->seg_code   = segment_code[p_LED_display_ctrl->data_part[segment_index[p_LED_display_ctrl->index]]];
}

// ==========================================================================================================
// LED�����ˢ������(Ӳ���ӿ�)
// 
// ��ѯ��Ϣ��EVENT_SEG_UPDATE
//           EVENT_DIGITAL_FORMAT
// ��Ϣ������32λ��ֵ
// ������Ϣ����
// 
// ˵����
// (1). ��ϵͳ��ʱ��������������ж�ʱˢ��(����Ϊ1������ȥ����)
// 
// ==========================================================================================================
void task_LED_display(void)
{
    uint32_t temp = 0;

    Drv_IO_toggle_bit(PORTD, PD3);

    // ---------------------------------------------------
    // ��Ϣ��ѯ
    if(TRUE == sys_event_data(EVENT_SEG_UPDATE, &temp))
    {
        p_LED_display_ctrl->set_data   = TRUE;  // ����õ����µ����ݡ����������ݲ��
        p_LED_display_ctrl->data_index = 0;     // ������ڲ�ֹ����С���һ����Ҫ��֣�����Ҫ��������.data_indexΪ0
        p_LED_display_ctrl->data       = temp;
        p_LED_display_ctrl->data_copy  = temp;
    }
    if(TRUE == sys_event_data(EVENT_DIGITAL_FORMAT, &temp))
    {
        p_LED_display_ctrl->set_format = TRUE;
        p_LED_display_ctrl->format     = temp;
    }

    // ---------------------------------------------------
    // ��������
    // --------
    // �������
    if(TRUE == p_LED_display_ctrl->set_data)
    {
        p_LED_display_ctrl->set_data = Mod_LED_display_set_data(p_LED_display_ctrl->format);
    }
    if(TRUE == p_LED_display_ctrl->set_format)
    {
        p_LED_display_ctrl->set_format = FALSE;
        p_LED_display_ctrl->set_data   = TRUE;
        p_LED_display_ctrl->data_index = 0;
        p_LED_display_ctrl->data = p_LED_display_ctrl->data_copy;
    }

    // --------
    // ˢ����ʾ
    Mod_LED_display_update();

    // ---------------------------------------------------
    // ��Ϣ����
}

// ==========================================================================================================
// LED�������ʾ(�ϲ�ӿ�)
// 
// ==========================================================================================================
void Mod_LED_display(const uint32_t data)
{
    sys_event_post(EVENT_SEG_UPDATE, data);
}



#else   // #if USE_SEG_LED_DISPLAY



void Mod_LED_display_init(void)
{
}
void Mod_LED_display(uint32_t data)
{
}
void task_LED_display(void)
{
}

#endif  // #if USE_SEG_LED_DISPLAY
















