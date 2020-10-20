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
#include "USBSerial.h"
#include "CommandProcessor.h"
#include "EEPROMUtils.h"
#include "inc/hw_memmap.h"

uint8_t CommandCircularBuffer[COMMAND_BUFFER_SIZE];
uint8_t CommandBuffer[COMMAND_BUFFER_SIZE];
uint8_t CommandCount;
uint8_t CommandWritePointer;
uint8_t CommandReadPointer;
uint8_t* PolarityMessage[] = {"positive", "negative"};
uint8_t* OffOnMessage[] = {"off", "on"};
uint8_t* AxisMessage[] = {"X", "Y"};

void GetNextCommand(void)
{
	static uint8_t Character = 0;
	static uint8_t LastCharacter = 0;
	static uint8_t CommandLength = 0;
	uint8_t CommandFound = 0;
	int32_t DataRead;

	while((FoundBM == 0) && (CommandFound == 0))
	{
//		if ((DataAvailable = USBBufferDataAvailable((tUSBBuffer *)&g_sRxBuffer)) == 0)
//			WaitFor(2);
		DataRead = USBBufferRead((tUSBBuffer *)&g_sRxBuffer, &Character, 1);
		if (DataRead != 0)
		{
			if ((Character == 'M') && (LastCharacter == 'B'))
			{
				FoundBM = 1;
				CommandLength = 0;
			}
			else
			{
				//Convert line feed to carriage return
				if (Character == 10)
					Character = 13;
				//If multiple carriage returns, then just ignore
				if ((Character == 13) && (LastCharacter ==13))
					Character = 0;
				//If first character and a space then ignore it
				if ((Character == 32) && (CommandLength == 0))
					Character = 0;
				//If carriage return and something in the buffer then a command has been found
				if ((Character == 13) && (CommandLength != 0))
				{
					//Terminate the command string
					CommandBuffer[CommandLength] = 0;
					//Reset ready for next command
					CommandLength = 0;
					if (CommandBuffer[0] != ';')//';' is a comment. Only valid at line start
						//Flag command found
						CommandFound = 1;
				}
				//If valid character then put in the buffer
				else if (Character != 0)
				{
					CommandBuffer[CommandLength] = toupper(Character);
					CommandLength++;
				}
			}
			LastCharacter = Character;
		}
	}
}
	
void
ProcessHelp(void)
{
//	USB_Serial_SendMessage((unsigned char*)"\n");
}
	
	void 
		ProcessSetSerial(unsigned char * Buffer)
	{
		SerialNumber = strtol ((const char*)Buffer, NULL, 0);
		EEPROMProgram((uint32_t*)&SerialNumber, EEPROMSize - sizeof(SerialNumber), sizeof(SerialNumber));		
	}
	
	void 
		ProcessReset(void)
	{
		EEPROMMassErase();
		USB_Serial_SendMessage((unsigned char*)"EEPROM reset. Power cycle to continue");
	}
	
	void
		ProcessSetDisplayFormat(uint32_t Format)
	{
		DeviceInfo.NLFormat = Format;
		EEPROMProgram((uint32_t *)&DeviceInfo, 0, sizeof(DeviceInfo));
	}
	
	void ComProc_ProcessCommand(void)
		{
			uint8_t State = 0;

			GetNextCommand();
			if (FoundBM == 1)
			{
				//BM header found so process the bitmap
//				ImageProc_PrintBMP();
				//Clear the flag
				FoundBM = 0;

			}
			else
			{
				if (CommandBuffer[0] == '#')
				{
					//Skip comments
				}
				else if      (memcmp((const char*)&CommandBuffer, "HELP", 4) == 0)
				{
					ProcessHelp();
				}
				else if (memcmp((const char*)&CommandBuffer, "STSERIAL ", 9) == 0)
				{
					ProcessSetSerial(&CommandBuffer[9]);
				}
				else if (memcmp((const char*)&CommandBuffer, "ReSeT", 5) == 0)
				{
					ProcessReset();
				}
				else if (memcmp((const char*)&CommandBuffer, "SETUNIX", 5) == 0)
				{
					ProcessSetDisplayFormat(UNIX_FORMAT);
				}
				else if (memcmp((const char*)&CommandBuffer, "SETDOS", 5) == 0)
				{
					ProcessSetDisplayFormat(DOS_FORMAT);
				}
				else if (memcmp((const char*)&CommandBuffer, "SETOLD", 5) == 0)
				{
					ProcessSetDisplayFormat(PRE_OSX_FORMAT);
				}
				else if (memcmp((const char*)&CommandBuffer, "ECHOON", 6) == 0)
				{
					DeviceInfo.EchoEnable = 1;
					EEPROMProgram((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
				}
				else if (memcmp((const char*)&CommandBuffer, "ECHOOFF", 7) == 0)
				{
					DeviceInfo.EchoEnable = 0;
					EEPROMProgram((uint32_t *) &DeviceInfo, 0, sizeof(DeviceInfo));
				}
				else if (CommandBuffer[0] != 0)
				{
//					ShowMessage((unsigned char*)"Unknown command\n");
//					ShowMessage((unsigned char*)CommandBuffer);
//					ShowMessage((unsigned char*)"\n");
					State = 1;
				}

				if (State == 0)
					USB_Serial_SendMessage((unsigned char*)"OK");
				else
					USB_Serial_SendMessage((unsigned char*)"NAK");

		}
}
