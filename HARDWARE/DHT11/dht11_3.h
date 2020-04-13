#ifndef __DHT11_3_H
#define __DHT11_3_H
#include "sys.h"

//IO��������
#define DHT11_3_IO_IN()  {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=0<<5*2;}	//PG9����ģʽ
#define DHT11_3_IO_OUT() {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=1<<5*2;} 	//PG9���ģʽ 
////IO��������											   
#define	DHT11_3_DQ_OUT PCout(5) //���ݶ˿�	PG9 
#define	DHT11_3_DQ_IN  PCin(5)  //���ݶ˿�	PG9 

u8 DHT11_3_Init(void);//��ʼ��DHT11
u8 DHT11_3_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_3_Read_Byte(void);//����һ���ֽ�
u8 DHT11_3_Read_Bit(void);//����һ��λ
u8 DHT11_3_Check(void);//����Ƿ����DHT11
void DHT11_3_Rst(void);//��λDHT11  


#endif


