/*
 * **************************************
 * Copyright (c) 2018-2019, Ò°Å£ÎïÁªÍø.
 *      All rights reserved.
 * **************************************
 */
#ifndef __NB_AT_H
#define __NB_AT_H

#include "stm32f4xx.h" 
#include "main.h"



#define ATRESP_LINENUM	5
#define ATRESP_LINELEN	96

#define ATRESP_TABLENUM 2

struct at_urc
{
    const char	*cmd_prefix;
    const char	*cmd_suffix;
    void		(*func)( const char *data, uint16_t size );
};
typedef struct at_urc *at_urc_t;

typedef struct
{
    uint8_t lineSize;
    uint8_t respBuf[ATRESP_LINENUM][ATRESP_LINELEN];
    uint8_t line_counts;
}ATRespLine_TypeDef;

struct at_client
{
    const struct at_urc *urc_table;
    uint8_t				urc_table_size;
    uint8_t				recvBuf[128];
    uint16_t			cur_recv_len;
    ATRespLine_TypeDef	*resp;
};
typedef struct at_client *at_client_t;

void NBAT_init(void);


uint8_t NBAT_sendCmd( char *cmd, char *ack, uint16_t waittime, uint8_t *outResponse );


uint8_t NBAT_recvData( char *ack, uint16_t waittime, uint8_t *outResponse );


uint8_t NBAT_getUrc( at_client_t client );


uint8_t NBAT_checkUrc( uint8_t *outResponse );

uint8_t NBBCxx_parser( ATRespLine_TypeDef *resp, char* inData );

#endif
