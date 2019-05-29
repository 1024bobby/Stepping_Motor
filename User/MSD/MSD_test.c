#include "MSD_test.h"  
#include <math.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_exti.h" 
#include "MicroStepDriver.h" 
void ShowHelp(void);
void ShowData(int position, int acceleration, int deceleration, int speed, int steps);
void Delay(__IO u32 nCount);

/*! \brief 打印帮助命令
 */
void ShowHelp(void)
{
    printf("\n\r——————————————步进电机驱动程序-—————————————");
    printf("\n\r开发板按键，KEY1：驱动器禁止输出(脱机状态)  KEY2：驱动器恢复正常工作 ");
		printf("\nsteps为正时顺时针旋转，为负时逆时针旋   \n");
		printf("\n输入  move:steps,accel,decel,speed后按回车键配置参数(脉冲，加速度，减速度，最大速度)\r");
		printf("\n如:   move:6400,20,20,20\n\r");
	
		printf("\n输入  modex,cy:+-y 配置速度与圈数，x为1,2分别为低、中速,“y” 为圈数,“+-” 为逆顺时针\r");
		printf("\n输入  modex,cy:+-1/y 配置速度与圈数, “1/y” 为y分之一圈数,“+-” 为逆顺时针\r");
		printf("\n如:   mode2,cy:1 或 mode1,cy:-1/4\n\n\n\n\n");
}

/*! \brief 打印电机参数
 *  \param acceleration 加速度
 *  \param deceleration 减速度
 *  \param speed        最大速度
 *  \param steps        移动步数
 */

void ShowData(int position, int acceleration, int deceleration, int speed, int steps)
{
	printf("脉冲数:%d",steps);
  printf("	加速度:%.2frad/s^2",1.0*acceleration);
  printf("  减速度:%.2frad/s^2",1.0*deceleration);
  printf("  最大速度:%.2frad/s(%.2frpm)",1.0*speed,9.55*speed);
  printf("\r电机当前位置: %d\r\n\r\r\r",position);
}


/**

  * @brief  处理串口接收到的数据

  * @param  无

  * @retval 无

  */
void DealSerialData(void)
{
	
    static char showflag =1;
    //默认移动步数
    static int steps;
    //默认加速度
    static int acceleration;
    //默认减速度
    static int deceleration;
    //默认最大速度
    static int speed;
    //默认脉冲数
    static int pulse = 6400;
    //收到串口的圈数指令
    static int cy_num;		
	
    
//    int acc_temp=0;
//    int dec_temp=0;
//    int speed_temp=0;
    
    //接收到正确的指令才为TRUE
    char okCmd = FALSE;
    if(showflag)
    {
      showflag = 0;
    }
    //检查是否接收到指令
    if(status.cmd == TRUE)
    {
      if(UART_RxBuffer[0] == 'm')
      {
        //以固定步数移动
        if(UART_RxBuffer[1] == ' ')
        {
          //从串口获取步数
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
              //设置步数、加速度、减速度、最大速度
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
*  输入mode1为配置低速，mode2配置中速
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
								/*  32细分   mode1配置低速，加速度1，减速度1，最高速度1  */
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
															/*分数形式*/
																	if(UART_RxBuffer[9] == '-')
																	{
																		if(UART_RxBuffer[11] == '/')
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+12);//收到串口的圈数指令
																			steps = -(pulse / cy_num);
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																			else  /*圈数为整数*/
																			{
																				cy_num = atoi((char const *)UART_RxBuffer+10);//收到串口的圈数指令
																				steps = -(pulse * cy_num);//脉冲数 = 默认脉冲数 * 圈数
																				
																				MSD_Move(steps,acceleration, deceleration, speed);
																				okCmd = TRUE;
																			}
																	}
																	
																	else if(UART_RxBuffer[10] == '/')  /*分数形式*/
																	{
																		cy_num = atoi((char const *)UART_RxBuffer+11);//收到串口的圈数指令
																		steps = pulse / cy_num;
																		
																		MSD_Move(steps,acceleration, deceleration, speed);
																		okCmd = TRUE;
																	}
																		else  /*圈数为整数*/
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+9);//收到串口的圈数指令
																			steps = pulse * cy_num;//脉冲数 = 默认脉冲数 * 圈数
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																	
													}
												}
											}
										}	
									}
								
								
								/*  32细分   mode2配置中速，加速度40，减速度40，最高速度40  */
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
															/*分数形式*/
																	if(UART_RxBuffer[9] == '-')
																	{
																		if(UART_RxBuffer[11] == '/')
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+12);//收到串口的圈数指令
																			steps = -(pulse / cy_num);
																			
																			MSD_Move(steps,acceleration, deceleration, speed);
																			okCmd = TRUE;
																		}
																			else  /*圈数为整数*/
																			{
																				cy_num = atoi((char const *)UART_RxBuffer+10);//收到串口的圈数指令
																				steps = -(pulse * cy_num);//脉冲数 = 默认脉冲数 * 圈数
																				
																				MSD_Move(steps,acceleration, deceleration, speed);
																				okCmd = TRUE;
																			}
																	}
																	
																	else if(UART_RxBuffer[10] == '/')  /*分数形式*/
																	{
																		cy_num = atoi((char const *)UART_RxBuffer+11);//收到串口的圈数指令
																		steps = pulse / cy_num;
																		
																		MSD_Move(steps,acceleration, deceleration, speed);
																		okCmd = TRUE;
																	}
																		else  /*圈数为整数*/
																		{
																			cy_num = atoi((char const *)UART_RxBuffer+9);//收到串口的圈数指令
																			steps = pulse * cy_num;//脉冲数 = 默认脉冲数 * 圈数
																			
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
			
			
			
			
      //如果指令有无则打印帮助命令
      if(okCmd != TRUE)
      {
        printf("\n\r 输入有误，请重新输入...");
        ShowHelp();
      }

      //清空串口接收缓冲数组
      status.cmd = FALSE;
      uart_FlushRxBuffer();

      if(status.running == TRUE)
      {
        if(status.out_ena == TRUE)
            printf("\n电机正在运行...");
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



void Delay(__IO uint32_t nCount)	 //简单的延时函数

{

	for(; nCount != 0; nCount--);

}
/*********************************************END OF FILE**********************/
