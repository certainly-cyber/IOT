/**
  ******************************************************************************
  * @file           : my_app.c
  * @brief          : 顶层应用程序源文件
  * @details        : 
  * @author         : Charmander 有为电子科技 QQ: 228303760
  * @date           : 2020/9/2
  * @version        : V0.1
  * @copyright      : Copyright (C) 
  ******************************************************************************
  * @attention      
  * NONE
  ******************************************************************************
  * @verbatim
  * 修改历史:    
  *      1. 日期：2020/9/2
  *         作者：Charmander
  *         修改：
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

/* 应程序初始化 */
void my_app_init()
{          
    mcp3421_init(&mcp) ;  /* 初始化MCP3421 */ 
    lmt70_init() ;          /* 初始化lmt70 */
    
}

/* 应用程序 */
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



