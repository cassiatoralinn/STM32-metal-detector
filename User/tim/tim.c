#include "tim.h"
#include "bsp_led.h"
#include "bsp_usart.h"

extern unsigned int m;
extern int flag;

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽����Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ�ʣ���λ��Mhz
//����ʹ�õ��Ƕ�ʱ��3��
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimBaseStructure;  //��ʱ�����
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʱ��ʹ��
	
	//��ʱ��3��ʼ��
	TIM_TimBaseStructure.TIM_Period = arr;//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimBaseStructure.TIM_Prescaler = psc;//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶ��
	TIM_TimBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָTDTS = Tck_tim
	TIM_TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//�ж����ȼ�����
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// ʹ�ܶ�ʱ��1
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     	

   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80KHZ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx  ͨ��1

  TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
 
   
}


void Servo_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 			//����GPIO��ʼ���ṹ�����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //������ʱ��2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ͨ��2ʱ��ʹ�ܺ���

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//����GPIOΪ�������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA1 PA2			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�����Ϊ 50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//�������ϲ������� GPIO�ĳ�ʼ��
	TIM_InternalClockConfig(TIM2);//TIM��ʱ����Ԫ���ڲ�ʱ�ӿ���

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr; //ARR �Զ���װ����ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler =psc; //PSC Ԥ��Ƶ����ֵ ��72M��720000000������ 7200��Ƶ ��10K��Ƶ���� ��10000���� 1s��ʱ��
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//�ظ���������ֵ CCR
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure); 

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�������ѡ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//���״̬ʹ��
	TIM_OCInitStructure.TIM_Pulse = 50;//CCR,��ռ�ձ�Ϊ 10%
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);//OC���Ҫ��ͨ����Ŷ�Ӧ

	TIM_Cmd(TIM2,ENABLE);
}








