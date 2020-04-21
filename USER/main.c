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
void check_DHT11_Status(void);
void handle_cloud_cmd(char *data);


/*收到数据的回调函数 */
static void urc_recvdata_func( const char *data, uint16_t size )
{
    char outData[96];
    memset( outData, 0x00, sizeof(outData) ); //清空临时缓冲区
		memcpy( outData, data, sizeof(outData) ); //不能直接操作全局变量
   // printf( "[INFO]urc_recvdata_func\r\n" );		
    printf( "[INFO]recv data:%s", outData );
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
	uint16_t ppm_1 = 0;
	uint16_t ppm_2 = 0;
	uint16_t ppm_3 = 0;
	
	//DHT11相关变量,赋初值，避免引传感器出错，导致上报数据异常，引起平台和应用服务器报错
	u8 t=0;			    
	u8 temperature1 = 26;
	u8 temperature2 = 26;
	u8 temperature3 = 26;	    
	u8 humidity1 = 0;
	u8 humidity2 = 0;
	u8 humidity3 = 0; 
	

	//NB模组驱动相关变量
	uint8_t res		= 0;
	char *headstr = "00";
	char	tmpBuf1[128];
	char	tmpBuf2[128];
	char	tmpBuf3[128];
  struct at_client	clientObj;
  at_client_t			client = &clientObj;
  NBBCxx_setUrcTable( client, urc_table, sizeof(urc_table) / sizeof(urc_table[0]) );
	
	
	//外设初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(9600);	//初始化串口波特率为9600
	LED_Init();					//初始化板载测试LED 

	
	MQ2_Init();						//初始化MQ2
	HC_SR_Init();		      //人体红外

	//DHT11相关
	check_DHT11_Status(); //检查DHT11硬件是否连接正常
	ADC_SoftwareStartConv(ADC1); //开始ADC转换
	
	//NB-COAP相关
	NBBCxx_init();  //NB M5310A模组驱动初始化
  check_NB_Status();  //在驱动初始化完成后，发送指令检查模组是否正常，记得关闭PSM模式！！！！！！！！！
	
	
	while(1)
	{ 
		
		NBBCxx_getUrc( client );//获取客户端对象，成功可获取一次回调，异步获取
		
		if(t%100 == 0)
		{
			
			DHT11_Read_Data(&temperature1,&humidity1);		//读取温湿度值	
			DHT11_2_Read_Data(&temperature2,&humidity2);		//读取温湿度值			
			DHT11_3_Read_Data(&temperature3,&humidity3);		//读取温湿度值
			
			printf("[DATA]temperature1:%d  humidity:%d\r\n",temperature1,humidity1);
			printf("[DATA]temperature2:%d  humidity2:%d\r\n",temperature2,humidity2);
			printf("[DATA]temperature3:%d  humidity3:%d\r\n",temperature3,humidity3);
			
			if(temperature1>99) temperature1 = 26; //设备上电瞬间有一个垃圾值，会很大，用初值代替
			if(temperature2>99) temperature2 = 26;
			if(temperature3>99) temperature3 = 26;

			if(humidity1>99) humidity1 = 50; //设备上电瞬间有一个垃圾值，会很大，用初值代替
			if(humidity2>99) humidity2 = 50;
			if(humidity3>99) humidity3 = 50;
			
			ppm1 = MQ2_GetPPM(0);
			ppm2 = MQ2_GetPPM(1);
			ppm3 = MQ2_GetPPM(2);
			
			ppm_1 = ppm1;
			ppm_2 = ppm2;
			ppm_3 = ppm3;
			
			printf("[DATA]ppm1:%.2f \r\n",ppm1);
			printf("[DATA]ppm2:%.2f \r\n",ppm2);
			printf("[DATA]ppm3:%.2f \r\n",ppm3);
			
			if(ppm_1>9999) ppm_1 = 9999;  //大于10000ppm 直接报最浓,数值达到阈值后，可以减少传输的字长，减少流量
			if(ppm_2>9999) ppm_2 = 9999;
			if(ppm_3>9999) ppm_3 = 9999;
			
			sprintf( tmpBuf1, "%2d%2d%3d%2d%2d%3d%2d%2d%3d",temperature1,humidity1,ppm_1,temperature2,humidity2,ppm_2,temperature3,humidity3,ppm_3);
			printf( "\r\n[INFO]coap send primary data:%s\r\n", tmpBuf1);
			/*进行格式转换 */
			memset(tmpBuf2, 0x00, sizeof(tmpBuf2) );
			hexToStr( tmpBuf1, tmpBuf2 );
			sprintf(tmpBuf3,"%s%s",headstr,tmpBuf2);
			NBBCxx_setNMGS_char( tmpBuf3 );
			printf("[INFO]coap send HEX data:%s\r\n",tmpBuf3);
			
		}
		
		delay_ms(20);
		if(t>200) t=0;
		t++;
	
		
	}
}


void  check_DHT11_Status(void)
{
	uint8_t error = 0;
	while(DHT11_Init())	//DHT11初始化	
	{
		error = 1;
		printf(" DHT11_1 init error");
	}
		delay_ms(10);
	
		while(DHT11_2_Init())	//DHT11_2初始化	
	{
		error = 1;
		printf(" DHT11_2 init error");
	}
	
	 delay_ms(10);
	
		while(DHT11_3_Init())	//DHT11_3初始化	
	{
		error = 1;
		printf(" DHT11_3 init error");
	}
	
		 if(error)
	 {
		   //可以重启
		  //点亮故障灯
	 }
	 else
	 {
		 printf("DHT11 INIT SUCESS!!!\r\n");
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
		  //点亮故障灯
	 }
	 else
	 {
		 printf("NB MODEL INIT SUCESS!!!\r\n");
	 }
		
}


void handle_cloud_cmd(char *data)
{

	char messageId; //命令头部(messageId)

	char *p = NULL; //匹配字符位置指针
	
	char *cmd;//平台下发的命令体(messageId+content)
	
	char *cmd_body; //命令体原始16进制字符串
	
	char body[50];//命令输出缓冲区，输出为ascii字符串
	
	char *temperatrueSet = "26";
	
	uint32_t outLen = 0;//转化后的字符长度，正常是原来的一半
	
	uint8_t res; //结果码
	
	//+NNMI:2,0343\r\n
	
	p = (char *)strstr((const char *)data,(const char *)",");
	
	cmd = p+1;
	
	cmd_body = p+3;
	
	messageId = cmd[1];

	//cmd_body[strlen(cmd_body)-4] = '\0';//测试的带有\r\n结尾，将第一个\设置为'\0'，去掉回车换行,但是真正平台发回来的却没有，神奇

	memset(body,0x00,sizeof(body));//在转化前将临时缓冲区清除
	res = strToHex(cmd_body,body,&outLen);//将16进字符串转化为ascii字符，注意头部不能转，是特殊符号，会无法显示
	
	//printf("bodyout:%s\r\n",body);
	
	
	//分离数据 
	switch(messageId)
	{
		case 0x31:
			//如果是原开灯命令（已废弃）
			break;
		case 0x32:
			//如果是空调命令
			printf("设置空调...\r\n");
			if(body[0] == 'O')
			{
				switch(body[1])
				{
					case 'A':
						printf("[aircondition]->模式:自动 L\r\n");
						break;
					case 'L':
						printf("[aircondition]->模式: 制冷\r\n");
						break;
					case 'R':
						printf("[aircondition]->模式: 制热\r\n");
						break;
					case 'F':
						printf("[aircondition]->模式: 送风\r\n");
						break;
				}
				
				switch(body[4])
				{
					case 'A':
						printf("[aircondition]->风速: 自动\r\n");
						break;
					case 'L':
						printf("[aircondition]->风速: 低速\r\n");
						break;
					case 'M':
						printf("[aircondition]->风速: 中速\r\n");
						break;
					case 'H':
						printf("[aircondition]->风速: 高速\r\n");
						break;
				}
				
				sprintf(temperatrueSet,"%c%c",body[2],body[3]);
				printf("[aircondition]->温度: %s℃\r\n",temperatrueSet);
			}
			break;
		case 0x33:
			printf("控制窗帘...\r\n");
			//如果是窗帘命令
			if(strstr(body,"O"))
			{
				printf("打开窗帘\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭窗帘\r\n");
			}
			break;
		case 0x34:
			//如果是家庭模式命令
			printf("设置家庭模式%s: \r\n",body);
			if(strstr(body,"O"))
			{
				printf("打开所有灯\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭所有灯\r\n");
			}
		
			break;
		case 0x35:
			//如果是风扇命令
			printf("控制风机\r\n");
			if(strstr(body,"O"))
			{
				printf("打开风机\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭风机\r\n");
			}
			break;
		case 0x36:
			//如果是客厅灯命令
			printf("控制客厅灯\r\n");
			if(strstr(body,"O"))
			{
				printf("打开客厅灯\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭客厅灯\r\n");
			}
			break;
		case 0x37:
			//如果是卧室灯命令
			printf("控制卧室灯\r\n");
			if(strstr(body,"O"))
			{
				printf("打开卧室灯\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭卧室灯\r\n");
			}
			break;
		case 0x38:
			//如果是厨房灯命令
			printf("控制厨房灯\r\n");
			if(strstr(body,"O"))
			{
				printf("打开厨房灯\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("关闭厨房灯\r\n");
			}
			break;
		case 0x39:
			//如果是浴室灯命令
			printf("控制浴室灯\r\n");
			break;
		case 0x61:
			//如果是拓展命令
			printf("设置拓展命令\r\n");
			if(strstr(body,"O"))
			{
				printf("拓展开\r\n");
			}
			else if(strstr(body,"C"))
			{
				printf("拓展关\r\n");
			}
			break;
	}
}







