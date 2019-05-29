#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f4xx.h"


#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//系统状态
struct GLOBAL_FLAGS {
  //当步进电机正在运行时，值为1
  unsigned char running:1;
  //当串口接收到数据时，值为1
  unsigned char cmd:1;
  //当驱动器正常输出时,值为1
  unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS status;
extern int stepPosition;

#define T1_FREQ 1000000     //定时器频率
#define FSPR    200         //步进电机单圈步数
#define SPR     (FSPR*100)  //100细分的步数
// 数学常数。 用于MSD_Move函数的简化计算
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // 
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//速度曲线状态
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

typedef struct {
  //电机运行状态
  unsigned char run_state : 3;
  //电机运行方向
  unsigned char dir : 1;
  //下一个脉冲延时周期，启动时为加速度速率
  unsigned int step_delay;
  //开始减速的位置
  unsigned int decel_start;
  //减速距离
  signed int decel_val;
  //最小延时（即最大速度）
  signed int min_delay;
  //加速或者减速计数器
  signed int accel_count;
} speedRampData;


// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 这里我们使用定时器TIM2
#define            MSD_PULSE_TIM                    TIM2
#define            MSD_PULSE_TIM_APBxClock_FUN      RCC_APB1PeriphClockCmd
#define            MSD_PULSE_TIM_CLK                RCC_APB1Periph_TIM2
// 定时器输出PWM通道，PA0是通道1
#define            MSD_PULSE_OCx_Init               TIM_OC4Init
#define            MSD_PULSE_OCx_PreloadConfig      TIM_OC4PreloadConfig
// 定时器中断
#define            MSD_PULSE_TIM_IRQ                TIM2_IRQn
#define            MSD_PULSE_TIM_IRQHandler         TIM2_IRQHandler

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            MSD_PULSE_TIM_PERIOD             (72-1)
#define            MSD_PULSE_TIM_PSC                149//719//(180-1)

// 步进电机脉冲输出通道
#define            MSD_PULSE_AHBxClock_FUN          RCC_AHB1PeriphClockCmd
#define            MSD_PULSE_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define            MSD_PULSE_PORT                   GPIOA
#define            MSD_PULSE_PIN                    GPIO_Pin_3
#define            MSD_PULSE_PIN_AF                 GPIO_AF_TIM2
#define            MSD_PULSE_PIN_SOURCE             GPIO_PinSource3

// 步进电机方向控制
#define            MSD_DIR_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            MSD_DIR_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define            MSD_DIR_PORT                     GPIOC
#define            MSD_DIR_PIN                      GPIO_Pin_0

// 步进电机输出使能引脚
#define            MSD_ENA_AHBxClock_FUN            RCC_AHB1PeriphClockCmd
#define            MSD_ENA_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define            MSD_ENA_PORT                     GPIOB
#define            MSD_ENA_PIN                      GPIO_Pin_14


#define DIR(a)	if (a == CW)	\
					GPIO_ResetBits(MSD_DIR_PORT,MSD_DIR_PIN);\
					else		\
					GPIO_SetBits(MSD_DIR_PORT,MSD_DIR_PIN)
                    
/**************************函数声明********************************/

void MSD_Init(void);
void MSD_ENA(FunctionalState NewState);
void MSD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

#endif	/* __BSP_ADVANCETIME_H */


