**这里将解决print()的使用问题**<br>
主要内容：<br>
1. 环形buffer的访问冲突问题
2. 环形buffer的大小问题

# 概述：<br>
1. 我们在print()里面要将数据放入环形buffer，而在USART中断里面要从环形buffer里面取出数据<br>
   这就会造成访问冲突<br>
2. 在USART发送速度(波特率)确定的情况下，环形buffer的大小应该如何确定<br>


# 一、环形buffer的访问冲突问题：<br>



# 二、环形buffer的大小问题：<br>
