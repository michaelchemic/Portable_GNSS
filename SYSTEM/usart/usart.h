/*========================usart.h=====================*/

#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"
#include "sys.h"

#define USART1_REC_LEN 200 // 定义最大接收字节数 200
#define EN_USART1_RX 1     // 使能（1）/禁止（0）串口1接收
// 串口1
#define USART1_GPIO_PORT GPIOA
#define USART1_GPIO_CLK RCC_APB2Periph_GPIOA
#define USART1_TX_GPIO_PIN GPIO_Pin_9
#define USART1_RX_GPIO_PIN GPIO_Pin_10

void usart_init(void);
void Init_Usart(void);
void USART_Send_Byte(USART_TypeDef *USARTx, uint16_t Data);
void USART_Send_String(USART_TypeDef *USARTx, char *str);

extern u8 USART1_RX_BUF[USART1_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART1_RX_STA;                // 接收状态标记

#endif /* __USART_H */
