#ifndef __OLED_H
#define __OLED_H	 
#include "delay.h"

 


#define OLED0561_ADD	0x78  // OLED的I2C地址（禁止修改）
#define COM				0x00  // OLED 指令（禁止修改）
#define DAT 			0x40  // OLED 数据（禁止修改）
		  	 
//-----------------OLED IIC端口定义------------------- 
//-----------------    接线   ------------------------  
//-----------------	C2 -> SCL-------------------------
//-----------------	C3 -> SDA------------------------- 					   

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_2)    //C2引脚置低
#define OLED_SCLK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_2)      //C2引脚置高

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define OLED_SDIN_Set() GPIO_SetBits(GPIOC,GPIO_Pin_3)

#define OLED_CMD  0	  //写命令
#define OLED_DATA 1	  //写数据

void OLED_Init(void);    //函数声明
void OLED_Clear(void); 
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Command(unsigned char IIC_Command);



void OLED_Init(void);//初始化
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);//全屏填充
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);//显示字符串
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);//显示汉字
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);//显示图片
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);
void OLED_All(u8 bmp_dat);
void OLED_P8x16Str(u8 x,u8 y,u8 ch[]);

#endif



