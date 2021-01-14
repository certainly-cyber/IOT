#ifndef __OLED_H
#define __OLED_H	 
#include "delay.h"

 


#define OLED0561_ADD	0x78  // OLED��I2C��ַ����ֹ�޸ģ�
#define COM				0x00  // OLED ָ���ֹ�޸ģ�
#define DAT 			0x40  // OLED ���ݣ���ֹ�޸ģ�
		  	 
//-----------------OLED IIC�˿ڶ���------------------- 
//-----------------    ����   ------------------------  
//-----------------	C2 -> SCL-------------------------
//-----------------	C3 -> SDA------------------------- 					   

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_2)    //C2�����õ�
#define OLED_SCLK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_2)      //C2�����ø�

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define OLED_SDIN_Set() GPIO_SetBits(GPIOC,GPIO_Pin_3)

#define OLED_CMD  0	  //д����
#define OLED_DATA 1	  //д����

void OLED_Init(void);    //��������
void OLED_Clear(void); 
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Command(unsigned char IIC_Command);



void OLED_Init(void);//��ʼ��
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);//ȫ�����
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);//��ʾ�ַ���
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);//��ʾ����
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);//��ʾͼƬ
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);
void OLED_All(u8 bmp_dat);
void OLED_P8x16Str(u8 x,u8 y,u8 ch[]);

#endif



