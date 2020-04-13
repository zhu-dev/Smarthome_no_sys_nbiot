/*
 * **************************************
 * Copyright (c) 2018-2019, 野牛物联网.
 *      All rights reserved.
 * **************************************
 */
#include "nb_at.h"
#include "stm32f4xx.h" 
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "main.h"
#include "led.h"

static uint16_t				uartxRxstate = 0;
static uint8_t				uartxRxBuf[128];
static uint8_t				uartxTxBuf[128];
static uint8_t				aRxBuffer[128];


#define    UARTX_RECV_END (0x8000)

uint8_t NBBCxx_parser( ATRespLine_TypeDef *resp, char* inData )
{
    char	ch;
    char	last_ch;
		int i ;
    uint8_t lineNum = 0;
    uint8_t pos		= 0;
    for (i = 0; i < strlen( inData ); i++ )
    {
        ch = inData[i];
        resp->respBuf[lineNum][pos++]	= ch;

        /* is newline or URC data */
        if ( last_ch == '\r' && ch == '\n' )
        {
            if ( pos == 2 )
            {
                /*只有\r\n */
                pos = 0;
                continue;
            }
            resp->respBuf[lineNum][pos - 2] = '\0';
            lineNum++;
            pos = 0;
        }
        last_ch = ch;
    }
    resp->respBuf[lineNum][0] = '\0';
    return (0);
}



void USART2_IRQHandler( void )
{
		uint8_t		res;
		
        if ( USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
        {
            res = (uint8_t) (USART2->DR & (uint16_t) 0x00FF);
            if ( (uartxRxstate & UARTX_RECV_END) == 0 )  //未接收完成
            {
                if ( uartxRxstate == 0x00 )  
                {
                    memset( (uint8_t *) uartxRxstate, 0x00, sizeof(uartxRxstate) );
                }
                if ( uartxRxstate < sizeof(uartxRxBuf) )
                {
                    TIM7->CNT = 0;                          /*计数器清空 */
                  	TIM_Cmd(TIM7, ENABLE);  
                    uartxRxBuf[uartxRxstate++] = res;       /*记录接收到的值 */
			
                }
                else
                {
                    uartxRxstate |= UARTX_RECV_END;         /*强制标记接收完成 */
                }
            }

        }
    
   
}

void TIM7_IRQHandler( void )
{
    if ( (uartxRxstate & 0x7FFF) != 0 )
    {
        /*10ms定时器中断 */
        uartxRxstate |= UARTX_RECV_END;
        //DEBUG_switch( DEBUG_INDEX_1, DEBUG_STATE_TOGGLE, 4 );
    }
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
}


/*10ms定时器 */
static void TIM7_init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}



void USART2_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

 	USART_DeInit(USART2);  //复位串口2
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2和GPIOA3初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化GPIOA2，和GPIOA3
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART2, &USART_InitStructure); //初始化串口3
 
	USART_Cmd(USART2, ENABLE);               //使能串口 
	
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}


void UARTx_write( uint8_t *buf, uint16_t length )
{
    uint16_t i;
    for ( i = 0; i < length; i++ )  /*循环发送数据 */
    {
        while ( (USART2->SR & 0x40) == 0 )
            ;
        USART2->DR = buf[i];
    }
}


void NBAT_init(void)
{
    USART2_init(9600);
		TIM7_init(840-1,1000-1);
}


int NBAT_send( void *buf, uint32_t len, uint32_t xTimeOut )
{
    if ( len > 1024 )
    {
        printf( "NBAT_send len is error\r\n" );
        return (0);
    }
    UARTx_write( buf, len );
    return (len);
}


char* NBAT_checkCmd( char *str )
{
    char *strx = 0;
    if ( uartxRxstate & UARTX_RECV_END )
    {                                               /*接收到一次数据了 */
        uartxRxBuf[uartxRxstate & 0x7FFF]	= 0;    /*添加结束符 */
        strx								= (char *) strstr( (const char *) uartxRxBuf, (const char *) str );
    }
    return (strx);
}


uint8_t NBAT_sendCmd( char *cmd, char *ack, uint16_t waittime, uint8_t *outResponse )
{
    uint8_t		res		= 0;
    uint16_t	sendLen = 0;
    uartxRxstate = 0;
    if ( strlen( cmd ) > sizeof(uartxTxBuf) )
    {
        printf( "NBAT_sendCmd Len is %d\r\n", strlen( cmd ) );
        return (2);
    }
    if ( (uint32_t) cmd <= 0xFF )
    {
        while ( (USART2->SR & 0x40) == 0 )
            ;                                       /*等待上一次数据发送完成 */
        USART2->DR = (uint32_t) cmd;
    }
    else
    {
        sendLen = sprintf( (char *) uartxTxBuf, "%s", cmd );
        UARTx_write( uartxTxBuf, sendLen );         /*发送命令 */
    }
    if ( ack && waittime )                          /*需要等待应答 */
    {
        while ( --waittime )                        /*等待倒计时 */
        {
           
            delay_ms( 10 );
            if ( uartxRxstate & UARTX_RECV_END )    /*接收到期待的应答结果 */
            {
             
                if ( NBAT_checkUrc( (uint8_t *) ack ) == 0 )
                {
                    ;
                }
                else if ( NBAT_checkCmd( ack ) )
                {
                    memcpy( outResponse, uartxRxBuf, uartxRxstate & 0x7FFF );
                    outResponse[uartxRxstate & 0x7FFF]	= 0;
                    uartxRxstate						= 0;
                    break;                          /*得到有效数据 */
                }
                else if ( NBAT_checkCmd( "ERROR" ) )
                {
                    memcpy( outResponse, uartxRxBuf, uartxRxstate & 0x7FFF );
                    outResponse[uartxRxstate & 0x7FFF]	= 0;
                    uartxRxstate						= 0;
                    res									= 2;
                    break;
                }
            }
        }
        if ( waittime == 0 )
            res = 1;
    }
  
    return (res);
}


uint8_t NBAT_getUrc( at_client_t client )
{
    if ( (uartxRxstate & UARTX_RECV_END) == UARTX_RECV_END )
    {
				int i;
        /*printf( "debug urc:%s\r\n", uartxRxBuf ); */
        /*NBBCxx_parser( client->resp, uartxRxBuf ); */
        uartxRxstate			&= (~UARTX_RECV_END);
        client->cur_recv_len	= uartxRxstate;
        memcpy( client->recvBuf, uartxRxBuf, uartxRxstate );
        uartxRxstate = 0;
				
        for (i = 0; i < 5; i++ )
        {
            /*printf( "%s\r\n", client->resp[i] ); */
        }
        return (0);
    }

    return (1);
}


uint8_t NBAT_checkUrc( uint8_t *outResponse )
{
    if ( strstr( (char *) outResponse, "+NSOMI" ) )
    {
        printf( "respons: %s", outResponse );
    }
    return (1);
}


uint8_t NBAT_recvData( char *ack, uint16_t waittime, uint8_t *outResponse )
{
    uint8_t res = 0;
    if ( (uartxRxstate & UARTX_RECV_END) == 0 )
        return (2);
    if ( ack && waittime )                              /*需要等待应答 */
    {
        /*等待倒计时 */
        do
        {
            delay_ms( 10 );
            if ( uartxRxstate & UARTX_RECV_END )        /*接收到期待的应答结果 */
            {
                if ( NBAT_checkUrc( (uint8_t *) ack ) == 0 )
                {
                    ;
                }
                else if ( NBAT_checkCmd( ack ) )
                {
                    memcpy( outResponse, uartxRxBuf, uartxRxstate & 0x7FFF );
                    outResponse[uartxRxstate & 0x7FFF]	= 0;
                    uartxRxstate						= 0;
                    break;                              /*得到有效数据 */
                }
                else if ( NBAT_checkCmd( "ERROR" ) )
                {
                    ;
                }
            }
        }
        while ( --waittime );
        if ( waittime == 0 )
            res = 1;
    }
    return (res);
}


uint8_t NBAT_recv_down_cmd()
{
	uint8_t res = 0;
	char* ss;
	
	if(uartxRxstate&UARTX_RECV_END)		//接收到一次数据了
	{
			if(strstr((const char*)uartxRxBuf,"+NNMI"))//表示有命令下发
				{
						//这里可以使用回调函数
					
						memset(uartxRxBuf,0x00,sizeof(uartxRxBuf));//清除接收缓冲数组
				}
	}
			
		return 0;

}



