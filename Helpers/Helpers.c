#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "driverlib/rom.h"
#include "Main.h"
#include "Helpers.h"
#include "math.h"

volatile uint32_t DelayCounter = 0;
volatile uint32_t TimeTick = 0;
volatile uint16_t ResetTimeTick = 0;

void ResetTime()
{
	ResetTimeTick = 1;
	while(ResetTimeTick == 1);
}

void WaitFor(unsigned int Delayms)
{
	DelayCounter = Delayms;
	while(DelayCounter != 0);
}

