
#include "GIE_Interface.h"
/*
 * @brief Enables global interrupts.
 *
 * This function sets the global interrupt enable bit (I-bit) in the Status Register (SREG),
 * allowing the microcontroller to respond to enabled peripheral interrupts.
 *
 * @note Should be called after configuring and enabling the specific peripheral interrupt sources.
 */
void GIE_Enable( void )
{

	/* Enable Global Interrupt */
	SET_BIT( SREG , I );
}

/*
 * @brief Disables global interrupts.
 *
 * This function clears the global interrupt enable bit (I-bit) in the Status Register (SREG),
 * preventing the microcontroller from responding to any interrupts.
 *
 * @note Disabling global interrupts does not clear or disable individual peripheral interrupt flags.
 */
void GIE_Disable( void )
{

	/* Disable Global Interrupt */
	CLR_BIT( SREG , I );
}