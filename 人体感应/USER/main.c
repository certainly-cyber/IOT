#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "key.h"
int state=0;
int i=0;
int send=0;
//ALIENTEK ̽����STM32F407������ ʵ��8
//��ʱ���ж�ʵ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK  
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(9600);
	LED_Init();				//��ʼ��LED�˿�
  KEY_Init();
			printf("AT+NMGS=3,000000\r\n\r\n");
 //	TIM3_Int_Init(5000-1,8400-1);	//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms
  for(i=0;i<10;i++){delay_ms(1000);}																																			
	     LED1=0;
	
	while(1)
	{
   // printf("AT+NMGS=3,000200\r\n\r\n");

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) ==1){LED0=0;if(send==0){printf("AT+NMGS=3,000200\r\n\r\n");}send=send+1;}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0 ){LED0=1;send=0;}

	};
}
