#include "walkmotor.h"
#include "delay.h"
/********************************
A  	B  	C  		 D
D3 	D5 	D7		 XCLK
PC9 PB6 PE6 	 PA8
********************************/



void Walkmotor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOC时钟
  //GPIO初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_11 | GPIO_Pin_13| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	GPIO_ResetBits(GPIOD, GPIO_Pin_9| GPIO_Pin_11 | GPIO_Pin_13| GPIO_Pin_15);
}

void Walkmotor_ON(void)
{
	int i;
	for(i=0; i<256; i++)
	{
		Aout=1;Bout=0;Cout=0;Dout=0;delay_ms(3);
		Aout=0;Bout=1;Cout=0;Dout=0;delay_ms(3);
		Aout=0;Bout=0;Cout=1;Dout=0;delay_ms(3);
		Aout=0;Bout=0;Cout=0;Dout=1;delay_ms(3);
	}Aout=0;Bout=0;Cout=0;Dout=0;
}
	
void Walkmotor_OFF(void)
{
	int i;
	for(i=0; i<100; i++)
	{
		Aout=0;Bout=0;Cout=0;Dout=1;delay_ms(5);
		Aout=0;Bout=0;Cout=1;Dout=0;delay_ms(5);
		Aout=0;Bout=1;Cout=0;Dout=0;delay_ms(5);
		Aout=1;Bout=0;Cout=0;Dout=0;delay_ms(5);
	}Aout=0;Bout=0;Cout=0;Dout=0;
}
	

