#ifndef __LED_H
#define __LED_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//LED�˿ڶ���
//#define LED0 PDout(8)	// PD8 
#define LED0 PGout(13)	// DS0
#define LED1 PGout(14)	// DS1	 
//#define LED0 PFout(9)	// DS0
//#define LED1 PFout(10)	// DS1	 
void LED1_Init(void);//��ʼ��
void LED2_Init(void);
#endif
