//ʹ���������ڷ�������ʱ��Ҫ����"::"��β��Ч���룬����"123456::"

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

#define usart2_baund  57600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

//Ҫд�뵽STM32 FLASH���ַ�������
const u8 TEXT_Buffer[]={0x17,0x23,0x6f,0x60,0,0};
#define TEXT_LENTH sizeof(TEXT_Buffer)	 		  	//���鳤��	
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)
#define FLASH_SAVE_ADDR  0X0802C124 	//����FLASH �����ַ(����Ϊż��������������,Ҫ���ڱ�������ռ�õ�������.
										//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
u8** kbd_tbl;

void Display_Data(void);//��ʾʱ��
void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
int press_FR(void);//ˢָ��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ
int password(void);//������
void SetPassworld(void);//�޸�����
void starting(void);//����������Ϣ
u8 MFRC522_lock(void);//ˢ������
u8 Add_Rfid(void);		//¼��
void Set_Time(void);
void Massige(void);
u8 Pwd[7]="      ";  //��������1
u8 Pwd2[7]="      ";  //��������2
u8 cardid[6]={0,0,0,0,0,0};  //����1
int Error;  //������֤��Ϣ


//���ֵ�ASCII��
uc8 numberascii[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//��ʾ������
u8  dispnumber5buf[6];
u8  dispnumber3buf[4];
u8  dispnumber2buf[3];
//MFRC522������
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8  card_readbuf[18];
//SM05-S������
u8  sm05cmdbuf[15]={14,128,0,22,5,0,0,0,4,1,157,16,0,0,21};
//extern�������������ⲿ��C�ļ��ﶨ�壬���������ļ���ʹ��
extern u8  sm05receivebuf[16];	//���ж�C�ļ��ﶨ��
extern u8  sm05_OK;							//���ж�C�ļ��ﶨ��

u8 * week[7]={"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
u8 * setup[7]={"1��¼��ָ��","2��ɾ��ָ��","3���޸�����","4���޸�ʱ��","5��¼�뿨Ƭ","6���鿴��Ϣ"};

int main(void)
{
	u16 set=0;
	int key_num;
	int time1;
	int time2;		//����ʱ��
	char arrow=0;  //��ͷλ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	My_RTC_Init();		 		//��ʼ��RTC
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	delay_init(168);  //��ʼ����ʱ����
	uart_init(9600);	//��ʼ������1������Ϊ9600������֧��NBIOT
	uart3_init(9600);// ������ʼ��
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	LED_Init();					//��ʼ��LED  
	//	BEEP_Init();    		//beep��ʼ��
	Button4_4_Init();//���󰴼���ʼ��
	OLED_Init(); 			//oled��ʼ��
	W25QXX_Init();				//��ʼ��W25Q128
	
	Walkmotor_Init(); //���������ʼ��
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	starting();//������Ϣ  logo
	
	
	//0X08020004  ����1
	//0X08090004	����2
	//0X080f0004	����1
	//	STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
	STMFLASH_Read(0X08020004,(u32*)Pwd,2);	//��ȡ����1
	STMFLASH_Read(0X08090004,(u32*)Pwd2,2); //��ȡ����2
	STMFLASH_Read(0X080f0004,(u32*)cardid,1); //��ȡ����1
//	printf("pwd=%s",Pwd);
//	printf("pwd2=%s",Pwd2);
//	printf("cardid={%X,%X,%X,%X}\n",cardid[0],cardid[1],cardid[2],cardid[3]);

	 while(1)
	 {
//��������
MAIN:
			OLED_Show_Font(56,48,0);//��ʾ��ͼ��
			while(1)
			{
					time1++;
					if((time1%2000)==0)//ʱ����ʾ��ÿ50ms����һ����ʾ����
					{
						Display_Data();
					}
					if((time1%5000)==0)//ÿ200ms���һ�ο�Ƭ����������
					{
						//MFRC522����
						time1=0;
						MFRC522_Initializtion();			
						Error=MFRC522_lock();
						if(Error==0)goto MENU;	
						else 
							OLED_Show_Font(56,48,0);//��
						//�ֻ�������������1
						Error=usart3_cherk((char*)Pwd); //�������Ƿ�����        
						if(Error==0){
							OLED_Clear();
							Show_Str(20,10,128,24,"������...",24,0);	
							Walkmotor_ON();
							Show_Str(20,10,128,24,"�ѽ�����",24,0);
							goto MENU;	
						}
						else 
							OLED_Show_Font(56,48,0);//��
						//�ֻ�������������2
						Error=usart3_cherk((char*)Pwd2);
						if(Error==0){
							OLED_Clear();
							Show_Str(20,10,128,24,"������...",24,0);	
							Walkmotor_ON();
							Show_Str(20,10,128,24,"�ѽ�����",24,0);
							goto MENU;	
						}
						else 
							OLED_Show_Font(56,48,0);//��
						
					} 
					//ָ�ƽ���
					if(PS_Sta)	 //���PS_Sta״̬���������ָ����
					{
							while(PS_Sta){
							Error=press_FR();//ˢָ��
							if(Error==0){ goto MENU;}
							else 
								OLED_Show_Font(56,48,0);//��
						}
					}
					//������
					key_num=Button4_4_Scan();	//����ɨ��
					if(key_num!=-1)
					{
						Error=password();
						if(Error==0)goto MENU;	
						else 
							OLED_Show_Font(56,48,0);//��
					}
			}
//������
MENU:
			//��ҳ�˵���ʾ
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
			Show_Str(0,52,128,12,"��   ��   ����  ȷ��",12,0);
			time2=0;
			while(1)
			{
				//��ʱ����
				time2++;
				if(time2>10000 | key_num==15){  
					time2 =0;
					OLED_Clear();
						Show_Str(30,20,128,16,"��������",16,0);
						OLED_Show_Font(56,48,0);//��
						delay_ms(1000);
						OLED_Clear();
						goto MAIN;
				}
				//����ѡ��ѡ��
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
							case 0:Add_FR();		break;//¼ָ
							case 1:Del_FR();		break;//ɾָ��
							case 2:SetPassworld();break;//�޸�����
							case 3:Set_Time(); break;  //����ʱ��
							case 4:Add_Rfid(); break;  //¼�뿨Ƭ
//							case 5:Massige(); break;  //��ʾ��Ϣ
						}
						goto MENU;
					}		
				}delay_ms(1);
			}	
	}//while
}//main


//��ʾʱ��
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
//��ȡ������ֵ
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num)
		{
			if(key_num==13)return 0xFFFF;//�����ء���
			if(key_num==14)return 0xFF00;//		
			if(key_num>0&&key_num<10&&num<99)//��1-9����(��������3λ��)
				num =num*10+key_num;		
			if(key_num==15)num =num/10;//��Del����			
			if(key_num==10&&num<99)num =num*10;//��0����
			if(key_num==16)return num;  //��Enter����
		}
		OLED_ShowNum(78,32,num,3,12);
	}	
}
//������
int password(void)
{
	int  key_num=0,i=0,satus=0;
	u16 num=0,num2=0,time3=0;
	u8 pwd[11]="          ";
	u8 hidepwd[11]="          ";
	
	OLED_Clear();//����
	Show_Str(5,0,128,16,"���룺",16,0);
	Show_Str(10,16,128,12," 1   2   3   4  ",12,0);
	Show_Str(10,28,128,12," 5   6   7   8  ",12,0);
	Show_Str(10,40,128,12," 9   0      Dis ",12,0);
	Show_Str(10,52,128,12,"Del Clr Bck Ok  ",12,0);
//	Show_Str(102,36,128,12,"��ʾ",12,0);
//	Show_Str(0,52,128,12,"ɾ�� ���   ���� ȷ��",12,0);
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num)
		{	
			time3=0;
			if(key_num>=1 && key_num<10 && i>=0 && i<10){  //��1-9����
				pwd[i]=key_num+0x30;
				hidepwd[i]='*';
				i++;
			}	
			if(key_num==10 && i>=0 && i<10){       //��0����
				pwd[i]=0x30;
				hidepwd[i]='*';
				i++;
			}
			if(key_num==12){//��ʾ  
				satus=!satus;
			}
			if(key_num==13 && i>0){
				pwd[--i]=' ';  //��del����
				hidepwd[i]=' '; 
			}
			if(key_num==14){
				while(i--){
					pwd[i]=' ';  //����ա���
					hidepwd[i]=' ';  //����ա���
				}
				i=0;
			}
			if(key_num==15){//����
				OLED_Clear();//����
				delay_ms(500);
				return -1;
			}
			if(key_num==16)break;  //��Enter����
		}
		if(satus==0)OLED_ShowString(53,0,hidepwd,12);
		else OLED_ShowString(53,0,pwd,12);
		
		time3++;
		if(time3%1000==0){
			OLED_Clear();//����
			return -1;
		}
	}	
	for(i=0; i<10; i++){   //��֤��α����
		if(pwd[i]==Pwd[num])num++;
			else num=0;
		if(num==6)
			break;
	}
	for(i=0; i<10; i++){   //��֤����
		if(pwd[i]==Pwd2[num2])num2++;
			else num2=0;
		if(num2==6)
			break;
	}
	if(num==6 | num2==6){
		printf("AT+NMGS=3,000003\r\n\r\n");
		OLED_Clear();//����
		Show_Str(20,10,128,24,"������...",24,0);	
		Walkmotor_ON();
		Show_Str(20,10,128,24,"�ѽ����� ",24,0);	
		OLED_Show_Font(56,48,1);//����		
		delay_ms(1500);
		OLED_Clear();//����
		return 0;
	}
		else {
			OLED_Clear();//����
			Show_Str(45,48,128,16,"�������",16,0);
			delay_ms(1500);
			OLED_Clear();//����
			return -1;
		}
	
}


//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
	Show_Str(0,48,128,12,(u8*)EnsureMessage(ensure),12,0);	
	delay_ms(300);
	OLED_ShowString(0,48,"                ",12);	
}
//¼ָ��
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	int key_num;
	u16 ID;
	OLED_Clear();//����
	while(1)
	{
		key_num=Button4_4_Scan();	
		if(key_num==16){
			OLED_Clear();//����
			return ;
		}
		switch (processnum)
		{
			case 0:
				//OLED_Clear();//����
				i++;
				Show_Str(0,0,128,16,"=== ¼��ָ�� ===",16,0);
				Show_Str(0,24,128,12,"�밴ָ�ƣ�  ",12,0);	
				Show_Str(104,52,128,12,"����",12,0);			
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						Show_Str(0,24,128,12,"ָ��������    ",12,0);	
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);
				//OLED_Clear();//����
				break;
			
			case 1:
				i++;
				Show_Str(0,24,128,12,"���ٰ�һ��ָ��",12,0);		
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						Show_Str(0,24,128,12,"ָ��������",12,0);	
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				//OLED_Clear();//����
				break;

			case 2:		
				Show_Str(0,24,128,12,"�Ա�����ָ��        ",12,0);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					Show_Str(0,24,128,12,"����ָ��һ��       ",12,0);
					processnum=3;//�������Ĳ�
				}
				else 
				{
					Show_Str(0,24,128,12,"�Ա�ʧ�� ����¼    ",12,0);	
					ShowErrMessage(ensure);
					i=0;
					OLED_Clear();//����
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				//OLED_Clear();//����
				break;

			case 3:
			Show_Str(0,24,128,12,"����ָ��ģ��...   ",12,0);	
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
//					
					Show_Str(0,24,128,12,"����ָ��ģ��ɹ�!",12,0);
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				//OLED_Clear();//����
			Show_Str(0,24,128,12,"�����봢��ID:      ",12,0);
			Show_Str(122,52,128,12," ",12,0);
			Show_Str(0,52,128,12,"ɾ�� ���        ȷ��",12,0);
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//����ID����С��ģ������������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
          OLED_Clear();//����
					Show_Str(0,30,128,16,"¼ָ�Ƴɹ�!",16,0);	
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					Show_Str(66,52,128,12,"ʣ��",12,0);
					OLED_ShowNum(90,52,AS608Para.PS_max-ValidN,3,12);
					delay_ms(1500);
					OLED_Clear();	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}
				OLED_Clear();//����					
				break;				
		}
		delay_ms(400);
		if(i==10)//����5��û�а���ָ���˳�
		{
			OLED_Clear();
			break;
		}				
	}
}

//ˢָ��
int press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	OLED_Clear();
		Show_Str(0,0,128,16,"���ڼ��ָ��",16,0);
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			
			if(ensure==0x00)//�����ɹ�
			{				
				OLED_Clear();
				Show_Str(20,10,128,24,"������...",24,0);	
				Walkmotor_ON();
				Show_Str(20,10,128,24,"�ѽ�����",24,0);
				OLED_Show_Font(112,18,1);//����				
				str=mymalloc(SRAMIN,2000);
//				sprintf(str,"ID:%d      ƥ���:%d",seach.pageID,seach.mathscore);
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
	 BEEP=0;//�رշ�����	
	 delay_ms(2000);
		OLED_Clear();
		
	}
	return -1;	
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	OLED_Clear();
	Show_Str(0,0,128,16,"=== ɾ��ָ�� ===",16,0);	
	Show_Str(0,16,128,12,"����ָ��ID��",12,0);
	Show_Str(0,52,128,12,"���� ���    ȷ��ɾ��",12,0);
	delay_ms(50);
//	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
		goto MENU ; //������ҳ��
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		OLED_Clear();
		Show_Str(0,20,128,12,"ɾ��ָ�Ƴɹ���",12,0);		
		Show_Str(80,48,128,12,"ʣ��",12,0);		
	}
  else
		ShowErrMessage(ensure);	
	
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	OLED_ShowNum(110,48,AS608Para.PS_max-ValidN,3,12);
	delay_ms(1200);
	
MENU:	
	OLED_Clear();
}
//�޸�����
void SetPassworld(void)
{
	int pwd_ch=0;
	int  key_num=0,i=0,satus=0;
	u16 time4=0;
	u8 pwd[6]="      ";
	u8 hidepwd[6]="      ";
	u8 buf[10];
	OLED_Clear();//����
	Show_Str(10,16,128,12," 1   2   3   4  ",12,0);
	Show_Str(10,28,128,12," 5   6   7   8  ",12,0);
	Show_Str(10,40,128,12," 9   0  chg Dis ",12,0);
	Show_Str(10,52,128,12,"Del Clr Bck Ok  ",12,0);
	while(1)
	{
		sprintf((char*)buf,"������%d:",pwd_ch+1);
		Show_Str(5,0,128,16,buf,16,0);
		key_num=Button4_4_Scan();	
		if(key_num)
		{	
			time4=0;
			if(key_num>=1 && key_num<10 && i>=0 && i<6){  //��1-9����
				pwd[i]=key_num+0x30;
				hidepwd[i]='*';
				i++;
			}	
			if(key_num==10 && i>=0 && i<10){       //��0����
				pwd[i]=0x30;
				hidepwd[i]='*';
				i++;
			}
			if(key_num==11){//��ʾ  
				pwd_ch=!pwd_ch;
			}
			if(key_num==12){//��ʾ  
				satus=!satus;
			}
			if(key_num==13 && i>0){
				pwd[--i]=' ';  //��del����
				hidepwd[i]=' '; 
			}
			if(key_num==14){
				while(i--){
					pwd[i]=' ';  //����ա���
					hidepwd[i]=' ';  //����ա���
				}
				i=0;
			}
			if(key_num==15){//����
				OLED_Clear();//����
				delay_ms(500);
				return ;
			}
			if(key_num==16)break;  //��Enter����
		}
		if(satus==0)OLED_ShowString(70,0,hidepwd,12);
		else OLED_ShowString(70,0,pwd,12);
		
		time4++;
		if(time4%1000==0){
			OLED_Clear();//����
			return ;
		}
	}	
	if(pwd_ch==0)
	{
		STMFLASH_Write(0X08020004,(u32*)pwd,2);
		STMFLASH_Read(0X08020004,(u32*)Pwd,2);	//��ȡ����1
		printf("pwd=%s",Pwd);
	}
	else
	{		
		STMFLASH_Write(0X08090004,(u32*)pwd,2);
		STMFLASH_Read(0X08090004,(u32*)Pwd2,2); //��ȡ����2
		printf("pwd2=%s",Pwd2);
	}
	OLED_Clear();//����
	Show_Str(0,48,128,16,"�����޸ĳɹ� ��",16,0);
	delay_ms(1000);
}
//����ʱ��
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
	Show_Str(0,52,128,12,"��  ��    �л�  ȷ��",12,0);
	
	while(1)
	{
		time5++;
		key_num=Button4_4_Scan();	
			if(key_num==12 | time5==3000){
				OLED_Clear();//����
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
			switch(st)			//��˸
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
	RTC_Set_Time(hour,min,sec,RTC_H12_AM);	//����ʱ��
	RTC_Set_Date(year,mon,dat,wek);		//��������
	OLED_Clear();
	Show_Str(20,48,128,16,"���óɹ���",16,0);
	delay_ms(1000);
}

//¼���¿�
u8 Add_Rfid(void)
{
	u16 time6=0;
	u8 i,key_num,status=1,card_size;
	OLED_Clear();
	Show_Str(0,0,128,16,"=== ¼�뿨Ƭ ===",16,0);	
	Show_Str(0,20,128,12,"������¿�Ƭ��",12,0);	
	Show_Str(0,52,128,12,"����",12,0);
	MFRC522_Initializtion();			//��ʼ��MFRC522
	while(1)
	{
		AntennaOn();
		status=MFRC522_Request(0x52, card_pydebuf);			//Ѱ��
		if(status==0)		//���������
		{
			printf("rc522 ok\n");
			Show_Str(0,38,128,12,"�����ɹ���",12,0);
			status=MFRC522_Anticoll(card_numberbuf);			//��ײ����			
			card_size=MFRC522_SelectTag(card_numberbuf);	//ѡ��
			status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//�鿨
			status=MFRC522_Write(4, card_writebuf);				//д����д��ҪС�ģ��ر��Ǹ����Ŀ�3��
			status=MFRC522_Read(4, card_readbuf);					//����
			printf("�������ͣ�%#x %#x",card_pydebuf[0],card_pydebuf[1]);
			//�����к��ԣ����һ�ֽ�Ϊ����У����
			printf("�������кţ�");
			for(i=0;i<5;i++)
			{
				printf("%#x ",card_numberbuf[i]);
			}
			printf("\n");
			//��������ʾ����λΪKbits
			printf("����������%dKbits\n",card_size);
			AntennaOff();
			Show_Str(0,38,128,12,"����¼��.",12,0);
			STMFLASH_Write(0X080f0004,(u32*)card_numberbuf,2);
			STMFLASH_Read(0X080f0004,(u32*)cardid,1); //��ȡ����1
			printf("cardid={%X,%X,%X,%X}\n",cardid[0],cardid[1],cardid[2],cardid[3]);
			Show_Str(0,38,128,12,"¼��ɹ���",12,0);
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
//rfid����
u8 MFRC522_lock(void)
{
	u8 i,j,status=1,card_size;
	u8 count;
	AntennaOn();
  status=MFRC522_Request(0x52, card_pydebuf);			//Ѱ��
	if(status==0)		//���������
	{
		printf("rc522 ok\n");
		status=MFRC522_Anticoll(card_numberbuf);			//��ײ����			
		card_size=MFRC522_SelectTag(card_numberbuf);	//ѡ��
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//�鿨
		status=MFRC522_Write(4, card_writebuf);				//д����д��ҪС�ģ��ر��Ǹ����Ŀ�3��
		status=MFRC522_Read(4, card_readbuf);					//����
		//MFRC522_Halt();															//ʹ����������״̬
		//��������ʾ
		
		printf("�������ͣ�%#x %#x",card_pydebuf[0],card_pydebuf[1]);
		

		//�����к��ԣ����һ�ֽ�Ϊ����У����
		printf("�������кţ�");
		count=0;
		for(i=0;i<5;i++)
		{
			printf("%#x ",card_numberbuf[i]);
			if(card_numberbuf[i]==cardid[i])count++;
		}
		if(count==4)
		{
			printf("rfid ƥ��ɹ�������\n");
			printf("AT+NMGS=3,000002\r\n\r\n");
			OLED_Clear();
			Show_Str(20,10,128,24,"������...",24,0);	
			Walkmotor_ON();
			Show_Str(20,10,128,24,"�ѽ�����",24,0);
			return 0;
		}
		
		printf("\n");
		//��������ʾ����λΪKbits
		
		printf("����������%dKbits\n",card_size);
		
		
		
		//��һ�����������ʾ
		printf("�����ݣ�\n");
		for(i=0;i<2;i++)		//��������ʾ
		{
			for(j=0;j<9;j++)	//ÿ����ʾ8��
			{
				printf("%#x ",card_readbuf[j+i*9]);
			}
			printf("\n");
		}

	}	
	
	AntennaOff();
	return 1;
}
//��ʾ��Ϣ
void Massige(void)
{
	OLED_Clear();
	Show_Str(0,0,128,12,"��������ϵͳ",12,0); 
	delay_ms(1000);
}
//������Ϣ
void starting(void)
{
	u8 ensure;
	char *str;			  
	u8 key;
	delay_ms(3000);
	OLED_Clear();
	while(font_init()) 			//����ֿ�
	{
			OLED_Clear();	   	   	  
			OLED_ShowString(0,0,"Explorer STM32F4",6);
			OLED_Refresh_Gram();//����OLED	
			while(SD_Init())			//���SD��
			{
				OLED_ShowString(0,12,"SD Card Failed!",6);
				delay_ms(200);
				OLED_ShowString(0,12,"               ",6);
				delay_ms(200);	
				OLED_Refresh_Gram();//����OLED	
			}								 						    
				OLED_ShowString(0,24,"SD Card OK",6);
				OLED_ShowString(0,36,"Font Updating...",6);
				key=update_font(20,110,16,"0:");//�����ֿ�
			while(key)//����ʧ��		
			{			 		  
				//LCD_ShowString(30,110,200,16,16,"Font Update Failed!");
				OLED_ShowString(0,48,"Font Update Failed!",6);
				delay_ms(200);
				OLED_ShowString(0,48,"                   ",6);
				delay_ms(200);		       
			} 		    
	} 
/*********************************������Ϣ��ʾ***********************************/
	OLED_Clear();
	Show_Str(0,0,128,12,"fingerprint system!",12,0); 
	Show_Str(0,12,128,12,"connect to as608",12,0);
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
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
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	
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






