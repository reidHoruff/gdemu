#include <stdio.h>
#include <stdlib.h>

#include "dcpu.h"
#include "disassembler.h"
#include "hardware.h"
#include "gui.h"

/* could be written a lot better */
int open_file(const char *f_name)
{
	FILE *file = fopen(f_name, "r");
	
	if(!file){
		return 0;
	}
	
	dcpu_read_file( file );
	
	return 1;
}

int main(int argc, char **argv)
{
	printf("\nDCPU-16 emulator (CPU spec 1.7) - Reid Horuff\n");
	
	init_dcpu();
	init_hardware();
	init_disassembler();
	
	if(argc > 1)
	{
		if(!open_file( argv[1] )){
			printf("Error opening file\n");
		}
		else if( argc > 2 && !strcmp(argv[2], "-d")){
			init_disassembler();
			dump_all();
			exit(0);
		}
	}	
	
	init_gui(argc, argv);
	return 0;
}
