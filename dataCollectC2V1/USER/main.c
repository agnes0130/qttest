#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "stmflash.h"
#include "usb_lib.h"
#include "math.h"
#include "hw_config.h"
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "task_manage.h"
#include "bsp_spi_ad7606.h"

/**********************************************************
* 函数功能 ---> USB连接/断开
* 入口参数 ---> usb_sw：USB开启/关闭控制。0：关闭
*                                         1：打开
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/	   
void USB_Port_Set(u8 usb_sw)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //使能PORTA时钟
		   	 
	if(usb_sw)	_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12) = 0;	    		  
	}
}
uint16_t sampleVol[8];
int32_t int_sampleVol[8];
uint8_t* int2byte(int data);
uint8_t *tx;
void Timer2_Init(void);
void TIM2_IRQHandler(void);
char transStr[30];
char showLcd[30];
 int main(void)
 {
	
	char showLcd[30];
	int i;
	
  SystemInit();
	delay_init(72);	     //延时初始化
	NVIC_Configuration();
	uart_init(256000);   //串口初始化
	key_init();//按键初始化
	initial_lcd();//液晶初始化
	LCD_Clear();
	LCD_Refresh_Gram();
	LCD_Show_CEStr(0,0,"AD7606");//黑色
	LCD_Show_CEStr(0,2,"Unit:1mV");//黑色
	LCD_Refresh_Gram();
	delay_ms(100);
 	USB_Port_Set(0); 	//USB先断开
	delay_ms(600);
	USB_Port_Set(1);	//USB再次连接
	USB_Config();
	Timer2_Init();           //定义定时器2的周期0.1s，并且定义定时器2的中断
	bsp_InitAD7606();	/* 配置AD7606所用的GPIO */
	AD_RANGE_10V();
	ad7606_StartRecord(200);//10ms
	//sprintf(transStr, "Range = %d, OS = %d, \r\n", g_tAD.usBuf[0], g_tAD.usBuf[1]);
	//USB_TxWrite((uint8_t*)transStr, 20);
	delay_ms(100);
	while(1)
	{
		
		
	}
 } 
 
 void Timer2_Init(void)      //定义定时器5的周期为1s，并且定义定时器5的中断
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef TIM_NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=199;   //自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1; //分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //开启定时器中断
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	TIM_NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//定时器中断通道
	TIM_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级
	TIM_NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级
	TIM_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&TIM_NVIC_InitStructure);	
	
	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	//delay_ms(100);       //100ms读一次采集的数据，然后上传
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		int_sampleVol[0] = 4*((int32_t)10000)*((float)((short)g_tAD.usBuf[0])/32768);	//单位1mv
		sprintf(transStr, "A%05d", int_sampleVol[0]);
		USB_TxWrite((uint8_t*)transStr, strlen(transStr));
		int_sampleVol[1] = ((int32_t)10000)*((float)((short)g_tAD.usBuf[1])/32768);	//单位1mv
		sprintf(transStr, "B%05d", int_sampleVol[1]);
		USB_TxWrite((uint8_t*)transStr, strlen(transStr));
		g_tAD.usWrite = 0;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}