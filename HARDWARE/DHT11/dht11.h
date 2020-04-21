#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   


//IO��������
#define DHT11_IO_IN()  {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=0<<3*2;}	//PC3����ģʽ
#define DHT11_IO_OUT() {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=1<<3*2;} 	//PC3���ģʽ 
////IO��������											   
#define	DHT11_DQ_OUT PCout(3) //���ݶ˿�	PG9 
#define	DHT11_DQ_IN  PCin(3)  //���ݶ˿�	PG9 

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















