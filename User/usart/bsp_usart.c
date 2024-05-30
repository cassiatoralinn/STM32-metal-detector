/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����õ�printf���ڣ��ض���printf������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F103C8T6 ���İ� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 


#include "./usart/bsp_usart.h"
#include "sys.h"
#include "tim.h"

u16 USART_RX_STA=0;//����״̬��־λ

 /**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStrue;

	// �򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

		 
	/*�������ȼ�����*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStrue.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=3;
	
	NVIC_Init(&NVIC_InitStrue);

	// ʹ�ܴ���
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(DEBUG_USARTx, ENABLE);
}


///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}


void USART1_IRQHandler(void)
{
	u8 res;//���ڽ��յ�������
	if(USART_GetITStatus(USART1,USART_IT_RXNE))//���ڽ��յ�������
	{
		res=USART_ReceiveData(USART1);
		GPIO_SetBits(GPIOB, GPIO_Pin_3);	 // �ر�����LED3

	}
}

void usart_send_data(u8 *buff,u8 len)
{
	u8 t;
	
	for(t=0;t<len;t++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)  ==  RESET);
		USART_SendData(USART1,buff[t]);
	}
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)  ==  RESET);
	
}
