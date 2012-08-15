#include "hardware.h"
#include "keyboard.h"
#include "lem.h"
#include "clock.h"

hardware_device **connected_devices;

void init_hardware()
{
	connected_devices = (hardware_device**)malloc( sizeof(hardware_device*) * MAX_CONECTED_DEVICES );
	no_connected_devices = 0;
	
	init_keyboard();
	init_lem();
	init_clock();
}

void reset_hardware()
{
	udcpu_t i;
	
	for(i=0; i < no_connected_devices; i++){
		connected_devices[i]->reset();
	}
}

void add_hardware_device(hardware_device *dev)
{
	printf("hardware connected~ id: 0x%08X manufacturer: 0x%08X version: 0x%04X\n", dev->id, dev->manufacturer, dev->version);
	connected_devices[no_connected_devices] = dev;
	no_connected_devices++;
}
