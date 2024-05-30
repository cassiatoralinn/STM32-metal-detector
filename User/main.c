#include "stm32f10x.h"
#include "delay.h"
#include "./led/bsp_led.h"  
#include "./usart/bsp_usart.h"
#include "tim.h"
#include "sys.h"
#include "adc.h"
unsigned char  data_to_send[] = "Sund 33 66 4.1 55 66\r\n";  //�����ַ�ռ����
extern u8 dat[5];//DHT11���ݴ������
int flag=0;//��ʪ�Ȳɼ���־λ��1.5s�ɼ�һ������
unsigned int m,n;//����������
float tempurate,humi;//�¶� ʪ��  ��������
char str[64]={0};//��Ļ��ʾ�ַ���
u8 data_len=8;
unsigned int cichang;

int main(void)
{
/*��ʼ��LED�����ڡ�OLED��DHT11*/
	delay_init();//��ʼ���ӳٺ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ�����
	USART_Config();//����1��ʼ������
	LED_GPIO_Config();//LED��ʼ������z
	Adc_Init();
	
	TIM3_Init(1000-1,72-1);     //�򿪶�ʱ��3������Ϊ500us

	while(1)
	{
		
		  cichang=Get_Adc(8);
        
        // ��cichang��ֵ��ʽ��Ϊ�ַ���
      sprintf((char*)data_to_send, "cichang: %d\r\n", cichang);
        
        // ���͸�ʽ������ַ���������
			if(cichang<1610)
			{
				BEEP=1;
				LED=0;
			}
			if(cichang>=1610)
			{
				BEEP=0;
				LED=1;
			}
	
			}
		
}

/*********************************************END OF FILE**********************/

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		n++;
		
		if(n>=300)
		{
	        usart_send_data(data_to_send, strlen((char*)data_to_send));
				n=0;
		}
	}
}



