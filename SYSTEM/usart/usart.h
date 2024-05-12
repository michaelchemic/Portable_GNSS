/*========================usart.h=====================*/

#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"
#include "sys.h"

#define USART1_REC_LEN 200 // �����������ֽ��� 200
#define EN_USART1_RX 1     // ʹ�ܣ�1��/��ֹ��0������1����
// ����1
#define USART1_GPIO_PORT GPIOA
#define USART1_GPIO_CLK RCC_APB2Periph_GPIOA
#define USART1_TX_GPIO_PIN GPIO_Pin_9
#define USART1_RX_GPIO_PIN GPIO_Pin_10

void usart_init(void);
void Init_Usart(void);
void USART_Send_Byte(USART_TypeDef *USARTx, uint16_t Data);
void USART_Send_String(USART_TypeDef *USARTx, char *str);

extern u8 USART1_RX_BUF[USART1_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART1_RX_STA;                // ����״̬���

#endif /* __USART_H */
