#ifndef __light_H
#define __light_H

#include "sys.h"


#define Parlour_light PBout(5)	
#define Bedroom_light PBout(6)	
#define Kitchen_light PBout(7)	 

#define Fan PBout(8)		 

void light_init(void);
void fan_init(void);
#endif


