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

void OLED_WR_Byte(unsigned dat, unsigned cmd)    //1Ϊд���0Ϊд���ݡ�
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
		OLED_WR_Byte (0xb0 + i, OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00, OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10, OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0; n<128; n++)
		OLED_WR_Byte(0,OLED_DATA);         //0Ϊ����״̬����Ч�������뿴������Ч�����ɸ�Ϊ0xff.  
	} 
}

void OLED_Init(void)  //��ʼ��SSD1306�����������������ɡ�
{  	 
 	GPIO_InitTypeDef GPIO_InitStructure; 	 //��ʼ��GPIO���ṹ��������������ǰ�档
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //���췽ʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //������ʽ
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
//	HAL_Delay(100); //�������ʱ����Ҫ
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
//	WriteCmd(0xff); //���ȵ��� 0x00~0xff
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

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//ȫ�����
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


void OLED_CLS(void)//����
{
	OLED_Fill(0x00);
}

void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}

void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}


// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
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


// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������.h�е�����
// Description    : ��ʾASCII_8x16.h�еĺ���,16*16����
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



// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
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
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
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


//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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
void OLED_All(u8 bmp_dat)               //OLEDȫ��дͬһ����
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
void OLED_Set_Pos(u8 x, u8 y) //����OLED����
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
        if(x>120){x=0;y++;}                 //�߽�����  ǿ������
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
