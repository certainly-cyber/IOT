#include "oled.h"
#include "ascii.h"
#include "delay.h"
//#include "oledfont.h" 

void IIC_Start()
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	delay_us(5);
	OLED_SDIN_Clr();
	delay_us(6);
	OLED_SCLK_Clr();
}

void IIC_Stop()
{	
	OLED_SCLK_Clr() ;
	OLED_SDIN_Clr();
	OLED_SCLK_Set();
	delay_us(6);
	OLED_SDIN_Set();	
	delay_us(6);
}

void IIC_Ack()
{
	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	delay_us(2);
	OLED_SCLK_Set();
	delay_us(5);	
	OLED_SCLK_Clr();
}

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m, da;
	da = IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0; i<8; i++)		
	{
		m = da;
		OLED_SCLK_Clr();
		m = m & 0x80;
		if(m == 0x80)
		{
			OLED_SDIN_Set();
		}
		else
	    OLED_SDIN_Clr();
		da = da << 1;
		delay_us(2);
		OLED_SCLK_Set();
		delay_us(2);
		OLED_SCLK_Clr();
		delay_us(2);
	}
}

void WriteDat(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			// D/C#=0; R/W#=0
	IIC_Ack();	
    Write_IIC_Byte(0x40);			// write data
	IIC_Ack();	
    Write_IIC_Byte(IIC_Data);
    IIC_Ack();	
    IIC_Stop();
}

void WriteCmd(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Ack();	
    Write_IIC_Byte(0x00);			//write command
	IIC_Ack();	
    Write_IIC_Byte(IIC_Command); 
	IIC_Ack();	
    IIC_Stop();
}

void OLED_WR_Byte(unsigned dat, unsigned cmd)    //1为写命令，0为写数据。
{
   if(cmd) 
   WriteDat(dat);
   else 
   WriteCmd(dat);
}

void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0; i<8; i++)  
	{  
		OLED_WR_Byte (0xb0 + i, OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00, OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10, OLED_CMD);      //设置显示位置—列高地址   
		for(n=0; n<128; n++)
		OLED_WR_Byte(0,OLED_DATA);         //0为灭屏状态，无效果，若想看到亮屏效果，可改为0xff.  
	} 
}

void OLED_Init(void)  //初始化SSD1306，主函数调用它即可。
{  	 
 	GPIO_InitTypeDef GPIO_InitStructure; 	 //初始化GPIO，结构体的声明，需放在前面。
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //推挽方式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉方式
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	delay_ms(200);
	OLED_WR_Byte(0xae,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0x10,OLED_CMD);
	OLED_WR_Byte(0x40,OLED_CMD);
	OLED_WR_Byte(0xb0,OLED_CMD);
	OLED_WR_Byte(0x81,OLED_CMD);
	OLED_WR_Byte(0xff,OLED_CMD);
	OLED_WR_Byte(0xa1,OLED_CMD);
	OLED_WR_Byte(0xa6,OLED_CMD);
	OLED_WR_Byte(0xc9,OLED_CMD);
	OLED_WR_Byte(0xa8,OLED_CMD);
	OLED_WR_Byte(0x1f,OLED_CMD);
	OLED_WR_Byte(0xd3,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0xd5,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0xd9,OLED_CMD);
	OLED_WR_Byte(0xf1,OLED_CMD);
	OLED_WR_Byte(0xda,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0xdb,OLED_CMD);
	OLED_WR_Byte(0x40,OLED_CMD);
	OLED_WR_Byte(0x8d,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0xaf,OLED_CMD);
	OLED_Clear();
}  
// 
//	void OLED_Init(void)
//{
//	HAL_Delay(100); //这里的延时很重要
//	
//	WriteCmd(0xAE); //display off
//	WriteCmd(0x20);	//Set Memory Addressing Mode	
//	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
//	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
//	WriteCmd(0xc8);	//Set COM Output Scan Direction
//	WriteCmd(0x00); //---set low column address
//	WriteCmd(0x10); //---set high column address
//	WriteCmd(0x40); //--set start line address
//	WriteCmd(0x81); //--set contrast control register
//	WriteCmd(0xff); //亮度调节 0x00~0xff
//	WriteCmd(0xa1); //--set segment re-map 0 to 127
//	WriteCmd(0xa6); //--set normal display
//	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
//	WriteCmd(0x3F); //
//	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
//	WriteCmd(0xd3); //-set display offset
//	WriteCmd(0x00); //-not offset
//	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
//	WriteCmd(0xf0); //--set divide ratio
//	WriteCmd(0xd9); //--set pre-charge period
//	WriteCmd(0x22); //
//	WriteCmd(0xda); //--set com pins hardware configuration
//	WriteCmd(0x12);
//	WriteCmd(0xdb); //--set vcomh
//	WriteCmd(0x20); //0x20,0.77xVcc
//	WriteCmd(0x8d); //--set DC-DC enable
//	WriteCmd(0x14); //
//	WriteCmd(0xaf); //--turn on oled panel
//}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}


void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}


// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
void OLED_ShowStr(unsigned char x, unsigned char y,  unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在.h中的索引
// Description    : 显示ASCII_8x16.h中的汉字,16*16点阵
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}



// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}
  





void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>128-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_SetPos(x,y);	
			for(i=0;i<8;i++)
			WriteDat(F8X16[c*16+i]);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
			WriteDat(F8X16[c*16+i+8]);
			}
			else {	
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
				WriteDat(F6x8[c][i]);
				
			}
}
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	


//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
void OLED_All(u8 bmp_dat)               //OLED全屏写同一数据
{
    u8 y,x;
    for(y=0;y<8;y++)
    {
        WriteCmd(0xb0+y);
        WriteCmd(0x01);
        WriteCmd(0x10);
        for(x=0;x<128;x++)
        WriteDat(bmp_dat);
    }
}
void OLED_Set_Pos(u8 x, u8 y) //设置OLED坐标
{
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}
void OLED_P8x16Str(u8 x,u8 y,u8 ch[])
{
    u8 c=0,i=0,j=0;
    while (ch[j]!='\0')
    {
        c =ch[j]-32;
        if(x>120){x=0;y++;}                 //边界限制  强制拉回
        OLED_Set_Pos(x,y);
        for(i=0;i<8;i++)
        WriteDat(F8X16[c*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
        WriteDat(F8X16[c*16+i+8]);
        x+=8;
        j++;
    }
}
