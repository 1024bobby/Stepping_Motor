#include "MSD_test.h"  
#include <math.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_exti.h" 
#include "MicroStepDriver.h" 
void ShowHelp(void);
void ShowData(int position, int acceleration, int deceleration, int speed, int steps);
void Delay(__IO u32 nCount);

/*! \brief ��ӡ��������
 */
void ShowHelp(void)
{
    printf("\n\r�������������������������������������������-��������������������������");
    printf("\n\r�����尴����KEY1����������ֹ���(�ѻ�״̬)  KEY2���������ָ��������� ");
		printf("\nstepsΪ��ʱ˳ʱ����ת��Ϊ��ʱ��ʱ����   \n");
		printf("\n����  move:steps,accel,decel,speed�󰴻س������ò���(���壬���ٶȣ����ٶȣ�����ٶ�)\r");
		printf("\n��:   move:6400,20,20,20\n\r");
	
		printf("\n����  modex,cy:+-y �����ٶ���Ȧ����xΪ1,2�ֱ�Ϊ�͡�����,��y�� ΪȦ��,��+-�� Ϊ��˳ʱ��\r");
		printf("\n����  modex,cy:+-1/y �����ٶ���Ȧ��, ��1/y�� Ϊy��֮һȦ��,��+-�� Ϊ��˳ʱ��\r");
		printf("\n��:   mode2,cy:1 �� mode1,cy:-1/4�\\n\n\n\n\n");
}

/*! \brief ��ӡ�������
 *  \param acceleration ���ٶ�
 *  \param deceleration ���ٶ�
 *  \param speed        ����ٶ�
 *  \param steps        �ƶ�����
 */

void ShowData(int position, int acceleration, int deceleration, int speed, int steps)
{
	printf("������:%d",steps);
  printf("	���ٶ�:%.2frad/s^2",1.0*acceleration);
  printf("  ���ٶ�:%.2frad/s^2",1.0*deceleration);
  printf("  ����ٶ�:%.2frad/s(%.2frpm)",1.0*speed,9.55*speed);
  printf("\r�����ǰλ��: %d\r\n\r\r\r",position);
}


/**

  * @brief  �����ڽ��յ�������

  * @param  ��

  * @retval ��

  */
void DealSerialData(void)
{
	
    static char showflag =1;
    //Ĭ���ƶ�����
    static int steps;
    //Ĭ�ϼ��ٶ�
    static int acceleration;
    //Ĭ�ϼ��ٶ�
    static int deceleration;
    //Ĭ������ٶ�
    static int speed;
    //Ĭ��������
    static int pulse = 6400;
    //�յ����ڵ�Ȧ��ָ��
    static int cy_num;		
	
    
//    int acc_temp=0;
//    int dec_temp=0;
//    int speed_temp=0;
    
    //���յ���ȷ��ָ���ΪTRUE
    char okCmd = FALSE;
    if(showflag)
    {
      showflag = 0;
    }
    //����Ƿ���յ�ָ��
    if(status.cmd == TRUE)
    {
      if(UART_RxBuffer[0] == 'm')
      {
        //�Թ̶������ƶ�
        if(UART_RxBuffer[1] == ' ')
        {
          //�Ӵ��ڻ�ȡ����
          steps = atoi((char const *)UART_RxBuffer+2);
          MSD_Move(steps, acceleration, deceleration, speed);
          okCmd = TRUE;
          printf("\n\r  ");
        }
        else if(UART_RxBuffer[1] == 'o')
        {
          if(UART_RxBuffer[2] == 'v')
          {
            if(UART_RxBuffer[3] == 'e')
            {
              //���ò��������ٶȡ����ٶȡ�����ٶ�
              if(UART_RxBuffer[4] == ':')
              {
                int i = 6;
                steps = atoi((char const *)UART_RxBuffer+5);
                while((UART_RxBuffer[i] != ',') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                acceleration = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ',') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                deceleration = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ',') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                speed = atoi((char const *)UART_RxBuffer+i);
                MSD_Move(steps, acceleration, deceleration, speed);
                okCmd = TRUE;
              }
            }
          }
        }
      }
/*
*  ����mode1Ϊ���õ��٣�mode2��������
*			
*/			
			if(UART_RxBuffer[0] == 'm')
			{
				if(UART_RxBuffer[1] == 'o')
				{
					if(UART_RxBuffer[2] == 'd')
					{
						if(UART_RxBuffer[3] == 'e')
						{
								/*  32ϸ��   mode1���õ��٣����ٶ�1�����ٶ�1������ٶ�1  */
								if(UART_RxBuffer[4] == '1')
								{
										if(UART_RxBuffer[5] == ',')
										{
											if(UART_RxBuffer[6] == 'c')
											{
												if(UART_RxBuffer[7] == 'y')
												{
													if(UART_RxBuffer[8] == ':')
													{
														int i = 6;
														acceleration = 1;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
														deceleration = 1;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
														speed = 1;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
															/*������ʽ*/
																	if(UART_RxBuffer[9] == '-')
																	{
																		if(UART_RxBuffer[11] == '/')
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+12);//�յ����ڵ�Ȧ��ָ��
																			steps = -(pulse / cy_num);
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																			else  /*Ȧ��Ϊ����*/
																			{
																				cy_num = atoi((char const *)UART_RxBuffer+10);//�յ����ڵ�Ȧ��ָ��
																				steps = -(pulse * cy_num);//������ = Ĭ�������� * Ȧ��
																				
																				MSD_Move(steps,acceleration, deceleration, speed);
																				okCmd = TRUE;
																			}
																	}
																	
																	else if(UART_RxBuffer[10] == '/')  /*������ʽ*/
																	{
																		cy_num = atoi((char const *)UART_RxBuffer+11);//�յ����ڵ�Ȧ��ָ��
																		steps = pulse / cy_num;
																		
																		MSD_Move(steps,acceleration, deceleration, speed);
																		okCmd = TRUE;
																	}
																		else  /*Ȧ��Ϊ����*/
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+9);//�յ����ڵ�Ȧ��ָ��
																			steps = pulse * cy_num;//������ = Ĭ�������� * Ȧ��
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																	
													}
												}
											}
										}	
									}
								
								
								/*  32ϸ��   mode2�������٣����ٶ�40�����ٶ�40������ٶ�40  */
								else if(UART_RxBuffer[4] == '2')
								{
										if(UART_RxBuffer[5] == ',')
										{
											if(UART_RxBuffer[6] == 'c')
											{
												if(UART_RxBuffer[7] == 'y')
												{
													if(UART_RxBuffer[8] == ':')
													{
														int i = 6;
														acceleration = 20;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
														deceleration = 20;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
														speed = 20;
														while((UART_RxBuffer[i] != 13)) i++;
														i++;
															/*������ʽ*/
																	if(UART_RxBuffer[9] == '-')
																	{
																		if(UART_RxBuffer[11] == '/')
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+12);//�յ����ڵ�Ȧ��ָ��
																			steps = -(pulse / cy_num);
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																			else  /*Ȧ��Ϊ����*/
																			{
																				cy_num = atoi((char const *)UART_RxBuffer+10);//�յ����ڵ�Ȧ��ָ��
																				steps = -(pulse * cy_num);//������ = Ĭ�������� * Ȧ��
																				
																				MSD_Move(steps,acceleration, deceleration, speed);
																				okCmd = TRUE;
																			}
																	}
																	
																	else if(UART_RxBuffer[10] == '/')  /*������ʽ*/
																	{
																		cy_num = atoi((char const *)UART_RxBuffer+11);//�յ����ڵ�Ȧ��ָ��
																		steps = pulse / cy_num;
																		
																		MSD_Move(steps,acceleration, deceleration, speed);
																		okCmd = TRUE;
																	}
																		else  /*Ȧ��Ϊ����*/
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+9);//�յ����ڵ�Ȧ��ָ��
																			steps = pulse * cy_num;//������ = Ĭ�������� * Ȧ��
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																	
													}
												}
											}
										}	
									}
								
						}
					
					}
				
				}
			
			
			}
			
			
			
			
      //���ָ���������ӡ��������
      if(okCmd != TRUE)
      {
        printf("\n\r ������������������...");
        ShowHelp();
      }

      //��մ��ڽ��ջ�������
      status.cmd = FALSE;
      uart_FlushRxBuffer();

      if(status.running == TRUE)
      {
        if(status.out_ena == TRUE)
            printf("\n�����������...");
        while(status.running == TRUE)
        {
          if(status.out_ena != TRUE)
          break;
        };
        if(status.out_ena == TRUE)
        {
          printf("OK\n");
          ShowData(stepPosition, acceleration, deceleration, speed, steps);  
        }

      }

      
    }//end if(cmd)
}



void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����

{

	for(; nCount != 0; nCount--);

}
/*********************************************END OF FILE**********************/
