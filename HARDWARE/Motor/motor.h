#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"


void TIM14_PWM_Init(u32 arr,u32 psc);

void Motor_sw_Init(void);
void open_pwm(void);
void close_pwm(void);
void open_sw(void);
void close_sw(void);

#endif


