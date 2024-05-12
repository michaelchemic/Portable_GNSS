/*=======================usart.c======================*/
#include "usart.h"
#include "sys.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  
  
/**
 * 功能：串口初始化函数
 * 参数：None
 * 返回值：None
 */
void usart_init(void)
{
    GPIO_InitTypeDef GPIO_Init_Structure;                            //定义GPIO结构体
    USART_InitTypeDef USART_Init_Structure;                          //定义串口结构体
	  NVIC_InitTypeDef  NVIC_Init_Structure;														//定义中断结构体
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
    RCC_APB2PeriphClockCmd(USART1_GPIO_CLK,  ENABLE);                 //开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);            //开启APB2总线复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,  ENABLE);          //开启USART1时钟
    
    //配置PA9  TX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_AF_PP;                //复用推挽
    GPIO_Init_Structure.GPIO_Pin   = USART1_TX_GPIO_PIN;
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init( USART1_GPIO_PORT, &GPIO_Init_Structure);
    
    //配置PA10 RX
    GPIO_Init_Structure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;                //复用推挽
    GPIO_Init_Structure.GPIO_Pin   = USART1_RX_GPIO_PIN;
    GPIO_Init( USART1_GPIO_PORT, &GPIO_Init_Structure);
    //串口相关配置
  	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);																					//串口中断配置
    USART_Init_Structure.USART_BaudRate = 115200;                                          //波特率设置为115200
    USART_Init_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;       //硬件流控制为无
    USART_Init_Structure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                       //模式设为收和发
    USART_Init_Structure.USART_Parity = USART_Parity_No;                                   //无校验位
    USART_Init_Structure.USART_StopBits = USART_StopBits_1;                                //一位停止位
    USART_Init_Structure.USART_WordLength = USART_WordLength_8b;                           //字长为8位   
    USART_Init(USART1, &USART_Init_Structure);                                             //初始化	
    USART_Cmd(USART1, ENABLE);                                                            //串口使能
		
		//中断结构体配置
	NVIC_Init_Structure.NVIC_IRQChannel 			=   USART1_IRQn;
	NVIC_Init_Structure.NVIC_IRQChannelCmd   	=   ENABLE;
	NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority  =  0;
	NVIC_Init_Structure.NVIC_IRQChannelSubPriority         =  1;
	NVIC_Init(&NVIC_Init_Structure);
   
}


//串口初始化函数
void Init_Usart(void)
{
	usart_init();
}



/**
 * 功能：串口写字节函数
 * 参数1：USARTx ：串口号
 * 参数2：Data   ：需写入的字节
 * 返回值：None
 */
void USART_Send_Byte(USART_TypeDef* USARTx, uint16_t Data)
{
    USART_SendData(USARTx, Data);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
}
/**
 * 功能：串口写字符串函数
 * 参数1：USARTx ：串口号
 * 参数2：str    ：需写入的字符串
 * 返回值：None
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
 * 功能：重定向
 */
int fgetc(FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==RESET);
    return (int)USART_ReceiveData(USART1);
}

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//	{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//		Res =USART_ReceiveData(USART1);	//读取接收到的数据
//		
//		if((USART1_RX_STA&0x8000)==0)//接收未完成
//			{
//			if(USART1_RX_STA&0x4000)//接收到了0x0d
//				{
//				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
//				else USART1_RX_STA|=0x8000;	//接收完成了 
//				}
//			else //还没收到0X0D
//				{	
//				if(Res==0x0d)USART1_RX_STA|=0x4000;
//				else
//					{
//					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
//					USART1_RX_STA++;
//					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
//					}		 
//				}
//			}   		 
//   } 
//}
#endif

