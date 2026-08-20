#ifndef LED_INTERFACE_H_
#define LED_INTERFACE_H_

#include "STD_TYPES.h"

void HAL_LED_voidInit(u8 copy_u8Port, u8 copy_u8Pin);
void HAL_LED_voidOn(u8 copy_u8Port, u8 copy_u8Pin);
void HAL_LED_voidOff(u8 copy_u8Port, u8 copy_u8Pin);
void HAL_LED_voidToggle(u8 copy_u8Port, u8 copy_u8Pin);

#endif