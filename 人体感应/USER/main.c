#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "key.h"
int state=0;
int i=0;
int send=0;
//ALIENTEK 探索者STM32F407开发板 实验8
//定时器中断实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK  
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(9600);
	LED_Init();				//初始化LED端口
  KEY_Init();
			printf("AT+NMGS=3,000000\r\n\r\n");
 //	TIM3_Int_Init(5000-1,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms
  for(i=0;i<10;i++){delay_ms(1000);}																																			
	     LED1=0;
	
	while(1)
	{
   // printf("AT+NMGS=3,000200\r\n\r\n");

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) ==1){LED0=0;if(send==0){printf("AT+NMGS=3,000200\r\n\r\n");}send=send+1;}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0 ){LED0=1;send=0;}

	};
}
