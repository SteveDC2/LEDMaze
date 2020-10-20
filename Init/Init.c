#include "main.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "inc/hw_sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/ROM_map.h"
#include "driverlib/eeprom.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "utils/ustdlib.h"
#include "usb_serial_structs.h"
#include "utils/uartstdio.h"
#include "driverlib/ssi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "USBSerial.h"
#include "Init.h"
#include "USBSerial.h"
#include "EEPROMUtils.h"
#include "Helpers.h"
#include "LEDPanel.h"

//*****************************************************************************
//
// Initialization
//
//*****************************************************************************
	
void ConfigureUSB(void)
	{
    //
    // Enable the UART that we will be redirecting.
    //
    ROM_SysCtlPeripheralEnable(USB_UART_PERIPH);

    //
    // Initialize the transmit and receive buffers.
    //
    USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);

    //
    // Set the USB stack mode to Device mode with VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDCDCInit(0, &g_sCDCDevice);

    //
    // Enable interrupts now that the application is ready to start.
    //
    ROM_IntEnable(USB_UART_INT);
	}


void ConfigurePins(void)
	{
    //
    // Configure the required pins for USB operation.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
    //
    // Enable the GPIO ports
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		
//	//Unlock the GPIO inputs which are locked
//	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |=  GPIO_PIN_7;

	//Set the data port
	ROM_GPIOPinTypeGPIOOutput(LEDPANELDATAPORT, LEDDATAMASK | FRAMERATECHECKMASK);
	//Set the control port
	ROM_GPIOPinTypeGPIOOutput(LEDPANELCONTROLPORT, LEDROWSELECTMASK | LEDCLOCKMASK | LEDSTROBEMASK | LEDOEMASK);

/*	while(1)
	{
		GPIOPinWrite(LEDOUTPUTENABLE, SETLOW);
		GPIOPinWrite(LEDOUTPUTENABLE, SETHIGH);
	}
*/
	//Set the default states
	GPIOPinWrite(LEDWRITESTROBE, SETLOW);
	//Re-enable output drivers
	GPIOPinWrite(LEDOUTPUTENABLE, SETHIGH);
	GPIOPinWrite(LEDWRITECLOCK, SETLOW);
}

void Init_SystemInit()
	{
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    ROM_FPULazyStackingEnable();

    // Set the clocking to run from the PLL at 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_INT);
	
    // Enable the system tick.
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();
}
	

void ConfigurePanelTimers(void)
{
	//Initialize timer for main pixel processing
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//Set the initial timer period
	TimerLoadSet(TIMER0_BASE, TIMER_A, LEDTIMERPERIOD);
	//Trigger clear timer interrupt
	TimerEnable(TIMER0_BASE, TIMER_A);

//	//Initialize timer for pulse generation
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
////	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC_UP);
//	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
//	IntEnable(INT_TIMER1A);
//	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

}

void Init_PeripheralInit(void)
	{
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		
    // Configure GPIO pins
		ConfigurePins();

		//Configure USB
		ConfigureUSB();
		
		//Configure timers used for panel driver
		ConfigurePanelTimers();

		//Initialize the EEPROM sub-system
		EEPROM_Initialize();
		
		//Read the serial number from EEPROM
		EEPROMRead((uint32_t*)&SerialNumber, EEPROMSize - sizeof(SerialNumber), sizeof(SerialNumber));		
	}


void Init_ReadEEPROMDefaults()
{
	int State;
	
	//Read the last configuration index used
	EEPROMRead((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
	
	//Read settings from EEPROM and check if valid settings exist at indicated configuration
	State = EEPROM_GetSettings(&CurrentConfiguration, DeviceInfo.CurrentConfigurationIndex);
	//If the settings are not valid then try the default index 0
	if (State == -1)
	{
		DeviceInfo.CurrentConfigurationIndex = 0;
		State = EEPROM_GetSettings(&CurrentConfiguration, DeviceInfo.CurrentConfigurationIndex);
		EEPROMProgram((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
	}
	//If STILL not valid then set the defaults in index 0
	if (State == -1)
	{
		//Default configuration is invalid so re-configure and store
		strncpy((char*)&CurrentConfiguration.ConfigurationName, "Default config", 16);
		
		EEPROM_StoreSettings(&CurrentConfiguration, DeviceInfo.CurrentConfigurationIndex, VALID_KEY);
		EEPROMProgram((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
	}
	if (DeviceInfo.NLFormat >= LAST_FORMAT)
	{
		DeviceInfo.NLFormat = 0;
		DeviceInfo.EchoEnable = 1;
		EEPROMProgram((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
	}
}	
	
void Init_SetDefaults(void)
{
	USBTimeout = 3000;
}
