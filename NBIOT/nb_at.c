/*
 * **************************************
 * Copyright (c) 2018-2019, Ұţ������.
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
                /*ֻ��\r\n */
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
            if ( (uartxRxstate & UARTX_RECV_END) == 0 )  //δ�������
            {
                if ( uartxRxstate == 0x00 )  
                {
                    memset( (uint8_t *) uartxRxstate, 0x00, sizeof(uartxRxstate) );
                }
                if ( uartxRxstate < sizeof(uartxRxBuf) )
                {
                    TIM7->CNT = 0;                          /*��������� */
                  	TIM_Cmd(TIM7, ENABLE);  
                    uartxRxBuf[uartxRxstate++] = res;       /*��¼���յ���ֵ */
			
                }
                else
                {
                    uartxRxstate |= UARTX_RECV_END;         /*ǿ�Ʊ�ǽ������ */
                }
            }

        }
    
   
}

void TIM7_IRQHandler( void )
{
    if ( (uartxRxstate & 0x7FFF) != 0 )
    {
        /*10ms��ʱ���ж� */
        uartxRxstate |= UARTX_RECV_END;
        //DEBUG_switch( DEBUG_INDEX_1, DEBUG_STATE_TOGGLE, 4 );
    }
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
}


/*10ms��ʱ�� */
static void TIM7_init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}



void USART2_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

 	USART_DeInit(USART2);  //��λ����2
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��GPIOA2����GPIOA3
	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������3
 
	USART_Cmd(USART2, ENABLE);               //ʹ�ܴ��� 
	
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}


void UARTx_write( uint8_t *buf, uint16_t length )
{
    uint16_t i;
    for ( i = 0; i < length; i++ )  /*ѭ���������� */
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
    {                                               /*���յ�һ�������� */
        uartxRxBuf[uartxRxstate & 0x7FFF]	= 0;    /*��ӽ����� */
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
            ;                                       /*�ȴ���һ�����ݷ������ */
        USART2->DR = (uint32_t) cmd;
    }
    else
    {
        sendLen = sprintf( (char *) uartxTxBuf, "%s", cmd );
        UARTx_write( uartxTxBuf, sendLen );         /*�������� */
    }
    if ( ack && waittime )                          /*��Ҫ�ȴ�Ӧ�� */
    {
        while ( --waittime )                        /*�ȴ�����ʱ */
        {
           
            delay_ms( 10 );
            if ( uartxRxstate & UARTX_RECV_END )    /*���յ��ڴ���Ӧ���� */
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
                    break;                          /*�õ���Ч���� */
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
    if ( ack && waittime )                              /*��Ҫ�ȴ�Ӧ�� */
    {
        /*�ȴ�����ʱ */
        do
        {
            delay_ms( 10 );
            if ( uartxRxstate & UARTX_RECV_END )        /*���յ��ڴ���Ӧ���� */
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
                    break;                              /*�õ���Ч���� */
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
	
	if(uartxRxstate&UARTX_RECV_END)		//���յ�һ��������
	{
			if(strstr((const char*)uartxRxBuf,"+NNMI"))//��ʾ�������·�
				{
						//�������ʹ�ûص�����
					
						memset(uartxRxBuf,0x00,sizeof(uartxRxBuf));//������ջ�������
				}
	}
			
		return 0;

}



