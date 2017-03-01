这是最终的print()函数，函数声明如下：<br>
```java
// 说明：
// (1). 支持格式：%c    char字符
//                %s    char *字符串
//                %d    16位signed int
//                %l    32位signed long int
//                %u    16位unsigned int
//                %U    32位unsigned long int
//                %x    16位int的十六进制格式(负数会保留符号位)
//                %X    32位long int的十六进制格式
// (2). 空格不能在%和d/./x之间，错误举例：print("a = % d\n",a);
// (3). 不要在中断中或抢占式任务中使用这个print函数，因为它是不可重入的，会修改全局的buffer
void print(const char fmt[], ...);
```
完整代码见本目录的Mode_USART_print.c<br>
后续可能会由修改，最新代码见AVR_build/module/Mode_USART_print.c<br>

测试：<br>
1、%c char 字符<br>
分别测试下面代码中的case 0和case1(使用step变量切换)：<br>
```java
char a1 = ' ';
char a2 = ' ';

         char a0[] = { -128, 127, 0, -45, 0xFF, 0, 0x7F, 0x80, '1' };
unsigned char a4[] = { 0, 255 };

         int a = 0xFFFF;
unsigned int b = 65500;
unsigned long int c = 0xFABCDEF1;

// ==========================================================================================================
// USART 测试模块
//
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 1;

    switch(step)
    {
        case 0: print("%c%c%c%c%c%c\n\r",a0[0], a0[1], a0[2], a0[3], a0[4]);
                step = 20;
                break;
        case 1: print("%c%c%c", a4[0], a1, a2);
                a4[0]++;
                a1    = (0 == (a4[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a4[0] % 10)) ? '\n' : ' ';
                step += (0 == a4[0]);
                break;
        case 2: print(" [%d] \r\n",a4[0]);
                step = 20;
                break;

        default:break;
    }
}
```
case 0测试ok，<br>
测试print("%c%c%c%c%c%c\n\r",a, b, c);也ok(从栈空间里面读取了6个int数据)<br>
case 1的结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-c-case_1.png)<br>
将case 1改为如下可测试char a0[]：<br>
```java
        case 1: print("%c%c%c", a4[0], a1, a2);
                a0[0]++;
                a1    = (0 == (a0[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a0[0] % 10)) ? '\n' : ' ';
                step += (-127 == a0[0]);
                break;
```

2、%s char 字符串<br>
测试代码：<br>
```java
         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

print("[%s |%s| ]\n\r",a0, a4);
```
输出为：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-s-s1.png)<br>
Secure CRT在遇到a4[]中的255就不显示了、255及其后面的42(星号)都不显示了<br>
而在逻辑分析仪里面是有这些数据的(a4[5] = 0 = '0'，print()不会发送)：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-s-logic-1.png)<br>
访问SRAM之外的地址时、要确保不会读到寄存器，所以%s有进行地址检查：<br>
```java
                case 's':addr = sram_addr_overflow_check(va_arg(ap, int));
                         print_buffer_string((char *)addr);
                         break;
```
地址检查函数：<br>
```java
// ==========================================================================================================
// sram_addr_overflow_check()
//
// 参数：addr       地址
//
// 说明：
// (1). 有些寄存器读出数据之后就会被清0或者引发其他隐患，所以print()的%s等地方想要进行地址检查
// (2). 地址越界、就赋予RAMSTART(iom16.h)
//
// ==========================================================================================================
uint16_t sram_addr_overflow_check(uint16_t addr)
{
    uint16_t Fin = addr;

    if((addr < RAMSTART) || (RAMEND < addr))
    {
        Fin = RAMSTART;
    }
    return Fin;
}
```
例如使用越界的0x10地址：<br>
```java
print("%s\n\r",0x10);
```
这个地址被重置为SRAM的首地址0x60、于是读到的是SRAM0x60处的数据而不是地址为0x10的寄存器R16：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-s-sram.png)<br>
Secure CRT输出：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-s-s2.png)<br>

3、%d 16位signed int<br>
测试代码：<br>
```java
         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

print("%d,%d,%d,%d,%d,%d,%d\n\r",a5[0], a5[1], a5[2], a5[3], a5[4], a5[5], a5[6]);
print("%d,%d,%d,%d\n\r",a6[0], a6[1], a6[2], a6[3]);
```
%d按照signed int类型(16位)去栈空间读数据，所以是带符号数，同时超过signed int类型(16位)的数据会溢出：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-d-s1.png)<br>
下面全部输出整个unsigned int类型：<br>
```java
         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         int a = 0xFFFF;
unsigned int b = 65500;
unsigned long int c = 0xFABCDEF1;

// ==========================================================================================================
// USART 测试模块
//
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 1;

    switch(step)
    {
        case 0: print("%d,%d,%d,%d\n\r",a6[0], a6[1], a6[2], a6[3]);
                step = 20;
                break;
        case 1: print("%d%c%c", a5[0], a1, a2);
                a5[0]++;
                a1    = (0 == (a5[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a5[0] % 10)) ? '\n' : ' ';
                step += (32767 == a5[0]);
                break;
        case 2: print(" [%d] \r\n",a5[0]);
                step = 20;
                break;

        default:break;
    }
}
```
我们在PC端输出同样的一组数据、用来进行对比，使用VS2012 IDE：<br>
```java
         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

FILE *fp;
int common_test(int data)
{
    unsigned int Fin = 0;
    static unsigned char step = 0;

    switch(step)
    {
        case 0: // 路径请加 \\ 号，如 C:\\Windows\\System32，参数：r（只读）、w（只写）、a（追加）等以及相应的组合
                // 这里使用了a（追加）、因此每次都需要先清除文件再写
                fp=fopen("C:\\Users\\myname\\Documents\\compare_L.txt","w");
                if (fp == NULL)
                {
                    printf("\n\rFalse to open this file\n\r");
                    Fin  = 0xFFFFF;
                    step = 100;
                    break;
                }
                fprintf(fp,"");
                fclose(fp);
                fp=fopen("C:\\Users\\myname\\Documents\\compare_L.txt","a");
                if (fp == NULL)
                {
                    printf("\n\rFalse to open this file\n\r");
                    Fin  = 0xFFFFF;
                    step = 100;
                    break;
                }
                step = 1;
                break;
        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%d  " ,a5[0]);
                printf("...");
                if(0 == (a5[0] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a5[0]++;
                step += (32767 == a5[0]);
                break;
        case 2: // 关闭文件
                fclose(fp);
                printf("\n\rdone\n\r");
                step = 20;
                Fin = 0xFFFFF;
                break;
        default:break;
    }
    return Fin;
}
```
Atmega16的结果将输出到Secure CRT的log文件<br>
而VS2012的结果将输出到compare_L.txt文件<br>
我们使用Beyond Compare软件比较compare_L.txt 和 Secure CRT的log文件：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-d-bc-1.png)<br>
可以看出，二者一致，说明测试ok，%d输出整个signed int数据都是ok的<br>

其中、第一个数据和最后一个数据不一样是因为格式的原因、同时PC端没有输出最后一个数据32767：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-d-bc2.png)<br>

4、%l 32位signed long int<br>
测试代码：<br>
```java
long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 };

print("%l,%l,%l,%l,%l,%l,%l,%l\n\r",a10[0], a10[1], a10[2], a10[3], a10[4], a10[5], a10[6], a10[7]);
```
a10[]的值：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-w1.png)<br>
a10[]入栈后的情形(从a10[0]开始)：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-stack1.png)<br>
Debug看到、case %l 读到了32位数据：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-w2.png)<br>
解析也成功(0x80000000 = -2147483648)：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-sram1.png)<br>
Secure CRT接收到的输出也ok：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-s1.png)<br>

下面进行100个负数边界值输出测试：<br>
```java
        case 1: print("%l%c%c", a10[0], a1, a2);
                a10[0]++;
                a1    = (0 == (a10[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a10[0] % 10)) ? '\n' : ' ';
                step += (-(2147483648 - 100) == a10[0]);
                break;
```
PC端：<br>
```java
        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%d  " ,a10[0]);
                printf("...");
                if(0 == (a10[0] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a10[0]++;
                step += (-long(2147483648 - 100) == a10[0]);
                break;
```
结果ok：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-l-bc-1.png)<br>

5、%x 16位int的十六进制格式(负数会保留符号位)<br>
测试代码：<br>
```java
         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 };
unsigned long int a20[] = { 100 };

print("%x,%x,%x,%x,%x,%x,%x\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], 0x10);
```
测试结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-s1.png)<br>
ipython计算结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-py-1.png)<br>
a0的地址就是 0x9C。<br>
a10[0] = 0x80000000L是32位，而%x是16位的，所以这个%x只取了低16位的0x0000、得到0值，<br>
剩下的高16位0x8000仍然在栈空间，会被下一个%x取到，因此最后一个%x得到负数0x8000(32格式输出就是0xFFFF8000)。<br>
所以最后一个%x得到0x8000，而我们的0x10虽然已经入栈、却没有被取到。<br>
最后一个0x10在0x80000000的后面，它在栈空间的情况如下：
![icon](/lessons/1010-USART/1002-print/material/1002-x-stack-1.png)<br>

signed int遍历测试：<br>
```java
         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 };
unsigned long int a20[] = { 100 };

 signed char e = -128;
         int a = 0xFFFF;
unsigned int b = 65500;
unsigned long int c = 0xFABCDEF1;

// ==========================================================================================================
// USART 测试模块
//
// ==========================================================================================================
void task_Mode_USART(void)
{
    static uint8_t step = 1;

    switch(step)
    {
        case 0: print("%x,%x,%x,%x,%x,%x,%x\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], 0x10);
                step = 20;
                break;
        case 1: print("%x%c%c", a5[0], a1, a2);
                a5[0]++;
                a1    = (0 == (a5[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a5[0] % 10)) ? '\n' : ' ';
                step += (32767 == a5[0]);
                break;
        case 2: print("%x\r\n",a5[0]);
                step = 20;
                break;

        default:break;
    }
}
```
和PC端输出的结果一致，测试结果ok：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-bc1.png)<br>

unsigned int遍历测试：<br>
```java
// print()使用a6[0]并改为如下即可：
void task_Mode_USART(void)
{
    static uint8_t step = 1;

    switch(step)
    {
        case 0: print("%x,%x,%x,%x,%x,%x,%x\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], 0x10);
                step = 20;
                break;
        case 1: print("%x%c%c", a6[0], a1, a2);
                a6[0]++;
                a1    = (0 == (a6[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a6[0] % 10)) ? '\n' : ' ';
                step += (0 == a6[0]);
                break;
        case 2: print("%x\r\n",a6[0]);
                step = 20;
                break;

        default:break;
    }
}
```
PC端：<br>
```java
        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%x  " ,a6[0]);
                printf("...");
                if(0 == (a6[0] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a6[0]++;
                step += (65537 == a6[0]);
                break;
```
测试结果ok<br><br>
在[0,7FFF]范围内，两者结果一致：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-bc2.png)<br>

而在[8000, FFFF]范围内，Atmega16端得到的是负数，所以会多出高8位的FFFF(负号)，如FFFF80000，<br>
但PC端是32位的，在[8000, FFFF]范围内仍然是正数、所以没有高16位的FFFF(负号)。<br>
开始出现负号的差异：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-bc3.png)<br>

同时我们的%x是以32位来显示的，所以负数是显示了32位，而正数的高16位是0就没有显示。<br>
PC端的unsigned int是32位没有翻转到负数，所以它的高16位都是0。<br>
这个输出格式上负号的差异一致持续到最后：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-x-bc4.png)<br>
最后Atmega16的unsigned int溢出到0值，而PC端加到第十六位。<br>

6、%X 32位long int的十六进制格式<br>
测试代码：<br>
```java
         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 };
unsigned long int a20[] = { 100 };

print("%X,%X,%X,%X\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], a20[0]);
```
Secure CRT收到的结果为：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-XX-s1.png)<br>

结果解释：<br>
参数入栈后的结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-XX-stack-1.png)<br>
这些参数的值依次是：<br>
a0的地址0x0093、a0[0]的值0x0061(入栈后由0x61提升为int型数据，就是16位的0x0061了)：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-XX-w1.png)<br>
a4[0]的值0x0040(同样被提升为16位数据)：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-XX-w2.png)<br>

a5[0]的值0x8000(int型的-32768、其本身就是int型数据、不会被提升)<br>
a6[1]的值0x8001(unsigned int型的32769)<br>
a10[0]的值0x80000000(long int型的-2147483648)<br>
a20[0]的值0x00000064(unsigned long int型的100)<br>

我们的print()连续取4次%X、也就是连续取4次、每次取32位数，然后输出。<br>
所以从0x0443开始：<br>
第一个32位数就是0x00610093，<br>
第二个32位数就是0x800040，<br>
第三个32位数就是0x00008001，<br>
第四个32位数就是0x00648000。<br>
这就是Secure CRT输出的结果，不过入栈的数据比4个32位数要多出16位(这个数是0x0000)，<br>
多出来的0x0000没有被用到。<br>


signed long int边界遍历测试：<br>
```java
long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
        case 1: print("%X%c%c", a10[7], a1, a2);
                a10[7]++;
                a1    = (0 == (a10[7] % 10)) ? '\r' : ' ';
                a2    = (0 == (a10[7] % 10)) ? '\n' : ' ';
                step += ((-2147483648 + 100) == a10[7]);
                break;
```
PC端：<br>
```java
long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%x  " ,a10[7]);
                printf("...");
                if(0 == (a10[7] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a10[7]++;
                step += ((-(long)2147483648 + 100) == a10[7]);
```
二者的输出结果一致：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-XX-bc1.png)<br>

7、%u 16位unsigned int<br>
测试代码：<br>
```java
         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
unsigned long int a20[] = { 100 };

print("%u,%u,%u,%u,%u,%u,%u,%u,%u\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], a20[0]);
```
Secure CRT的结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-u-s1.png)<br>

结果分析：<br>
从0x043F开始、入栈的参数如下：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-u-stack1.png)<br>
第一个%u取出第一个16位数据0x00A2 = 162，<br>
第二个%u取出第二个16位数据0x0061 = 97，<br>
第三个%u取出第三个16位数据0x0040 = 64，<br>
第四个%u取出第四个16位数据0x8000 = -32768，<br>
这个数转化位uint32_t后是0xFFFF8000，符号位得到了扩展，所以在%u的实现里面、我们去掉了高16位的符号位：<br>
```java
print_buffer_udata((uint32_t)(va_arg(ap, int)) & 0x0000FFFF);
```
去掉符号位的数就是uint32_t的0x8000 = 32768，<br>

第五个%u取出第五个16位数据0x8001 = -32769，和前面一样、转化为uint32_t类型之后去掉符号位，得到+32769，<br>
第六个%u取出第六个16位数据0x0000 = 0，<br>
第七个%u取出第七个16位数据0x8000 = -32768，和前面一样、转化为uint32_t类型之后去掉符号位，得到+32768，<br>
第八个%u取出第八个16位数据0x0064 = 100，<br>
第九个%u取出第九个16位数据0x0000 = 0，<br>

这就是这9个输出的来源。<br>

signed int遍历测试：<br>
```java
int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };

        case 1: print("%u%c%c", a5[0], a1, a2);
                a5[0]++;
                a1    = (0 == (a5[0] % 10)) ? '\r' : ' ';
                a2    = (0 == (a5[0] % 10)) ? '\n' : ' ';
                step += (32767 == a5[0]);
                break;
        case 2: print("%u\r\n",a5[0]);
                step = 20;
                break;
```
PC端：<br>
```java
int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };

        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%u  " ,a5[0] & 0xFFFF);
                printf("...");
                if(0 == (a5[0] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a5[0]++;
                step += (32768 == a5[0]);
                break;
```
两者输出一致：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-u-bc1.png)<br>

用a6[0]从0加到32767的unsinged int遍历输出也一样ok。<br>

8、%U 32位unsigned long int<br>
测试代码：<br>
```java
         char a0[] = "abc12345";
unsigned char a4[] = { 64, 100, 56, 255, 42, 0 };

         int  a5[] = { -32768, -32767, 100, 56, 255, 1000, 32767 };
unsigned int  a6[] = { 0, 32769, 65535, 10000 };

         long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };
unsigned long int a20[] = { 100 };

print("%U,%U,%U,%U,%U,%U,%U,%U,%U\n\r",a0, a0[0], a4[0], a5[0], a6[1], a10[0], a20[0]);
```
Secure CRT输出结果：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-uu-s1.png)<br>
输出解释：<br>
参数入栈后的情况如下：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-uu-stack1.png)<br>
第一个%u取出第一个32位数据0x006100A2 = 6357154，<br>
第二个%u取出第二个32位数据0x80000040 = 2147483712，<br>
第三个%u取出第三个32位数据0x00008001 = 32769，<br>
第四个%u取出第四个32位数据0x00648000 = 6586368，<br>
第五个%u取出第五个32位数据0xCC210000 = 3424714752 ，<br>
第六个%u取出第六个32位数据0x21085F04 = 554196740，<br>
第七个%u取出第七个32位数据0x20000319 = 536871705，这里已经超出了入栈参数的范围，<br>
栈空间的这个位置的数据在每次运行时都可能不一样，所以Secure CRT接收到的也会不一样，比如不一样的某一次：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-uu-stack2.png)<br>
这个输出就是540476185。<br>
第八个%u取出第八个32位数据，这里已经超出了栈空间的范围，我们看不到，只知道都到了一些数据<br>
第九个%u取出第九个32位数据，这里已经超出了栈空间的范围，我们看不到，只知道都到了一些数据<br>

signed long int边界遍历测试：<br>
测试代码：<br>
```java
long int a10[] = { -2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };

        case 1: print("%U%c%c", a10[7], a1, a2);
                a10[7]++;
                a1    = (0 == (a10[7] % 10)) ? '\r' : ' ';
                a2    = (0 == (a10[7] % 10)) ? '\n' : ' ';
                step += ((-2147483648 + 100) == a10[7]);
                break;
        case 2: print("%u\r\n",a10[7]);
                step = 20;
```
PC端：<br>
```java
long int a10[] = { -(long)2147483648, 0xFFFFFFFF, 0xFFFFFFFE, -10000, 0, 10000, 0x7FFFFFFF, 2147483647 - 100 };

        case 1: // 写入文件：fp 文件流变量，"%d"写入格式，b 是写入的数据
                fprintf(fp,"%u  " ,a10[7]);
                printf("...");
                if(0 == (a10[7] % 10))
                {
                    fprintf(fp, "\n");
                    printf("\n");
                }
                a10[7]++;
                step += ((-(long)2147483648 + 100) == a10[7]);
                break;
```
二者输出一致：<br>
![icon](/lessons/1010-USART/1002-print/material/1002-uu-bc1.png)<br>

unsigned long int边界遍历测试也ok。<br>
