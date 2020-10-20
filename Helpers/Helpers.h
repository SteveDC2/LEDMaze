#ifndef __HELPERS_H
#define __HELPERS_H		

#include <stdint.h>

extern volatile uint32_t DelayCounter;
extern volatile uint32_t TimeTick;
extern volatile uint16_t ResetTimeTick;

void WaitFor(unsigned int Delayms);
uint8_t InRegion(uint16_t CheckX, uint16_t CheckY, uint16_t X ,uint16_t Y, uint16_t W, uint16_t H);
uint8_t InRange(uint16_t Target, uint16_t Value, uint16_t Tolerance);
uint8_t VerifyAction(uint8_t *Message);
void ResetTime();

uint32_t SquareRoot(uint32_t V);
uint32_t SquareRootRounded(uint32_t V);
uint32_t fisqrt(uint32_t V);
uint32_t int_sqrt32(uint32_t V);

//Square root options
//7755
//#define SQRT(V) sqrt(V)
//183
//#define SQRT(V) SquareRoot(V)
//187
#define SQRT(V) SquareRootRounded(V)
//124
//#define SQRT(V) fisqrt(V)
//260
//#define SQRT(V) int_sqrt32(V)

#endif
