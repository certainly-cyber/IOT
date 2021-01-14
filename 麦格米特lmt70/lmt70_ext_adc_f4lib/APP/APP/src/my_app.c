/**
  ******************************************************************************
  * @file           : my_app.c
  * @brief          : ����Ӧ�ó���Դ�ļ�
  * @details        : 
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
#include "my_app.h"

#include "lmt70.h"
#include "mcp3421.h"

#include "port_debug.h"
#include "port_delay.h"
/** @addtogroup My_App
  * @{
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t TAG[] = "app" ;

float port_ad ;
float tep ;

float voltage1 ;
mcp3421_s mcp ;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Ӧ�����ʼ�� */
void my_app_init()
{          
    mcp3421_init(&mcp) ;  /* ��ʼ��MCP3421 */ 
    lmt70_init() ;          /* ��ʼ��lmt70 */
    
}

/* Ӧ�ó��� */
float my_app()
{        
		voltage1 = mcp3421_one_conversion(&mcp) ;	
        tep = lmt70_get_temp( voltage1) ;  
  //  delay_ms(1000);    can not use
		return tep;
}

/**
  * @}
  */



