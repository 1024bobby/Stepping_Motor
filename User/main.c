#include "stm32f4xx.h"
#include "./key/bsp_exti.h" 
#include "./MSD/MicroStepDriver.h" 
#include "./usart/bsp_debug_usart.h"
#include "./MSD/MSD_test.h" 


int main(void)
{
    //初始化按键
    EXTI_Key_Config();
    //初始化串口
    Debug_USART_Config();
		//步进电机初始化
		MSD_Init();
    //打印帮助命令
    ShowHelp();
    
    while(1) 
    {
        //处理串口数据
        DealSerialData();
    }
                          
}

