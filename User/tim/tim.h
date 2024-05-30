#ifndef __TIM_H
#define __TIM_H
#include "sys.h"


extern  unsigned int modbus_time;
void TIM3_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u16 arr,u16 psc);
void Servo_PWM_Init(u16 arr,u16 psc);
#endif

