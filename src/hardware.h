#ifndef _hardware_h_
#define _hardware_h_

#include "dcpu.h"

#define MAX_CONECTED_DEVICES 25

extern udcpu_t no_connected_devices;

typedef struct{
	
	uldcpu_t id;
	udcpu_t version;
	uldcpu_t manufacturer;
	
	void (*call_int)(udcpu_t*);
	void (*reset)(void);
	
}hardware_device;

void init_hardware(void);
void add_hardware_device(hardware_device*);
void reset_hardware(void);

#endif
