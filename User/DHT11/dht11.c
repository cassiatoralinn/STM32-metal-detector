#include "dht11.h"
#include "delay.h"
uint8_t dat[5]={0x00,0x00,0x00,0x00,0x00};
uint8_t sum = 0;

/*
*��ʼ�����
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
*��ʼ������
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
*��ȡһ���ֽ�
*/
uint8_t DHT_Read_Byte(void)
{
	uint8_t temp;
	uint8_t ReadDat = 0;
	
	uint8_t retry = 0;
	uint8_t i;
	for(i=0; i<8;i++)
	{	
		//��ȡһλ����ʱ�������յ��͵�ƽ�ź�50us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0 && retry< 100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;
		Delay_us(30);
		temp = 0;
		//�ٽ��ոߵ�ƽ�ź�16us-28us����������0����ߵ�ƽ�ź�70us����������1��
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
*��ȡһ������
*/
uint8_t DHT_Read(void)
{
	uint8_t i;
	uint8_t retry = 0;
	//��������Ϊ���������18ms
	DHT11_GPIO_OUT();
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	Delay_ms(18);
	//����������40ms
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	Delay_us(40);
	//��������Ϊ����
	DHT11_GPIO_IN();
	Delay_us(20);
	//�ȴ���Ӧ�ź�Ϊ�͵�ƽ
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
	{	
		//�ȴ���Ӧ�ź�Ϊ�ߵ�ƽ����ʱ�ȴ�100us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==0&&retry<100)
		{
			Delay_us(1);
			retry++;
		}
		retry=0;
		//�ȴ����ݴ��ͣ���ʱ�ȴ�100us
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==1&&retry<100)
		{
			Delay_us(1);
			retry++;
		}
		retry=0;
		//ѭ����ȡ���ݣ�dat[0]Ϊʪ���������ݣ�dat[1]Ϊʪ��С�����ݣ�dat[2]Ϊ�¶��������ݣ�dat[3]Ϊ�¶�С�����ݣ�dat[4]ΪУ�������
		for(i=0;i<5;i++)
		{
			dat[i] = DHT_Read_Byte();
		}
		Delay_us(50);
	}
	//��У�������=ʪ����������+ʪ��С������+�¶���������+�¶�С�����ݣ�˵����ȷ���͡�
	sum = dat[0] + dat[1] + dat[2] + dat[3];
	if(dat[4]  == sum)
	{
		return 1;
	}
	else
		return 0;
}
