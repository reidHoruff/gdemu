#ifndef _clock_h_
#define _clock_h_

#include "hardware.h"
#include "dcpu.h"

#define CLOCK_ID           0x12d0b402
#define CLOCK_VERSION      0x1
#define CLOCK_MANUFACTURER 0x0

hardware_device *dcpu_clock;
void init_clock(void);
void dcpu_clock_handle_int(udcpu_t*);
void clock_ticks(void);
void clock_reset(void);
void clock_trigger_int(void);

udcpu_t clock_b_value, clock_interrupt_message;
uldcpu_t clock_ticks_elapsed;

#include "clock.c"
#endif
