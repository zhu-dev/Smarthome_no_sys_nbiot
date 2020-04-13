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


void check_NB_Status(void);
void  check_DHT11_Status(void);
void handle_cloud_cmd(char *data);


/*�յ����ݵĻص����� */
static void urc_recvdata_func( const char *data, uint16_t size )
{
    char outData[96];
    memset( outData, 0x00, sizeof(outData) ); //�����ʱ������
		memcpy( outData, data, sizeof(outData) ); //����ֱ�Ӳ���ȫ�ֱ���
   // printf( "[INFO]urc_recvdata_func\r\n" );		
    printf( "[INFO]recv data:%s\r\n", outData );
		handle_cloud_cmd(outData);//������յ�������
}


/*URC-usart received callback���� { "\r\nCONNECT",  "\r\n", urc_connect_func	},*/
static const struct at_urc urc_table[] = {
    { "\r\n+NNMI:", "\r\n", urc_recvdata_func },
};



int main(void)
{ 
 	
	//MQ sensor��ر���
	float ppm1,ppm2,ppm3;
	uint16_t ppm_1,ppm_2,ppm_3;
	
	//DHT11��ر���
	u8 t=0;			    
	u8 temperature1, temperature2, temperature3;  	    
	u8 humidity1, humidity2, humidity3;  
	

	//NBģ��������ر���
	uint8_t res		= 0;
	char	tmpBuf1[512];
	char	tmpBuf2[512];
  struct at_client	clientObj;
  at_client_t			client = &clientObj;
  NBBCxx_setUrcTable( client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]) );
	
	
	//�����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);    //��ʼ����ʱ����
	uart_init(9600);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED 
 	LCD_Init();         //��ʼ��LCD�ӿ�
	
	MQ2_Init();						//��ʼ��MQ2
	HC_SR_Init();		


	//DHT11���
	//check_DHT11_Status(); //���DHT11Ӳ���Ƿ���������
	//ADC_SoftwareStartConv(ADC1); //��ʼADCת��
	
	//NB-COAP���
	NBBCxx_init();  //NB M5310Aģ��������ʼ��
  //check_NB_Status();  //��������ʼ����ɺ󣬷���ָ����ģ���Ƿ��������ǵùر�PSMģʽ������������������
	
	
	while(1)
	{ 
		
		NBBCxx_getUrc( client );//��ȡ�ͻ��˶��󣬳ɹ��ɻ�ȡһ�λص����첽��ȡ
			
//		if(tim2count > 2000)
//		{
//			
//			TIM_Cmd(TIM2, DISABLE);  //�ر�TIM2
//			
//			DHT11_Read_Data(&temperature1,&humidity1);		//��ȡ��ʪ��ֵ	
//			DHT11_2_Read_Data(&temperature2,&humidity2);		//��ȡ��ʪ��ֵ			
//			DHT11_3_Read_Data(&temperature3,&humidity3);		//��ȡ��ʪ��ֵ
//			
//			printf("[DATA]temperature1:%d  humidity:%d\r\n",temperature1,humidity1);
//			printf("[DATA]temperature2:%d  humidity2:%d\r\n",temperature2,humidity2);
//			printf("[DATA]temperature3:%d  humidity3:%d\r\n",temperature3,humidity3);
//			
//			if(temperature1>100) temperature1 = 26; //�豸�ϵ�˲����һ������ֵ����ܴ�
//			if(temperature2>100) temperature2 = 26;
//			if(temperature3>100) temperature3 = 26;

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
//			if(ppm1>10000) ppm1 = 9999.0f;  //����10000ppm ֱ�ӱ���Ũ,��ֵ�ﵽ��ֵ�󣬿��Լ��ٴ�����ֳ�����������
//			if(ppm2>10000) ppm1 = 9999.0f;
//			if(ppm3>10000) ppm1 = 9999.0f;
//			
//			//��Ҫ�������������Ϊ50ʱ�Ƿ������4���ַ�
//			sprintf( tmpBuf1, "%2d%2d%4d%2d%2d%4d%2d%2d%4d",temperature1,humidity1,ppm_1,temperature2,humidity2,ppm_2,temperature2,humidity1,ppm_3);
//			printf( "[INFO]coap send primary data:%s\r\n", tmpBuf1 );
//			/*���и�ʽת�� */
//			memset( tmpBuf2, 0x00, sizeof(tmpBuf2) );
//			hexToStr( tmpBuf1, tmpBuf2 );
//			NBBCxx_setNMGS_char( tmpBuf2 );
//			printf("[INFO]coap send HEX data:%s\r\n",tmpBuf2);
//			
//			TIM_Cmd(TIM2, ENABLE);  //����TIM2
//			
//			//������ʾ���ݵ�LCD��
//			//LCD_show_data();
//			
//			
//			if(temperature1>50||temperature2>50||temperature3>50)
//			{
//				//���͸��¶��źͲ�������
//			}
//			
//			if(humidity1>99||humidity1>99||humidity1>99)
//			{
//				//���͸�ʪ�ȶ��źͲ�������
//			}
//			
//			if(ppm_1||ppm_2||ppm_3)
//			{
//				//���������޶��źͲ�������
//			}
//			
//		}


		
	}
}


void  check_DHT11_Status(void)
{
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		printf(" DHT11_1 init error");
	}
		delay_ms(10);
	
		while(DHT11_2_Init())	//DHT11_2��ʼ��	
	{
		printf(" DHT11_2 init error");
	}
	
	 delay_ms(10);
	
		while(DHT11_3_Init())	//DHT11_3��ʼ��	
	{
		printf(" DHT11_3 init error");
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
	 }	 
}


void handle_cloud_cmd(char *data)
{

	char messageId; //����ͷ��(messageId)

	char * p = NULL; //ƥ���ַ�λ��ָ��
	
	char * cmd;//ƽ̨�·���������(messageId+content)
	
	char * cmd_body; //������ԭʼ16�����ַ���
	
	char body[50];//������������������Ϊascii�ַ���
	
	
	uint32_t outLen = 0;//ת������ַ����ȣ�������ԭ����һ��
	
	uint8_t res; //�����
	
	p = (char *)strstr((const char *)data,(const char *)",");
	
	cmd = p+1;
	
	cmd_body = p+3;
	
	messageId = cmd[1];

//	printf("cmd_body:%s",cmd_body);
//	printf("cmd_body len%d\r\n",strlen(cmd_body));
	
	cmd_body[strlen(cmd_body)-4] = '\0';//ƽ̨�������Ĵ���\r\n��β������һ��\����Ϊ'\0'��ȥ���س�����
	//printf("cmd_body2 len%d\r\n",strlen(cmd_body));
	
	memset(body,0x00,sizeof(body));//��ת��ǰ����ʱ���������
	res = strToHex(cmd_body,body,&outLen);//��16���ַ���ת��Ϊascii�ַ���ע��ͷ������ת����������ţ����޷���ʾ
	
//	printf("res%d\r\n",res);
	printf("out%s\r\n",body);
	
	
	//�������� 
	switch(messageId)
	{
		case 0x31:
			//�����ԭ��������ѷ�����
			break;
		case 0x32:
			//����ǿյ�����
		printf("aircondition\r\n");
			if(body[0] == 'O')
			{
				switch(body[1])
				{
					case 'A':
						break;
					case 'L':
						printf("aircondition->MODE: L\r\n");
						break;
					case 'R':
						break;
					case 'F':
						break;
				}
				
				switch(body[4])
				{
					case 'A':
						break;
					case 'L':
						break;
					case 'M':
						break;
					case 'H':
						break;
				}
				
				//temperature = body[2`3]
			}
			break;
		case 0x33:
			//����Ǵ�������
			if(strstr(body,"O"))
			{
				printf("curtain_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("curtain_CLOSE\r\n");
			}
			break;
		case 0x34:
			//����Ǽ�ͥģʽ����
			printf("home_mode%s: \r\n",body);
			if(strstr(body,"O"))
			{
				printf("home_mode_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("home_mode_CLOSE\r\n");
			}
		
			break;
		case 0x35:
			//����Ƿ�������
			if(strstr(body,"O"))
			{
				printf("fanning_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("fanning_CLOSE\r\n");
			}
			break;
		case 0x36:
			//����ǿ���������
			if(strstr(body,"O"))
			{
				printf("lighting_parlour_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("lighting_parlour_CLOSE\r\n");
			}
			break;
		case 0x37:
			//��������ҵ�����
			if(strstr(body,"O"))
			{
				printf("lighting_bedroom_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("lighting_bedroom_CLOSE\r\n");
			}
			break;
		case 0x38:
			//����ǳ���������
			if(strstr(body,"O"))
			{
				printf("lighting_kitchen_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("lighting_kitchen_CLOSE\r\n");
			}
			break;
		case 0x39:
			//�����ԡ�ҵ�����
			break;
		case 0x61:
			//�������չ����
			if(strstr(body,"O"))
			{
				printf("extend_other_OPEN\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("extend_other_CLOSE\r\n");
			}
			break;
	}
}







