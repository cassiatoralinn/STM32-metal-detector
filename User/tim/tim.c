#include "tim.h"
#include "bsp_led.h"
#include "bsp_usart.h"

extern unsigned int m;
extern int flag;

//通用定时器3中断初始化
//arr：自动重装值
//psc：时钟预分频数
//定时器溢出时间计算方法：Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率，单位：Mhz
//这里使用的是定时器3！
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimBaseStructure;  //定时器句柄
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//时钟使能
	
	//定时器3初始化
	TIM_TimBaseStructure.TIM_Period = arr;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimBaseStructure.TIM_Prescaler = psc;//设置用来作为TIMx时钟频率除数的预分频数
	TIM_TimBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割：TDTS = Tck_tim
	TIM_TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM3,&TIM_TimBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_Cmd(TIM3,ENABLE);
}






void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	 GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 使能定时器1
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	

   //设置该引脚为复用输出功能,输出TIM1 CH1的PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 80KHZ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx  通道1

  TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
 
   
}


void Servo_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 			//定义GPIO初始化结构体变量
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //开启定时器2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//通道2时钟使能函数

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//设置GPIO为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA1 PA2			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度设置为 50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//按照以上参数进行 GPIO的初始化
	TIM_InternalClockConfig(TIM2);//TIM的时基单元由内部时钟控制

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr; //ARR 自动重装器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler =psc; //PSC 预分频器的值 对72M（720000000）进行 7200分频 即10K的频率下 计10000个数 1s的时间
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//重复计数器的值 CCR
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure); 

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性选择
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//输出状态使能
	TIM_OCInitStructure.TIM_Pulse = 50;//CCR,即占空比为 10%
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);//OC编号要与通道编号对应

	TIM_Cmd(TIM2,ENABLE);
}








