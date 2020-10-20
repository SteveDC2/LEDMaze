#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"
#include "main.h"
#include "Helpers.h"
#include "EEPROMUtils.h"

uint32_t MaxConfigurations;
ConfigurationType CurrentConfiguration;
uint32_t EEPROMSize;
uint32_t EEPROMGood = 0;

void EEPROM_Initialize(void)
	{
		uint32_t ErrorMessage;
		SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
		// Check if EEPROM Does not have a error code.
		EEPROMGood = 1;
		ErrorMessage = EEPROMInit();
		if(ErrorMessage != EEPROM_INIT_OK)
		{
			EEPROMGood = 0;
//			while(1);
		}
		EEPROMSize = EEPROMSizeGet();
		MaxConfigurations = (EEPROMSize / (sizeof (ConfigurationType))) - 2;
	}

/////////////////////////////////////////////////////////////////////////
//                    Store settings to EEPROM
/////////////////////////////////////////////////////////////////////////
	
void EEPROM_StoreSettings(ConfigurationType* Configuration, unsigned char ConfigIndex, uint32_t Key)
{
	int16_t Offset;

	if (ConfigIndex <= MaxConfigurations)
	{
		Configuration->ValidKey = Key;
		Offset = ConfigIndex * (sizeof(ConfigurationType)) + sizeof (DeviceInfo);//Move past device configuration info
		EEPROMProgram((uint32_t*)Configuration, Offset, sizeof(ConfigurationType));
	}
}

/////////////////////////////////////////////////////////////////////////
//                     Check EEPROM and read default setting
/////////////////////////////////////////////////////////////////////////
int EEPROM_GetSettings(ConfigurationType* Configuration, unsigned char ConfigIndex)
{
	int16_t Offset = 0;
	ConfigurationType EEPROMCheck;
	
	if (ConfigIndex <= MaxConfigurations)
	{
		Offset = ConfigIndex * (sizeof(ConfigurationType)) + sizeof (DeviceInfo);//Move past device configuration info
		
		EEPROMRead((uint32_t*)&EEPROMCheck, Offset, sizeof(ConfigurationType));
		
		if (EEPROMCheck.ValidKey == 0x2869ea83)//If the key was valie then read then configuration
		{
			EEPROMRead((uint32_t*)Configuration, Offset, sizeof(ConfigurationType));
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
			return -1;
	}
}
