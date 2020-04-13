#ifndef __MQ_SENSOR_H
#define __MQ_SENSOR_H

#include "sys.h"

#define RL			4.7		// RL阻值
#define R0   		30 // 元件在洁净空气中的阻值  35K

#define   PER      12        //外设数量
#define   NUM      10        //采集次数

extern u16 results[NUM][PER];
extern u16 res[PER];

float MQ2_GetPPM(u8 mqx);
void MQ2_Init(void);

#endif






