#ifndef __BEEP_H
#define __BEEP_H

#include <sys.h>
#include <stm32f4xx_rcc.h>

//定义开关
#define BEEPON 1
#define BEEPOFF 0
//BEEP端口定义
#define BEEP PFout(8)	// BEEP

void BEEP_Init(void);

#endif

