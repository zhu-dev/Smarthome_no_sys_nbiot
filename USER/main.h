#ifndef __MAIN_H
#define __MAIN_H

void _Error_Handler( char * file, int line );


/*COAP·þÎñÆ÷ÅäÖÃ */
#define NBIOT_COAP_SERVICE_IP "49.4.85.232"


typedef enum
{
    NBDemoPath_NONE,
    NBDemoPath_TCP,
    NBDemoPath_COAP,
    NBDemoPath_END
}NBAppDemoPath_t;

typedef enum
{
    NBState_NONE,
    NBState_INIT,
    NBState_TCPCONNET,
    NBState_COAPCONNET,
    NBState_PROCESS,
    NBState_END
}NBState_t;

typedef enum
{
    NBInit_NONE = 0x10,
    NBInit_CHECKMODEL,
    NBInit_REBOOT,
    NBInit_GETCIMI,
    NBInit_GETCSQ,
    NBInit_GETCEREG,
    NBInit_END
}NBInitStup_t;

typedef enum
{
    NBTcp_NONE = 0x20,
    NBTcp_CREATERSOCKET,
    NBTcp_CONNECTNET,
    NBTcp_END
}NBTcp_t;

typedef enum
{
    NBCoap_NONE = 0x30,
    NBCoap_CONNECTNET,
    NBCoap_END
}NBCoap_t;

typedef enum
{
    NBProcess_NONE = 0x40,
    NBProcess_TCPGETCSQ,
    NBProcess_TCPSENDDATA,
    NBProcess_COAPSENDDATA,
    NBProcess_END
}NBProcess_t;




#endif



