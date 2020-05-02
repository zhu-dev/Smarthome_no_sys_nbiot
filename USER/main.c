#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"

#include "main.h"
#include "timer.h"
#include "mq_sensor.h"
#include "hc_sr501.h"
#include "dht11.h"
#include "motor.h"
#include "dht11_2.h"
#include "dht11_3.h"
#include "nb_interface.h"
#include "usart3.h"
#include "sim800a.h"
#include "uart4.h"
#include "light.h"

void check_NB_Status(void);
void check_DHT11_Status(void);
void handle_cloud_cmd(char *data);
void UART_LCD_Init(void);
void show_room_info(u8 room,u8 temperature,u8 humidity,uint16_t smoke);
void show_airconditin_info(char *temperatrue,char mode,char windspeed);




/*�յ����ݵĻص����� */
static void urc_recvdata_func( const char *data, uint16_t size )
{
    char outData[96];
    memset( outData, 0x00, sizeof(outData) ); //�����ʱ������
		memcpy( outData, data, sizeof(outData) ); //����ֱ�Ӳ���ȫ�ֱ���
   // printf( "[INFO]urc_recvdata_func\r\n" );		
    printf( "[INFO]recv data:%s", outData );
		handle_cloud_cmd(outData);//������յ�������
}


/*URC-usart received callback���� { "\r\nCONNECT",  "\r\n", urc_connect_func	},*/
static const struct at_urc urc_table[] = {
    { "\r\n+NNMI:", "\r\n", urc_recvdata_func },
};



int main(void)
{ 
	u8 count = 0;
 	char showbuf[64];
	//MQ sensor��ر���
	float ppm1,ppm2,ppm3;
	uint16_t ppm_1 = 0;
	uint16_t ppm_2 = 0;
	uint16_t ppm_3 = 0;
	
	//DHT11��ر���,����ֵ�����������������������ϱ������쳣������ƽ̨��Ӧ�÷���������
	u8 t=0;			    
	u8 temperature1 = 26;
	u8 temperature2 = 26;
	u8 temperature3 = 26;	    
	u8 humidity1 = 0;
	u8 humidity2 = 0;
	u8 humidity3 = 0; 
	

	//NBģ��������ر���
	uint8_t res		= 0;
	char *headstr = "00";
	char	tmpBuf1[128];
	char	tmpBuf2[128];
	char	tmpBuf3[128];
  struct at_client	clientObj;
  at_client_t			client = &clientObj;
  NBBCxx_setUrcTable( client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]) );
	
	
	//�����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ����
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	
	usart3_init(115200);
	uart4_init(9600);
	
	LED_Init();					//��ʼ�����ز���LED 
	
	light_init();
	fan_init();
	
	UART_LCD_Init(); //��ʼ��Һ������ʾ��ʼֵ
	
//	MQ2_Init();						//��ʼ��MQ2
//	HC_SR_Init();		      //�������

//	//DHT11���
//	check_DHT11_Status(); //���DHT11Ӳ���Ƿ���������
//	ADC_SoftwareStartConv(ADC1); //��ʼADCת��
//	
//	//NB-COAP���
//	NBBCxx_init();  //NB M5310Aģ��������ʼ��
//  check_NB_Status();  //��������ʼ����ɺ󣬷���ָ����ģ���Ƿ�������2sһ�����ݣ�������ȥ�ر�PSMģʽ
	
	//txtToVoiceInit(27);
	
	LED0 = 0;
	while(1)
	{ 
		printf("ok\r\n");
		//playAudio(3);
		delay_ms(5000);
		//NBBCxx_getUrc( client );//��ȡ�ͻ��˶��󣬳ɹ��ɻ�ȡһ�λص����첽��ȡ
		
//		if(t%100 == 0)
//		{
//			
//			DHT11_Read_Data(&temperature1,&humidity1);		//��ȡ��ʪ��ֵ	
//			DHT11_2_Read_Data(&temperature2,&humidity2);		//��ȡ��ʪ��ֵ			
//			DHT11_3_Read_Data(&temperature3,&humidity3);		//��ȡ��ʪ��ֵ
//			
//			printf("[DATA]temperature1:%d  humidity:%d\r\n",temperature1,humidity1);
//			printf("[DATA]temperature2:%d  humidity2:%d\r\n",temperature2,humidity2);
//			printf("[DATA]temperature3:%d  humidity3:%d\r\n",temperature3,humidity3);
//			
//			if(temperature1>99) temperature1 = 26; //�豸�ϵ�˲����һ������ֵ����ܴ��ó�ֵ����
//			if(temperature2>99) temperature2 = 26;
//			if(temperature3>99) temperature3 = 26;

//			if(humidity1>99) humidity1 = 50; //�豸�ϵ�˲����һ������ֵ����ܴ��ó�ֵ����
//			if(humidity2>99) humidity2 = 50;
//			if(humidity3>99) humidity3 = 50;
//			
//			ppm1 = MQ2_GetPPM(0);
//			ppm2 = MQ2_GetPPM(1);
//			ppm3 = MQ2_GetPPM(2);
//			
//			ppm_1 = ppm1;
//			ppm_2 = ppm2;
//			ppm_3 = ppm3;
//			
//			printf("[DATA]ppm1:%.2f \r\n",ppm1);
//			printf("[DATA]ppm2:%.2f \r\n",ppm2);
//			printf("[DATA]ppm3:%.2f \r\n",ppm3);
//			
//			if(ppm_1>9999) ppm_1 = 9999;  //����10000ppm ֱ�ӱ���Ũ,��ֵ�ﵽ��ֵ�󣬿��Լ��ٴ�����ֳ�����������
//			if(ppm_2>9999) ppm_2 = 9999;
//			if(ppm_3>9999) ppm_3 = 9999;
//			
//			
//			//��ʾ��Һ��
//			show_room_info(1,temperature1,humidity1,ppm_1);
//			show_room_info(2,temperature2,humidity2,ppm_2);
//			show_room_info(3,temperature3,humidity3,ppm_3);
//			
//			sprintf( tmpBuf1, "%2d%2d%3d%2d%2d%3d%2d%2d%3d",temperature1,humidity1,ppm_1,temperature2,humidity2,ppm_2,temperature3,humidity3,ppm_3);
//			printf( "\r\n[INFO]coap send primary data:%s\r\n", tmpBuf1);
//			/*���и�ʽת�� */
//			memset(tmpBuf2, 0x00, sizeof(tmpBuf2) );
//			hexToStr( tmpBuf1, tmpBuf2 );//ת��Ϊ16�����ַ���
//			sprintf(tmpBuf3,"%s%s",headstr,tmpBuf2);
//			NBBCxx_setNMGS_char( tmpBuf3 );//���͵�ģ��
//			printf("[INFO]coap send HEX data:%s\r\n",tmpBuf3);
//			
//		}
//	
//		delay_ms(20);
//		if(t>200) t = 0;
//		t++;


		
	}
}


void  check_DHT11_Status(void)
{
	uint8_t error = 0;
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		error = 1;
		printf(" DHT11_1 init error");
	}
		delay_ms(10);
	
		while(DHT11_2_Init())	//DHT11_2��ʼ��	
	{
		error = 1;
		printf(" DHT11_2 init error");
	}
	
	 delay_ms(10);
	
		while(DHT11_3_Init())	//DHT11_3��ʼ��	
	{
		error = 1;
		printf(" DHT11_3 init error");
	}
	
		 if(error)
	 {
		   //��������
		  //�������ϵ�
	 }
	 else
	 {
		 printf("DHT11 INIT SUCESS!!!\r\n");
	 }
}



void check_NB_Status(void)
{
	 uint8_t error = 0;
	
	 while(NBBCxx_checkModel()!=0)   //���ģ���Ƿ���������
	 {
		  error = 1;
		  printf("[NB_ERROR]CHECK MODEL Fail\r\n");
	 }
	 delay_ms(400);
	 
	 while(NBBCxx_getCSQ(NULL)!=0)  //�鿴ģ���ź�ǿ��
	 {
		  error = 1;
		  printf("[NB_ERROR]MODEL GETCSQ Fail\r\n" );
	 }
	 delay_ms(400);
	 
	 while(NBBCxx_getCEREG()!=0)  //�鿴ģ���Ƿ�ע������ɹ�
	 {
		  error = 1;
		 printf( "[NB_ERROR]MODEL GETCEREG Fail\r\n" );
	 }
	 delay_ms(400);

	 while(NBBCxx_setNCDP( NBIOT_COAP_SERVICE_IP )!=0)  //����Coap��Ip�Ͷ˿ڣ��˿�Ĭ��5683��
	 {
		  error = 1;
		 printf( "[NB_ERROR]MODEL SETNCDP Fail\r\n" );
	 } 
	 delay_ms(400);
	 
	 if(error)
	 {
		   //��������ģ��
		  //�������ϵ�
	 }
	 else
	 {
		 printf("NB MODEL INIT SUCESS!!!\r\n");
	 }
		
}


void handle_cloud_cmd(char *data)
{

	char messageId; //����ͷ��(messageId)

	char *p = NULL; //ƥ���ַ�λ��ָ��
	
	char *cmd;//ƽ̨�·���������(messageId+content)
	
	char *cmd_body; //������ԭʼ16�����ַ���
	
	char body[50];//������������������Ϊascii�ַ���
	
	char *temperatrueSet = "26";
	
	uint32_t outLen = 0;//ת������ַ����ȣ�������ԭ����һ��
	
	uint8_t res; //�����
	
	//+NNMI:2,0343\r\n
	
	p = (char *)strstr((const char *)data,(const char *)",");
	
	cmd = p+1;
	
	cmd_body = p+3;
	
	messageId = cmd[1];

	//cmd_body[strlen(cmd_body)-4] = '\0';//���ԵĴ���\r\n��β������һ��\����Ϊ'\0'��ȥ���س�����,��������ƽ̨��������ȴû�У�����

	memset(body,0x00,sizeof(body));//��ת��ǰ����ʱ���������
	res = strToHex(cmd_body,body,&outLen);//��16���ַ���ת��Ϊascii�ַ���ע��ͷ������ת����������ţ����޷���ʾ
	
	//printf("bodyout:%s\r\n",body);
	
	
	//�������� 
	switch(messageId)
	{
		case 0x31:
			//�����ԭ��������ѷ�����
			break;
		case 0x32:
			//����ǿյ�����
			printf("���ÿյ�...\r\n");
			if(body[0] == 'O')
			{
				switch(body[1])
				{
					case 'A':
						printf("[aircondition]->ģʽ:�Զ� L\r\n");
						break;
					case 'L':
						printf("[aircondition]->ģʽ: ����\r\n");
						break;
					case 'R':
						printf("[aircondition]->ģʽ: ����\r\n");
						break;
					case 'F':
						printf("[aircondition]->ģʽ: �ͷ�\r\n");
						break;
				}
				
				switch(body[4])
				{
					case 'A':
						printf("[aircondition]->����: �Զ�\r\n");
						break;
					case 'L':
						printf("[aircondition]->����: ����\r\n");
						break;
					case 'M':
						printf("[aircondition]->����: ����\r\n");
						break;
					case 'H':
						printf("[aircondition]->����: ����\r\n");
						break;
				}
				
				sprintf(temperatrueSet,"%c%c",body[2],body[3]);
				printf("[aircondition]->�¶�: %s��\r\n",temperatrueSet);
				//��ʾ��Һ��
				show_airconditin_info(temperatrueSet,body[1],body[4]);
				
			}
			break;
		case 0x33:
			printf("���ƴ���...\r\n");
			//����Ǵ�������
			if(strstr(body,"O"))
			{
				printf("�򿪴���\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�رմ���\r\n");
			}
			break;
		case 0x34:
			//����Ǽ�ͥģʽ����
			printf("���ü�ͥģʽ%s: \r\n",body);
			if(strstr(body,"O"))
			{
				printf("�����е�\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�ر����е�\r\n");
			}
		
			break;
		case 0x35:
			//����Ƿ�������
			printf("���Ʒ��\r\n");
			if(strstr(body,"O"))
			{
				printf("�򿪷��\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�رշ��\r\n");
			}
			break;
		case 0x36:
			//����ǿ���������
			printf("���ƿ�����\r\n");
			if(strstr(body,"O"))
			{
				printf("�򿪿�����\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�رտ�����\r\n");
			}
			break;
		case 0x37:
			//��������ҵ�����
			printf("�������ҵ�\r\n");
			if(strstr(body,"O"))
			{
				printf("�����ҵ�\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�ر����ҵ�\r\n");
			}
			break;
		case 0x38:
			//����ǳ���������
			printf("���Ƴ�����\r\n");
			if(strstr(body,"O"))
			{
				printf("�򿪳�����\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("�رճ�����\r\n");
			}
			break;
		case 0x39:
			//�����ԡ�ҵ�����
			printf("����ԡ�ҵ�\r\n");
			break;
		case 0x61:
			//�������չ����
			printf("������չ����\r\n");
			if(strstr(body,"O"))
			{
				printf("��չ��\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("��չ��\r\n");
			}
			break;
	}
}


void UART_LCD_Init(void)
{
	delay_ms(200);//�ȴ�Һ����ʼ��    
	// \xBF\xAA->��
	// \xB9\xD8 ->��
	
GpuSend("CLS(0);"); //CLS(0);
GpuSend("W8DF(4,2,'12334566');"); //W8DF(4,2,'12334566');
GpuSend("W8MU(10,10,90,90,5,0);"); //W8MU(10,10,90,90,5,0);
GpuSend("W8UE(1);DS16(2,2,'\xBF\xCD\xCC\xFC',7);LABL(24,2,25,88,'26\xA1\xE6',41,1);LABL(16,2,50,88,'\xCA\xAA\xB6\xC8: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;");
	//W8UE(1);DS16(2,2,'����',41);LABL(24,2,25,88,'26��',41,1);LABL(16,2,50,88,'ʪ��: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;
	
GpuSend("W8UE(2);DS16(2,2,'\xCE\xD4\xCA\xD2',7);LABL(24,2,25,88,'26\xA1\xE6',41,1);LABL(16,2,50,88,'\xCA\xAA\xB6\xC8: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;"); 
	//W8UE(2);DS16(2,2,'����',41);LABL(24,2,25,88,'26��',41,1);LABL(16,2,50,88,'ʪ��: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;
	
GpuSend("W8UE(4);DS16(2,2,'\xB3\xF8\xB7\xBF',7);LABL(24,2,25,88,'26\xA1\xE6',41,1);LABL(16,2,50,88,'\xCA\xAA\xB6\xC8: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;");
	//W8UE(4);DS16(2,2,'����',41);LABL(24,2,25,88,'26��',41,1);LABL(16,2,50,88,'ʪ��: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;
	
GpuSend("W8UE(5);DS16(2,2,'\xD4\xA4\xC1\xF4',41);LABL(24,2,25,88,'26\xA1\xE6',41,1);LABL(16,2,50,88,'\xCA\xAA\xB6\xC8: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;"); 
	//W8UE(5);DS16(2,2,'Ԥ��',41);LABL(24,2,25,88,'26��',41,1);LABL(16,2,50,88,'ʪ��: 99%',41,1);LABL(16,2,68,88,'smoke:9999',41,1);SXY(0,0);;
	
GpuSend("W8UE(3);PIC(2,2,14);DS16(30,2,'\xBF\xD5\xB5\xF7\xC9\xE8\xD6\xC3',15);LABL(16,2,32,88,'\xC9\xE8\xB6\xA8: 25\xA1\xE6',41,1);LABL(16,2,50,88,'\xC4\xA3\xCA\xBD: \xD7\xD4\xB6\xAF',41,1);LABL(16,2,68,88,'\xB7\xE7\xCB\xD9: \xD7\xD4\xB6\xAF',41,1);SXY(0,0);;"); 
	//W8UE(3);PIC(2,2,14);DS16(30,2,'�յ�����',15);LABL(16,2,32,88,'�趨: 25��',41,1);LABL(16,2,50,88,'ģʽ: �Զ�',41,1);LABL(16,2,68,88,'����: �Զ�',41,1);SXY(0,0);;
	
GpuSend("W8UE(6);PIC(2,2,20);DS16(30,2,'\xB5\xC6\xB9\xE2\xD7\xB4\xCC\xAC',15);LABL(16,2,32,88,'\xBF\xCD\xCC\xFC: \xB9\xD8',41,1);LABL(16,2,50,88,'\xB3\xF8\xB7\xBF: \xB9\xD8',41,1);LABL(16,2,68,88,'\xB3\xF8\xB7\xBF: \xB9\xD8',41,1);SXY(0,0);;"); 
	//W8UE(6);PIC(2,2,20);DS16(30,2,'�ƹ�״̬',15);LABL(16,2,32,88,'����: ��',41,1);LABL(16,2,50,88,'����: ��',41,1);LABL(16,2,68,88,'����: ��',41,1);SXY(0,0);;
	
GpuSend("W8SE(0);"); //W8SE(0);
GpuSend("SBC(0);"); //SBC(0);
GpuSend("\r\n");

		
	delay_ms(1000);//����������ȶ�
}



void show_room_info(u8 room,u8 temperature,u8 humidity,uint16_t smoke)
{
	char showbuf[128];
	
	switch(room)
	{
		case 1:
			//��ʾ��������
			//memset(showbuf,0x00,sizeof(showbuf));
			sprintf(showbuf,"W8UE(1);DS16(2,2,'����',7);LABL(24,2,25,88,'%2d��',41,1);LABL(16,2,50,88,'ʪ��: %2d%',41,1);LABL(16,2,68,88,'smoke:%4d',41,1);SXY(0,0);;\r\n",temperature,humidity,smoke);
			GpuSend(showbuf);
			break;
		case 2:
			//��ʾ��������
			//memset(showbuf,0x00,sizeof(showbuf));
			sprintf(showbuf,"W8UE(1);DS16(2,2,'����',7);LABL(24,2,25,88,'%2d��',41,1);LABL(16,2,50,88,'ʪ��: %2d%',41,1);LABL(16,2,68,88,'smoke:%4d',41,1);SXY(0,0);;\r\n",temperature,humidity,smoke);
			GpuSend(showbuf);
			break;
		case 3:
			//��ʾ��������
			//memset(showbuf,0x00,sizeof(showbuf));
			sprintf(showbuf,"W8UE(1);DS16(2,2,'����',7);LABL(24,2,25,88,'%2d��',41,1);LABL(16,2,50,88,'ʪ��: %2d%',41,1);LABL(16,2,68,88,'smoke:%4d',41,1);SXY(0,0);;\r\n",temperature,humidity,smoke);
			GpuSend(showbuf);
			break;
	}
	
}

void show_airconditin_info(char *temperatrue,char mode,char windspeed)
{
	char showbuf[128];
	char *mode_str;
	char *windspeed_str;
	
	switch(mode)
	{
		case 'A':
			mode_str = "�Զ�";
			break;
		case 'L':
			mode_str = "����";
			break;
		case 'R':
			mode_str = "����";
			break;
		case 'F':
			mode_str = "�ͷ�";
			break;
	}
	
		switch(windspeed)
	{
		case 'A':
			windspeed_str = "�ͷ�";
			break;
		case 'L':
			windspeed_str = "����";
			break;
		case 'M':
			windspeed_str = "����";
			break;
		case 'H':
			windspeed_str = "����";
			break;
	}
	
			
			sprintf(showbuf,"W8UE(3);PIC(2,2,14);DS16(30,2,'\xBF\xD5\xB5\xF7\xC9\xE8\xD6\xC3',15);LABL(16,2,32,88,'\xC9\xE8\xB6\xA8: %s\xA1\xE6',41,1);LABL(16,2,50,88,'\xC4\xA3\xCA\xBD: %s',41,1);LABL(16,2,68,88,'\xB7\xE7\xCB\xD9: %s',41,1);SXY(0,0);;",temperatrue,mode_str,windspeed_str);
			GpuSend(showbuf);
	

}


