#include "sim800a.h"
#include "stdio.h"

char *msg_warning="0891683108701705F011000D91688100874376F40008AA14005B8B66544A005D67094EBA8FDB5165623F95F4";//[警告]有人进入了房间

//const char *commom_1 = "089168"; //包含通用的信息中心头信息
//const char *commom_2 = "11000D9168";//包含通用的接收方头信息
//const char *commom_3 = "0008AA";//包含数据包编码通用信息
//const char *message_center_number = "3108701705F0";  //每两位倒置，86已经放到通用头部，所以这里不添加
//const char *received_number = "8100874376F4";//每两位倒置，86已经放到通用头部，所以这里不添加

//char *data_lenght = ""; //数据信息长度，通用的接收方头信息+实际信息体
//char *data = ""; //实际信息体



char* sim800a_check_cmd(char *str)
{
	
	char *strx=0;
	if(USART6_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART6_RX_BUF[USART6_RX_STA&0X7FFF]=0;//添加结束符
		printf("[ack]:%s\r\n",USART6_RX_BUF);
		strx=strstr((const char*)USART6_RX_BUF,(const char*)str);
	} 
	return strx;
}


u8 sim800a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART6_RX_STA=0;
	u6_printf("%s\r\n",cmd);	//发送命令
	printf("[cmd]%s\r\n",cmd);
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(100);
			if(USART6_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim800a_check_cmd(ack))
				{
					//printf("[ack]:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART6_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


u8 sim800a_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART6_RX_STA=0;
	u6_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART6_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(sim800a_check_cmd(ack))//得到有效数据 
				{
					//printf("[ack]:%s\r\n",(u8*)ack);
					break;
				}
				
				USART6_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


u8 sim800a_pdu_init(void)
{
	u8 res = 0;  
	printf("[debug]sim800 init start\r\n");
	while(sim800a_send_cmd("AT+CREG?","+CREG: 0,1",70)) 
	{
		printf("[ERROR] SIM800 CREG FAILD;..\r\n");
	}
	
		while(sim800a_send_cmd("AT+CMGF=0","OK",20)) 
	{
		printf("[ERROR] SIM800 SET PDU MDOE FAILD..\r\n");
	}
	
	printf("[debug]sim800 init end\r\n");
	return res;
}


void sim800a_send_warning(void)
{
	char end[20];
	u8 res = 0;
	sim800a_send_cmd("AT+CMGS=35",">",10);
	sim800a_send_data(msg_warning,NULL,2);
	//printf("data:%s\r\n",msg_warning);
	sprintf(end,"%c",0x1A);
	//printf("data_end:%s\r\n",end);
	res = sim800a_send_cmd(end,"OK",50);
	//printf("res:%d\r\n",res);
	if(res == 0)
	{
		 printf("[info]send mesage success..\r\n");
	}
	else
	{
		 printf("[ERROR]send mesage faild..\r\n");
	}
}


