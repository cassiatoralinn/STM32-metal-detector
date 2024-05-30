#include "dht11.h"
#include "delay.h"
uint8_t dat[5]={0x00,0x00,0x00,0x00,0x00};
uint8_t sum = 0;

/*
*初始化输出
*/
void DHT11_GPIO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
		
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*初始化输入
*/
void DHT11_GPIO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*读取一个字节
*/
uint8_t DHT_Read_Byte(void)
{
	uint8_t temp;
	uint8_t ReadDat = 0;
	
	uint8_t retry = 0;
	uint8_t i;
	for(i=0; i<8;i++)
	{	
		//读取一位数据时，会先收到低电平信号50us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0 && retry< 100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;
		Delay_us(30);
		temp = 0;
		//再接收高电平信号16us-28us（代表数据0）或高电平信号70us（代表数据1）
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==1) temp=1;
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1 && retry<100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;
		ReadDat<<=1;
		ReadDat|=temp;
	}
	return ReadDat;
}

/*
*读取一次数据
*/
uint8_t DHT_Read(void)
{
	uint8_t i;
	uint8_t retry = 0;
	//总线设置为输出并拉低18ms
	DHT11_GPIO_OUT();
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	Delay_ms(18);
	//总线再拉高40ms
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	Delay_us(40);
	//总线设置为输入
	DHT11_GPIO_IN();
	Delay_us(20);
	//等待响应信号为低电平
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
	{	
		//等待响应信号为高电平，超时等待100us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==0&&retry<100)
		{
			Delay_us(1);
			retry++;
		}
		retry=0;
		//等待数据传送，超时等待100us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==1&&retry<100)
		{
			Delay_us(1);
			retry++;
		}
		retry=0;
		//循环读取数据，dat[0]为湿度整数数据，dat[1]为湿度小数数据，dat[2]为温度整数数据，dat[3]为温度小数数据，dat[4]为校验和数据
		for(i=0;i<5;i++)
		{
			dat[i] = DHT_Read_Byte();
		}
		Delay_us(50);
	}
	//当校验和数据=湿度整数数据+湿度小数数据+温度整数数据+温度小数数据，说明正确传送。
	sum = dat[0] + dat[1] + dat[2] + dat[3];
	if(dat[4]  == sum)
	{
		return 1;
	}
	else
		return 0;
}
