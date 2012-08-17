#include "clock.h"
#include "dcpu.h"

uldcpu_t clock_ticks_elapsed;
hardware_device *dcpu_clock;
udcpu_t clock_b_value, clock_interrupt_message;


void init_clock()
{
	dcpu_clock = (hardware_device*)malloc( sizeof(hardware_device) );
	dcpu_clock->id = CLOCK_ID;
	dcpu_clock->manufacturer = CLOCK_MANUFACTURER;
	dcpu_clock->version = CLOCK_VERSION;
	
	dcpu_clock->call_int = &dcpu_clock_handle_int;
	dcpu_clock->reset = &clock_reset;
	add_hardware_device( dcpu_clock );
}

void dcpu_clock_handle_int(udcpu_t *a)
{
	printf("clock int recieved\n");
	
	switch( register_literals[REGISTER_A])
	{
		case 0:
			clock_b_value = register_literals[REGISTER_B];
			clock_ticks_elapsed = 0;
			break;
		
		case 1:
			register_literals[REGISTER_C] = clock_ticks_elapsed;
			break;
			
		case 2:
			clock_interrupt_message = register_literals[REGISTER_B];
			break;
			
		default:
			printf("undefined int to clock 0x%x\n", register_literals[REGISTER_A] );
			break;
	}
	
	//printf("Clock int recieved\n");
}

/* should be called every 100th second */
void clock_tick()
{
	static udcpu_t clicks = 0;
	/* if interrupts enabled and clock is on */
	if(clock_interrupt_message && clock_b_value){
		
		clicks++;
		
		if(clicks >= (clock_b_value/500.0)){
			clock_ticks_elapsed++;
			queue_interrupt(clock_interrupt_message);
			clicks = 0;
		}
	}
}

void clock_trigger_int()
{
	if(clock_interrupt_message && clock_b_value){
		printf("clock interrupt\n");
		queue_interrupt(clock_interrupt_message);
	}
}

void clock_reset()
{
	clock_interrupt_message = 0;
	clock_b_value = 0;
}
