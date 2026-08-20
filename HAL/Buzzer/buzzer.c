/*
 * buzzer.c
 *
 * Created: 8/19/2026 1:59:31 PM
 *  Author: User
 */ 
#include "STD_TYPES.h"
#include "DIO_Interface.h"
#include "buzzer_config.h"
#include "buzzer_inteface.h"

void buzzer_voidInit(void)
{
    DIO_voidSetPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
}

void buzzer_voidOn(void)
{
    DIO_voidSetPinValue(BUZZER_PORT, BUZZER_PIN, PIN_HIGH);
}

void buzzer_voidOff(void)
{
    DIO_voidSetPinValue(BUZZER_PORT, BUZZER_PIN, PIN_LOW);
}