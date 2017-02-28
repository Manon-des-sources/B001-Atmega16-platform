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
// -----------------------------------
// 本文定义了Atmega16下的SPI读写管理器
// 
// -----------------------------
// 本文的SPI接口工作在主机模式下
// 
// ==========================================================================================================
#include "Mod_SPI_manager.h"




#if USE_SPI_MANAGER



volatile uint32_t temp2014 = 0;
volatile uint32_t temp2015 = 0;
volatile uint32_t temp2020 = 0;

static T_SPI_MGR_CTRL  SPI_mgr_ctrl;

// SPI接口用户管理器
static pT_SPI_MGR_USER SPI_mgr_box[4];

// SPI状态机函数
static p_bool_funtion_void Mod_SPI_MGR_run;

// 用于存放状态寄存器读写指令、以及读写数据(读写数据紧跟在对应的读写指令后面，如[com_SR2|data_SR2|com_SR1|data_SR1|com_SR0|data_SR0])
// 也用于发送读写指令和地址
// 必须是偶数个大小
static uint8_t SPI_MGR_buffer[6];


// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// 第一部分：初始化



// ==========================================================================================================
// SPI管理器初始化 以及 SPI接口初始化
// 
// ==========================================================================================================
void Mod_SPI_MGR_init(void)
{
    uint8_t index;

    // SPI管理器的初始操作状态
    SPI_mgr_ctrl.step     = 0;
    SPI_mgr_ctrl.end      = FALSE;
    SPI_mgr_ctrl.count    = 0;
    SPI_mgr_ctrl.number   = 0;
    SPI_mgr_ctrl.pBuffer  = NULL;
    SPI_mgr_ctrl.user     = 0;
    SPI_mgr_ctrl.suspend  = 0;
    SPI_mgr_ctrl.user_sus = 0;

    // SPI管理器初始化
    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        SPI_mgr_box[index] = NULL;
    }

    // 初始化SPI接口：主机模式、mode-3、MSB first、128分频
    Drv_SPI_init(SPI_MODE_MSTR, SPI_SHIFT_MODE_00, SPI_SHIFT_FIRST_MSB, SPI_CLK_SOURCE_DIV_128);
    Drv_SPI_INT_Enable(ENABLE);

    // 设置第一个状态
    Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);


    Drv_IO_mode_bit(DDRD, DDD1, IO_OUTPUT);
    Drv_IO_clr_bit(PORTD, PD1);
}

// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// 第二部分：用户结构与用户管理



// ==========================================================================================================
// 更新用户信息
// 
// 参数：  user         用户的地址
//         pBuffer      用户的缓存
//         size_buffer  缓存的大小
//         size_SR      状态寄存器SR的大小
// 说明：
// (1). n_SR的存在、是因为有的SPI Flash的状态寄存器是1个字节、2个字节或3个字节
// (2). 将指令、地址、数据都放入pBuffer，是为了方便SPI中断的编程
//      但对于用户来说、pBuffer是不可见的(管理器使用)，用户只使用pData和nData来读写数据
// 
// ==========================================================================================================
void Mod_SPI_MGR_user_msg(const pT_SPI_MGR_USER user, uint8_t *const pBuffer, const uint32_t size_buffer)
{
    user->action  = 0;
    user->state   = 0;
    user->command = 0;
    user->addr    = 0;
    user->number  = 0;
    user->pBuffer = pBuffer;
    user->pData   = pBuffer + 4;
    user->nData   = size_buffer - 4;
}

// ==========================================================================================================
// 管理器注册接口
//               ---- 将buffer注册到用户、并将用户注册到SPI管理器
// 
// 参数：  user         用户的地址
//         pBuffer      用户的缓存
//         size_buffer  缓存的大小
//         size_SR      状态寄存器SR的大小
// 
// 返回值：status   0 = 用户注册成功
//                  1 = 用户已经注册、此次将用更新户信息
//                  2 = SPI管理器已满
//                  3 = 未找到用户
//                  4 = 
//                  5 = 
//                  6 = 缓存必须存在
//                  7 = 缓存不能为空
// 
// 说明：
// (1). 如果用户不存在、就注册该用户
//      如果用户已经存在、就更新用户信息
// 
// ==========================================================================================================
uint8_t Mod_SPI_MGR_user_add(const pT_SPI_MGR_USER user, uint8_t *const pBuffer, const uint32_t size_buffer)
{
    uint8_t status = 0;
    uint8_t index;

    if(NULL == user)     { status = 3; }
    if(NULL == pBuffer)  { status = 6; }
    if(0 == size_buffer) { status = 7; }

    if(0 == status)
    {
        for(index = 0; index < _countof(SPI_mgr_box); index++)
        {
            if(user == SPI_mgr_box[index]) { break; }
        }
        if(index < _countof(SPI_mgr_box))
        {
            // 如果用户已经存在、就更新用户信息
            Mod_SPI_MGR_user_msg(user, pBuffer, size_buffer);
            user->state = SPI_MGR_STATE_IDLE;
            status = 1;
        }
        else// 如果用户不存在、就注册该用户
        {
            for(index = 0; index < _countof(SPI_mgr_box); index++)
            {
                if(NULL == SPI_mgr_box[index])
                {
                    SPI_mgr_box[index] = user;
                    Mod_SPI_MGR_user_msg(user, pBuffer, size_buffer);
                    user->state = SPI_MGR_STATE_IDLE;
                    status = 0;
                    break;
                }
            }
            if(index >= _countof(SPI_mgr_box))
            {
                status = 2;
            }
        }
    }
    return status;
}

// ==========================================================================================================
// 获取用户在管理器中的编号(位置)
// 
// 参数：  user     用户的地址
// 
// 返回值：index    用户在管理器中的编号(位置)
//                 ( < _countof(SPI_mgr_box)的编号是有效的编号)
// 
// ==========================================================================================================
uint8_t Mod_SPI_MGR_user_index(pT_SPI_MGR_USER const user)
{
    uint8_t index;

    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        if(user == SPI_mgr_box[index]) { break; }
    }
    return index;
}

// ==========================================================================================================
// 申请使用SPI接口
// 
// 参数：  user     用户的地址
//         command  读数据的指令
//         addr     24bit的操作地址
//         size     数据量
// 
// 返回值：Fin      TRUE  = 操作申请成功、后续可以在SPI_MGR_STATE_WORK或SPI_MGR_STATE_DONE状态下获得结果
//                  FALSE = 操作申请失败(数据量太大)
// 
// 说明：
// (1). 用户已申请的操作的状态：
//                              SPI_MGR_STATE_WORK = 操作状态：读写擦指令已经生效
//                              SPI_MGR_STATE_DONE = 操作状态：读写数据已经传输完毕
//                              SPI_MGR_STATE_BUSY = 操作状态：操作进行中、或已准备就绪(等待执行)
//                              SPI_MGR_STATE_IDLE = 操作状态：操作未启动(未就绪)
// (2). 操作申请成功后、用户可以读取自己的状态信息.state、并在SPI_MGR_STATE_WORK状态下去获取结果
// 
// ==========================================================================================================
bool Mod_SPI_MGR_application(pT_SPI_MGR_USER const user, const uint8_t action, const uint8_t command, const uint32_t addr, const uint32_t size)
{
    bool Fin = FALSE;
    uint8_t index;

    if(user->nData >= size)
    {
        index = Mod_SPI_MGR_user_index(user);
        if(index < _countof(SPI_mgr_box))
        {
            Fin = TRUE;
            SPI_mgr_box[index]->action  = action;       // 操作类型
            SPI_mgr_box[index]->command = command;      // 操作指令
            SPI_mgr_box[index]->addr    = addr;         // 操作地址
            SPI_mgr_box[index]->number  = size;         // 用户得到的数据量(字节)
//          SPI_mgr_box[index]->number  = (SPI_MGR_ACT_EARASE_ADDR == action) ? 0 : size;  // 擦除指令后面多出数据、都将导致擦除失败
            SPI_mgr_box[index]->state   = SPI_MGR_STATE_READY;  // 用户已经就绪、申请操作SPI接口
        }
    }
    return Fin;
}


// ==========================================================================================================
// ----------------------------------------------------------------------------------------------------------
// 第三部分：SPI操作流程状态机



// ==========================================================================================================
// 状态函数0：查找已经就绪的用户
// 
// 返回值：  FALSE = 结束调度
//           TRUE  = 继续调度状态函数
// 
// ==========================================================================================================
bool Mod_SPI_MGR_start(void)
{
    bool Fin = FALSE;
    uint8_t index;

    for(index = 0; index < _countof(SPI_mgr_box); index++)
    {
        temp2014 = index;
        if(NULL != SPI_mgr_box[index])
        {
            // 该用户已就绪
            if(SPI_MGR_STATE_READY == SPI_mgr_box[index]->state)
            {
                SPI_mgr_box[index]->state = SPI_MGR_STATE_BUSY;
                switch(SPI_mgr_box[index]->action)
                {
                    case SPI_MGR_ACT_READ_COM   : 
                    case SPI_MGR_ACT_READ_ADDR  : Mod_SPI_MGR_set_run(Mod_SPI_MGR_transfer);
                                                  Fin = TRUE;
                                                  break;
                    case SPI_MGR_ACT_WRIRE_COM  : 
                    case SPI_MGR_ACT_WRITE_ADDR :
                    case SPI_MGR_ACT_EARASE_ADDR: Mod_SPI_MGR_set_run(Mod_SPI_MGR_write_WEL);
                                                  Fin = TRUE;
                                                  break;
                    default : break;  // 其他无效的操作
                }
                // 已经拿到一个用户的有效申请、不再查找后面的用户、可以直接调度下一个任务函数
                if(TRUE == Fin)
                {
                    SPI_mgr_ctrl.user = index;
                    break;
                }
            }
        }
    }
    return Fin;
}

// ==========================================================================================================
// 状态函数1：写使能
// 
// 返回值：  FALSE = 结束调度
//           TRUE  = 继续调度状态函数
// 
// 说明：
// (0). 鉴于我所遇到过的SPI Flash的WEL位都在SR[0]，所以这里只检查SR[0]
// (1). 由于主机的中断里面肯定会发送结束、保证SPI_mgr_ctrl.end = TRUE;
//      所以这里case 1和case 2的等待传输不会导致task_SPI_MGR()中的while()死循环
// (2). 同时、传输完成后、case 2里面只检查一次就退出，所以不会有循环
//      也就是说、哪怕SPI从机没接、或SPI总线断裂，也不会导致死循环
// 
// ==========================================================================================================
bool Mod_SPI_MGR_write_WEL(void)
{
    bool Fin = FALSE;
    uint16_t status = 0;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // 发送指令(通常、写入指令后、写使能会立即生效，也就是立即读出SR、就可得到SR.WEL=1)
                SPI_MGR_buffer[0] = SPI_COMMAND_WRITE_ENABLE;
                Mod_SPI_MGR_shift(1, SPI_MGR_buffer);
                SPI_mgr_ctrl.step = 1;
                Fin = TRUE;  // 发送1字节耗时140us，可以重新进来、等待发送完毕
                break;
        case 1: // 写使能指令传输完成后、去读取SR.WEL
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    SPI_MGR_buffer[0] = SPI_COMMAND_READ_STATUS_L;
                    SPI_MGR_buffer[1] = 0;
                    Mod_SPI_MGR_shift(2, SPI_MGR_buffer);
                    SPI_mgr_ctrl.step = 2;
                }
                Fin = TRUE;  // 发送2字节耗时280us
                break;
        case 2: // 检查写使能
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    status = SPI_MGR_buffer[1];
                    temp2014 = status;
                    if(_WEL_ == (status & _WEL_))
                    {
                        // 去发送数据
                        Mod_SPI_MGR_set_run(Mod_SPI_MGR_transfer);
                        Fin = TRUE;
                    }
                    else
                    {   // 写使能未生效、下次再来读SR寄存器
                        SPI_mgr_ctrl.end  = TRUE;
                        SPI_mgr_ctrl.step = 1;
                        Fin = FALSE;
                    }
                }
                else
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// 状态函数2：检查忙状态
// 
// 返回值：  FALSE = 结束调度
//           TRUE  = 继续调度状态函数
// 
// 说明：
// (0). 鉴于我所遇到过的SPI Flash的WIP位都在SR[0]，所以这里只检查SR[0]
// (1). 由于主机的中断里面肯定会发送结束、保证SPI_mgr_ctrl.end = TRUE;
//      所以这里case 1不会导致task_SPI_MGR()中的while()死循环
// (2). 同时、发送结束后、case 1里面只检查一次就退出，所以不会有循环
//      也就是说、哪怕SPI从机没接、或SPI总线断裂，也不会导致死循环
// 
// ==========================================================================================================
bool Mod_SPI_MGR_read_WIP(void)
{
    bool Fin = FALSE;
    uint8_t status = 0;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // 准备
                SPI_MGR_buffer[0] = SPI_COMMAND_READ_STATUS_L;
                SPI_MGR_buffer[1] = 0;
                Mod_SPI_MGR_shift(2, SPI_MGR_buffer);
                SPI_mgr_ctrl.step = 1;
                Fin = TRUE;  // 发送2字节耗时280us
                break;
        case 1: // 检查忙状态
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    status = SPI_MGR_buffer[1];
                    temp2020 ++;
                    temp2014 = status;
                    if(_WIP_ == (status & _WIP_))
                    {
                        // 忙、则下次重新进来、去读SR寄存器
                        SPI_mgr_ctrl.step = 0;
                        Fin = FALSE;
                    }
                    else
                    {   // 擦写指令已经生效、去查询下一个用户的申请
                        SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                        Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                        Fin = TRUE;
                    }
                }
                else
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// 状态函数3：数据传输
// 
// 返回值：  FALSE = 结束调度
//           TRUE  = 继续调度状态函数
// 
// 说明：
// (1). case 2里面重新回到cas 1的设置、不会导致task_SPI_MGR()一直循环不退出
//      因为case 1里面不会重新进入调度
// (2). 如果使用8MHz & 2分频，那么传输256字节数据、耗时约280us
//      case 0里面就可以设置Fin = TRUE、来等待数据传输完毕
// (3). 由于主机的中断里面肯定会发送结束、保证SPI_mgr_ctrl.end = TRUE;
//      所以这里不会导致task_SPI_MGR()中的while()死循环
//      也就是说、哪怕SPI从机没接、或SPI总线断裂，也不会导致死循环
// 
// ==========================================================================================================
bool Mod_SPI_MGR_transfer(void)
{
    bool Fin = FALSE;

    switch(SPI_mgr_ctrl.step)
    {
        case 0: // 取出指令和地址
                switch(SPI_mgr_box[SPI_mgr_ctrl.user]->action)
                {
                    case SPI_MGR_ACT_READ_COM   : 
                    case SPI_MGR_ACT_WRIRE_COM  : SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3] = SPI_mgr_box[SPI_mgr_ctrl.user]->command;
                                                  Mod_SPI_MGR_shift(SPI_mgr_box[SPI_mgr_ctrl.user]->number + 1, &SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3]);
                                                  Fin = TRUE;   // 这类读寄存器的指令、其数据量不大、可以进来等待
                                                  break;
                    case SPI_MGR_ACT_READ_ADDR  : 
                    case SPI_MGR_ACT_WRITE_ADDR :
                    case SPI_MGR_ACT_EARASE_ADDR: SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[0] = SPI_mgr_box[SPI_mgr_ctrl.user]->command;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[1] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr >> 16;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[2] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr >> 8;
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer[3] = SPI_mgr_box[SPI_mgr_ctrl.user]->addr;
                                                  Mod_SPI_MGR_shift(SPI_mgr_box[SPI_mgr_ctrl.user]->number + 4, SPI_mgr_box[SPI_mgr_ctrl.user]->pBuffer);
                                                  Fin = FALSE;  // number很大时、不需要进来
                                                  break;
                    default                     : // 其他无效的操作被视为结束、去查询下一个用户的申请
                                                  SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                  Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                  Fin = TRUE;
                                                  break;
                }
                SPI_mgr_ctrl.step = 1;
                break;
        case 1: // 数据发送完毕
                if(TRUE == SPI_mgr_ctrl.end)
                {
                    switch(SPI_mgr_box[SPI_mgr_ctrl.user]->action)
                    {
                        case SPI_MGR_ACT_READ_COM   : 
                        case SPI_MGR_ACT_READ_ADDR  : SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                      Fin = TRUE;
                                                      break;
                        case SPI_MGR_ACT_WRIRE_COM  : 
                        case SPI_MGR_ACT_WRITE_ADDR :
                        case SPI_MGR_ACT_EARASE_ADDR: SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_DONE;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_read_WIP);
                                                      Fin = FALSE;  // 擦写耗时、不需要立即进来检查忙信号
                                                      break;
                        default                     : // 其他无效的操作被视为结束、去查询下一个用户的申请
                                                      SPI_mgr_box[SPI_mgr_ctrl.user]->state = SPI_MGR_STATE_WORK;
                                                      Mod_SPI_MGR_set_run(Mod_SPI_MGR_start);
                                                      Fin = TRUE;
                                                      break;
                    }
                }
                else  // 等待数据发送完毕
                {
                    Fin = TRUE;
                }
                break;
    }
    return Fin;
}

// ==========================================================================================================
// 设置并启动数据发送
// 
// 参数：  number   数据量(字节)
//         pBuffer  数据缓存的地址
// 
// ==========================================================================================================
void Mod_SPI_MGR_shift(uint16_t number, uint8_t *const pBuffer)
{
    SPI_mgr_ctrl.end     = FALSE;
    SPI_mgr_ctrl.count   = 0;
    SPI_mgr_ctrl.number  = number;
    SPI_mgr_ctrl.pBuffer = pBuffer;
    Drv_SPI_clr_SS();
    SPDR = SPI_mgr_ctrl.pBuffer[0];
}

// ==========================================================================================================
// 设置下一个状态函数
// 
// 参数：  p_bool_funtion_void      接下来将被运行的状态函数
//         这类型函数的返回值：     TRUE  = 继续调度状态函数
//                                  FALSE = 结束调度
// 
// ==========================================================================================================
void Mod_SPI_MGR_set_run(p_bool_funtion_void const fun)
{
    SPI_mgr_ctrl.step = 0;
    Mod_SPI_MGR_run   = fun;
}

// ==========================================================================================================
// 处理用户的SPI操作申请
// 
// 说明：
// (1). 每1ms调度一次、所以最好将每次调度时间控制在500us之内或更少
// (2). 带一个参数、使得每次可以执行多个状态函数，当然可以演化为调度器
//      run  = FALSE：结束调度
//             TRUE ：继续调度状态函数
//                    如果一个状态函数没有操作SPI接口，或该操作耗时不大，
//                    就可以直接进入下一个状态函数，或重新进入本次调度函数、以允许其中的下一个分支
//                    这样可以避免操作SPI接口的动作被延迟至少1ms
// 
// ==========================================================================================================
void task_SPI_MGR(void)
{
    bool run = TRUE;
    
    Drv_IO_toggle_bit(PORTD, PD1);
    while(run)
    {
        run = Mod_SPI_MGR_run();
        temp2015 = run;
    }
}

// ==========================================================================================================
// SPI传输完成中断
// 
// 说明：
// (1). 与从机交换数据、直到所有数据交换完毕
// (2). 使用主机模式
//      主机模式下，如果SS引脚配置为输入、SS引脚收到低电平、就会导致SPI接口自动变为从机模式
//      同时SPIF置1，我们在中断里面需要恢复主机模式：
//      if(SPI_MODE_SLAVE == (SPCR & (1 << MSTR)))
//      {
//          SPCR |= (SPI_MODE_MSTR << MSTR);
//      }
// (3). 由于主机的这个中断里面使用了判断：if(SPI_mgr_ctrl.count < SPI_mgr_ctrl.number)
//      所以、一定会有SPI_mgr_ctrl.count > SPI_mgr_ctrl.number的时刻、进而完成传输：SPI_mgr_ctrl.end = TRUE;
// 
// ==========================================================================================================
ISR(SPI_STC_vect)
{
    // 一次传输完成后、获取与从机交换得到的数据
    SPI_mgr_ctrl.pBuffer[SPI_mgr_ctrl.count] = SPDR;

    // 检查数据量、直到所有数据发送完毕
    SPI_mgr_ctrl.count++;
    if(SPI_mgr_ctrl.count < SPI_mgr_ctrl.number)
    {
        SPDR = SPI_mgr_ctrl.pBuffer[SPI_mgr_ctrl.count];
    }
    else
    {
        Drv_SPI_set_SS();
        SPI_mgr_ctrl.end = TRUE;
    }
}




#endif  // #if USE_SPI_MANAGER
