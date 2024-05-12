#include "stm32f10x.h"
#include "sys.h"
#include "lcd.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "adc.h"
#include "SEGGER_RTT.h"
#include "NMEA.h"
#include "string.h"
#include "usmart.h"
#include "led.h"

u8 GNSS_STATE  = 0;
u8 LCD_DEBUG[] = {"test OK"};
u8 USART1_TX_BUF[USART3_MAX_RECV_LEN];                       // 串口1,发送缓存区
nmea_msg gpsx;                                               // GPS信息
__align(4) u8 dtbuf[50];                                     // 打印缓存器
const u8 *fixmode_tbl[4] = {"Fail", "Fail", " 2D ", " 3D "}; // fix mode字符串

/*
此处可添加全局常量
一行16个字节，一个汉字2字节，一个字母1字节，每个汉字只能在偶字节处起
*/
// 显示GPS定位信息
void Gps_Msg_Show(void)
{
    float tp;
    u8 Longitude[50];
    u8 Latitude[50];
    u8 UTCTime[50];

    sprintf((char *)dtbuf, "UTCTime:%02d:%02d:%02d", gpsx.utc.hour, gpsx.utc.min, gpsx.utc.sec); // 显示UTC时间
    // printf("\r\n%s\r\n",dtbuf);
    strcpy(UTCTime, (char *)dtbuf);
    LCD_Wmessage(UTCTime, LINE1);

    tp = gpsx.longitude;
    sprintf((char *)dtbuf, "%.5f %1c   ", tp /= 100000, gpsx.ewhemi); // 得到经度字符串
    // printf("\r\n%s\r\n",dtbuf);
    strcpy(Longitude, (char *)dtbuf);
    LCD_Wmessage(Longitude, LINE2);
    // delay_ms (500);
    // LCD_Wmessage(LCD_CLEAR32, LINE2);

    tp = gpsx.latitude;
    sprintf((char *)dtbuf, "%.5f %1c   ", tp /= 100000, gpsx.nshemi); // 得到纬度字符串
    // printf("\r\n%s\r\n",dtbuf);
    strcpy(Latitude, (char *)dtbuf);
    LCD_Wmessage(Latitude, LINE3);
    // delay_ms (500);
    // LCD_Wmessage(LCD_CLEAR32, LINE3);

    //	tp=gpsx.altitude;
    // 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
    //	//printf("\r\n%s\r\n",dtbuf);
    //
    //	tp=gpsx.speed;
    // 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串
    //	//printf("\r\n%s\r\n",dtbuf);
    //
    //	if(gpsx.fixmode<=3)														//定位状态
    //	{
    //		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);
    //    //printf("\r\n%s\r\n",dtbuf);
    //	}
    //	sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的GPS卫星数
    //	//printf("\r\n%s\r\n",dtbuf);
    //
    //	sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//可见GPS卫星数
    //	//printf("\r\n%s\r\n",dtbuf);
    //
    //	sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//可见北斗卫星数
    //	//printf("\r\n%s\r\n",dtbuf);
    //
    //	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
    //  //printf("\r\n%s\r\n",dtbuf);
    //

    // delay_ms (500);
    // LCD_Wmessage(LCD_CLEAR32, LINE1);
}

int main(void)
{
    u16 i, rxlen;
    u8 upload = 0; // 置1主动上传

    SEGGER_RTT_Init();
    Init_Usart(); // 串口初始化，接收发送函数调用。
    LED_Init();
    USART3_Init(115200);                            // GNSS模块串口初始化，中科微模块115200不可更改。
    delay_init();                                   // 延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    LCD_IO_Init();                                  // stm32 接LCD的IO接口初始化
    LCD_Init();                                     // LCD 初始化
    SEGGER_RTT_printf(0, "j-link rtt viewer test success!");
    usmart_dev.init(72); // usmart初始化

    while (1) {
        if (GNSS_STATE == 1) { // 串口没有收到数据
            LED0 = 0;
            LED1 = 0;
            delay_ms(100); // 延时300ms
            LED0 = 1;
            LED1 = 1;
            delay_ms(100); // 延时300ms
        }
        delay_ms(1);
        if (USART3_RX_STA & 0X8000) // 接收到一次数据了
        {

            rxlen = USART3_RX_STA & 0X7FFF; // 得到数据长度
            for (i = 0; i < rxlen; i++) USART1_TX_BUF[i] = USART3_RX_BUF[i];
            USART3_RX_STA    = 0;                     // 启动下一次接收
            USART3_TX_BUF[i] = 0;                     // 自动添加结束符
            GPS_Analysis(&gpsx, (u8 *)USART1_TX_BUF); // 分析字符串
            Gps_Msg_Show();                           // 显示信息

            if (upload) printf("\r\n%s\r\n", USART1_TX_BUF); // 发送接收到的数据到串口1
            GNSS_STATE = 1;
        }
        // LCD_Wmessage(LCD_DEBUG, LINE4); //debug use
    }
}

void USART1_IRQHandler(void)
{
    char key_value; // 读取按键键位

    // 按键测试
    u8 dis1[] = {"KeyValue:      1 "};
    u8 dis2[] = {"KeyValue:      2 "};
    u8 dis3[] = {"KeyValue:      3 "};
    u8 dis4[] = {"KeyValue:      4 "};
    u8 dis5[] = {"KeyValue:      5 "};
    u8 dis6[] = {"KeyValue:      6"};
    u8 dis7[] = {"KeyValue:      7 "};
    u8 dis8[] = {"KeyValue:      8 "};
    u8 dis0[] = {"KeyValue:      0 "};

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        key_value = USART_ReceiveData(USART1);
        switch (key_value) {
            case '1':
                LCD_Wmessage(dis1, LINE4);
                break;
            case '2':
                LCD_Wmessage(dis2, LINE4);
                break;
            case '3':
                LCD_Wmessage(dis3, LINE4);
                break;
            case '4':
                LCD_Wmessage(dis4, LINE4);
                break;
            case '5':
                LCD_Wmessage(dis5, LINE4);
                break;
            case '6':
                LCD_Wmessage(dis6, LINE4);
                break;
            case '7':
                LCD_Wmessage(dis7, LINE4);
                break;
            case '8':
                LCD_Wmessage(dis8, LINE4);
                break;
            case '0':
                LCD_Wmessage(dis0, LINE4);
                break;
                // Add more cases as needed

                // Handle the case when key_value is none of the specified values
                break;
        }
    }
}
