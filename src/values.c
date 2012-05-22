#include "values.h"
#include "dcpu.h"

udcpu_t* get_value(udcpu_t code, udcpu_t *literal, bool a)
{
	if(code >= REGISTER_A && code <= REGISTER_J){
		return &register_literals[ code ];
	}
	
	if(code >= REGISTER_A_MEM && code <= REGISTER_J_MEM){
		return &dcpu_mem[ register_literals[ (code - REGISTER_A_MEM) ] ];
	}
	
	if(code >= NEXT_WORD_PLUS_REG_A_MEM && code <= NEXT_WORD_PLUS_REG_J_MEM){
		ADD_CYCLES(2);
		return &dcpu_mem[ 
			dcpu_mem[register_literals[VALUE_PC]++] + register_literals[code-NEXT_WORD_PLUS_REG_A_MEM]
		];
	}
	
	if(code == PUSH_POP && a){ //POP
		return &dcpu_mem[ register_literals[VALUE_SP]++ ];
	}
	
	if(code == PUSH_POP && !a){ //PUSH
		return &dcpu_mem[ --register_literals[VALUE_SP] ];
	}
	
	if(code == PEEK){
		return &dcpu_mem[ register_literals[VALUE_SP] ];
	}
	
	if(code == ADDR_SP_NEXT_WORD){
		ADD_CYCLES(2);
		return &dcpu_mem[ register_literals[VALUE_SP] + dcpu_mem[ register_literals[VALUE_PC]++ ] ];
	}
	
	if(code == STACK_POINTER){
		return &register_literals[VALUE_SP];
	}
	
	if(code == PROGRAM_COUNTER){
		return &register_literals[VALUE_PC];
	}
	
	if(code == OVERFLOW){
		return &register_literals[VALUE_EX];
	}
	
	if(code == NEXT_WORD_MEM){
		ADD_CYCLES(2);
		return &dcpu_mem[
			dcpu_mem[ register_literals[VALUE_PC]++ ]
		];
	}
	
	if(code == NEXT_WORD_LITERAL){
		ADD_CYCLES(2);
		return &dcpu_mem[ register_literals[VALUE_PC]++ ];
	}
	
	if(code >= LITERAL_VALUE_MIN && code <= LITERAL_VALUE_MAX){
		*literal = LITERAL_VALUE_OFFSET(code);
		return literal;
	}
	
	printf("VALUE ERROR: get_value(0x%X);\n", code);	
	return NULL;
}
