/*
 * **************************************
 * Copyright (c) 2018-2019, Ò°Å£ÎïÁªÍø.
 *      All rights reserved.
 * **************************************
 */
#ifndef __NB_BC95_H
#define __NB_BC95_H

#include "stm32f4xx.h" 
#include "nb_at.h"


typedef struct
{
    uint8_t adr0;
    uint8_t adr1;
    uint8_t adr2;
    uint8_t adr3;
} SERVER_INFO;

typedef enum
{
    kSOCKETType_INIT	= 0x00,
    kSOCKETType_TCP		= 0x01,
    kSOCKETType_UDP		= 0x02,
    kSOCKETType_END		= 0x03,
} BC95_SOCKETTypeDef;


void NBBCxx_init(void);

uint8_t NBBCxx_reboot( void );

uint8_t NBBCxx_checkModel( void );

uint8_t NBBCxx_getACK( void );


uint8_t NBBCxx_setCMEE( void );


uint8_t NBBCxx_getNBAND( void );


uint8_t NBBCxx_getCIMI( void );


uint8_t NBBCxx_setCGATT( void );


uint8_t NBBCxx_getCSQ( uint8_t* csq );


uint8_t NBBCxx_getCEREG( void );


uint8_t NBBCxx_setCEREG( void );


uint8_t NBBCxx_getCOPS( void );


uint8_t NBBCxx_setCGDCONT( void );


uint8_t NBBCxx_getCGATT( void );


uint8_t NBBCxx_getCSCON( void );


uint8_t NBBCxx_setNCDP( char *ipAdr );


uint8_t NBBCxx_setNMGS_char( char *inData );


uint8_t NBBCxx_getUrc( at_client_t client );


void NBBCxx_setUrcTable( at_client_t client, const struct at_urc *urc_table, uint16_t table_sz );


void hexToStr( char *inBuf, char *outBuf );

uint8_t strToHex( char *inBuf, char *outBuf, uint32_t *outLen );

uint8_t strToHex( char *inBuf, char *outBuf, uint32_t *outLen );

char *leftSlice(char *dst,char *src,int n);

char *rightSlice(char *dst,char *src,int n);

uint8_t substring(char *dst,char *src,int start,int len);

#endif
