首先、我们做一个print()函数，支持最基本的格式输出：%d,%c,%s。<br>

一、操作代码：<br>
1、使用环形buffer，用如下结构进行写入和读出控制：<br>
```c
#define PRINT_RING_BUFFER_MAX   64
typedef struct
{
    uint8_t in;     // 写指针
    uint8_t out;    // 读指针
    uint8_t num;    // 已写入的数据量
}T_PRINT_CTRL;
T_PRINT_CTRL print_ctrl = { .in  = 0,
                            .out = 0,
                            .num = 0
                          };
uint8_t print_ring_buffer[PRINT_RING_BUFFER_MAX];
```
代码来源：http://blog.csdn.net/aizhiqing86/article/details/8570804<br>
这种操作方式省去了使用in和out来判断空和满的方法、而是直接可读出数据长度。<br>

buffer操作函数：<br>
```java
// ==========================================================================================================
// print_ctrl_add() 读写指针自加移动控制
//
// 参数：p          读写指针的值
//
// ==========================================================================================================
uint8_t print_ctrl_add(uint8_t p)
{
    return (PRINT_RING_BUFFER_MAX == (p + 1)) ? 0 : (p + 1);
}

// ==========================================================================================================
// print_data_in()  向缓冲区写入1个数据
//
// 参数：data       数据值
//
// 返回值：Fin      TRUE = 数据写入成功、FALSE = buffer已满
//
// ==========================================================================================================
bool print_data_in(uint8_t data)
{
    bool Fin = TRUE;

    if(print_ctrl.num < (PRINT_RING_BUFFER_MAX + 1))
    {
        print_ring_buffer[print_ctrl.in] = data;
        print_ctrl.in = print_ctrl_add(print_ctrl.in);
        print_ctrl.num++;
    }
    else
    {
        Fin = FALSE;
    }
    return Fin;
}

// ==========================================================================================================
// print_data_out() 从缓冲区读出1个数据
//
// 参数：*data      用于接收数据值
//
// 返回值：Fin      TRUE = 数据读出成功、FALSE = buffer为空
//
// ==========================================================================================================
bool print_data_out(uint8_t *data)
{
    bool Fin = TRUE;

    if(print_ctrl.num > 0)
    {
       *data = print_ring_buffer[print_ctrl.out];
        print_ctrl.out = print_ctrl_add(print_ctrl.out);
        print_ctrl.num--;
    }
    else
    {
        Fin = FALSE;
    }
    return Fin;
}
```
2、print()函数：<br>
```java
// ==========================================================================================================
// print_buffer_data() 拆分十进制整数数据的每一位到数据buffer
//
// 参数：data       整数数据
//
// ==========================================================================================================
void print_buffer_data(uint32_t data)
{
    uint8_t index = 0;
    temp2017[4] = (uint8_t)data;

    for(index = 0; 0 != data; index++)
    {
        data_part[index] = (uint8_t)(data % 10);
        data -= (uint32_t)data_part[index];
        data /= 10;
    }
    do
    {
        index--;
        print_data_in(data_part[index] + '0');
    }while(0 < index);
}

// ==========================================================================================================
// print_buffer_data() 组织字符数据到数据buffer
//
// 参数：data       字符数据
//
// ==========================================================================================================
void print_buffer_char(char data)
{
    temp2017[4] = data;
    print_data_in(data);
}

// ==========================================================================================================
// print_buffer_string() 组织字符串到数据buffer
//
// 参数：data       字符串地址
//
// ==========================================================================================================
void print_buffer_string(char *data)
{
    temp2020 = (uint8_t *)data;
    while('\0' != *data)
    {
        print_data_in((uint8_t)*data);
        data++;
    }
}

    va_list  ap     = NULL;  // 方便查看ap值，调试接受后哟啊放回到print()函数去
// ==========================================================================================================
// print()
//
// 参数：fmt        格式控制字符串
//
// 说明：
// (1). 通过格式控制字符串fmt可以确定有多少个参数
// (2). 支持格式：%d(32位),%c,%s，可以带空格、但不能在%和d/c/s之间，举例：print("a = %d\n",a);
//      不要支持%5d这类带宽度的格式、否则buffer很可能不够用
// (3). 不要在中断中或抢占式任务中使用这个print函数，因为它是不可重入的，会修改全局的buffer
//
// ==========================================================================================================
void print(char fmt[], ...)
{
    uint8_t *format = (uint8_t *)fmt;
    uint8_t  number = 0;

    va_start(ap, fmt);

    while('\0' != *format)
    {
        temp2019 = format;

        if('%' == *format)
        {
            format++;
            switch(*format)
            {
                case 'd':print_buffer_data((uint32_t)va_arg(ap, int));
                         break;
                case 'c':print_buffer_char((char)va_arg(ap, int));
                         break;
                case 's':print_buffer_string((char *)va_arg(ap, char *));
                         break;
                default: va_arg(ap, int);  // 跳过这个参数、它的格式设置不对，结果就是这个参数的具体值没有被存下来
                         print_buffer_char(*format);
                         break;
            }
        }
        else
        {
            print_buffer_char(*format);
        }
        format++;
        number++;
    }
    // 触发数据发送(如果发送器正在发送，这个触发并不会影响正在发送的操作，但会修改发送数据控制)
    if(0 < number)
    {
        Mod_USART_transfer(print_data_out);
    }
    va_end(ap);
}
```

3、发送器使用print提供的print_data_out()来读取数据：<br>
```java
// ==========================================================================================================
// USART 启动数据发送
//
// 参数：data           数据buffer
//
// ==========================================================================================================
void Mod_USART_transfer(p_bool_funtion_uint8_t_p fun)
{
    if(NULL != fun)
    {
        transfer_ctrl.end    = FALSE;
        transfer_ctrl.fun    = fun;
        Drv_USART_transfer_start(USART_TRANSFER_TX, ENABLE);
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, ENABLE);
    }
}

// ==========================================================================================================
// USART 发送缓冲寄存器为空中断
//
// ==========================================================================================================
ISR(USART_UDRE_vect)
{
    if(FALSE == transfer_ctrl.fun((uint8_t *)&UDR))
    {
        transfer_ctrl.end = TRUE;
        Drv_USART_INT_Enable(USART_INT_MODE_UDR, DISABLE);
        Drv_USART_transfer_start(USART_TRANSFER_TX, DISABLE);
    }
}
```

二、测试：<br>
0、测试代码：<br>
```java
char buffer_usart[] = "123456\n\r";

// ==========================================================================================================
// USART 测试模块
//
// 说明：
// (1). 每隔4ms被调度运行1次
//
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 0;
    switch(step)
    {
        case 0: print("out1 = %d, out2 = % s\r\n", 5, buffer_usart);
                step = 1;
                break;
        case 1: print("abcdef\r\n");
                step = 2;
                break;
        case 2: print("out0 = %c\r\n", 'A');
                step = 100;
                break;
        default:break;
    }
}
```
1、函数的参数是按照从右到左的顺序依次入栈的<br>
运行完va_start(ap, fmt)之后，ap = 0x0451：<br>
![ap_0451](/lessons/1010-USART/1002-print/material/ap_0451.png)<br>

print(char fmt[], ...)里目的参数列表'...'中的第一个参数(也就是fmt后面的第一个参数)被放在地址为0x0451的堆栈里面：<br>
![stack_0451](/lessons/1010-USART/1002-print/material/stack_0451.png)<br>
它的值是0x0005，这就是测试函数print("out1 = %d, out2 = % s\r\n", 5, buffer_usart); 里面的常数5，<br>
它正好是fmt字符串后面的第一个参数。<br>

接着我们来看常数5前后的两个参数。<br>
首先、参数buffer_usart 在内存中的地址为0x0098：<br>
![buffer_usart](/lessons/1010-USART/1002-print/material/buffer_usart.png)<br>

再者、字符串fmt在内存中的地址为0x0064(对应第一个字符 'o' )：<br>
![fmt](/lessons/1010-USART/1002-print/material/fmt.png)<br>

而堆栈里面、常数5被放在0x0451地址处，它的上一个地址处存放的值是0x0098，正好是变量buffer_usart的地址，<br>
它对应着一个指向变量buffer_usart的指针，<br>
它作为一个char 指针，被放在print("out1 = %d, out2 = % s\r\n", 5, buffer_usart); 的最右边。<br>
常数5的下一个地址处存放的值是0x0064，正好是fmt字符串"out1 = %d, out2 = % s\r\n"的地址，它也是一个char 指针参数。<br>

我们可以得到堆栈里面存放的参数列表如下：<br>
![stack_3](/lessons/1010-USART/1002-print/material/stack_3.png)<br>

这告诉我们：函数的参数是按照从右到左的顺序依次入栈的，所以总会保证在参数被入栈后、函数的第一个参数在栈顶。<br>
这使得我们可以使用va_arg(ap, int)来依次顺序访问参数列表里面的变量。<br>

另外、我们看到常量5的是作为int变量入栈的，这是因为C语言中的常量都是int类型。<br>

2、环形buffer的赋值情况<br>
print()解析数据之后，会将字符都写入环形buffer。<br>
buffer的地址为0x0116：<br>
![ring_buffer](/lessons/1010-USART/1002-print/material/ring_buffer.png)<br>
最后两个字节0x0D和0x0A就是\r和\n。<br>

发送结束后，print()控制变量in和out再次重合(一开始它们都是0)，同时num=0，表示buffer为空：<br>
![print_ctal](/lessons/1010-USART/1002-print/material/print_ctal.png)<br>

Secure CRT也收到了信息，光标在下一行(0x0D和0x0A(\r和\n)的效果)：<br>
![Secure_2](/lessons/1010-USART/1002-print/material/Secure_2.png)<br>
但是这里的buffer_usart没有被发送出去，只发送了fmt格式字符串里面的out2 =  s\r\n。<br>
这是因为out2 = % s\r\n里面，%和s之间多了一个空格，格式不对，所以print()忽略了这个%格式控制符对应的参数，<br>
这正是print里面default: va_arg(ap, int);这一句作用，专门处理不正确的格式。<br>
为了简化print()代码、这里只支持这种写法，如有需要、以后会进行改进<br>

逻辑分析仪也正确的收到并解析出了这个环形buffer里面的数据：<br>
![Logic_2](/lessons/1010-USART/1002-print/material/Logic_2.png)<br>

3、参数列表入栈情况确实表明参数都被提升为int型数据<br>
测试代码：<br>
```java
char buffer_usart[] = "s123456";
char a = 'c';
unsigned char b = 65;
uint16_t c = 12345;
uint32_t d = 123456798;
```
ap值：<br>
![ap_0449](/lessons/1010-USART/1002-print/material/ap_0449.png)<br>

参数入栈情况：<br>
![stack_0449](/lessons/1010-USART/1002-print/material/stack_0449.png)<br>
![stack_5](/lessons/1010-USART/1002-print/material/stack_5.png)<br>

变量d由123456798=0x075BCD1E作为32位int型数据、可以完整的传入。<br>
而我们在代码里面只读出了它的低16位：
```java
case 'd':print_buffer_data((uint32_t)va_arg(ap, int));  // int是16位、long才是32位
         break;
```
并将其转换为了32位的uint32_t 数据，而int 型的0xCD1E最高位为1，是负数，<br>
所以转换后的结果为0xFFFFCD1E，就得到Secure CRT收到的数据：<br>
![Secure_5](/lessons/1010-USART/1002-print/material/Secure_5.png)<br>
下一个版本会改进成32位的%d格式输出。<br>

这里正好应征了一点：<br>
参数列表'...'中的所有参数都被提升为int型数据、因为它们没有被指定参数的类型。<br>
char型的a和unsigned char型的b都是被提升到int型。<br>
