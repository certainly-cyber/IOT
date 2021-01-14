#ifndef __DISTIMER_H
#define __DISTIMER_H
#include "sys.h" 	

#define Trig1 PBout(6)

void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void Trig_Init(void);

#endif

