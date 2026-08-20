/*
 * UART.c
 *
 * Created: 6/27/2023 1:34:42 PM
 *  Author: Rizk
 */ 
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "UART_Private.h"
#include "UART_Configh.h"
#include "UART_Interface.h"
#define  F_CPU 8000000UL



void USART_Init(u16 UART_BAUDRATE)
{
	u8 LOC_Var =0;
	u16 BAUD_PRESCALE= (((F_CPU / (UART_BAUDRATE * 16UL))) - 1);
	

	/*	Speed mode Double		*/
	CLR_BIT(UCSRA, 1);
	
	/* Set baud rate */
	UBRRH=(u8)(BAUD_PRESCALE>>8);
	UBRRL=(u8)BAUD_PRESCALE;
	
	
	/* Enable receiver and transmitter Tx Rx 	*/
	SET_BIT( UCSRB , 3 );
	SET_BIT( UCSRB , 4 );
	
	

	
	/*Asynchronous Mode*/
	CLR_BIT(LOC_Var,6);
	
	
	/*	Parity mode Disabled  	*/
	CLR_BIT(LOC_Var, 4 );
	CLR_BIT(LOC_Var, 5 );
	
	
	/*	Stop bit -> one Bits */
	CLR_BIT( LOC_Var , 3 );
	
	/*Select 8-bits Data*/
	SET_BIT( LOC_Var , 1 );
	SET_BIT( LOC_Var , 2 );
	CLR_BIT( LOC_Var , 3 );
	
	/*Write Value in UCSRC Register */
	SET_BIT(LOC_Var,7);
	
	/*Store UCSRC Register */
	UCSRC = LOC_Var;
	
}

void USART_Transmit( u8 data )
{
	
	/* Wait for empty transmit buffer */
	while( GET_BIT(UCSRA,5) == 0 );
	
	/* Put data into buffer, sends the data */
	UDR = data;
}


u8 USART_Receive( )
{
	/* Wait for data to be received 				*/
	while( GET_BIT( UCSRA , 7 ) ==0 );
	
	/* clear the flag by writing logical one on it */
	SET_BIT( UCSRA , 7 );

	/* Get and return received data from buffer 	*/
	return UDR;
}