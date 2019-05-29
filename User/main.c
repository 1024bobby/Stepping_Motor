#include "stm32f4xx.h"
#include "./key/bsp_exti.h" 
#include "./MSD/MicroStepDriver.h" 
#include "./usart/bsp_debug_usart.h"
#include "./MSD/MSD_test.h" 


int main(void)
{
    //��ʼ������
    EXTI_Key_Config();
    //��ʼ������
    Debug_USART_Config();
		//���������ʼ��
		MSD_Init();
    //��ӡ��������
    ShowHelp();
    
    while(1) 
    {
        //����������
        DealSerialData();
    }
                          
}

