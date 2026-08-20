#  ATmega32 Smart Home Automation System

An embedded smart home control and monitoring system developed for the **AVR ATmega32** microcontroller using a clean layered architecture (**MCAL, HAL, APP, LIB**).

---

##  Key Features

* ** Emergency Fire & Gas Alert:**
  * Instant safety shutdown triggered via External Interrupt (**EXT0**).
  * Activates the emergency buzzer, switches the indicator LED from Green to Red, and displays an alert on the LCD while isolating loads.

* ** Smart Temperature & Fan Speed Control:**
  * Reads ambient temperature using the **LM35** sensor via **ADC**.
  * Dynamically controls cooling fan speed using **Fast PWM (Timer0)** ($0\% \rightarrow 40\% \rightarrow 70\% \rightarrow 100\%$).

* ** Multi-Mode Lighting Automation:**
  * **Mode 1 (Manual):** Toggle control using a push button with software debouncing.
  * **Mode 2 (Bluetooth):** Wireless remote control via **USART** (HC-05 module).
  * **Mode 3 (Torch / LDR):** Automatic day/night light control based on light intensity (**LDR + ADC**).
  * **Mode 0 (Standby):** Automatic power-saving shutdown.

* ** Weather & Rain Sensing:**
  * Real-time rain sensor integration to monitor weather conditions.

* ** System Dashboard (LCD):**
  * Live status display for temperature, fan speed, lighting mode, lamp state, LDR readings, and rain detection.

---

##  Software Architecture

* **APP Layer:** Main application logic, safety checks, and lighting modes.
* **HAL Layer:** Drivers for LCD, LEDs, and Buzzer.
* **MCAL Layer:** ATmega32 hardware drivers (DIO, ADC, Timer0 PWM, EXT_INT, UART, GIE).
* **LIB Layer:** Common macros (`BIT_MATH.h`) and standard definitions (`STD_TYPES.h`).

---

##  Hardware Requirements

* **Microcontroller:** ATmega32 (@ 8MHz)
* **Display:** 16x4 / 20x4 Character LCD
* **Sensors:** LM35 (Temperature), LDR (Light), Rain Sensor, Flame/Gas Sensor
* **Actuators:** DC Fan (PWM), Relay / Lamp, Buzzer, Indicator LEDs
* **Communication:** HC-05 Bluetooth Module
