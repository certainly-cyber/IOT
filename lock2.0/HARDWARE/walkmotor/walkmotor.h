#ifndef _WALKMOTOR_H
#define _WALKMOTOR_H
#include "sys.h"
/********************************
A  	B  	C  		 D
D3 	D5 	D7		 XCLK
PC9 PB6 PE6 	 PA8
********************************/
#define  Aout  PDout(9) 
#define  Bout  PDout(11)
#define  Cout PDout(13)
#define  Dout  PDout(15)

void Walkmotor_Init(void);
void Walkmotor_ON(void);
void Walkmotor_OFF(void);
	
#endif
