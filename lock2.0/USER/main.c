//使用蓝牙串口发送密码时需要有以"::"结尾的效验码，比如"123456::"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "sram.h"   
#include "malloc.h" 
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "oled.h"
#include "beep.h"
#include "button4_4.h"
#include "AS608.h"
#include "walkmotor.h"
#include "usart2.h"
#include "rtc.h"
#include "MFRC522.h"
#include "usart3.h"	
#include "stmflash.h"

#define usart2_baund  57600//串口2波特率，根据指纹模块波特率更改

//要写入到STM32 FLASH的字符串数组
const u8 TEXT_Buffer[]={0x17,0x23,0x6f,0x60,0,0};
#define TEXT_LENTH sizeof(TEXT_Buffer)	 		  	//数组长度	
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)
#define FLASH_SAVE_ADDR  0X0802C124 	//设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
										//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数
u8** kbd_tbl;

void Display_Data(void);//显示时间
void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
int press_FR(void);//刷指纹
void ShowErrMessage(u8 ensure);//显示确认码错误信息
int password(void);//密码锁
void SetPassworld(void);//修改密码
void starting(void);//开机界面信息
u8 MFRC522_lock(void);//刷卡解锁
u8 Add_Rfid(void);		//录入
void Set_Time(void);
void Massige(void);
u8 Pwd[7]="      ";  //解锁密码1
u8 Pwd2[7]="      ";  //解锁密码2
u8 cardid[6]={0,0,0,0,0,0};  //卡号1
int Error;  //密码验证信息


//数字的ASCII码
uc8 numberascii[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//显示缓冲区
u8  dispnumber5buf[6];
u8  dispnumber3buf[4];
u8  dispnumber2buf[3];
//MFRC522数据区
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8  card_readbuf[18];
//SM05-S数据区
u8  sm05cmdbuf[15]={14,128,0,22,5,0,0,0,4,1,157,16,0,0,21};
//extern声明变量已在外部的C文件里定义，可以在主文件中使用
extern u8  sm05receivebuf[16];	//在中断C文件里定义
extern u8  sm05_OK;							//在中断C文件里定义

u8 * week[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
u8 * setup[7]={"1、录入指纹","2、删除指纹","3、修改密码","4、修改时间","5、录入卡片","6、查看信息"};

int main(void)
{
	u16 set=0;
	int key_num;
	int time1;
	int time2;		//锁屏时间
	char arrow=0;  //箭头位子
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	My_RTC_Init();		 		//初始化RTC
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
	delay_init(168);  //初始化延时函数
	uart_init(9600);	//初始化串口1波特率为9600，用于支持NBIOT
	uart3_init(9600);// 蓝牙初始化
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	LED_Init();					//初始化LED  
	//	BEEP_Init();    		//beep初始化
	Button4_4_Init();//矩阵按键初始化
	OLED_Init(); 			//oled初始化
	W25QXX_Init();				//初始化W25Q128
	
	Walkmotor_Init(); //步进电机初始化
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	starting();//开机信息  logo
	
	
	//0X08020004  密码1
	//0X08090004	密码2
	//0X080f0004	卡号1
	//	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
	STMFLASH_Read(0X08020004,(u32*)Pwd,2);	//读取密码1
	STMFLASH_Read(0X08090004,(u32*)Pwd2,2); //读取密码2
	STMFLASH_Read(0X080f0004,(u32*)cardid,1); //读取卡号1
//	printf("pwd=%s",Pwd);
//	printf("pwd2=%s",Pwd2);
//	printf("cardid={%X,%X,%X,%X}\n",cardid[0],cardid[1],cardid[2],cardid[3]);

	 while(1)
	 {
//锁屏界面
MAIN:
			OLED_Show_Font(56,48,0);//显示锁图标
			while(1)
			{
					time1++;
					if((time1%2000)==0)//时间显示：每50ms更新一次显示数据
					{
						Display_Data();
					}
					if((time1%5000)==0)//每200ms检测一次卡片，蓝牙数据
					{
						//MFRC522解锁
						time1=0;
						MFRC522_Initializtion();			
						Error=MFRC522_lock();
						if(Error==0)goto MENU;	
						else 
							OLED_Show_Font(56,48,0);//锁
						//手机蓝牙解锁密码1
						Error=usart3_cherk((char*)Pwd); //检查接收是否有误        
						if(Error==0){
							OLED_Clear();
							Show_Str(20,10,128,24,"解锁中...",24,0);	
							Walkmotor_ON();
							Show_Str(20,10,128,24,"已解锁！",24,0);
							goto MENU;	
						}
						else 
							OLED_Show_Font(56,48,0);//锁
						//手机蓝牙解锁密码2
						Error=usart3_cherk((char*)Pwd2);
						if(Error==0){
							OLED_Clear();
							Show_Str(20,10,128,24,"解锁中...",24,0);	
							Walkmotor_ON();
							Show_Str(20,10,128,24,"已解锁！",24,0);
							goto MENU;	
						}
						else 
							OLED_Show_Font(56,48,0);//锁
						
					} 
					//指纹解锁
					if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
					{
							while(PS_Sta){
							Error=press_FR();//刷指纹
							if(Error==0){ goto MENU;}
							else 
								OLED_Show_Font(56,48,0);//锁
						}
					}
					//密码锁
					key_num=Button4_4_Scan();	//按键扫描
					if(key_num!=-1)
					{
						Error=password();
						if(Error==0)goto MENU;	
						else 
							OLED_Show_Font(56,48,0);//锁
					}
			}
//主界面
MENU:
			//主页菜单显示
			OLED_Clear();
			if(arrow<3){
				Show_Str(5,arrow*16,128,16,"->",16,0);
				set=0;}
			else {
				Show_Str(5,(arrow-3)*16,128,16,"->",16,0);
				set=3;}
			Show_Str(25,0,128,16,setup[set],16,0);
			Show_Str(25,16,128,16,setup[set+1],16,0);
			Show_Str(25,32,128,16,setup[set+2],16,0);
			Show_Str(0,52,128,12,"上   下   锁定  确定",12,0);
			time2=0;
			while(1)
			{
				//超时锁屏
				time2++;
				if(time2>10000 | key_num==15){  
					time2 =0;
					OLED_Clear();
						Show_Str(30,20,128,16,"已锁定！",16,0);
						OLED_Show_Font(56,48,0);//锁
						delay_ms(1000);
						OLED_Clear();
						goto MAIN;
				}
				//功能选项选择
				key_num=Button4_4_Scan();	
				if(key_num)
				{
					if(key_num==13){
						if(arrow>0)arrow--;
						goto MENU;
					}
					if(key_num==14){
						if(arrow<5)arrow++;
						goto MENU;
					}
					if(key_num==16){
						switch(arrow)
						{
							case 0:Add_FR();		break;//录指
							case 1:Del_FR();		break;//删指纹
							case 2:SetPassworld();break;//修改密码
							case 3:Set_Time(); break;  //设置时间
							case 4:Add_Rfid(); break;  //录入卡片
//							case 5:Massige(); break;  //显示信息
						}
						goto MENU;
					}		
				}delay_ms(1);
			}	
	}//while
}//main


//显示时间
void Display_Data(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	u8 tbuf[40];
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)tbuf,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
	OLED_ShowString(0,0,tbuf,24);	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)tbuf,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year+1,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
	OLED_ShowString(68,26,tbuf,12);		
	sprintf((char*)tbuf,"%s",week[RTC_DateStruct.RTC_WeekDay-1]); 
	OLED_ShowString(110,12,tbuf,12);	
}
//获取键盘数值
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num)
		{
			if(key_num==13)return 0xFFFF;//‘返回’键
			if(key_num==14)return 0xFF00;//		
			if(key_num>0&&key_num<10&&num<99)//‘1-9’键(限制输入3位数)
				num =num*10+key_num;		
			if(key_num==15)num =num/10;//‘Del’键			
			if(key_num==10&&num<99)num =num*10;//‘0’键
			if(key_num==16)return num;  //‘Enter’键
		}
		OLED_ShowNum(78,32,num,3,12);
	}	
}
//密码锁
int password(void)
{
	int  key_num=0,i=0,satus=0;
	u16 num=0,num2=0,time3=0;
	u8 pwd[11]="          ";
	u8 hidepwd[11]="          ";
	
	OLED_Clear();//清屏
	Show_Str(5,0,128,16,"密码：",16,0);
	Show_Str(10,16,128,12," 1   2   3   4  ",12,0);
	Show_Str(10,28,128,12," 5   6   7   8  ",12,0);
	Show_Str(10,40,128,12," 9   0      Dis ",12,0);
	Show_Str(10,52,128,12,"Del Clr Bck Ok  ",12,0);
//	Show_Str(102,36,128,12,"显示",12,0);
//	Show_Str(0,52,128,12,"删除 清空   返回 确认",12,0);
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num)
		{	
			time3=0;
			if(key_num>=1 && key_num<10 && i>=0 && i<10){  //‘1-9’键
				pwd[i]=key_num+0x30;
				hidepwd[i]='*';
				i++;
			}	
			if(key_num==10 && i>=0 && i<10){       //‘0’键
				pwd[i]=0x30;
				hidepwd[i]='*';
				i++;
			}
			if(key_num==12){//显示  
				satus=!satus;
			}
			if(key_num==13 && i>0){
				pwd[--i]=' ';  //‘del’键
				hidepwd[i]=' '; 
			}
			if(key_num==14){
				while(i--){
					pwd[i]=' ';  //‘清空’键
					hidepwd[i]=' ';  //‘清空’键
				}
				i=0;
			}
			if(key_num==15){//返回
				OLED_Clear();//清屏
				delay_ms(500);
				return -1;
			}
			if(key_num==16)break;  //‘Enter’键
		}
		if(satus==0)OLED_ShowString(53,0,hidepwd,12);
		else OLED_ShowString(53,0,pwd,12);
		
		time3++;
		if(time3%1000==0){
			OLED_Clear();//清屏
			return -1;
		}
	}	
	for(i=0; i<10; i++){   //验证虚伪密码
		if(pwd[i]==Pwd[num])num++;
			else num=0;
		if(num==6)
			break;
	}
	for(i=0; i<10; i++){   //验证密码
		if(pwd[i]==Pwd2[num2])num2++;
			else num2=0;
		if(num2==6)
			break;
	}
	if(num==6 | num2==6){
		printf("AT+NMGS=3,000003\r\n\r\n");
		OLED_Clear();//清屏
		Show_Str(20,10,128,24,"解锁中...",24,0);	
		Walkmotor_ON();
		Show_Str(20,10,128,24,"已解锁！ ",24,0);	
		OLED_Show_Font(56,48,1);//开锁		
		delay_ms(1500);
		OLED_Clear();//清屏
		return 0;
	}
		else {
			OLED_Clear();//清屏
			Show_Str(45,48,128,16,"密码错误！",16,0);
			delay_ms(1500);
			OLED_Clear();//清屏
			return -1;
		}
	
}


//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
	Show_Str(0,48,128,12,(u8*)EnsureMessage(ensure),12,0);	
	delay_ms(300);
	OLED_ShowString(0,48,"                ",12);	
}
//录指纹
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	int key_num;
	u16 ID;
	OLED_Clear();//清屏
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num==16){
			OLED_Clear();//清屏
			return ;
		}
		switch (processnum)
		{
			case 0:
				//OLED_Clear();//清屏
				i++;
				Show_Str(0,0,128,16,"=== 录入指纹 ===",16,0);
				Show_Str(0,24,128,12,"请按指纹！  ",12,0);	
				Show_Str(104,52,128,12,"返回",12,0);			
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						Show_Str(0,24,128,12,"指纹正常！    ",12,0);	
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);
				//OLED_Clear();//清屏
				break;
			
			case 1:
				i++;
				Show_Str(0,24,128,12,"请再按一次指纹",12,0);		
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						Show_Str(0,24,128,12,"指纹正常！",12,0);	
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				//OLED_Clear();//清屏
				break;

			case 2:		
				Show_Str(0,24,128,12,"对比两次指纹        ",12,0);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					Show_Str(0,24,128,12,"两次指纹一样       ",12,0);
					processnum=3;//跳到第四步
				}
				else 
				{
					Show_Str(0,24,128,12,"对比失败 请重录    ",12,0);	
					ShowErrMessage(ensure);
					i=0;
					OLED_Clear();//清屏
					processnum=0;//跳回第一步		
				}
				delay_ms(1200);
				//OLED_Clear();//清屏
				break;

			case 3:
			Show_Str(0,24,128,12,"生成指纹模板...   ",12,0);	
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
//					
					Show_Str(0,24,128,12,"生成指纹模板成功!",12,0);
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				//OLED_Clear();//清屏
			Show_Str(0,24,128,12,"请输入储存ID:      ",12,0);
			Show_Str(122,52,128,12," ",12,0);
			Show_Str(0,52,128,12,"删除 清空        确认",12,0);
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//输入ID必须小于模块容量最大的数值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				if(ensure==0x00) 
				{			
          OLED_Clear();//清屏
					Show_Str(0,30,128,16,"录指纹成功!",16,0);	
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					Show_Str(66,52,128,12,"剩余",12,0);
					OLED_ShowNum(90,52,AS608Para.PS_max-ValidN,3,12);
					delay_ms(1500);
					OLED_Clear();	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}
				OLED_Clear();//清屏					
				break;				
		}
		delay_ms(400);
		if(i==10)//超过5次没有按手指则退出
		{
			OLED_Clear();
			break;
		}				
	}
}

//刷指纹
int press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	OLED_Clear();
		Show_Str(0,0,128,16,"正在检测指纹",16,0);
	if(ensure==0x00)//获取图像成功 
	{	
		BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			
			BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			
			if(ensure==0x00)//搜索成功
			{				
				OLED_Clear();
				Show_Str(20,10,128,24,"解锁中...",24,0);	
				Walkmotor_ON();
				Show_Str(20,10,128,24,"已解锁！",24,0);
				OLED_Show_Font(112,18,1);//开锁				
				str=mymalloc(SRAMIN,2000);
//				sprintf(str,"ID:%d      匹配分:%d",seach.pageID,seach.mathscore);
				if(seach.pageID==0){printf("AT+NMGS=3,010000\r\n\r\n");}
				if(seach.pageID==1){printf("AT+NMGS=3,000100\r\n\r\n");}
				if(seach.pageID==2){printf("AT+NMGS=3,000001\r\n\r\n");}
				Show_Str(0,52,128,12,(u8*)str,12,0);	
				myfree(SRAMIN,str);
				delay_ms(1800);
				OLED_Clear();
				return 0;
			}
			else {
				ShowErrMessage(ensure);	
				delay_ms(1000);
				OLED_Clear();
				return -1;
			}				
	  }
		else
			ShowErrMessage(ensure);
	 BEEP=0;//关闭蜂鸣器	
	 delay_ms(2000);
		OLED_Clear();
		
	}
	return -1;	
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	OLED_Clear();
	Show_Str(0,0,128,16,"=== 删除指纹 ===",16,0);	
	Show_Str(0,16,128,12,"输入指纹ID：",12,0);
	Show_Str(0,52,128,12,"返回 清空    确认删除",12,0);
	delay_ms(50);
//	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//获取返回的数值
	if(num==0xFFFF)
		goto MENU ; //返回主页面
	else if(num==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(num,1);//删除单个指纹
	if(ensure==0)
	{
		OLED_Clear();
		Show_Str(0,20,128,12,"删除指纹成功！",12,0);		
		Show_Str(80,48,128,12,"剩余",12,0);		
	}
  else
		ShowErrMessage(ensure);	
	
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	OLED_ShowNum(110,48,AS608Para.PS_max-ValidN,3,12);
	delay_ms(1200);
	
MENU:	
	OLED_Clear();
}
//修改密码
void SetPassworld(void)
{
	int pwd_ch=0;
	int  key_num=0,i=0,satus=0;
	u16 time4=0;
	u8 pwd[6]="      ";
	u8 hidepwd[6]="      ";
	u8 buf[10];
	OLED_Clear();//清屏
	Show_Str(10,16,128,12," 1   2   3   4  ",12,0);
	Show_Str(10,28,128,12," 5   6   7   8  ",12,0);
	Show_Str(10,40,128,12," 9   0  chg Dis ",12,0);
	Show_Str(10,52,128,12,"Del Clr Bck Ok  ",12,0);
	while(1)
	{
		sprintf((char*)buf,"新密码%d:",pwd_ch+1);
		Show_Str(5,0,128,16,buf,16,0);
		key_num=Button4_4_Scan();	
		if(key_num)
		{	
			time4=0;
			if(key_num>=1 && key_num<10 && i>=0 && i<6){  //‘1-9’键
				pwd[i]=key_num+0x30;
				hidepwd[i]='*';
				i++;
			}	
			if(key_num==10 && i>=0 && i<10){       //‘0’键
				pwd[i]=0x30;
				hidepwd[i]='*';
				i++;
			}
			if(key_num==11){//显示  
				pwd_ch=!pwd_ch;
			}
			if(key_num==12){//显示  
				satus=!satus;
			}
			if(key_num==13 && i>0){
				pwd[--i]=' ';  //‘del’键
				hidepwd[i]=' '; 
			}
			if(key_num==14){
				while(i--){
					pwd[i]=' ';  //‘清空’键
					hidepwd[i]=' ';  //‘清空’键
				}
				i=0;
			}
			if(key_num==15){//返回
				OLED_Clear();//清屏
				delay_ms(500);
				return ;
			}
			if(key_num==16)break;  //‘Enter’键
		}
		if(satus==0)OLED_ShowString(70,0,hidepwd,12);
		else OLED_ShowString(70,0,pwd,12);
		
		time4++;
		if(time4%1000==0){
			OLED_Clear();//清屏
			return ;
		}
	}	
	if(pwd_ch==0)
	{
		STMFLASH_Write(0X08020004,(u32*)pwd,2);
		STMFLASH_Read(0X08020004,(u32*)Pwd,2);	//读取密码1
		printf("pwd=%s",Pwd);
	}
	else
	{		
		STMFLASH_Write(0X08090004,(u32*)pwd,2);
		STMFLASH_Read(0X08090004,(u32*)Pwd2,2); //读取密码2
		printf("pwd2=%s",Pwd2);
	}
	OLED_Clear();//清屏
	Show_Str(0,48,128,16,"密码修改成功 ！",16,0);
	delay_ms(1000);
}
//设置时间
void Set_Time(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	u8 year,mon,dat,wek,hour,min,sec;
	u16 time5=0;
	u8 tbuf[40];
	int key_num;
	int st=0;
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	year=RTC_DateStruct.RTC_Year;
	mon=RTC_DateStruct.RTC_Month;
	dat=RTC_DateStruct.RTC_Date;
	wek=RTC_DateStruct.RTC_WeekDay;
	hour=RTC_TimeStruct.RTC_Hours;
	min=RTC_TimeStruct.RTC_Minutes;
	sec=RTC_TimeStruct.RTC_Seconds;
	OLED_Clear();
	Show_Str(98,38,128,12,"<--",12,0);
	Show_Str(0,52,128,12,"减  加    切换  确定",12,0);
	
	while(1)
	{
		time5++;
		key_num=Button4_4_Scan();	
			if(key_num==12 | time5==3000){
				OLED_Clear();//清屏
				return ;
			}
			if(key_num==13){
				switch(st)
				{
					case 0:if(hour>0)hour--;break;
					case 1:if(min>0)min--;break;
					case 2:if(sec>0)sec--;break;
					case 3:if(wek>0)wek--;break;
					case 4:if(year>0)year--;break;
					case 5:if(mon>0)mon--;break;
					case 6:if(dat>0)dat--;break;
				}
			}
			if(key_num==14){
				switch(st)
				{
					case 0:if(hour<23)hour++;break;
					case 1:if(min<59)min++;break;
					case 2:if(sec<59)sec++;break;
					case 3:if(wek<7)wek++;break;
					case 4:if(year<99)year++;break;
					case 5:if(mon<12)mon++;break;
					case 6:if(dat<31)dat++;break;
				}
			}
			if(key_num==15){
				if(st<7)st++;
				if(st==7)st=0;
			}
			if(key_num==16){
				break;
			}
		if(time5%250==0)
		{
			switch(st)			//闪烁
				{
					case 0:OLED_ShowString(0,0,"  ",24);break;
					case 1:OLED_ShowString(36,0,"  ",24);break;
					case 2:OLED_ShowString(72,0,"  ",24);break;
					case 3:OLED_ShowString(110,12,"   ",12);break;
					case 4:OLED_ShowString(68,26,"    ",12);break;
					case 5:OLED_ShowString(98,26,"  ",12);break;
					case 6:OLED_ShowString(116,26,"  ",12);break;
				}
		}
		if(time5%500==0)
		{
			time5=0;
			sprintf((char*)tbuf,"%02d:%02d:%02d",hour,min,sec); 
			OLED_ShowString(0,0,tbuf,24);	
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			sprintf((char*)tbuf,"20%02d-%02d-%02d",year,mon,dat); 
			OLED_ShowString(68,26,tbuf,12);		
			sprintf((char*)tbuf,"%s",week[wek-1]); 
			OLED_ShowString(110,12,tbuf,12);	
		}delay_ms(1);
	}
	RTC_Set_Time(hour,min,sec,RTC_H12_AM);	//设置时间
	RTC_Set_Date(year,mon,dat,wek);		//设置日期
	OLED_Clear();
	Show_Str(20,48,128,16,"设置成功！",16,0);
	delay_ms(1000);
}

//录入新卡
u8 Add_Rfid(void)
{
	u16 time6=0;
	u8 i,key_num,status=1,card_size;
	OLED_Clear();
	Show_Str(0,0,128,16,"=== 录入卡片 ===",16,0);	
	Show_Str(0,20,128,12,"请放入新卡片：",12,0);	
	Show_Str(0,52,128,12,"返回",12,0);
	MFRC522_Initializtion();			//初始化MFRC522
	while(1)
	{
		AntennaOn();
		status=MFRC522_Request(0x52, card_pydebuf);			//寻卡
		if(status==0)		//如果读到卡
		{
			printf("rc522 ok\n");
			Show_Str(0,38,128,12,"读卡成功！",12,0);
			status=MFRC522_Anticoll(card_numberbuf);			//防撞处理			
			card_size=MFRC522_SelectTag(card_numberbuf);	//选卡
			status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//验卡
			status=MFRC522_Write(4, card_writebuf);				//写卡（写卡要小心，特别是各区的块3）
			status=MFRC522_Read(4, card_readbuf);					//读卡
			printf("卡的类型：%#x %#x",card_pydebuf[0],card_pydebuf[1]);
			//卡序列号显，最后一字节为卡的校验码
			printf("卡的序列号：");
			for(i=0;i<5;i++)
			{
				printf("%#x ",card_numberbuf[i]);
			}
			printf("\n");
			//卡容量显示，单位为Kbits
			printf("卡的容量：%dKbits\n",card_size);
			AntennaOff();
			Show_Str(0,38,128,12,"正在录入.",12,0);
			STMFLASH_Write(0X080f0004,(u32*)card_numberbuf,2);
			STMFLASH_Read(0X080f0004,(u32*)cardid,1); //读取卡号1
			printf("cardid={%X,%X,%X,%X}\n",cardid[0],cardid[1],cardid[2],cardid[3]);
			Show_Str(0,38,128,12,"录入成功！",12,0);
			delay_ms(1000);
			OLED_Clear();
			return 0;
		}
		key_num=Button4_4_Scan();	
		time6++;
		if(time6%5000==0 | key_num==13)
		{
			OLED_Clear();
			return 1;
		}
	}
}
//rfid卡锁
u8 MFRC522_lock(void)
{
	u8 i,j,status=1,card_size;
	u8 count;
	AntennaOn();
  status=MFRC522_Request(0x52, card_pydebuf);			//寻卡
	if(status==0)		//如果读到卡
	{
		printf("rc522 ok\n");
		status=MFRC522_Anticoll(card_numberbuf);			//防撞处理			
		card_size=MFRC522_SelectTag(card_numberbuf);	//选卡
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//验卡
		status=MFRC522_Write(4, card_writebuf);				//写卡（写卡要小心，特别是各区的块3）
		status=MFRC522_Read(4, card_readbuf);					//读卡
		//MFRC522_Halt();															//使卡进入休眠状态
		//卡类型显示
		
		printf("卡的类型：%#x %#x",card_pydebuf[0],card_pydebuf[1]);
		

		//卡序列号显，最后一字节为卡的校验码
		printf("卡的序列号：");
		count=0;
		for(i=0;i<5;i++)
		{
			printf("%#x ",card_numberbuf[i]);
			if(card_numberbuf[i]==cardid[i])count++;
		}
		if(count==4)
		{
			printf("rfid 匹配成功，解锁\n");
			printf("AT+NMGS=3,000002\r\n\r\n");
			OLED_Clear();
			Show_Str(20,10,128,24,"解锁中...",24,0);	
			Walkmotor_ON();
			Show_Str(20,10,128,24,"已解锁！",24,0);
			return 0;
		}
		
		printf("\n");
		//卡容量显示，单位为Kbits
		
		printf("卡的容量：%dKbits\n",card_size);
		
		
		
		//读一个块的数据显示
		printf("卡数据：\n");
		for(i=0;i<2;i++)		//分两行显示
		{
			for(j=0;j<9;j++)	//每行显示8个
			{
				printf("%#x ",card_readbuf[j+i*9]);
			}
			printf("\n");
		}

	}	
	
	AntennaOff();
	return 1;
}
//显示信息
void Massige(void)
{
	OLED_Clear();
	Show_Str(0,0,128,12,"智能门锁系统",12,0); 
	delay_ms(1000);
}
//开机信息
void starting(void)
{
	u8 ensure;
	char *str;			  
	u8 key;
	delay_ms(3000);
	OLED_Clear();
	while(font_init()) 			//检查字库
	{
			OLED_Clear();	   	   	  
			OLED_ShowString(0,0,"Explorer STM32F4",6);
			OLED_Refresh_Gram();//更新OLED	
			while(SD_Init())			//检测SD卡
			{
				OLED_ShowString(0,12,"SD Card Failed!",6);
				delay_ms(200);
				OLED_ShowString(0,12,"               ",6);
				delay_ms(200);	
				OLED_Refresh_Gram();//更新OLED	
			}								 						    
				OLED_ShowString(0,24,"SD Card OK",6);
				OLED_ShowString(0,36,"Font Updating...",6);
				key=update_font(20,110,16,"0:");//更新字库
			while(key)//更新失败		
			{			 		  
				//LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
				OLED_ShowString(0,48,"Font Update Failed!",6);
				delay_ms(200);
				OLED_ShowString(0,48,"                   ",6);
				delay_ms(200);		       
			} 		    
	} 
/*********************************开机信息提示***********************************/
	OLED_Clear();
	Show_Str(0,0,128,12,"fingerprint system!",12,0); 
	Show_Str(0,12,128,12,"connect to as608",12,0);
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(400);
		Show_Str(0,24,128,12,"connect failed! ",12,0);
		delay_ms(800);
		Show_Str(0,24,128,12,"connect to as608",12,0);	
	}
	Show_Str(0,24,128,12,"connect succeed!",12,0);	
	str=mymalloc(SRAMIN,30);
	sprintf(str,"baud:%d  addr:%x",usart2_baund,AS608Addr);
	Show_Str(0,36,128,12,(u8*)str,12,0);
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息	
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	
	if(ensure==0x00)
	{
		mymemset(str,0,50);
		sprintf(str,"capacity:%d  Lv: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		Show_Str(0,48,128,12,(u8*)str,12,0);
	}
	else
		ShowErrMessage(ensure);	
	
	myfree(SRAMIN,str);
	delay_ms(1000);
	OLED_Clear();

	
}






