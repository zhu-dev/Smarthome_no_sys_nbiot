#ifndef __DHT11_2_H
#define __DHT11_2_H
#include "sys.h"


//IO��������
#define DHT11_2_IO_IN()  {GPIOC->MODER&=~(3<<(4*2));GPIOC->MODER|=0<<4*2;}	//PG9����ģʽ
#define DHT11_2_IO_OUT() {GPIOC->MODER&=~(3<<(4*2));GPIOC->MODER|=1<<4*2;} 	//PG9���ģʽ 
////IO��������											   
#define	DHT11_2_DQ_OUT PCout(4) //���ݶ˿�	PG9 
#define	DHT11_2_DQ_IN  PCin(4)  //���ݶ˿�	PG9 

u8 DHT11_2_Init(void);//��ʼ��DHT11
u8 DHT11_2_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_2_Read_Byte(void);//����һ���ֽ�
u8 DHT11_2_Read_Bit(void);//����һ��λ
u8 DHT11_2_Check(void);//����Ƿ����DHT11
void DHT11_2_Rst(void);//��λDHT11  


#endif


