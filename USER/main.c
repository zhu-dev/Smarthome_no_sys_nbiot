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


/*收到数据的回调函数 */
static void urc_recvdata_func( const char *data, uint16_t size )
{
    char outData[96];
    memset( outData, 0x00, sizeof(outData) ); //清空临时缓冲区
		memcpy( outData, data, sizeof(outData) ); //不能直接操作全局变量
   // printf( "[INFO]urc_recvdata_func\r\n" );		
    printf( "[INFO]recv data:%s\r\n", outData );
		handle_cloud_cmd(outData);//处理接收到的命令
}


/*URC-usart received callback设置 { "\r\nCONNECT",  "\r\n", urc_connect_func	},*/
static const struct at_urc urc_table[] = {
    { "\r\n+NNMI:", "\r\n", urc_recvdata_func },
};



int main(void)
{ 
 	
	//MQ sensor相关变量
	float ppm1,ppm2,ppm3;
	uint16_t ppm_1,ppm_2,ppm_3;
	
	//DHT11相关变量
	u8 t=0;			    
	u8 temperature1, temperature2, temperature3;  	    
	u8 humidity1, humidity2, humidity3;  
	

	//NB模组驱动相关变量
	uint8_t res		= 0;
	char	tmpBuf1[512];
	char	tmpBuf2[512];
  struct at_client	clientObj;
  at_client_t			client = &clientObj;
  NBBCxx_setUrcTable( client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]) );
	
	
	//外设初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(9600);	//初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();         //初始化LCD接口
	
	MQ2_Init();						//初始化MQ2
	HC_SR_Init();		


	//DHT11相关
	//check_DHT11_Status(); //检查DHT11硬件是否连接正常
	//ADC_SoftwareStartConv(ADC1); //开始ADC转换
	
	//NB-COAP相关
	NBBCxx_init();  //NB M5310A模组驱动初始化
  //check_NB_Status();  //在驱动初始化完成后，发送指令检查模组是否正常，记得关闭PSM模式！！！！！！！！！
	
	
	while(1)
	{ 
		
		NBBCxx_getUrc( client );//获取客户端对象，成功可获取一次回调，异步获取
			
//		if(tim2count > 2000)
//		{
//			
//			TIM_Cmd(TIM2, DISABLE);  //关闭TIM2
//			
//			DHT11_Read_Data(&temperature1,&humidity1);		//读取温湿度值	
//			DHT11_2_Read_Data(&temperature2,&humidity2);		//读取温湿度值			
//			DHT11_3_Read_Data(&temperature3,&humidity3);		//读取温湿度值
//			
//			printf("[DATA]temperature1:%d  humidity:%d\r\n",temperature1,humidity1);
//			printf("[DATA]temperature2:%d  humidity2:%d\r\n",temperature2,humidity2);
//			printf("[DATA]temperature3:%d  humidity3:%d\r\n",temperature3,humidity3);
//			
//			if(temperature1>100) temperature1 = 26; //设备上电瞬间有一个垃圾值，会很大
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
//			if(ppm1>10000) ppm1 = 9999.0f;  //大于10000ppm 直接报最浓,数值达到阈值后，可以减少传输的字长，减少流量
//			if(ppm2>10000) ppm1 = 9999.0f;
//			if(ppm3>10000) ppm1 = 9999.0f;
//			
//			//需要测试下烟雾参数为50时是否还是输出4个字符
//			sprintf( tmpBuf1, "%2d%2d%4d%2d%2d%4d%2d%2d%4d",temperature1,humidity1,ppm_1,temperature2,humidity2,ppm_2,temperature2,humidity1,ppm_3);
//			printf( "[INFO]coap send primary data:%s\r\n", tmpBuf1 );
//			/*进行格式转换 */
//			memset( tmpBuf2, 0x00, sizeof(tmpBuf2) );
//			hexToStr( tmpBuf1, tmpBuf2 );
//			NBBCxx_setNMGS_char( tmpBuf2 );
//			printf("[INFO]coap send HEX data:%s\r\n",tmpBuf2);
//			
//			TIM_Cmd(TIM2, ENABLE);  //开启TIM2
//			
//			//这里显示数据到LCD上
//			//LCD_show_data();
//			
//			
//			if(temperature1>50||temperature2>50||temperature3>50)
//			{
//				//发送高温短信和播放语音
//			}
//			
//			if(humidity1>99||humidity1>99||humidity1>99)
//			{
//				//发送高湿度短信和播放语音
//			}
//			
//			if(ppm_1||ppm_2||ppm_3)
//			{
//				//发送烟雾超限短信和播放语音
//			}
//			
//		}


		
	}
}


void  check_DHT11_Status(void)
{
	while(DHT11_Init())	//DHT11初始化	
	{
		printf(" DHT11_1 init error");
	}
		delay_ms(10);
	
		while(DHT11_2_Init())	//DHT11_2初始化	
	{
		printf(" DHT11_2 init error");
	}
	
	 delay_ms(10);
	
		while(DHT11_3_Init())	//DHT11_3初始化	
	{
		printf(" DHT11_3 init error");
	}
}



void check_NB_Status(void)
{
	 uint8_t error = 0;
	
	 while(NBBCxx_checkModel()!=0)   //监测模组是否连接正常
	 {
		  error = 1;
		  printf("[NB_ERROR]CHECK MODEL Fail\r\n");
	 }
	 delay_ms(400);
	 
	 while(NBBCxx_getCSQ(NULL)!=0)  //查看模组信号强度
	 {
		  error = 1;
		  printf("[NB_ERROR]MODEL GETCSQ Fail\r\n" );
	 }
	 delay_ms(400);
	 
	 while(NBBCxx_getCEREG()!=0)  //查看模组是否注册网络成功
	 {
		  error = 1;
		 printf( "[NB_ERROR]MODEL GETCEREG Fail\r\n" );
	 }
	 delay_ms(400);

	 while(NBBCxx_setNCDP( NBIOT_COAP_SERVICE_IP )!=0)  //设置Coap的Ip和端口，端口默认5683，
	 {
		  error = 1;
		 printf( "[NB_ERROR]MODEL SETNCDP Fail\r\n" );
	 } 
	 delay_ms(400);
	 
	 if(error)
	 {
		   //可以重启模组
	 }	 
}


void handle_cloud_cmd(char *data)
{

	char messageId; //命令头部(messageId)

	char * p = NULL; //匹配字符位置指针
	
	char * cmd;//平台下发的命令体(messageId+content)
	
	char * cmd_body; //命令体原始16进制字符串
	
	char body[50];//命令输出缓冲区，输出为ascii字符串
	
	
	uint32_t outLen = 0;//转化后的字符长度，正常是原来的一半
	
	uint8_t res; //结果码
	
	p = (char *)strstr((const char *)data,(const char *)",");
	
	cmd = p+1;
	
	cmd_body = p+3;
	
	messageId = cmd[1];

//	printf("cmd_body:%s",cmd_body);
//	printf("cmd_body len%d\r\n",strlen(cmd_body));
	
	cmd_body[strlen(cmd_body)-4] = '\0';//平台发回来的带有\r\n结尾，将第一个\设置为'\0'，去掉回车换行
	//printf("cmd_body2 len%d\r\n",strlen(cmd_body));
	
	memset(body,0x00,sizeof(body));//在转化前将临时缓冲区清除
	res = strToHex(cmd_body,body,&outLen);//将16进字符串转化为ascii字符，注意头部不能转，是特殊符号，会无法显示
	
//	printf("res%d\r\n",res);
	printf("out%s\r\n",body);
	
	
	//分离数据 
	switch(messageId)
	{
		case 0x31:
			//如果是原开灯命令（已废弃）
			break;
		case 0x32:
			//如果是空调命令
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
			//如果是窗帘命令
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
			//如果是家庭模式命令
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
			//如果是风扇命令
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
			//如果是客厅灯命令
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
			//如果是卧室灯命令
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
			//如果是厨房灯命令
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
			//如果是浴室灯命令
			break;
		case 0x61:
			//如果是拓展命令
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







