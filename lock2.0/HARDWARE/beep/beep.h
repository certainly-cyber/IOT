#ifndef __BEEP_H
#define __BEEP_H

#include <sys.h>
#include <stm32f4xx_rcc.h>

//���忪��
#define BEEPON 1
#define BEEPOFF 0
//BEEP�˿ڶ���
#define BEEP PFout(8)	// BEEP

void BEEP_Init(void);

#endif

