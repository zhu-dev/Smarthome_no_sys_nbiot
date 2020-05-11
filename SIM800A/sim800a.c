#include "sim800a.h"
#include "stdio.h"


const char *msg_head = "0891683108701705F011000D91688100874376F40008AA10";
	 //char *msg_warning="0891683108701705F011000D91688100874376F40008AA14 005B8B66544A005D67094EBA8FDB5165623F95F4";//[����]���˽����˷���
																																			 //8B66544A5BA253856E295EA68FC79AD8
const char *msg_warning_1 = "8B66544A5BA253856E295EA68FC79AD8";
const char *msg_warning_2 = "8B66544A53675BA46E295EA68FC79AD8";
const char *msg_warning_3 = "8B66544A53A8623F6E295EA68FC79AD8 ";



char* sim800a_check_cmd(char *str)
{
	
	char *strx=0;
	if(USART6_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART6_RX_BUF[USART6_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("[ack]:%s\r\n",USART6_RX_BUF);
		strx=strstr((const char*)USART6_RX_BUF,(const char*)str);
	} 
	return strx;
}


u8 sim800a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART6_RX_STA=0;
	u6_printf("%s\r\n",cmd);	//��������
	printf("[cmd]%s\r\n",cmd);
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(100);
			if(USART6_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim800a_check_cmd(ack))
				{
					//printf("[ack]:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
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
	u6_printf("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART6_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim800a_check_cmd(ack))//�õ���Ч���� 
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
	while(sim800a_send_cmd("AT+CREG?","+CREG: 0,1",50)) 
	{
		printf("[ERROR] SIM800 CREG FAILD;..\r\n");
		delay_ms(1000);
	}
	
		while(sim800a_send_cmd("AT+CMGF=0","OK",10)) 
	{
		printf("[ERROR] SIM800 SET PDU MDOE FAILD..\r\n");
	}
	
	printf("[debug]sim800 init end\r\n");
	return res;
}


void sim800a_send_warning(u8 room)
{
	char end[20];
	char msg_warning[200];

	u8 res = 0;
	
	sim800a_send_cmd("AT+CMGS=31",">",6);
	switch(room)
	{
		case 1:
						sprintf(msg_warning,"%s%s",msg_head,msg_warning_1);
						sim800a_send_data(msg_warning,NULL,2);
			break;
		case 2:
						sprintf(msg_warning,"%s%s",msg_head,msg_warning_2);
						sim800a_send_data(msg_warning,NULL,2);			
			break;
		case 3:
						sprintf(msg_warning,"%s%s",msg_head,msg_warning_3);
						sim800a_send_data(msg_warning,NULL,2);			
			break;		
	}

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


