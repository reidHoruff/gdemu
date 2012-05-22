#include "disassembler.h"
#include "dcpu.h"

udcpu_t d_pc;
udcpu_t tree_depth = 0;

void init_disassembler()
{
	/*TODO: use defines instead of literal OP values */
	reg_inst_names[0x1] = "SET";
	reg_inst_names[0x2] = "ADD";
	reg_inst_names[0x3] = "SUB";
	reg_inst_names[0x4] = "MUL";
	reg_inst_names[0x5] = "MLI";
	reg_inst_names[0x6] = "DIV";
	reg_inst_names[0x7] = "DVI";
	reg_inst_names[0x8] = "MOD";
	reg_inst_names[0x9] = "MDI";
	reg_inst_names[0xA] = "AND";
	reg_inst_names[0xB] = "BOR";
	reg_inst_names[0xC] = "XOR";
	reg_inst_names[0xD] = "SHR";
	reg_inst_names[0xE] = "ASR";
	reg_inst_names[0xF] = "SHL";

	reg_inst_names[0x10] = "IFB";
	reg_inst_names[0x11] = "IFC";
	reg_inst_names[0x12] = "IFE";
	reg_inst_names[0x13] = "IFN";
	reg_inst_names[0x14] = "IFG";
	reg_inst_names[0x15] = "IFA";
	reg_inst_names[0x16] = "IFL";
	reg_inst_names[0x17] = "IFU";

	reg_inst_names[0x1A] = "ADX";
	reg_inst_names[0x1B] = "SBX";
	reg_inst_names[0x1E] = "STI";
	reg_inst_names[0x1F] = "STD";
	
	
	/* special */
	spec_inst_names[OP_JSR] = "JSR";
	spec_inst_names[OP_INT] = "INT";
	spec_inst_names[OP_IAG] = "IAG";
	spec_inst_names[OP_IAS] = "IAS";
	spec_inst_names[OP_RFI] = "RFI";
	spec_inst_names[OP_IAQ] = "IAQ";
	spec_inst_names[OP_HWN] = "HWN";
	spec_inst_names[OP_HWQ] = "HWQ";
	spec_inst_names[OP_HWI] = "HWI";
}

void describe_live_instruction(udcpu_t addr)
{
	udcpu_t *ptr = &dcpu_mem[addr];
	printf("%04x: ", addr);
	desc_inst( &ptr );
}

void put_tab(udcpu_t tabs)
{
	while(tabs-->0) printf("\t"); 
}

void desc_inst(udcpu_t **addr)
{
	char output[50], a_ret[30], b_ret[30];
	udcpu_t instr = **addr;
	
	if(!IS_SPECIAL_OP(instr))
	{	
		if( reg_inst_names[INSTRUCTION_OP(instr)] == NULL ){
			printf("DAT 0x%X\n", instr);
			tree_depth = 0;
		}
		else{
			if( IS_BRANCH_OP( INSTRUCTION_OP(instr) )){
				tree_depth++;
			}
			else{
				tree_depth = 0;
			}
			
			desc_value( INSTRUCTION_A(instr), addr, true, a_ret );
			desc_value( INSTRUCTION_B(instr), addr, false, b_ret );
			printf("%s %s, %s\n", reg_inst_names[INSTRUCTION_OP(instr)], b_ret, a_ret );
		}
	}
	else
	{
		if( spec_inst_names[SPECIAL_OP_OP(instr)] == NULL ){
			printf("DAT 0x%X\n", instr);
		}
		else{
			desc_value( SPECIAL_OP_A(instr), addr, true, a_ret );
			printf("%s %s\n",  spec_inst_names[SPECIAL_OP_OP(instr)], a_ret );
		}
		
		tree_depth = 0;
	}
}

void dump_all()
{
	udcpu_t *mem_ptr = &dcpu_mem[0];
	udcpu_t *mem_start = mem_ptr;
	
	int x = 1200;
	while(x --> 0)
	{
		//printf("%04X:", (udcpu_t)(mem_ptr - mem_start));
		put_tab(tree_depth);
		desc_inst( &mem_ptr );
		mem_ptr++;
	}	
}

void desc_value(udcpu_t code, udcpu_t **instr, bool a, char *str)
{	
	if(code >= REGISTER_A && code <= REGISTER_J){
		sprintf(str, "%c", reg_names[code]);
	}
	
	else if(code >= REGISTER_A_MEM && code <= REGISTER_J_MEM){
		sprintf(str, "[%c]", reg_names[code-REGISTER_A_MEM]);
	}
	
	else if(code >= NEXT_WORD_PLUS_REG_A_MEM && code <= NEXT_WORD_PLUS_REG_J_MEM){
		(*instr)++;
		sprintf(str, "[%c+0x%x]", reg_names[code-NEXT_WORD_PLUS_REG_A_MEM], **instr );
	}
	
	else if(code == PUSH_POP && a){ //POP
		sprintf(str, "POP");
	}
	
	else if(code == PUSH_POP && !a){ //PUSH
		sprintf(str, "PUSH");
	}
	
	else if(code == PEEK){
		sprintf(str, "PEEK");
	}
	
	else if(code == ADDR_SP_NEXT_WORD){
		(*instr)++;
		sprintf(str, "[SP+0x%X]", **instr);
	}
	
	else if(code == STACK_POINTER){
		sprintf(str, "SP");
	}
	
	else if(code == PROGRAM_COUNTER){
		sprintf(str, "PC");
	}
	
	else if(code == OVERFLOW){
		sprintf(str, "EX");
	}
	
	else if(code == NEXT_WORD_MEM){
		(*instr)++;
		sprintf(str, "[0x%X]", **instr);
	}
	
	else if(code == NEXT_WORD_LITERAL){
		(*instr)++;
		sprintf(str, "0x%X", **instr);
	}
	
	else if(code >= LITERAL_VALUE_MIN && code <= LITERAL_VALUE_MAX){
		sprintf(str, "0x%X", LITERAL_VALUE_OFFSET(code) );
	}
	
	else{
		sprintf(str, "{?}", code);
	}
}
