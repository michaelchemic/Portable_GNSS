/*=======================usart.c======================*/
#include "usart.h"
#include "sys.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	  
  
/**
 * ���ܣ����ڳ�ʼ������
 * ������None
 * ����ֵ��None
 */
void usart_init(void)
{
    GPIO_InitTypeDef GPIO_Init_Structure;                            //����GPIO�ṹ��
    USART_InitTypeDef USART_Init_Structure;                          //���崮�ڽṹ��
	  NVIC_InitTypeDef  NVIC_Init_Structure;														//�����жϽṹ��
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
    RCC_APB2PeriphClockCmd(USART1_GPIO_CLK,  ENABLE);                 //����GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);            //����APB2���߸���ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,  ENABLE);          //����USART1ʱ��
    
    //����PA9  TX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_AF_PP;                //��������
    GPIO_Init_Structure.GPIO_Pin   = USART1_TX_GPIO_PIN;
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init( USART1_GPIO_PORT, &GPIO_Init_Structure);
    
    //����PA10 RX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;                //��������
    GPIO_Init_Structure.GPIO_Pin   = USART1_RX_GPIO_PIN;
    GPIO_Init( USART1_GPIO_PORT, &GPIO_Init_Structure);
    //�����������
  	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);																					//�����ж�����
    USART_Init_Structure.USART_BaudRate = 115200;                                          //����������Ϊ115200
    USART_Init_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;       //Ӳ��������Ϊ��
    USART_Init_Structure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                       //ģʽ��Ϊ�պͷ�
    USART_Init_Structure.USART_Parity = USART_Parity_No;                                   //��У��λ
    USART_Init_Structure.USART_StopBits = USART_StopBits_1;                                //һλֹͣλ
    USART_Init_Structure.USART_WordLength = USART_WordLength_8b;                           //�ֳ�Ϊ8λ   
    USART_Init(USART1, &USART_Init_Structure);                                             //��ʼ��	
    USART_Cmd(USART1, ENABLE);                                                            //����ʹ��
		
		//�жϽṹ������
	NVIC_Init_Structure.NVIC_IRQChannel 			=   USART1_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelCmd   	=   ENABLE;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority  =  0;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority         =  1;
	NVIC_Init(&NVIC_Init_Structure);
   
}


//���ڳ�ʼ������
void Init_Usart(void)
{
	usart_init();
}



/**
 * ���ܣ�����д�ֽں���
 * ����1��USARTx �����ں�
 * ����2��Data   ����д����ֽ�
 * ����ֵ��None
 */
void USART_Send_Byte(USART_TypeDef* USARTx, uint16_t Data)
{
    USART_SendData(USARTx, Data);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
}
/**
 * ���ܣ�����д�ַ�������
 * ����1��USARTx �����ں�
 * ����2��str    ����д����ַ���
 * ����ֵ��None
 */
void USART_Send_String(USART_TypeDef* USARTx, char *str)
{
    uint16_t i=0;
    do
    {
        USART_Send_Byte(USARTx,  *(str+i));
        i++;
    }
    while(*(str + i) != '\0');
        
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
}

/**
 * ���ܣ��ض���
 */
int fgetc(FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
    return (int)USART_ReceiveData(USART1);
}

//void USART1_IRQHandler(void)                	//����1�жϷ������
//	{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		
//		if((USART1_RX_STA&0x8000)==0)//����δ���
//			{
//			if(USART1_RX_STA&0x4000)//���յ���0x0d
//				{
//				if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
//				else USART1_RX_STA|=0x8000;	//��������� 
//				}
//			else //��û�յ�0X0D
//				{	
//				if(Res==0x0d)USART1_RX_STA|=0x4000;
//				else
//					{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//					}		 
//				}
//			}   		 
//   } 
//}
#endif

