#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern u8 tim2count;
//extern u8 tim3count;

//void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
#endif
