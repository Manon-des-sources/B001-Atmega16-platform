**测试代码：**<br>
```c
// 用于发送的数据
uint8_t buffer_usart[] = "123456";
uint8_t buffer_end[] = "789\n\r";

// USART硬件初始化为奇校验、8bit数据、波特率=19200bps
void Mod_USART_init(void)
{
    Drv_USART_init(USART_PARITY_ODD, USART_CHAR_SIZE_8, 19200);
    Drv_IO_set_bit(PORTD,PD1);
    Drv_IO_mode_bit(DDRD,PD1,IO_OUTPUT);
}

// 打印函数(数据发送)
void print(uint8_t *p_data, uint8_t number)
{
    if((NULL != p_data) && (number > 1))
    {
        print_ctrl.count  = 0;
        print_ctrl.number = number - 1;  // 字符串中的最后一个字节是'\0'，不需要发送
        print_ctrl.p_data = p_data;
        Drv_USART_transfer_start(USART_TRANSFER_TX, ENABLE);
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, ENABLE);
    }
}

// 测试函数、每隔4ms被调度器调用一次
void task_Mode_USART(void)
{
    static uint8_t step = 0;
    switch(step)
    {
        case 0: print(buffer_usart, _countof(buffer_usart));
                step = 1;
                break;
        case 1: print(buffer_end, _countof(buffer_end));
                step = 2;
                break;
        case 2: print(buffer_usart, _countof(buffer_usart));
                step = 3;
                break;
        case 3:
                break;
        default:break;
    }
}

// UDRE中断里面写入数据去发送，最后一个数据写完后就可以关闭UDRE中断和发送器
ISR(USART_UDRE_vect)
{
    UDR = print_ctrl.p_data[print_ctrl.count++];
    if(print_ctrl.count >= print_ctrl.number)
    {
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, DISABLE);
        Drv_USART_transfer_start(USART_TRANSFER_TX, DISABLE);
    }
}
```

**测试结果**<br>
**1、\n\r**<br>
\n和\r的ASCII码是0x0A和0x0D<br>
\n(0x0A) = LF 换行，光标会移到下一行，但不会回到这一行的开头<br>
\r(0x0D) = CR 回车，光标会回到一行开头<br>
只使用\n的情况下，光标会移到下一行，但不会回到这一行的开头，Secure CRT可以看到这样的结果如下：<br>
![\n](/lessons/1010-USART/1001-USART-option/material/n_LF.png)<br>
只使\r的情况下，光标会回到一行开头，导致该行前面部分被修改，光标停在这一行中间如下：<br>
![\r](/lessons/1010-USART/1001-USART-option/material/r_CR.png)<br>
所以在gcc中要同时使用\n\r，结果如下：<br>
![\n\r](/lessons/1010-USART/1001-USART-option/material/nr_LF_CR.png)<br>

**2、UDRE中断**<br>
UDRE使能位在初始化时就是1，所以**只需要启动UDR中断**，就可以直接激发这个中断，进入中断里面去写数据<br>
因此在print()函数中只是启动了发送器并使能UDRE中断，**没有写数据**<br>
写数据的操作是在UDRE中断里面完成的，<br>
在最后一个数据写完后就可以关闭UDRE中断和发送器，发送器会在将最后的数据被发送出去后再停止<br>

**3、逻辑分析仪的测试结果**<br>
•   通道配置：<br>
![Logic option](/lessons/1010-USART/1001-USART-option/material/Logic_option.png)<br>

•  逻辑分析仪收到的数据为：123456789\n\r123456<br>
由于最开始的电平为低电平，所以CH4通道使用高电平触发<br>
![123456789](/lessons/1010-USART/1001-USART-option/material/123456789.png)<br>

•  波特率=19200bps，每个bit是52us：<br>
![1](/lessons/1010-USART/1001-USART-option/material/1.png)<br>

•  0x31的波形<br>
这一帧包含：起始位 + 8bit数据 + 1bit校验位 + 2bit停止位<br>
![0x31](/lessons/1010-USART/1001-USART-option/material/0x31.png)<br>
起始位前面的脉冲的来源：<br>
MCU复位时TxD引脚表现为高阻态、电平由外部电路决定，这里表现为低电平，复位后在代码里面被设置为高电平<br>
停止位后面一直时高电平：<br>
停止位结束后，发送器硬件会置位为高电平，同时我在代码里面关闭了发送器，使得TxD引脚恢复为通用IO，就表现为复位后设置的高电平<br>

•  数据按照LSB优先发送，所以0x31这一帧从起始位开始的8bit数据是0b10001100，而不是0b00110001<br>
•  校验位：使用了奇校验，0x31中由3个1，已经是奇数个1，所以校验位为0，即第9位为0<br>

4、误差<br>
•   8MHz下、115200的误差为-3.5%(倍速)、Secure CRT可以收到数据，但逻辑分析仪表示收到的数据中有一半左右是错误的<br>
•   而波特率更高的250000bps的误差为0，逻辑分析仪可以全部收对<br>
•   也就是说误差太大就可能导致错误，这个需要测试<br>

5、检查寄存器配置函数(Debug下只能看到UCSRA寄存器和UBRRL)<br>
•   波特率设置ok(例如250000bps/0%误差)<br>
•   1bit宽度是4us，从逻辑分析仪可以看出波形ok，同时每个字节数据都没问题：<br>
![width](/lessons/1010-USART/1001-USART-option/material/width.png)<br>

•  倍速设置ok(左图置位、右图清除)<br>
![U2X_set](/lessons/1010-USART/1001-USART-option/material/U2X_set.png)
![U2X_clear](/lessons/1010-USART/1001-USART-option/material/U2X_clear.png)<br>

•  停止位和奇偶校验设置<br>
默认为1bit、但我用了2bit，同时设置的是奇校验、0x31的校验位bit7应是0，结果如下：<br>
![odd_stopv](/lessons/1010-USART/1001-USART-option/material/odd_stop.png)<br>

•  为什么会有2个停止位的需求<br>
应该是延长两帧数据之间的长度，方便接收端、避免接收端速度较慢来你不急接收<br>
1个或2个，甚至1.5个的停止位也能正常收到、除非接收端卡住停止位长度，不然都会收得到<br>
![stop](/lessons/1010-USART/1001-USART-option/material/stop.png)<br>

•  数据宽度设置为9位、同时开启奇校验<br>
结果是：发送器发送了10bit，其中有9bit数据和1bit校验位，起始位和停止位没变<br>
TXB8=0的结果如下：<br>
![TXB8=0_0x031](/lessons/1010-USART/1001-USART-option/material/TXB8=0_0x031.png)<br>
![TXB8=0_0x00A](/lessons/1010-USART/1001-USART-option/material/TXB8=0_0x00A.png)<br>

TXB8=1的结果如下：<br>
![TXB8=1_0x131](/lessons/1010-USART/1001-USART-option/material/TXB8=1_0x131.png)<br>
![TXB8=1_0x139](/lessons/1010-USART/1001-USART-option/material/TXB8=1_0x139.png)<br>

•  数据宽度设置为7位、同时开启奇校验<br>
结果是：发送器发送了8bit，其中有7bit数据和1bit校验位，起始位和停止位没变<br>
结果如下：<br>
![7bit_0x39](/lessons/1010-USART/1001-USART-option/material/7bit_0x39.png)<br>
![7bit_0x0D](/lessons/1010-USART/1001-USART-option/material/7bit_0x0D.png)<br>
