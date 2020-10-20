#ifndef __TIVAMONITOR_H
#define __TIVAMONITOR_H		

#include <stdint.h>
#include "EEPROMUtils.h"

//#define SOFTSPI

//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND 1000
#define SYSTICK_PERIOD_MS (1000 / SYSTICKS_PER_SECOND)

#define SETHIGH        0xff
#define SETLOW         0x00

#define VALID_KEY 0x2869ea83
#define INVALID_KEY 0xffffffff

#define LED_CYCLE_DELAY 30

#define PCB_V1

#ifdef PCB_V1
	//////////////////////////////////////////

	#define YLIMIT_SWITCH_PORT GPIO_PORTC_BASE
	#define YLIMIT_SWITCH_BIT  GPIO_PIN_7
	#define XLIMIT_SWITCH_PORT GPIO_PORTD_BASE
	#define XLIMIT_SWITCH_BIT  GPIO_PIN_6

	#define X_STEP_PORT        GPIO_PORTD_BASE
	#define X_STEP_BIT         GPIO_PIN_1
	#define X_DIR_PORT         GPIO_PORTD_BASE
	#define X_DIR_BIT          GPIO_PIN_0

	#define Y_STEP_PORT        GPIO_PORTB_BASE
	#define Y_STEP_BIT         GPIO_PIN_4
	#define Y_DIR_PORT         GPIO_PORTD_BASE
	#define Y_DIR_BIT          GPIO_PIN_7

	#define STEP_RESET_PORT    GPIO_PORTD_BASE
	#define STEP_RESET_BIT     GPIO_PIN_2

	#define LASER_PORT         GPIO_PORTF_BASE
	#define LASER_BIT          GPIO_PIN_2

	#define PANIC_PORT         GPIO_PORTF_BASE
	#define PANIC_BIT          GPIO_PIN_1
#else
	//////////////////////////////////////////
	#define XLIMIT_SWITCH_PORT GPIO_PORTB_BASE
	#define XLIMIT_SWITCH_BIT  GPIO_PIN_0
	#define YLIMIT_SWITCH_PORT GPIO_PORTB_BASE
	#define YLIMIT_SWITCH_BIT  GPIO_PIN_1

	#define Y_STEP_PORT        GPIO_PORTC_BASE
	#define Y_STEP_BIT         GPIO_PIN_7
	#define Y_DIR_PORT         GPIO_PORTD_BASE
	#define Y_DIR_BIT          GPIO_PIN_7

	#define X_STEP_PORT        GPIO_PORTC_BASE
	#define X_STEP_BIT         GPIO_PIN_6
	#define X_DIR_PORT         GPIO_PORTD_BASE
	#define X_DIR_BIT          GPIO_PIN_6

	#define STEP_RESET_PORT    GPIO_PORTC_BASE
	#define STEP_RESET_BIT     GPIO_PIN_5
#endif
//////////////////////////////////////////

#define MAX_X_MM 190
#define MAX_Y_MM 165

#define X_BELT_PITCH 2
#define X_TEETH_PER_REV 16
#define X_MICROSTEPS 16
//#define X_TICKS_PER_MM 160
#define X_TICKS_PER_MM ((200 * X_MICROSTEPS) / (X_BELT_PITCH * X_TEETH_PER_REV))

//Leadscrew pitch #define Y_THREAD_PITCH 2
//M5 threaded bar pitch
#define Y_THREAD_PITCH .8
#define Y_TICKS_PER_REV 200
#define Y_MICROSTEPS 16
//#define Y_TICKS_PER_MM 1600
#define Y_TICKS_PER_MM ((Y_TICKS_PER_REV * Y_MICROSTEPS) / Y_THREAD_PITCH)

#define STEP_ENABLE_PORT   GPIO_PORTF_BASE
#define STEP_ENABLE_BIT    GPIO_PIN_4
#define STEPPERS_ON        0x00
#define STEPPERS_OFF       0xff

#define ENDSTOP_ERROR1 0x01
#define ENDSTOP_ERROR2 0x02

#define MOVE_X_MOTOR 0
#define MOVE_Y_MOTOR 1
#define MOVE_POSITIVE -1
#define MOVE_NEGATIVE 1

#define CHECK_X_LIMIT 1
#define CHECK_Y_LIMIT 2

//#define BOARDCOLOR 0x82aa
#define BOARDCOLOR 0x0000
//#define COPPERCOLOR 0xebc7
//RGB
#define COPPERCOLOR 0xfc00
//BRG
//#define COPPERCOLOR 0x0c17

extern const unsigned char FWVersion[];
extern const unsigned char BuildDate[];
extern const unsigned char BuildTime[];

extern uint32_t SerialNumber;
extern DeviceInfoType DeviceInfo; 

extern uint8_t OKSendUSB;
extern uint8_t Error;
extern uint8_t LaserEnable;
extern uint16_t YTicksPermm;
extern float YTicksPerInch;
extern uint32_t MaxYTravel;
extern uint32_t MaxYTravelTicks;
extern uint32_t CurrentYPosition;

extern uint16_t XTicksPermm;
extern float XTicksPerInch;
extern uint32_t MaxXTravel;
extern uint32_t MaxXTravelTicks;
extern uint32_t CurrentXPosition;
extern uint16_t ImageDPI;
extern float YPrintResolution;
extern float XOffset;

extern int32_t XMinStepClocks;
extern int32_t XMaxStepClocks;
extern int32_t XNomStepClocks;

extern unsigned char MiscBuffer[];

//Allows up to 8 inches wide at 4064 steps per inch, 1 bit per sample
#define LINE_BUFFER_DEPTH 4096
extern uint8_t LineBuffer[LINE_BUFFER_DEPTH];

#endif
