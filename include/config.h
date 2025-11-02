#ifndef CONFIG_H
#define CONFIG_H

#include <led.h>

// Sequence timing
#define TRIG_DELAY 0

// Pin definitions
#define PIN_UART_RX    2 // Software serial for audio module
#define PIN_UART_TX    3 // Software serial for audio module
#define PIN_230V       4 // Relais, to socket
#define PIN_FOG        5 // Relais, to JST-SM X2
#define PIN_WALL_OPEN  6 // Relais, to internal transponder
#define PIN_WALL_CLOSE 7 // Relais, to internal transponder
#define PIN_MOTOR_STATE_GREEN 8 // JST-SM X3 connector
#define PIN_MOTOR_STATE_RED   9 // JST-SM X3 connector
#define PIN_TRIG      A0 // JST-SM X2 connector
#define PIN_SWITCH_1  15 // UI on controller box
#define PIN_SWITCH_2  16 // UI on controller box
#define PIN_BUTTON    17 // UI on controller box
#define PIN_LED_RED   18 // UI on controller box
#define PIN_LED_GREEN 19 // UI on controller box

// Error codes
extern LED errorLED;
extern LED activeLED;
const bool wallBlocked[]       = {0,0,0};
const bool ackErrorClosing[]   = {0,0,1};
const bool timeoutAckClosing[] = {0,1,0};
const bool ackErrorOpening[]   = {0,1,1};
const bool timeoutAckOpening[] = {1,0,0};


#endif


