#include <button4_4.h>
#include <delay.h>

/*********************************************************************
//接口：PF0-PF7
1	-->PF7
2	-->PF6
3	-->PF5
4	-->PF4
5	-->PF0
6	-->PF1
7	-->PF2
8	-->PF3
*********************************************************************/
void Button4_4_Init(void)//
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_0;	 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//浮空
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	GPIO_ResetBits(GPIOF,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);//GPIOF9,F10设置低
	
//	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, ENABLE);	 //
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化

}

/*********************************************************************

**********************************************************************/
int Button4_4_Scan(void)
{
	int keyValue=0;//
	
	GPIO_Write(GPIOF,((GPIOF->ODR & 0xff00 )| 0x0000));//拉低
    
		if((GPIOF->IDR & 0x00f0)==0x00f0)
		return -1;
	else
	{
		delay_ms(20);
		
		if((GPIOF->IDR & 0x00f0)==0x00f0)//
		return -1;	
	
	}
	
	GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x000e);//?PA8-11??0001,?????
	  switch(GPIOF->IDR & 0x00f0)
		{
			case 0x00e0 : keyValue=16;break;
			case 0x00d0 : keyValue=15;break;
			case 0x00b0 : keyValue=14;break;
			case 0x0070 : keyValue=13;break;	
		}while((GPIOF->IDR & 0x00f0)!=0x00f0);
	
	GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x000d);//?PA8-11??0010,?????
	  switch(GPIOF->IDR & 0x00f0)
		{
			case 0x00e0 : keyValue=12;break;
			case 0x00d0 : keyValue=11;break;
			case 0x00b0 : keyValue=10;break;
			case 0x0070 : keyValue=9;break;	
		}while((GPIOF->IDR & 0x00f0)!=0x00f0);
	
	GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x000b);//?PA8-11??0100,?????
	  switch(GPIOF->IDR & 0x00f0)
		{
			case 0x00e0 : keyValue=8;break;
			case 0x00d0 : keyValue=7;break;
			case 0x00b0 : keyValue=6;break;
			case 0x0070 : keyValue=5;break;	
		}while((GPIOF->IDR & 0x00f0)!=0x00f0);
	GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0007);//?PA8-11??1000,?????
	  switch(GPIOF->IDR & 0x00f0)
		{
			case 0x00e0 : keyValue=4;break;
			case 0x00d0 : keyValue=3;break;
			case 0x00b0 : keyValue=2;break;
			case 0x0070 : keyValue=1;break;	
		}while((GPIOF->IDR & 0x00f0)!=0x00f0);
		
	return 17-keyValue;
}


