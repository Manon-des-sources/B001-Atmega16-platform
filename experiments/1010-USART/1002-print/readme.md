**这里将创建一个简化版的printf函数、并测试之**<br>

# 概述：<br>
1、.c和.h是最终的源代码文件，不是测试用的代码，测试的代码在各自的.md文件中<br>
2、代码开发次序和代码的测试次序一致，<br>
即1001-print.md -> 1002-print(%x).md<br>

# 一、print()函数定义：<br>
```javascript
// void print(const char *fmt, ...)
//
// 参数：fmt        格式控制字符串
//
// 说明：
// (1). 支持格式：%c    char  字符
//                %s    char *字符串
//                %d    16位signed int
//                %l    32位signed long int
//                %u    16位unsigned int
//                %U    32位unsigned long int
//                %x    16位int的十六进制格式(负数会保留符号位)
//                %X    32位long int的十六进制格式
// (2). 空格不能在%和d/./x之间，错误举例：print("a = % d\n",a);
// (3). 不要在中断中或抢占式任务中使用这个print函数，因为它是不可重入的，会修改全局的buffer
```

# 二、print()函数实现<br>
结构：<br>
使用了环形缓冲器来接收所有传入print()的格式化字符串及变量数据<br>
代码来源：http://blog.csdn.net/aizhiqing86/article/details/8570804<br>
它省去了使用in和out来判断空和满的方法、而是直接可读出数据长度<br>

print()的定义：<br>
void print(cosnt char *fmt, ...);<br>
fmt是格式控制字符串、后面的'...'称为参数列表，它使得print()成为了一个参数个数可变的函数<br>
在调用print()时、所有参数从右到左依次入栈，结果就是fmt最后入栈、成为stack_top<br>
而'...'里面的参数在fmt的后面依次排列，如下图所示：<br>
![stack_5](/experiments/1010-USART/1002-print/material/stack_5.png)<br>

# 三、可变参数函数需要使用的宏：<br>
va_list类定义如下：<br>
```javascript
#include <stdarg.h>
typedef   char  * va_list;
#define   _INTSIZEOF(type)  ((sizeof(type) + sizeof(int) - 1) & ~(sizeof(int) - 1))    // 结果为sizeof(int)的整数倍
#define   va_start(ap,v)    (ap = (va_list)&v + _INTSIZEOF(v))                         // 让ap指向参数v的参数列表中的第一个参数
#define   va_arg(ap,type)   (*(type*)((ap += _INTSIZEOF(type)) -  _INTSIZEOF(type)))   // 取出参数t、并让ap指向参数t的下一个参数
#define   va_end(ap)        (ap = (va_list)0)
#define   va_copy(dest, src)
```
这些宏在不同的操作系统，有不同的实现方式，上面的只是其中一种<br>


(1). va_start(ap,v)宏的作用: <br>
v是参数列表'...'里面的第一个参数fmt，fmt通过格式符'%'告诉我们'...'里面有几个参数<br>
每个参数对应一个%格式符，%以外的字符不对应任何参数、print()直接打印出来即可<br>
我们还通过fmt的地址来计算出'...'里面的第一个实际参数的地址，并将这个地址保持在ap指针变量里面<br>
也就是说、在运行了va_start(ap,v);之后、我们让指针ap指向了'...'里面的第一个参数<br>
我们接下来就通过ap指针来逐个引用参数列表'...'里面的所有参数<br>

(2). va_arg(ap,type)宏的作用:<br>
通过va_start(ap,v)，我们的ap的指针已经指向了'...'里面的第一个实际参数<br>
可以看到的是、ap指针先加上了一个值、这个值就是ap指针指向的实际参数的size，这让ap指针指向了下一个实际参数<br>
然后我们又减去这个值,最终把这个值转化为一个(type*)的地址<br>
最后取出这个地址里面的值<br>
结果就是：va_arg宏返回了ap指针指向的实际参数的值、同时使得ap指针指向下一个实际参数<br>
我们在print()里面就通过va_arg(ap,type)来逐个读出参数列表'...'里面的所有实际参数的值<br>

(3). va_end(ap)宏的作用:<br>
     将ap指针赋值为NULL，即0<br>


# 四、不定参数列表'...'涉及到的类型提升问题(类型转换)<br>
在C语言中、如果函数的某一个形式参数没有指定参数类型，那么在调用他们时会根据传递进来的实参的类型进行类型提升：<br>
```javascript
char、short -> int
int -> unsigned int
float -> double
```
其实质就是类型转换<br>

参数列表'...'中的实际参数都没有指定类型，所以一定会被提升。<br>
这就导致'...'里面的参数被传递到函数内部之后，他们的类型就变成了int、unsigned int、或double<br>
也就是说、type不能是char、unsigned char、singed char、signed short、unsigned short、float<br>

# 五、不使用va_list宏也可以写参数个数可变函数<br>
1、必须存在一个参数fmt、用来指定参数数量，或这我们可以从它得到'...'里面的所有参数<br>
2、知道处理器的函数参数入栈顺序，通过参数fmt访问第二个参数的地址<br>
3、最好每个参数的类型都固定，方便确定第三个第四个...参数的地址<br>
4、比如发送数据包时、我们不知道现在一共产生了多少个数据包，但是可以一起都发送出去，哪怕数据包数量每次不同<br>
![stack_5](/experiments/1010-USART/1002-print/material/stack_5.png)<br>
