# ifndef __SIM800A_H
# define __SIM800A_H

#include "sys.h"
#include "usart.h"		
#include "delay.h"		 
#include "string.h"    	
#include "usart6.h" 

char* sim800a_check_cmd(char *str);
u8 sim800a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim800a_send_data(u8 *data,u8 *ack,u16 waittime);
u8 sim800a_pdu_init(void);
void sim800a_send_warning(void);

#endif



