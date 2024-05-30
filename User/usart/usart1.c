#include "delay.h"
#include "usart1.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//����1 �����ڽ���SIM800Cģ������ݣ�
////////////////////////////////////////////////////////////////////////////////// 	   

//���ڽ��ջ����� 	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

//����ɨ���£�   timer=1S
//������ɨ���£� timer=10ms
//ͨ���жϽ�������2���ַ�֮���ʱ������timer�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART1_RX_STA=0;   	

void USART1_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(USART1);
		if((USART1_RX_STA&(1<<15))==0)              //�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART1_RX_STA<USART1_MAX_RECV_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM4,0);             //���������          				
				if(USART1_RX_STA==0) 				//ʹ�ܶ�ʱ��4���ж� 
				{
					TIM_Cmd(TIM4,ENABLE);           //ʹ�ܶ�ʱ��4
				}
				USART1_RX_BUF[USART1_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART1_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			}
		}
	}
}


//��ʼ��IO ����1
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart1_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //����1ʱ��ʹ��

 	USART_DeInit(USART1);                           //��λ����1
		 //USART3_TX   PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;      //PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //��ʼ��PB10
   
    //USART3_RX	  PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                          //��ʼ��PA10
	
	USART_InitStructure.USART_BaudRate = bound;                     //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;             //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  

	USART_Cmd(USART1, ENABLE);                  //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	TIM4_Int_Init(TMER4_ARR_DEFAULT,7199);	//20ms�ж�
	USART1_RX_STA=0;		//����
	TIM_Cmd(TIM4,DISABLE);	//�رն�ʱ��4

}

//����1,printf ����
//ȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�
void u1_printf(char* fmt,...)  
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	  USART_SendData(USART1,USART1_TX_BUF[j]); 
	}
}







