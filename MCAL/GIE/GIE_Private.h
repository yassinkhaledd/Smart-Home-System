
 #ifndef GIE_DEF_H_
 #define GIE_DEF_H_

 #include "STD_TYPES.h"


 /*---------------------------------------    Registers    ---------------------------------------*/

 /*Watchdog Control Register*/
 #define SREG	*((volatile u8 *)0x5F)	/*Watchdog Timer Control Register*/
 
 #define	I				7	/*Global Interrupt Enable*/



 #endif /* GIE_DEF_H_ */