#include <stdio.h>
#include <stdlib.h>

#include "dcpu.h"
#include "instructions.h"
#include "values.h"
#include "disassembler.h"
#include "hardware.h"
#include "keyboard.h"
#include "gui.h"

/* could be written a lot better */
int open_file(const char *f_name)
{
	FILE *file = fopen(f_name, "r");
	
	if(!file){
		return 0;
	}
	
	/* determine size of input file */
	fseek(file, 0, SEEK_END);
	code_size = ftell(file)/2;
	fseek(file, 0, SEEK_SET);
	
	int index = 0;
	while( code_size --> 0 ){
		dcpu_mem[index] = dcpu_rom[index] = fgetc(file);
		
		dcpu_mem[index] = dcpu_rom[index] <<= 8;
		
		dcpu_mem[index] = dcpu_rom[index] |= fgetc(file);
		index++;
	}
	
	code_size = index;
	fclose(file);
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
