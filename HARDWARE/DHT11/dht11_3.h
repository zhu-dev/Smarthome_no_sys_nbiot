#ifndef __DHT11_3_H
#define __DHT11_3_H
#include "sys.h"

//IO方向设置
#define DHT11_3_IO_IN()  {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=0<<5*2;}	//PG9输入模式
#define DHT11_3_IO_OUT() {GPIOC->MODER&=~(3<<(5*2));GPIOC->MODER|=1<<5*2;} 	//PG9输出模式 
////IO操作函数											   
#define	DHT11_3_DQ_OUT PCout(5) //数据端口	PG9 
#define	DHT11_3_DQ_IN  PCin(5)  //数据端口	PG9 

u8 DHT11_3_Init(void);//初始化DHT11
u8 DHT11_3_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_3_Read_Byte(void);//读出一个字节
u8 DHT11_3_Read_Bit(void);//读出一个位
u8 DHT11_3_Check(void);//检测是否存在DHT11
void DHT11_3_Rst(void);//复位DHT11  


#endif


