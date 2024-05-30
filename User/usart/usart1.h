#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

#define USART1_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART1_RX_EN 			1					//0,������;1,����.

#define TMER4_ARR_DEFAULT		199					//��ʱ��4����Ĭ��Ϊ 199 20ms

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART1_MAX_RECV_LEN�ֽ�
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�
extern vu16 USART1_RX_STA;   						//��������״̬

void usart1_init(u32 bound);		//����1��ʼ�� 
void u1_printf(char* fmt,...);
#endif












