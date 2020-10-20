//Use command 'copy Test.bmp \\.\COM16' to print
#include "main.h"
#include "math.h"
#include "Init.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
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
#include "driverlib/rom_map.h"
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
#include "Helpers.h"
#include "Bitmaps.h"
#include "USBSerial.h"
#include "CommandProcessor.h"
#include "EEPROMUtils.h"
#include "LEDPanel.h"
#include "inc/hw_memmap.h"

uint8_t FrameBuffer[LEDROWCOUNT * LEDPIXELSPERSEGMENT * LEDREGIONCOUNT];

uint8_t LineBuffer[LINE_BUFFER_DEPTH];

const unsigned char FWVersion[] = {"FW version 1.00"};
const unsigned char BuildDate[] = "Build date = " __DATE__;
const unsigned char BuildTime[] = "Build Time = " __TIME__;

uint32_t SerialNumber = 0;
DeviceInfoType DeviceInfo = {.CurrentConfigurationIndex = 0, .NLFormat = DOS_FORMAT, .EchoEnable = 1};

uint8_t OKSendUSB = 1;//Assume OK to send to terminal initially

//*****************************************************************************
//
// Interrupt handler for the system tick counter.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
	if (DelayCounter > 0)
		DelayCounter--;
	if (ResetTimeTick != 0)
	{
		TimeTick = 0;
		ResetTimeTick = 0;
	}
	else
	{
		TimeTick++;
	}
}

uint8_t GetPanelData(Pixel, Row, Plane)
{

	if (Plane < Row)
		return LEDREDCHANNEL | (LEDREDCHANNEL << 4);//Upper region = red, lower region = red
	else
		return 0;
}

/////////////////////////////////////////////////
// Interrupt handler for panel drive
/////////////////////////////////////////////////
void PanelInterrupt(void)
{
	//Initialize to last pixel so first time in we are at the beginning
	static uint8_t PlaneCount = LEDPLANECOUNT - 1;
	static uint8_t RowSelect = LEDROWCOUNT - 1;
	static uint16_t Pixel = LEDPIXELSPERSEGMENT - 1;
	uint8_t Data;

//	GPIOPinWrite(FRAMERATECHECK, SETHIGH);

	//Clear the interrupt flag
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	Pixel++;
	if (Pixel == LEDPIXELSPERSEGMENT)
	{
		//Finished a row so latch it
		//Strobe the shift register into the output latches
		GPIOPinWrite(LEDOUTPUTENABLE, SETHIGH);
		GPIOPinWrite(LEDWRITESTROBE, SETHIGH);
		//Set row drivers
		GPIOPinWrite(LEDROWADDRESS, RowSelect);
		GPIOPinWrite(LEDWRITESTROBE, SETLOW);
		GPIOPinWrite(LEDOUTPUTENABLE, SETLOW);
		Pixel = 0;
		RowSelect++;
		if (RowSelect == LEDROWCOUNT)
		{
			RowSelect = 0;
			PlaneCount++;
			if (PlaneCount == LEDPLANECOUNT)
			{
				PlaneCount = 0;
				GPIOPinWrite(FRAMERATECHECK, SETHIGH);
				GPIOPinWrite(FRAMERATECHECK, SETLOW);
			}
		}
	}
	Data = GetPanelData(Pixel, RowSelect, PlaneCount);
	GPIOPinWrite(LEDWRITEDATA, Data);
	GPIOPinWrite(LEDWRITECLOCK, SETHIGH);
	GPIOPinWrite(LEDWRITECLOCK, SETLOW);

//	GPIOPinWrite(FRAMERATECHECK, SETLOW);

}

void delayUs(uint32_t ui32Us)
{
	SysCtlDelay(ui32Us * (SysCtlClockGet() / 3 / 1000000));
}

/*
void SendFrameBuffer()
{
	uint8_t RowSelect;
	uint16_t Pixel;
	uint8_t Data;

	for (RowSelect = 0; RowSelect < LEDROWCOUNT; RowSelect++)
	{
		for (Pixel = 0; Pixel < LEDPIXELSPERSEGMENT; Pixel++)
		{
			Data = ImagePlaneBuffer[Pixel + (Segment * PixelsPerSegment) + (RowSelect * SegmentCount * PixelsPerSegment)]
//			Data = LEDREDCHANNEL | (LEDREDCHANNEL << 4);//Upper region = red, lower region = red
			GPIOPinWrite(LEDWRITEDATA, Data);
			GPIOPinWrite(LEDWRITECLOCK, SETHIGH);
			GPIOPinWrite(LEDWRITECLOCK, SETLOW);
		}
		//Disable output drivers
		GPIOPinWrite(LEDOUTPUTENABLE, SETHIGH);
		//Set row drivers
		GPIOPinWrite(LEDROWADDRESS, RowSelect);
		//Strobe the shift register into the output latches
		GPIOPinWrite(LEDWRITESTROBE, SETHIGH);
		GPIOPinWrite(LEDWRITESTROBE, SETLOW);
		//Re-enable output drivers
		GPIOPinWrite(LEDOUTPUTENABLE, SETLOW);
		//Pause for PoV and to effectively create 120fps (1M uS / 120 / pixel count)
		delayUs((1000000.0/120) / (LEDROWCOUNT * LEDPIXELSPERSEGMENT * LEDREGIONCOUNT));
	}
}
*/

//*****************************************************************************
//
// This is the main application entry function.
//
//*****************************************************************************
int
main(void)
{
	//Initialize stack and clocks

	Init_SystemInit();

	Init_PeripheralInit();

	Init_SetDefaults();

	while(1)
	{
		//SendFrameBuffer();
		if (CommandCount != 0)
			ComProc_ProcessCommand();
	}
}
