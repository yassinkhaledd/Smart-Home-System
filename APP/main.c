/*
 * NTIgraduation.c
 *
 * Created: 8/20/2026 1:15:48 AM
 * Author : yassi
 */ 
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

/* MCAL Drivers */
#include "DIO_Interface.h"
#include "ADC_Interface.h"
#include "EXT_Interface.h"
#include "GIE_Interface.h"
#include "UART_Interface.h"
#include "TIMER0.h"

/* HAL Drivers */
#include "LCD_Interface.h"
#include "LED_Interface.h"
#include "buzzer_inteface.h"

/* ==================== PIN DEFINITIONS ==================== */
/* Outputs (PORTB) */
#define LAMP_PORT           PORTB_ID
#define LAMP_PIN            PIN0
#define LED_GRN_PORT        PORTB_ID
#define LED_GRN_PIN         PIN1
#define LED_RED_PORT        PORTB_ID
#define LED_RED_PIN         PIN2
#define FAN_PIN             PIN3

/* Inputs (PORTA) */
#define LM35_CHANNEL        ADC_CHANNEL0
#define LDR_CHANNEL         ADC_CHANNEL1
#define SW_MANUAL_PIN       PIN2
#define SW_BT_PIN           PIN3
#define SW_LDR_PIN          PIN4
#define BTN_MANUAL_PIN      PIN5
#define RAIN_PIN            PIN6

/* Interrupt (PORTD) */
#define FIRE_INT_PIN        PIN2

#define LDR_THRESHOLD       500

/* ==================== GLOBAL VARIABLES ==================== */
volatile u8 alarm_flag = 0;
u8 lamp_state = 0;

/* ==================== ISR & ADC HELPERS ==================== */
void FireAlarm_ISR(void)
{
	alarm_flag = 1;
}

u16 ADC_Read(u8 ch)
{
	ADC_CHANNEL_SELECT(ch);
	ADC_START_CONVERSION();
	return ADC_Read_value();
}

u8 Get_Temp(void)
{
	u16 adc = ADC_Read(LM35_CHANNEL);
	return (u8)(((u32)adc * 500UL) / 1024UL);
}

/* ==================== MAIN ==================== */
int main(void)
{
	/* Variables */
	u8  temp       = 0;
	u16 ldr_val    = 0;
	u8  bt_cmd     = 0;
	u8  rain_val   = 0;
	u8  mode       = 0;

	/* Display & Button Trackers */
	static u8 prev_temp    = 0xFF;
	static u8 prev_mode    = 0xFF;
	static u8 prev_lamp    = 0xFF;
	static u8 prev_rain    = 0xFF;
	static u8 alarm_shown  = 0;
	static u8 prev_btn     = 0;

	/* 1. Init Peripherals */
	ADC_INIT();
	LCD_voidInit();
	USART_Init(9600);
	TIMER0_Init();

	/* Fast PWM on OC0 (PB3), Prescaler = 256 */
	TCCR0 = 0x6C;
	TIMER0_SetCompareValue(0);

	/* Disable Unused Interrupts */
	TIMSK &= ~((1 << TOIE0) | (1 << OCIE0));
	UCSRB &= ~(1 << RXCIE);
	ADCSRA &= ~(1 << ADIE);

	/* 2. Pin Directions using HAL Drivers */
	HAL_LED_voidInit(LAMP_PORT, LAMP_PIN);
	HAL_LED_voidInit(LED_GRN_PORT, LED_GRN_PIN);
	HAL_LED_voidInit(LED_RED_PORT, LED_RED_PIN);
	buzzer_voidInit();
	DIO_voidSetPinDirection(PORTB_ID, FAN_PIN, PIN_OUTPUT);
    DIO_voidSetPortDirection(PORTA_ID, ALL_PORT_INPUT); 
    DIO_voidSetPinDirection(PORTD_ID, FIRE_INT_PIN, PIN_INPUT);

	/* 3. External Interrupt */
	EXT_voidSetCallBack(FireAlarm_ISR, EXT0);
	EXT_voidInit(EXT0, RISING_EDGE);
	GIE_Enable();

	/* Initial State */
	HAL_LED_voidOff(LAMP_PORT, LAMP_PIN);
	lamp_state = 0;
	HAL_LED_voidOn(LED_GRN_PORT, LED_GRN_PIN);
	HAL_LED_voidOff(LED_RED_PORT, LED_RED_PIN);
	buzzer_voidOff();

	/* 4. Main Loop */
	while (1)
	{
		/* ==================== 1. FIRE / GAS ALARM ==================== */
		if (alarm_flag == 1)
		{
			if (DIO_voidGetBitValue(PORTD_ID, FIRE_INT_PIN) == PIN_HIGH)
			{
				HAL_LED_voidOff(LED_GRN_PORT, LED_GRN_PIN);
				HAL_LED_voidOn(LED_RED_PORT, LED_RED_PIN);
				buzzer_voidOn();

				TIMER0_SetCompareValue(0);
				HAL_LED_voidOff(LAMP_PORT, LAMP_PIN);
				lamp_state = 0;

				if (alarm_shown == 0)
				{
					HLCD_voidClearLCD();
					Lcd_Goto_Row_Column(0, 0);
					Lcd_DisplayStr((u8*)"* EMERGENCY!  * ");
					Lcd_Goto_Row_Column(1, 0);
					Lcd_DisplayStr((u8*)"  ALARM TRIGGERED!  ");
					Lcd_Goto_Row_Column(2, 0);
					Lcd_DisplayStr((u8*)"FIRE / GAS DETECTED ");
					Lcd_Goto_Row_Column(3, 0);
					Lcd_DisplayStr((u8*)"EVACUATE THE AREA!  ");
					alarm_shown = 1;
				}

				_delay_ms(50);
				continue;
			}
			else
			{
				alarm_flag  = 0;
				alarm_shown = 0;

				HAL_LED_voidOn(LED_GRN_PORT, LED_GRN_PIN);
				HAL_LED_voidOff(LED_RED_PORT, LED_RED_PIN);
				buzzer_voidOff();

				HLCD_voidClearLCD();
				prev_temp = 0xFF;
				prev_mode = 0xFF;
				prev_rain = 0xFF;
			}
		}

		/* ==================== 2. FAN SPEED (PWM) ==================== */
		temp = Get_Temp();

		if (temp < 20)          TIMER0_SetCompareValue(0);       /* OFF */
		else if (temp < 30)     TIMER0_SetCompareValue(102);     /* 40% */
		else if (temp < 40)     TIMER0_SetCompareValue(179);     /* 70% */
		else                    TIMER0_SetCompareValue(255);     /* 100% */

		/* ==================== 3. LIGHT MODES ==================== */
		ldr_val = ADC_Read(LDR_CHANNEL);

		/* Mode 1: Manual */
		if (DIO_voidGetBitValue(PORTA_ID, SW_MANUAL_PIN) == PIN_HIGH)
		{
			mode = 1;
			u8 btn = DIO_voidGetBitValue(PORTA_ID, BTN_MANUAL_PIN);

			if (btn == PIN_HIGH && prev_btn == 0)
			{
				_delay_ms(20);
				if (DIO_voidGetBitValue(PORTA_ID, BTN_MANUAL_PIN) == PIN_HIGH)
				{
					HAL_LED_voidToggle(LAMP_PORT, LAMP_PIN);
					lamp_state ^= 1;
				}
				prev_btn = 1;
			}
			else if (btn == PIN_LOW)
			{
				prev_btn = 0;
			}
		}
		/* Mode 2: Bluetooth */
		else if (DIO_voidGetBitValue(PORTA_ID, SW_BT_PIN) == PIN_HIGH)
		{
			mode = 2;
			if (GET_BIT(UCSRA, 7) == 1)
			{
				bt_cmd = USART_Receive();
				if (bt_cmd == '1' || bt_cmd == 'O' || bt_cmd == 'o')
				{
					HAL_LED_voidOn(LAMP_PORT, LAMP_PIN);
					lamp_state = 1;
				}
				else if (bt_cmd == '0' || bt_cmd == 'F' || bt_cmd == 'f')
				{
					HAL_LED_voidOff(LAMP_PORT, LAMP_PIN);
					lamp_state = 0;
				}
			}
		}
		/* Mode 3: Torch LDR */
		else if (DIO_voidGetBitValue(PORTA_ID, SW_LDR_PIN) == PIN_HIGH)
		{
			mode = 3;
			if (ldr_val > LDR_THRESHOLD)
			{
				HAL_LED_voidOff(LAMP_PORT, LAMP_PIN);
				lamp_state = 0;
			}
			else
			{
				HAL_LED_voidOn(LAMP_PORT, LAMP_PIN);
				lamp_state = 1;
			}
		}
		/* Mode 0: Idle */
		else
		{
			mode = 0;
			HAL_LED_voidOff(LAMP_PORT, LAMP_PIN);
			lamp_state = 0;
		}

		/* ==================== 4. RAIN & LCD DISPLAY ==================== */
		rain_val = DIO_voidGetBitValue(PORTA_ID, RAIN_PIN);

		if (temp != prev_temp || mode != prev_mode || lamp_state != prev_lamp || rain_val != prev_rain)
		{
			Lcd_Goto_Row_Column(0, 0);
			Lcd_DisplayStr((u8*)"T:");
			Lcd_Displaynum(temp);
			Lcd_DisplayStr((u8*)"C ");

			Lcd_Goto_Row_Column(0, 8);
			if (temp < 20)          Lcd_DisplayStr((u8*)"Fan: OFF");
			else if (temp < 30)     Lcd_DisplayStr((u8*)"Fan: 40%");
			else if (temp < 40)     Lcd_DisplayStr((u8*)"Fan: 70%");
			else                    Lcd_DisplayStr((u8*)"Fan:100%");

			Lcd_Goto_Row_Column(1, 0);
			if (mode == 1)      Lcd_DisplayStr((u8*)"Mode: Manual SW ");
			else if (mode == 2) Lcd_DisplayStr((u8*)"Mode: BT Control");
			else if (mode == 3) Lcd_DisplayStr((u8*)"Mode: Torch LDR ");
			else                Lcd_DisplayStr((u8*)"Mode: Standby   ");

			Lcd_Goto_Row_Column(2, 0);
			if (lamp_state == 1) Lcd_DisplayStr((u8*)"L:ON  ");
			else                 Lcd_DisplayStr((u8*)"L:OFF ");

			Lcd_Goto_Row_Column(2, 7);
			Lcd_DisplayStr((u8*)"LDR:");
			Lcd_Displaynum(ldr_val);
			Lcd_DisplayStr((u8*)"   ");

			Lcd_Goto_Row_Column(3, 0);
			if (rain_val == PIN_HIGH)
			{
				Lcd_DisplayStr((u8*)"Rain: Raining!  ");
			}
			else
			{
				Lcd_DisplayStr((u8*)"Weather: Clear  ");
			}

			prev_temp = temp;
			prev_mode = mode;
			prev_lamp = lamp_state;
			prev_rain = rain_val;
		}

		_delay_ms(15);
	}

	return 0;
}