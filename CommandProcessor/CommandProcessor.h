#ifndef __COMPROC_H
#define __COMPROC_H

#include <stdint.h>

extern uint8_t CommandCircularBuffer[];
extern uint8_t CommandBuffer[];
extern uint8_t CommandCount;
extern uint8_t CommandWritePointer;
extern uint8_t CommandReadPointer;

extern void ComProc_ProcessCommand(void);
extern void ComProc_ProcessStat(void);

#endif
