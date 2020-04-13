#ifndef __MQ_SENSOR_H
#define __MQ_SENSOR_H

#include "sys.h"

#define RL			4.7		// RL��ֵ
#define R0   		30 // Ԫ���ڽྻ�����е���ֵ  35K

#define   PER      12        //��������
#define   NUM      10        //�ɼ�����

extern u16 results[NUM][PER];
extern u16 res[PER];

float MQ2_GetPPM(u8 mqx);
void MQ2_Init(void);

#endif






