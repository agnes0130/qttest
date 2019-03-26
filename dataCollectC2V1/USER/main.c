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
* �������� ---> USB����/�Ͽ�
* ��ڲ��� ---> usb_sw��USB����/�رտ��ơ�0���ر�
*                                         1����
* ������ֵ ---> none
* ����˵�� ---> none
**********************************************************/	   
void USB_Port_Set(u8 usb_sw)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //ʹ��PORTAʱ��
		   	 
	if(usb_sw)	_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
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
	delay_init(72);	     //��ʱ��ʼ��
	NVIC_Configuration();
	uart_init(256000);   //���ڳ�ʼ��
	key_init();//������ʼ��
	initial_lcd();//Һ����ʼ��
	LCD_Clear();
	LCD_Refresh_Gram();
	LCD_Show_CEStr(0,0,"AD7606");//��ɫ
	LCD_Show_CEStr(0,2,"Unit:1mV");//��ɫ
	LCD_Refresh_Gram();
	delay_ms(100);
 	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(600);
	USB_Port_Set(1);	//USB�ٴ�����
	USB_Config();
	Timer2_Init();           //���嶨ʱ��2������0.1s�����Ҷ��嶨ʱ��2���ж�
	bsp_InitAD7606();	/* ����AD7606���õ�GPIO */
	AD_RANGE_10V();
	ad7606_StartRecord(200);//10ms
	//sprintf(transStr, "Range = %d, OS = %d, \r\n", g_tAD.usBuf[0], g_tAD.usBuf[1]);
	//USB_TxWrite((uint8_t*)transStr, 20);
	delay_ms(100);
	while(1)
	{
		
		
	}
 } 
 
 void Timer2_Init(void)      //���嶨ʱ��5������Ϊ1s�����Ҷ��嶨ʱ��5���ж�
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef TIM_NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period=199;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	TIM_NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//��ʱ���ж�ͨ��
	TIM_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�
	TIM_NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�
	TIM_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&TIM_NVIC_InitStructure);	
	
	TIM_Cmd(TIM2,ENABLE);
}

void TIM2_IRQHandler(void)
{
	//delay_ms(100);       //100ms��һ�βɼ������ݣ�Ȼ���ϴ�
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		int_sampleVol[0] = 4*((int32_t)10000)*((float)((short)g_tAD.usBuf[0])/32768);	//��λ1mv
		sprintf(transStr, "A%05d", int_sampleVol[0]);
		USB_TxWrite((uint8_t*)transStr, strlen(transStr));
		int_sampleVol[1] = ((int32_t)10000)*((float)((short)g_tAD.usBuf[1])/32768);	//��λ1mv
		sprintf(transStr, "B%05d", int_sampleVol[1]);
		USB_TxWrite((uint8_t*)transStr, strlen(transStr));
		g_tAD.usWrite = 0;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}