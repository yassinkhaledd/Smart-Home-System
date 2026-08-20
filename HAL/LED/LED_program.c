#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "LED_interface.h"

void HAL_LED_voidInit(u8 copy_u8Port, u8 copy_u8Pin)
{
	DIO_voidSetPinDirection(copy_u8Port, copy_u8Pin, PIN_OUTPUT);
}

void HAL_LED_voidOn(u8 copy_u8Port, u8 copy_u8Pin)
{
	DIO_voidSetPinValue(copy_u8Port, copy_u8Pin, PIN_HIGH);
}

void HAL_LED_voidOff(u8 copy_u8Port, u8 copy_u8Pin)
{
	DIO_voidSetPinValue(copy_u8Port, copy_u8Pin, PIN_LOW);
}

void HAL_LED_voidToggle(u8 copy_u8Port, u8 copy_u8Pin)
{
	DIO_voidToggelPin(copy_u8Port, copy_u8Pin);
}