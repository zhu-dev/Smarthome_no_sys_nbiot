#include "rain_detect.h"



void Rain_Detect_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//��Ӧ����PE2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOE
	
	
	Rain_EXIT_Init();
	
}




