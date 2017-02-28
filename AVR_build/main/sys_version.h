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
// --------------------------
// ���ļ�¼�˱�����İ汾��ʷ
// 
// --------------------------
// ������
// 
// ==========================================================================================================
#ifndef __VERSION_H__
#define __VERSION_H__



#define SYS_VERSION     "AVR_build-v1.2.0.20170209_alpha"


// ==========================================================================================================
// AVR_build-v1.2.0.20170209_alpha(branch = dev-USART)
// 
// 1������USARTģ������



// ==========================================================================================================
// AVR_build-1019(v1.1.0.20170113_alpha)
// 
// 1���޸�SPI��д�������ĵ�һ���汾��ֻ��SR[0]�����ٽ�SR�Ĵ������ֽ�����Ϊһ���û�����
//                                   �������Լ��ٶ�WEL��WIP�ĺ�ʱ
//                                   ��Ŀǰ������SPI Flash�ǵ�WEL��WIP����SR[0]����



// ==========================================================================================================
// AVR_build-1018
// 
// 1��sys_task_add()�еĲ���init������NULL�����ڷ�����Щ����Ҫ��ʼ��������
// 2������SPI��д�������ĵ�һ���汾�������û��Ķ���������ϵ�ǰ�û��Ĳ�д������ֻ�в�дָ����Ч��ſ�����Ӧ�����û��Ĳ�������
//                                   Ҳ����æ״̬�²���Ӧ�����û��Ķ�����
//    ��������״̬����������ѭ��������SPI�ӻ�û�ӡ���SPI���߶���




// ==========================================================================================================
// AVR_build-1017
// 
// 1������Atmega16��SPIģ��Ĳ�������
// 2������SPI Flash�Ĳ�������(��GD25Q32Ϊ׼)-�����ͽӿ�
//    ��Щ�ӿں�ʱ��̫�������ʺ�����ʱ�䴥���Ľṹ�У���һ���汾�������ʺ�����ʱ�䴥���Ľṹ�еĽӿ�




// ==========================================================================================================
// AVR_build-1016
// 
// 1��������1����ռʽ�����Խ������ʽ��������������ok
// 2���¼��Ĳ���dataȫ����Ϊ32bit
// 3��������Ϣ��������sys_event_push()�����ڵ�������Ƕ���¼�����
//    ��Ϣ���еķ��������ǽ���Ϣ�����е���Ϣ������Ϣ����
//    ��Ӧ����������Ϣʱ������Ϣ������(ֻ����ռʽ�����������������������������)
//    ������ʹ��sys_event_post_buffer()����Ϣ���뻺�棬������ķ��͵���Ϣ����
//    �ɵ����������sys_event_push()����������ʽ��������л����м�ȥ�����Ϣ������Ϣ���еĲ���
//    ��ȻҲ������sys_event_post()ֱ��д���¼�����ֻӦ�ڳ�ʼ����û�г�ͻ�������ʹ��



// ==========================================================================================================
// AVR_build-1015
// 
// 1�����¼����м��������ж�д�¼����еĺ��������ٽ�ֹ�жϡ�����ͨ���ж�����״̬����֪��дȨ��
// 2������ܿɰ���ָ��������ʾ
// 3��������EVENT_DIGITAL_FORMAT�¼������ڿ������ֵĽ���
// 4�������˺���ʽ�����������޸Ĳ���ok



// ==========================================================================================================
// AVR_build-1014
// 
// 1���޸���Mod_IR_send_init()�����ز�Ƶ�ʲ��Ե�Bug
// 2��sys_timer_init()ʹ�ú�Mod_IR_send_init()����һ���ĳ�ʼ�����÷�ʽ���Զ�����Ƶ�ʲ���
// 3����sys_task_add()�����������ĳ�ʼ����������Ϊһ����������������ӵ�ʱ��ͳ�ʼ������
//    ��ʹ��sys_init()ֻ��ʼ����ϵͳ��صĵط�����������صĵط����������Լ���ʼ��



// ==========================================================================================================
// AVR_build-1013
// 
// 1�����ֲ��޸��˷�ʱ���ȵ�һ��bug(���BUG�ᵼ��period=0ʱ��������ʱΪ255)
//    ��Ӱ��İ汾��[AVR_build-1009, AVR_build-1012]
//    �޸ģ�
/*
void sys_task_add(uint8_t delay, uint8_t period, p_task_funtion task)
{
    uint8_t index = 0;

    for(index = 0; index < ARRAY_SIZE(sys_task_ctrl); index++)
    {
        if(NULL == (p_sys_task_ctrl + index)->task) { break; }
    }
    if(index >= ARRAY_SIZE(sys_task_ctrl)) { return; }

    (p_sys_task_ctrl + index)->delay  = delay;
    (p_sys_task_ctrl + index)->period = (period > 0) ? (period - 1) : 0; // ���ֲ��޸�bug�ĵط�
    (p_sys_task_ctrl + index)->task   = task;
}
*/
// 2����д���¼�����ģ�飬�����˺ü����ӿڣ�ͬʱ�ɶ��Ը��ߡ�
// 3���������жϵ��ȵ�ǰ��̨�ṹ(�������������߽ӿ�(��δʵ�ֹ��ܡ�ֻ�������˽ṹ�ϵĽӿ�)�����������Ϊʡ��)��
//    ����ṹ�С��������񶼷����жϣ�
//                                  1���ⲿ������ʵʱ����
//                                  2����ʱ���ж����CPU����ѭ��
//                                  3��CPUÿ���˳��ж�֮��ֱ�ӽ������ߣ�������Ч��ʡ��



#endif  // #ifndef __VERSION_H__
