#ifndef _hardware_h_
#define _hardware_h_

#include "dcpu.h"

#define MAX_CONECTED_DEVICES 25

udcpu_t no_connected_devices;

typedef struct{
	
	uldcpu_t id;
	udcpu_t version;
	uldcpu_t manufacturer;
	
	void (*call_int)(udcpu_t*);
	void (*reset)(void);
	
}hardware_device;

hardware_device **connected_devices;

void init_hardware(void);
void add_hardware_device(hardware_device*);
void reset_hardware(void);

#include "hardware.c"
#endif
