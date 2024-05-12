#ifndef _LCD_H
#define _LCD_H
#include "sys.h"
#include "delay.h"

#define RS PBout(0) //并行的指令/数据选择信号，串行的片选信号
#define RW PBout(1) //1并行，0串行 硬件数据口切换
#define EN PCout(15)//并行作为使能信号，串行作为同步时钟

//屏幕显示行数宏定义
#define LINE1 0x80 //10000000
#define LINE2 0x90 //10010000
#define LINE3 0x88 //10001000
#define LINE4 0x98 //10011000

void LCD_IO_Init(void);
void CheckBusy(void);  
void LCD_wdat(u8 dat);
void LCD_wcmd(u8 com);
void LCD_Init(void);
void LCD_Wmessage(u8 *message, u8 address);

#endif
