
#include "main.h"
#include "my_app.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "oled.h"
#include "distimer.h"
#include "exti.h"
#include "led.h"
extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 
char b[5];
char b2[6];
int distancetime=0;
int main()
{
    float LMT70_data = 0;
	int xiaoshu;
	int zhengshu;
	int first=0;
	int secound=0;
	int tem_first;
	int tem_secound;
	long long temp=0,x_atual=0; 
    delay_init(168);  //��ʼ����ʱ����
	uart_init(9600) ;
    my_app_init() ;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM3_Int_Init(5000-1,8400-1);	//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms 
	Trig_Init();   
 	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1); //��1Mhz��Ƶ�ʼ��� 
	EXTIX_Init();
	OLED_Init();
	OLED_All(0);//����
	OLED_P8x16Str(0,0,(unsigned char*)"Tem:");
    OLED_P8x16Str(70,0,(unsigned char*)"0");
	OLED_P8x16Str(0,2,(unsigned char*)"Dis:");
	OLED_P8x16Str(30,2,(unsigned char*)"0");
    OLED_P8x16Str(70,2,(unsigned char*)"Win:");
	OLED_P8x16Str(100,2,(unsigned char*)"0");

    while(1)
    {
		if(lmt70flag==3)
		{
		
        LMT70_data=my_app();
//		if(LMT70_data>22){hottime=hottime+1;}
		zhengshu=(int)LMT70_data;
		secound=zhengshu%16;
		first=zhengshu/16;
		tem_first=zhengshu/10;
		tem_secound=zhengshu%10;
        sprintf(b,"%d",tem_first);
		sprintf(b2,"%d",tem_secound);
		
        OLED_P8x16Str(70,0,b);
		OLED_P8x16Str(80,0,b2);

        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	    printf("AT+NMGS=3,0000%X%X\r\n\r\n",first,secound);
        
		lmt70flag=0;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
		}
		delay_ms(100);
		Trig1=1;
		delay_us(25);
		Trig1=0;			 
 		if(TIM5CH1_CAPTURE_STA&0X80)        //�ɹ�������һ�θߵ�ƽ
		{

			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		         //���ʱ���ܺ�
			temp+=TIM5CH1_CAPTURE_VAL;		   //�õ��ܵĸߵ�ƽʱ��
            x_atual=temp*340/20000;
//			printf("HIGH:%lld cm\r\n",x_atual); //��ӡ�ܵĸߵ�ƽʱ��
			if(x_atual<10){    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);if(distancetime==0){LED2_Init();distancetime=1;  }OLED_P8x16Str(30,2,(unsigned char*)"1"); printf("AT+NMGS=3,000100\r\n\r\n");}
			TIM5CH1_CAPTURE_STA=0;			     //������һ�β���
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		}
        


	    

	 
    }

}
