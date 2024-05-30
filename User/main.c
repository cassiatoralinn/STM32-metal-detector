#include "stm32f10x.h"
#include "delay.h"
#include "./led/bsp_led.h"  
#include "./usart/bsp_usart.h"
#include "tim.h"
#include "sys.h"
#include "adc.h"
unsigned char  data_to_send[] = "Sund 33 66 4.1 55 66\r\n";  //中文字符占两个
extern u8 dat[5];//DHT11数据存放数组
int flag=0;//温湿度采集标志位，1.5s采集一次数据
unsigned int m,n;//毫秒计算变量
float tempurate,humi;//温度 湿度  变量定义
char str[64]={0};//屏幕显示字符串
u8 data_len=8;
unsigned int cichang;

int main(void)
{
/*初始化LED、串口、OLED、DHT11*/
	delay_init();//初始化延迟函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级设置
	USART_Config();//串口1初始化函数
	LED_GPIO_Config();//LED初始化函数z
	Adc_Init();
	
	TIM3_Init(1000-1,72-1);     //打开定时器3，周期为500us

	while(1)
	{
		
		  cichang=Get_Adc(8);
        
        // 将cichang的值格式化为字符串
      sprintf((char*)data_to_send, "cichang: %d\r\n", cichang);
        
        // 发送格式化后的字符串到串口
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



