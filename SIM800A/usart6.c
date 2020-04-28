#include "usart6.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"


//���ڷ��ͻ����� 	
__align(8) u8 USART6_TX_BUF[USART6_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
#ifdef USART6_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART6_RX_BUF[USART6_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

u16 USART6_RX_STA=0;  

void USART6_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)//���յ�����
	{	 
 
	res =USART_ReceiveData(USART6);		
	if((USART6_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART6_RX_STA<USART6_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM_SetCounter(TIM5,0);//���������        				 
			if(USART6_RX_STA==0)		
				TIM_Cmd(TIM5, ENABLE);  //ʹ�ܶ�ʱ��7 
			USART6_RX_BUF[USART6_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART6_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
			
		} 
	}  	
 }										 
}	
#endif

void usart6_init(u32 bound)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	/*ʹ��USART6����ʱ��*/  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART6ʱ��
	
	
		//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOA10����ΪUSART1
	
	/*��λ����6*/
 	//USART_DeInit(USART6);  
	
	/*USART6_GPIO��ʼ������*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PA9��PA10
   
    
   /*USART6 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1

  USART_Cmd(USART6, ENABLE);                    			//ʹ�ܴ��� 
	
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�


   /*Usart6 NVIC����*/
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	TIM5_Int_Init(1000-1,8400-1);		//100ms�ж�
	USART6_RX_STA=0;		//����
	TIM_Cmd(TIM5, DISABLE); //�رն�ʱ��7

}


//����6printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u6_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART6_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
	  while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������ 
		USART_SendData(USART6,(uint8_t)USART6_TX_BUF[j]); 	 //�������ݵ�����6
	}
	
}
