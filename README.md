# B001-Atmega16-platform
这是一个包含常用模块组件的开发平台<br>
这个平台类似一个标准代码仓库，它所包含的这些模块将加快开发进程<br>

**开发平台信息**<br>
• 开发工具：AVR Studio 4.19 + avr-toolchain-installer-3.4.1.1195-win32.win32.x86<br>
• 芯片型号：ATmega16<br>
• 芯片主频：8MHz<br>

**文件目录**<br>
1. 源码：AVR_build<br>
2. 电路：hardware<br>
3. 测试：experiments<br>

**当前进度** = 未完成(进度详见Wiki中的Development log)<br>

|.                | .
|---------------- | --------
|                 |
|**模块类型**     | **具体模块**
|1、BSP           | noinit, IO, SPI, Timer, WDT,
|2、sys           | sys init, sys warning, sys version,
|                 | scheduler, event manager, buffer manager,
|3、Mod           | SPI, IIC, UART,
|                 | digital tube display, key, IR,
|                 | print(),
|4、Con           | modbus，
|                 |
|**文件命名分类**     | **举例**
|0、系统调度相关.sys  | sys.event.c
|1、BSP驱动.drives   | Drv_Timer.c
|2、外设驱动.Mod     | task_Mod_LED_display.c
|3、通信协议类.Con   | task_Con_modbus.c
|4、具体应用.App     | task_App_Tetris.c
