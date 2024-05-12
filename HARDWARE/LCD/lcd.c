#include "lcd.h"
#include "sys.h"
#include "delay.h"

u16 temp;

void LCD_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	
}
//LCD初始化
void LCD_Init()
{
	LCD_wcmd(0x30);//功能设定：基本指令集
	delay_ms(5);
	LCD_wcmd(0x0C);//显示开，关光标
	delay_ms(5);
	LCD_wcmd(0x01);//清除显示
}
//忙判断
void CheckBusy(void)
{
	u8 status;
	RS=0;
	RW=1;
	GPIOA->ODR = 0xFF;
	do
	{
		EN = 1;
		delay_ms(5);
		status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);//判断BF位
	}while(status & 0x80);
	EN=0;
}
//LCD写命令
void LCD_wcmd(u8 cmd)
{
	CheckBusy();
	RS=0;
	RW=0;
	delay_ms(5);
	temp=(temp&0xff00)|cmd;
	GPIO_Write(GPIOA,temp);
	EN=1;
	delay_ms(10);
	EN=0;
}
//LCD写数据
void LCD_wdat(u8 dat)
{
	CheckBusy();
	RS=1;
	RW=0;
	delay_ms(5);
	temp=(temp&0xff00)|dat;
	GPIO_Write(GPIOA,temp);
	EN=1;
	delay_ms(10);
	EN=0;
}
//向LCD12864中写入一行数据
void LCD_Wmessage(u8 *message,u8 address)
{
	LCD_wcmd(address);//要显示的位置
	
	while(*message>0)//这个判断很关键
	{
		LCD_wdat(*message);
		message++;
	}
}
