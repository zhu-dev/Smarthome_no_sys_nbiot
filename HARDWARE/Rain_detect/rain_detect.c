#include "rain_detect.h"



void Rain_Detect_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//对应引脚PE2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOE
	
	
	Rain_EXIT_Init();
	
}




