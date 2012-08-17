#include "dcpu.h"
#include "hardware.h"

void init_lem()
{	
	lem = (hardware_device*)malloc( sizeof(hardware_device) );
	lem->id = LEM_ID;
	lem->version = LEM_VERSION;
	lem->manufacturer = LEM_MANUFACTURER;
	lem->call_int = &lem_handle_int;
	lem->reset = &lem_reset;
	add_hardware_device( lem );
	
	lem_old = (hardware_device*)malloc( sizeof(hardware_device) );
	lem_old->id = LEM_ID_OLD;
	lem_old->version = LEM_VERSION_OLD;
	lem_old->manufacturer = LEM_MANUFACTURER;
	lem_old->call_int = &lem_handle_int;
	lem_old->reset = &lem_reset;
	add_hardware_device( lem_old );
}

void lem_reset()
{
	lem_video_starting_addr = 0;
	lem_font_starting_addr = 0;
	lem_palette_starting_addr = 0;
	lem_border_color = 0;
}

void lem_handle_int(udcpu_t *a)
{	
	udcpu_t i, dest;

	switch( register_literals[REGISTER_A] )
	{
		case INT_MEM_MAP_SCREEN:
			printf("MEM_MAP_SCREEN: 0x%X\n", register_literals[REGISTER_B]);
			lem_video_starting_addr = register_literals[REGISTER_B];
			break;
		
		case INT_MEM_MAP_FONT:
			printf("MEM_MAP_FONT: 0x%X\n", register_literals[REGISTER_B]);
			lem_font_starting_addr = register_literals[REGISTER_B];
			break;
		
		case INT_MEM_MAP_PALETTE:
			printf("MEM_MAP_PALETTE: 0x%X\n", register_literals[REGISTER_B]);
			lem_palette_starting_addr = register_literals[REGISTER_B];
			break;
		
		case INT_SET_BORDER_COLOR:
			printf("MEM_MAP_COLOR: 0x%X\n", register_literals[REGISTER_B]);
			lem_border_color = dcpu_mem[ lem_palette_starting_addr + (register_literals[REGISTER_B]&0xF) ];
			break;
			
		case INT_DUMP_FONT:
			printf("MEM_MAP_FONT: 0x%X\n", register_literals[REGISTER_B]);
			dest = register_literals[REGISTER_B];
			
			for(i=0; i < 256; i++){
				dcpu_mem[dest++] = lem_font[i];
			}
			
			break;
			
		case INT_DUMP_PALETTE:
			printf("MEM_MAP_PALETTE: 0x%X\n", register_literals[REGISTER_B]);
			dest = register_literals[REGISTER_B];
			
			for(i=0; i < 16; i++){
				dcpu_mem[dest++] = lem_palette[i];
			}
			
			break;
		
		default:
			printf("lem int not defined: 0x%X\n", register_literals[REGISTER_A] );
			break;
	}
}
