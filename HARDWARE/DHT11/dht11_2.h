#ifndef __DHT11_2_H
#define __DHT11_2_H
#include "sys.h"


//IO方向设置
#define DHT11_2_IO_IN()  {GPIOC->MODER&=~(3<<(4*2));GPIOC->MODER|=0<<4*2;}	//PG9输入模式
#define DHT11_2_IO_OUT() {GPIOC->MODER&=~(3<<(4*2));GPIOC->MODER|=1<<4*2;} 	//PG9输出模式 
////IO操作函数											   
#define	DHT11_2_DQ_OUT PCout(4) //数据端口	PG9 
#define	DHT11_2_DQ_IN  PCin(4)  //数据端口	PG9 

u8 DHT11_2_Init(void);//初始化DHT11
u8 DHT11_2_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_2_Read_Byte(void);//读出一个字节
u8 DHT11_2_Read_Bit(void);//读出一个位
u8 DHT11_2_Check(void);//检测是否存在DHT11
void DHT11_2_Rst(void);//复位DHT11  


#endif


