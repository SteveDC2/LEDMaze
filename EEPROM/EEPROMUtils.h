#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdint.h>
typedef struct//This structure must be multiples of 4 bytes for correct EEPROM read/write
{
		unsigned char ConfigurationName[16];
		int32_t ValidKey;
}ConfigurationType;
	
typedef struct
{
		int8_t CurrentConfigurationIndex;
		int8_t NLFormat;
		int8_t EchoEnable;
		int8_t Spare2;
} DeviceInfoType;

extern uint32_t MaxConfigurations;
extern ConfigurationType CurrentConfiguration;
extern uint32_t EEPROMSize;
extern uint32_t EEPROMGood;


void EEPROM_Initialize(void);
void EEPROM_StoreSettings(ConfigurationType* Configuration, unsigned char ConfigIndex, uint32_t Key);
int EEPROM_GetSettings(ConfigurationType* Configuration, unsigned char ConfigIndex);

#endif
