/*
 * **************************************
 * 		NBIot-Coap协议接口Api
 *			接口封装文件
 *    	解耦具体实现  
 * **************************************
 */

#include "nb_interface.h"
#include "stm32f4xx.h" 
#include "string.h"
#include "usart.h"
#include "led.h"



static uint8_t	atRxBuf[512];
static char		atTxBuf[512];
char			tmpData[512];

void (*urcFunc)( const char *data, uint16_t size );

#define PP_HTONS( x ) ( ( ( (x) & 0xff) << 8) | ( ( (x) & 0xff00) >> 8) )

const char	hexStrTable[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };


void hexToStr( char *inBuf, char *outBuf )
{
    uint16_t i = 0;
    for (; i < (uint16_t) strlen( inBuf ); i++ )
    {
        outBuf[2 * i + 0]	= hexStrTable[(inBuf[i] >> 4)];
        outBuf[2 * i + 1]	= hexStrTable[(inBuf[i] & 0x0F)];
    }
    outBuf[2 * i + 0] = 0;
}


uint8_t strToHex( char *inBuf, char *outBuf, uint32_t *outLen )
{
    uint16_t	i	= 0;
    uint8_t		tmp = 0;
    if ( (strlen( inBuf ) % 2) != 0 )
        return (1);
    for (; i < (uint16_t) strlen( inBuf ); i++ )
    {
			//printf("1");
        tmp = 0;
        if ( inBuf[i] >= '0' && inBuf[i] <= '9' )
        {
					//printf("2");
            tmp = inBuf[i] - '0';
        }
        else if ( inBuf[i] >= 'A' && inBuf[i] <= 'F' )
        {
					//printf("3");
            tmp = inBuf[i] - 'A' + 0x0A;
        }
        else if ( inBuf[i] >= 'a' && inBuf[i] <= 'f' )
        {
            tmp = inBuf[i] - 'A' + 0x0A;
        }
        else
        {
            return (2);
        }
        if ( (i % 2) == 0 )
            outBuf[i / 2] = tmp << 4;
        else
            outBuf[i / 2] |= tmp;
    }
    *outLen = strlen( inBuf ) / 2;
//		printf("func out:%s\r\n",outBuf);
//		printf("func outlen%d\r\n",*outLen);
    return (0);
}


/**
* function :从源字符串左边拷贝N个字符到目标字符串
*
*/
char *leftSlice(char *dst,char *src,int n)
{
	char *p=src;
	char *q=dst;
	int len=strlen(src);
	if(n>len) n=len;
	while(n--) *(q++)=*(p++);
	*(q++)='\0';
	return dst;
}


/**
* function :从源字符串右边拷贝N个字符到目标字符串
*
*/
char *rightSlice(char *dst,char *src,int n)
{
	char *p=src;
	char *q=dst;
	int len=strlen(src);
	if(n>len) n=len;
	p+=(len-n);
	while(*(q++)=*(p++));
	return dst;
}


/**
* function :截取固定区间字符串
*
*/
uint8_t substring(char *dst,char *src,int start,int len)
{  
	int length=strlen(src); 
	
	if(start>=length||start<0) return (1);  
		
	if(len>length) len=length-start; 
		
	src+=start;
	
	while(len--)  
	{   
		*(dst++)=*(src++); 
	}  
	*(dst++)='\0';
  
	return (0);
}



/**
* function :初始化模组
*
*/
void NBBCxx_init()
{
    NBAT_init();
}
/**
*		function:	监测模组是否正常工作  "AT\r\n"
*		params; void
*		rerun:	1->OK   正常
*						2->ERROR  错误
*
*/
uint8_t NBBCxx_checkModel( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]CHECK MODEL RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    return (res);
}

/**
*		function:	重启模组   "AT+NRB\r\n"
*		params; void 
*		rerun:	REBOOTING
*
*/
uint8_t NBBCxx_reboot( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+NRB\r\n", "REBOOTING", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]REBOOT RESPONSE SUCCESS\r\n" );
    }
    return (res);
}


/**
*		function:	获取模组应答  "AT\r\n"
*		params; void
*		rerun:	1->OK   正常
*						2->ERROR  错误
*
*/
uint8_t NBBCxx_getACK( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETACK RESPONSE SUCCESS\r\n" );
    }
    return (res);
}


uint8_t NBBCxx_setCMEE( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CMEE=1\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETCMEE RESPONSE SUCCESS\r\n" );
    }
    return (res);
}

/**
*		function:	获取模组支持的频段  "AT+NBAND?\r\n"
*		params; void
*		rerun:	1->OK   正常
*						2->+NBAND:5,8  B5,B8频段
*
*/
uint8_t NBBCxx_getNBAND( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+NBAND?\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETNBAND RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    return (res);
}



/**
*		function:	获取 SIM 卡的 CIMI 号，模块回 15 字节数据
*		params; void
*		rerun:	1->OK   正常
*						2-> 15字节数据
*
*/
uint8_t NBBCxx_getCIMI( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CIMI\r\n", "46", 100, atRxBuf );
    if ( res == 0 )
    {
        /*NBBCxx_parser( (char*)atRxBuf ); */
        printf( "[INFO]GETCIMI RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    else
    {
        printf( "[INFO]AT RESPONSE FAIL\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    return (res);
}



/**
*		function:	
*		params; void
*		rerun:	
*						
*
*/
uint8_t NBBCxx_setCGATT( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CGATT=1\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETCGATT RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    else
    {
        printf( "[INFO]AT RESPONSE FAIL\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
    }
    return (res);
}



/**
*		function:	获取模组信号强度
*		params; void
*		rerun:	1->OK   正常
*						2-> +CSQ:xx,xx ,第一个参数大于16时表示可用
*/
uint8_t NBBCxx_getCSQ( uint8_t* csq )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CSQ\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        /*获取CSQ */
        uint8_t csqTmp = (atRxBuf[7] - 0x30) * 10 + (atRxBuf[8] - 0x30);
        if ( csqTmp == 99 )
        {
            printf( "[ERROR]GETCSQ Fail\n" );
            return (3);
        }
        if ( csq != NULL )
            *csq = csqTmp;


        /*
         * NBBCxx_parser(atRxBuf);
         * printf( "[INFO]GETCSQ RESPONSE:\r\n%s %s\r\n", atRespLine[0],atRespLine[1]);
         */
    }
    else
    {
        printf( "[INFO]AT RESPONSE FAIL\r\n" );
    }
    return (res);
}



/**
*		function:	监测模组是否注网成功
*		params; void
*		rerun:	1->OK   正常
*						2-> 返回0,1可用，返回其他异常
*/
uint8_t NBBCxx_getCEREG( void )
{
    uint8_t res			= 0;
    char	*str0Res	= NULL;
    char	*str1Res	= NULL;
    res = NBAT_sendCmd( "AT+CEREG?\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETCEREG RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
        str0Res = (char *) strstr( (const char *) atRxBuf, (const char *) "+CEREG:0,1" );
        str1Res = (char *) strstr( (const char *) atRxBuf, (const char *) "+CEREG:1,1" );
        if ( str0Res == NULL && str1Res == NULL )
        {
            res = 1;
        }
    }
    return (res);
}



/**
*		function:	
*		params; void
*		rerun:	
*						
*/
uint8_t NBBCxx_setCEREG( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CEREG=1\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETCEREG RESPONSE SUCCESS\r\n" );
    }
    return (res);
}



/**
*		function:	
*		params; void
*		rerun:	
*						
*/
uint8_t NBBCxx_getCOPS( void )
{
    uint8_t res			= 0;
    char	*str0Res	= NULL;
    char	*str1Res	= NULL;
    res = NBAT_sendCmd( "AT+COPS?\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETCOPS RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
        str0Res = (char *) strstr( (const char *) atRxBuf, (const char *) "46011" );
        str1Res = (char *) strstr( (const char *) atRxBuf, (const char *) "46000" );
        if ( (str0Res == NULL) && (str1Res == NULL) )
        {
            res = 1;
        }
    }
    return (res);
}

/**
*		function:	设置Coap协议服务器的IP和端口
*		params; ip address
*		rerun:	Ok
*						
*/
uint8_t NBBCxx_setNCDP( char *ipAdr )
{
    uint8_t res = 0;
    sprintf( atTxBuf, "AT+NCDP=%s\r\n", ipAdr );
    res = NBAT_sendCmd( atTxBuf, "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETNCDP RESPONSE SUCCESS\r\n" );
    }
    return (res);
}



/**
*		function:	通过COPA协议发送数据
*		params; data
*		rerun:	OK
*						
*/
uint8_t NBBCxx_setNMGS_char( char *inData )
{
    uint8_t res = 0;
    sprintf( atTxBuf, "AT+NMGS=%d,%s\r\n", strlen( inData ) / 2, inData );
    res = NBAT_sendCmd( atTxBuf, "OK", 2000, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETNMGS RESPONSE SUCCESS\r\n" );
    }
    return (res);
}



/**
*		function:	
*		params; void
*		rerun:	
*						
*/
uint8_t NBBCxx_setCGDCONT( void )
{
    uint8_t res = 0;
    res = NBAT_sendCmd( "AT+CGDCONT=1,\042IP\042,\042HUAWEI.COM\042\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]SETCGDCONT RESPONSE SUCCESS\r\n" );
    }
    return (res);
}




/**
*		function:	
*		params; void
*		rerun:	
*						
*/
uint8_t NBBCxx_getCGATT( void )
{
    uint8_t res			= 0;
    char	*str0Res	= NULL;
    res = NBAT_sendCmd( "AT+CGATT?\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETCGATT RESPONSE SUCCESS\r\n" );
        str0Res = (char *) strstr( (const char *) atRxBuf, (const char *) "+CGATT:1" );
        if ( str0Res == NULL )
        {
            res = 1;
        }
    }
    return (res);
}




/**
*		function:	
*		params; void
*		rerun:	
*						
*/
uint8_t NBBCxx_getCSCON( void )
{
    uint8_t res			= 0;
    char	*str0Res	= NULL;
    char	*str1Res	= NULL;
    res = NBAT_sendCmd( "AT+CSCON?\r\n", "OK", 100, atRxBuf );
    if ( res == 0 )
    {
        printf( "[INFO]GETCSCON RESPONSE SUCCESS\r\n" );
        printf( "[INFO]RESPONSE IS %s\r\n", atRxBuf );
        str0Res = (char *) strstr( (const char *) atRxBuf, (const char *) "+CSCON:0,1" );
        str1Res = (char *) strstr( (const char *) atRxBuf, (const char *) "+CSCON:0,0" );
        if ( str0Res == NULL && str1Res == NULL )
        {
            res = 1;
        }
    }
    return (res);
}



void NBBCxx_setUrcTable( at_client_t client, const struct at_urc *urc_table, uint16_t table_sz )
{
    client->urc_table		= urc_table;
    client->urc_table_size	= table_sz;
}


static const struct at_urc *get_urc_obj( at_client_t client )
{
    uint16_t	i, prefix_len, suffix_len;
    uint16_t	buf_sz;
    char		*buffer = NULL;

    buffer	= (char*)client->recvBuf;
    buf_sz	= client->cur_recv_len;

		
    for ( i = 0; i < client->urc_table_size; i++ )
    {

        prefix_len	= strlen( client->urc_table[i].cmd_prefix );
        suffix_len	= strlen( client->urc_table[i].cmd_suffix );
        if ( buf_sz < prefix_len + suffix_len )
        {
            continue;
        }
        if ( (prefix_len ? !strncmp( buffer, client->urc_table[i].cmd_prefix, prefix_len ) : 1)
             && (suffix_len ? !strncmp( buffer + buf_sz - suffix_len, client->urc_table[i].cmd_suffix, suffix_len ) : 1) )
        {
					
            return (&client->urc_table[i]);
        }
    }
    return (NULL);
}


/* brief:非阻塞式接收数据
 * return:0:成功  非0失败
 */
uint8_t NBBCxx_getUrc( at_client_t client )
{
    uint8_t res = 0;
    res = NBAT_getUrc( client );
    if ( res == 0 )
    {
        const struct at_urc *urc;
        if ( (urc = get_urc_obj( client ) ) != NULL )
        {
						LED0 = !LED0;
            urc->func( (char*)client->recvBuf, client->cur_recv_len );
        }
    }
    return (res);
}






