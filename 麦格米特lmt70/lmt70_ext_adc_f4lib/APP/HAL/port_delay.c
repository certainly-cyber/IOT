/**
  ******************************************************************************
  * @file           : port_delay.h
  * @brief          : �ײ������ʱͷ�ļ�
  * @details        : �ṩ�����΢���������ʱ�ӿ�
  * @author         : Charmander ��Ϊ���ӿƼ� QQ: 228303760
  * @date           : 2020/9/2
  * @version        : V0.1
  * @copyright      : Copyright (C) 
  ******************************************************************************
  * @attention      
  * NONE
  ******************************************************************************
  * @verbatim
  * �޸���ʷ:    
  *      1. ���ڣ�2020/9/2
  *         ���ߣ�Charmander
  *         �޸ģ�
  * @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "port_delay.h"

/** @addtogroup My_App
  * @{
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t  fac_us=0;							/* us��ʱ������ */			   
static uint16_t fac_ms=0;							/* ms��ʱ������,��ucos��,����ÿ�����ĵ�ms�� */
static uint8_t delay_init_flag = 0 ;                /* ��ʱ������ʼ����־λ */

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  ��ʼ���ӳٺ���
  * @details SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
  * @retval None
  */
static void port_delay_init(uint8_t sysclk)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) ;	/* ѡ���ⲿʱ��  HCLK/8 */
	fac_us = sysclk / 8 ;				                /* Ϊϵͳʱ�ӵ�1/8 */

	fac_ms = (uint16_t)fac_us * 1000 ;					    /* ��OS��,����ÿ��ms��Ҫ��systickʱ���� */   
}	

/** @brief  ΢����ʱ
  * @details 
  * @retval None
  */
void port_delay_us( uint32_t us) 
{
	uint32_t temp ;
    
    if ( delay_init_flag == 0)
    {
        port_delay_init(168) ;
        delay_init_flag = 1 ;
    }
    
	SysTick->LOAD = us * fac_us ; 					/* ʱ����� */	  		 
	SysTick->VAL = 0x00 ;        					/* ��ռ����� */   
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	    /* ��ʼ���� */
    
	do                                              /* �ȴ�ʱ�䵽�� */ 
	{
		temp = SysTick->CTRL ;
	}while( ( temp & 0x01) && ( !( temp & ( 1 << 16)))) ;		
    
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk ;	    /* �رռ����� */
	SysTick->VAL = 0X00 ;      					    /* ��ռ����� */
}    

/** @brief  ������ʱ
  * @details ��ʱnms,ע��nms�ķ�Χ,��72M������,nms<=1864
  * @retval None
  */
void port_delay_ms( uint32_t ms)
{
 		  	  
	uint32_t temp ;	
    
    if ( delay_init_flag == 0)
    {
        port_delay_init(168) ;
        delay_init_flag = 1 ;
    }
    
	SysTick->LOAD = (uint32_t)( ms * fac_ms) ; /* ʱ�����(SysTick->LOADΪ24bit) */
	SysTick->VAL = 0x00 ;					    /* ��ռ����� */
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	/* ��ʼ���� */
    
	do                                          /* �ȴ�ʱ�䵽�� */ 
	{
		temp=SysTick->CTRL;
	}while( ( temp & 0x01) && ( !( temp & ( 1 << 16)))) ;		  
    
	SysTick->CTRL &= ( ~SysTick_CTRL_ENABLE_Msk) ;	/* �رռ����� */
	SysTick->VAL = 0X00 ;       					/* ��ռ����� */	  	    
}


/**
  * @}
  */


