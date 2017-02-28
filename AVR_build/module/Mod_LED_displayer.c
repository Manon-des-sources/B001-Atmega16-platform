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
// 本文定义了Atmega16下的8段LED数码管驱动
// 文中对应的驱动电路：http://blog.csdn.net/manon_des_source/article/details/51783675
// 
// --------------------------------------
// 包含：
// 
// ==========================================================================================================
#include "Mod_LED_Displayer.h"



#if USE_SEG_LED_DISPLAY  // #include "config.h"



// 段码(共阴 == 高电平点亮)
static const uint8_t segment_code[17]=
{
    0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,  // 0 - 9
    0x77,0x7c,0x39,0x5e,0x79,0x71,                      // A - F(与之相与来得到某1位数码管的数值)
    0x00                                                // 全部熄灭(与之相或来熄灭当前选中的数码管的显示)
};

// 位码(低电平使能)(使用74HC138选通位选)
// 分别使能第0号到第7号数码管(与之相与(&)来使能)
static const uint8_t segment_index[8]= { 0,1,2,3,4,5,6,7 };

typedef struct 
{
    uint8_t *seg_index; // 位选端口的数据
    uint8_t *seg_code;  // 段选端口的数据
    uint8_t  index;     // 使能第index数码管

    uint8_t  format;    // 数据的显示格式、范围：[2,16]
    uint8_t  set_format;// 需要设置新的显示格式
    uint8_t  set_data;  // 有新的显示数据
    uint8_t  data_part[_countof(segment_index)];  // 送给[0:7]号数码管去显示的8个数据
    uint8_t  data_index;// 拆分data到data_part[]时、作为data_part的下标使用
    uint32_t data;      // 用于显示的数据，它将被拆分到data_part[]去保存
    uint32_t data_copy; // 数据备份、重置format时需要使用
}T_SEG_LED_DISPLAY_CTRL,*pT_SEG_LED_DISPLAY_CTRL;

static T_SEG_LED_DISPLAY_CTRL LED_display_ctrl = {  .seg_index = (uint8_t *)(&PORTA),
                                                    .seg_code  = (uint8_t *)(&PORTB),
                                                    .index = 0,

                                                    .format     = 10,   // 默认使用十进制格式显示
                                                    .set_format = FALSE,
                                                    .set_data   = FALSE,
                                                    .data_part  = { 0,0,0,0,0,0,0,0 },
                                                    .data_index = 0,
                                                    .data       = 0,
                                                    .data_copy  = 0
                                                  };
static pT_SEG_LED_DISPLAY_CTRL p_LED_display_ctrl = &LED_display_ctrl;


// ==========================================================================================================
// LED数码管硬件初始化
// 
// ==========================================================================================================
void Mod_LED_display_init(void)
{
    // ----------
    // 硬件初始化
    // 数码管位选使能(74HC138芯片使能)：输出高电平
    DDRC  |= (IO_OUTPUT << DDC7);
    PORTC |= (IO_OUTPUT << PC7 );
    // 段选控制：PORTB初始化为：输出低电平
    DDRB  = 0xFF;
    PORTB = 0x00;
    // 位选控制：PORTA[2:0]初始化为：输出低电平(选中第0号数码管)
    DDRA  |=   (IO_OUTPUT << DDA0) |(IO_OUTPUT << DDA1) | (IO_OUTPUT << DDA2);
    PORTA &= ~((1 << PA0 ) |(1 << PA1 ) | (1 << PA2 ));

    Drv_IO_mode_bit(DDRD, DDD3, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD3);
    
    // --------------
    // 消息缓存初始化
}

// ==========================================================================================================
//   拆分8位数据到data_part[](从第0号数据位置开始计算)
// 
// 说明：
// (1). 每次只拆分1位，数据中超过8位的部分会自动被截断，所以二进制格式时只会截取低8位
// (2). 实际上、每当data更新时、都需要重新计算8位数组p_LED_display_ctrl->data_part内的值
// (3). 如果data是3位数(如120)，就只需要计算3次，也就会出现3次较为耗时的计算
//      data=0后就不再进行耗时的计算，而是直接给数组p_LED_display_ctrl->data_part中对应的元素赋0值。
// (4). 如果data为0或没有变化，就不需要计算
// 
// ==========================================================================================================
bool Mod_LED_display_set_data(uint8_t format)
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
// LED数码管显示数据的刷新
// 
// ==========================================================================================================
void Mod_LED_display_update(void)
{
    // 熄灭当前数码管
    *p_LED_display_ctrl->seg_code = segment_code[_countof(segment_code) - 1];

    // 切换到下1个数码管
    p_LED_display_ctrl->index++;
    if(p_LED_display_ctrl->index > (_countof(segment_index) - 1))
    {
        p_LED_display_ctrl->index = 0;
    }
    // 修改位选、修改显示
    *p_LED_display_ctrl->seg_index |= segment_index[_countof(segment_index) - 1];
    *p_LED_display_ctrl->seg_index &= segment_index[p_LED_display_ctrl->index];
    *p_LED_display_ctrl->seg_code   = segment_code[p_LED_display_ctrl->data_part[segment_index[p_LED_display_ctrl->index]]];
}

// ==========================================================================================================
// LED数码管刷新任务(硬件接口)
// 
// 查询消息：EVENT_SEG_UPDATE
//           EVENT_DIGITAL_FORMAT
// 消息参数：32位数值
// 发送消息：无
// 
// 说明：
// (1). 在系统定时器或任务调度器中定时刷新(被作为1个任务去调度)
// 
// ==========================================================================================================
void task_LED_display(void)
{
    uint32_t temp = 0;

    Drv_IO_toggle_bit(PORTD, PD3);

    // ---------------------------------------------------
    // 消息查询
    if(TRUE == sys_event_data(EVENT_SEG_UPDATE, &temp))
    {
        p_LED_display_ctrl->set_data   = TRUE;  // 如果得到更新的数据、就启动数据拆分
        p_LED_display_ctrl->data_index = 0;     // 如果正在拆分过程中、又一次需要拆分，就需要重新设置.data_index为0
        p_LED_display_ctrl->data       = temp;
        p_LED_display_ctrl->data_copy  = temp;
    }
    if(TRUE == sys_event_data(EVENT_DIGITAL_FORMAT, &temp))
    {
        p_LED_display_ctrl->set_format = TRUE;
        p_LED_display_ctrl->format     = temp;
    }

    // ---------------------------------------------------
    // 任务正文
    // --------
    // 拆分数据
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
    // 刷新显示
    Mod_LED_display_update();

    // ---------------------------------------------------
    // 消息发送
}

// ==========================================================================================================
// LED数码管显示(上层接口)
// 
// ==========================================================================================================
void Mod_LED_display(uint32_t data)
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
















