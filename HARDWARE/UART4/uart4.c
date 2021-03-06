#include "uart4.h"
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	

//串口发送缓存区 	
__align(8) u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef UART4_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

u16 UART4_RX_STA=0;  


void UART4_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
 
		res =USART_ReceiveData(UART4);//(USART1->DR);	//读取接收到的数据
		
		if((UART4_RX_STA&0x8000)==0)//接收未完成
		{
			if(UART4_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)UART4_RX_STA=0;//接收错误,重新开始
				else UART4_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(res==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=res ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_MAX_RECV_LEN-1))UART4_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  	
 }											 
}	
#endif


//初始化IO 串口4
//bound:波特率	  
void uart4_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能USART3时钟

 	USART_DeInit(UART4);  //复位串口3
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOB10复用为USART3	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(UART4, &USART_InitStructure); //初始化串口3
 
	USART_Cmd(UART4, ENABLE);               //使能串口 
	
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}


//串口4printf 函数
//确保一次发送数据不超过USART4_MAX_SEND_LEN字节
void u4_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)UART4_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //等待上次传输完成 
		USART_SendData(UART4,(uint8_t)UART4_TX_BUF[j]); 	 //发送数据到串口6
	}
	
}

void sendData(u8 buf[],u8 len)
{
	u8 i,j;
	i = len;
		for(j=0;j<i;j++)//循环发送数据
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //等待上次传输完成 
		USART_SendData(UART4,buf[j]); 	 //发送数据到串口6
	}
}

void txtToVoiceInit(u8 level)
{
	u8 buf[5] = {0x7E,0x03,0x06,0x16,0xEF};
	buf[3] = level;
	printf("123");
	sendData(buf,5);
	delay_ms(50);
}

void playAudio(u8 index)
{
	u8 buf[6] = {0x7E,0x04,0x03,0x00,0x01,0xEF};
	buf[4] = index;
//	if(index == 10)  buf[4] = 0x0a;
//	if(index == 11)  buf[4] = 0x0b;
	sendData(buf,6);
	delay_ms(50);
}



