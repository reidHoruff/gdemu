#include "dcpu.h"
#include "instructions.h"
#include "disassembler.h"
bool if_skip_flag;

udcpu_t cycles_wait;
ldcpu_t code_size;
udcpu_t *interrupt_queue;
udcpu_t interrupt_queue_ptr;

bool interrupt_queueing;
bool queue_int_not_trigger;

void init_dcpu()
{
	/* allocate RAM and ROM and Quqeue*/
	dcpu_mem = (udcpu_t*)malloc( (MEM_SIZE_WORDS) * sizeof(udcpu_t) );
	dcpu_rom = (udcpu_t*)malloc( (MEM_SIZE_WORDS) * sizeof(udcpu_t) );
	interrupt_queue = (udcpu_t*)malloc( sizeof(udcpu_t) * INT_QUEUE_SIZE );
	
	register_literals[VALUE_SP] = STACK_MEM_START_LOC;
}

void reset_dcpu()
{
	uldcpu_t i;
	
	for(i = REGISTER_A; i <= VALUE_IA; i++){
		register_literals[i] = 0;
	}
	
	register_literals[VALUE_SP] = STACK_MEM_START_LOC;
	interrupt_queue_ptr = 0;
	cycles_wait = 0;
	
	for(i = 0; i < MEM_SIZE_WORDS; i++){
		dcpu_mem[i] = dcpu_rom[i];
	}
}

void disp_info()
{
	printf(" A:%04X  B:%04X  C:%04X\n X:%04X  Y:%04X  Z:%04X\n I:%04X  J:%04X PC:%04X\nSP:%04X EX:%04X IA:%04X\n", 
		register_literals[REGISTER_A],
		register_literals[REGISTER_B],
		register_literals[REGISTER_C],
		register_literals[REGISTER_X],
		register_literals[REGISTER_Y],
		register_literals[REGISTER_Z],
		register_literals[REGISTER_I],
		register_literals[REGISTER_J],
		register_literals[VALUE_PC],
		register_literals[VALUE_SP],
		register_literals[VALUE_EX],
		register_literals[VALUE_IA]
	);
}

void process_interrupt()
{
	if(interrupt_queue_ptr){
		udcpu_t msg = interrupt_queue[0];
		
		udcpu_t x;
		for(x=1; x < INT_QUEUE_SIZE; x++){
			interrupt_queue[x-1] = interrupt_queue[x];
		}
		
		interrupt_queue_ptr--;
		interrupt_queue[INT_QUEUE_SIZE-1] = 0;
		
		if(register_literals[VALUE_IA]){
			interrupt_queueing = ENABLED;
			dcpu_mem[ --register_literals[VALUE_SP] ] = register_literals[VALUE_PC];
			dcpu_mem[ --register_literals[VALUE_SP] ] = register_literals[REGISTER_A];
			register_literals[VALUE_PC] = register_literals[VALUE_IA];
			register_literals[REGISTER_A] = msg;
			printf("possible bug\n");
		}
	}
}

void skip_all_branches()
{
	while( IS_BRANCH_OP( INSTRUCTION_OP(dcpu_mem[register_literals[VALUE_PC]])) ){
		register_literals[VALUE_PC] += total_instruction_size( dcpu_mem[register_literals[VALUE_PC]] );
	}
	
	register_literals[VALUE_PC] += total_instruction_size( dcpu_mem[register_literals[VALUE_PC]] );
	
	if_skip_flag = false;
}

udcpu_t *a, al, *b, bl, instr;
//printf("\n0x%X: 0x%X  a: 0x%X, b: 0x%X, o: 0x%X\n",program_counter, instr, INSTRUCTION_A(instr), INSTRUCTION_B(instr), INSTRUCTION_OP(instr));

void dcpu_step()
{
	cycles_wait = 0;
	
	if( if_skip_flag ){
		skip_all_branches();
	}
	
	describe_live_instruction( register_literals[VALUE_PC] );
	
	dcpu_cycle();
}
	
void dcpu_cycle()
{	
	/* last instruction takes more than 1 cycle to complete */
	if(cycles_wait){
		cycles_wait--;
		return;
	}
	
	if( if_skip_flag ){
		skip_all_branches();
		return;
	}
	
	instr = dcpu_mem[register_literals[VALUE_PC]++];
	a = get_value( INSTRUCTION_A(instr), &al, true );
	
	if( !IS_SPECIAL_OP(instr) )
	{
		b = get_value( INSTRUCTION_B(instr), &bl, false );
		
		switch( INSTRUCTION_OP(instr) )
		{
			/* basic op codes */
			case OP_SET: instruction_SET(instr, a, b); break;
			case OP_ADD: instruction_ADD(instr, a, b); break;
			case OP_SUB: instruction_SUB(instr, a, b); break;
			case OP_MUL: instruction_MUL(instr, a, b); break;
			case OP_MLI: instruction_MLI(instr, a, b); break;
			case OP_DIV: instruction_DIV(instr, a, b); break;
			case OP_DVI: instruction_DVI(instr, a, b); break;
			case OP_MOD: instruction_MOD(instr, a, b); break;
			case OP_MDI: instruction_MDI(instr, a, b); break;
			case OP_AND: instruction_AND(instr, a, b); break;
			case OP_BOR: instruction_BOR(instr, a, b); break;
			case OP_XOR: instruction_XOR(instr, a, b); break;
			case OP_SHR: instruction_SHR(instr, a, b); break;
			case OP_ASR: instruction_ASR(instr, a, b); break;
			case OP_SHL: instruction_SHL(instr, a, b); break;
			case OP_ADX: instruction_ADX(instr, a, b); break;
			case OP_SBX: instruction_SBX(instr, a, b); break;
			case OP_STI: instruction_STI(instr, a, b); break;
			case OP_STD: instruction_STD(instr, a, b); break;
			
			/* branching */
			case OP_IFB: instruction_IFB(instr, a, b); break;
			case OP_IFC: instruction_IFC(instr, a, b); break;
			case OP_IFE: instruction_IFE(instr, a, b); break;
			case OP_IFN: instruction_IFN(instr, a, b); break;
			case OP_IFG: instruction_IFG(instr, a, b); break;
			case OP_IFA: instruction_IFA(instr, a, b); break;
			case OP_IFL: instruction_IFL(instr, a, b); break;
			case OP_IFU: instruction_IFU(instr, a, b); break;
			
			default: printf("Undefined Instruction %x @ %x\n", INSTRUCTION_OP(instr), register_literals[VALUE_PC] ); break;
		}
	}
	else /* special ops */
	{
		switch( SPECIAL_OP_OP(instr) )
		{
			case OP_JSR: instruction_JSR(instr, a); break;
			case OP_INT: instruction_INT(instr, a); break;
			case OP_IAG: instruction_IAG(instr, a); break;
			case OP_IAS: instruction_IAS(instr, a); break;
			case OP_RFI: instruction_RFI(instr, a); break;
			case OP_IAQ: instruction_IAQ(instr, a); break;
			case OP_HWN: instruction_HWN(instr, a); break;
			case OP_HWQ: instruction_HWQ(instr, a); break;
			case OP_HWI: instruction_HWI(instr, a); break;
			
			default: printf("Undefined Instruction %x @ %x\n", SPECIAL_OP_OP(instr), register_literals[VALUE_PC] ); break;
		}
	}
	
	process_interrupt();
}

void queue_interrupt(udcpu_t msg)
{
	if(interrupt_queue_ptr < INT_QUEUE_SIZE){
		interrupt_queue[interrupt_queue_ptr] = msg;
		interrupt_queue_ptr++;
	}
	else{
		printf("Interrupt overflow, fire\n");
	}
}

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

void instruction_SET(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	*b = *a;
}

void instruction_ADD(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	ldcpu_t chk = *b + *a;
	*b = *b + *a;
	register_literals[VALUE_EX] = (chk > MAX_DCPU_T) ? OVERFLOW_FLAG : NO_OVERFLOW_FLAG;
}

void instruction_SUB(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	ldcpu_t chk = *b - *a;
	*b = *b - *a;
	register_literals[VALUE_EX] = (chk < MIN_DCPU_T) ? UNDERFLOW_FLAG : NO_OVERFLOW_FLAG;
}

void instruction_MUL(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) = (*b) * (*a);
	register_literals[VALUE_EX] = (((*b)*(*a))>>16) & 0xffff;
}

void instruction_MLI(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) = SIGN(*b) * SIGN(*a);
	register_literals[VALUE_EX] = ((SIGN(*b)*SIGN(*a))>>16) & 0xffff;
}

void instruction_DIV(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	/*div by zero*/
	if((*a)==0){
		*b = 0;
		register_literals[VALUE_EX] = 0;
	}
	else{
		(*b) /= (*a);
		register_literals[VALUE_EX] = (((*b)<<16)/(*a))&0xffff;
	}
}

void instruction_DVI(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	/*div by zero*/
	if((*a)==0){
		*b = 0;
		register_literals[VALUE_EX] = 0;
	}
	else{
		(*b) = SIGN(*b) / SIGN(*a);
		register_literals[VALUE_EX] = ((SIGN(*b)<<16)/SIGN(*a))&0xffff;
	}
}

void instruction_MOD(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	/*mod by zero*/
	if((*a)==0){
		*b = 0;
		register_literals[VALUE_EX] = 0;
	}
	else{
		(*b) %= (*a);
	}
}

void instruction_MDI(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	/*mod by zero*/
	if((*a)==0){
		*b = 0;
		register_literals[VALUE_EX] = 0;
	}
	else{
		(*b) = SIGN(*b) % SIGN(*a);
	}
}

void instruction_SHR(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) >>= (*a);
	register_literals[VALUE_EX] = (((*b)<<16)>>(*a)) & 0xffff;
}

void instruction_ASR(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) = SIGN(*b) >> SIGN(*a);
	register_literals[VALUE_EX] = (((*b)<<16)>>(*a))&0xffff;
}

void instruction_SHL(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) <<= (*a);
	register_literals[VALUE_EX] = (((*b)<<(*a))>>16) & 0xffff;
}

void instruction_AND(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) &= (*a);
}

void instruction_BOR(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) |= (*a);
}

void instruction_XOR(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) ^= (*a);
}

void instruction_IFB(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !(((*b)&(*a)) != 0);
}

void instruction_IFC(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !(((*b)&(*a)) == 0);
}

void instruction_IFE(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !((*b) == (*a) );
	//printf("~ %d\n", if_skip_flag);
}

void instruction_IFN(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !((*b) != (*a));
}

void instruction_IFG(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !((*b) > (*a));
}

void instruction_IFA(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !( SIGN(*b) > SIGN(*a) );
}

void instruction_IFL(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !( (*b) < (*a) );
}

void instruction_IFU(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	if_skip_flag = !(SIGN(*b) < SIGN(*a));
}

void instruction_ADX(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	ldcpu_t tmp = (*b) + (*a) + register_literals[VALUE_EX];
	(*b) = tmp;
	register_literals[VALUE_EX] = (tmp > MAX_DCPU_T) ? OVERFLOW_FLAG : NO_OVERFLOW_FLAG;
}

void instruction_SBX(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	ldcpu_t tmp = (*b) - (*a) + register_literals[VALUE_EX];
	(*b) = tmp;
	register_literals[VALUE_EX] = (tmp < MIN_DCPU_T) ? UNDERFLOW_FLAG : NO_OVERFLOW_FLAG;
}

void instruction_STI(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) = (*a);
	register_literals[REGISTER_I]++;
	register_literals[REGISTER_J]++;
}

void instruction_STD(udcpu_t op, udcpu_t *a, udcpu_t *b)
{
	(*b) = (*a);
	register_literals[REGISTER_I]--;
	register_literals[REGISTER_J]--;
}

udcpu_t total_instruction_size(udcpu_t op)
{
	int size = 1;

	if(HAS_LOOKUP( INSTRUCTION_A(op) ) ){
		size++;
	}

	if(HAS_LOOKUP( INSTRUCTION_B(op) ) ){
		size++;
	}

	return size;
}

/* non basic instructions */
void instruction_JSR(udcpu_t op, udcpu_t *a)
{
	dcpu_mem[ --register_literals[VALUE_SP] ] = register_literals[VALUE_PC];
	register_literals[VALUE_PC] = *a;
}

void instruction_INT(udcpu_t op, udcpu_t *a)
{	
	queue_interrupt( *a );
	ADD_CYCLES(4);
}

void instruction_IAG(udcpu_t op, udcpu_t *a)
{
	*a = register_literals[VALUE_IA];
}

void instruction_IAS(udcpu_t op, udcpu_t *a)
{
	register_literals[VALUE_IA] = *a;
}

void instruction_RFI(udcpu_t op, udcpu_t *a)
{
	interrupt_queueing = DISABLED;
	register_literals[REGISTER_A] = dcpu_mem[ register_literals[VALUE_SP]++ ]; /* pop A */
	register_literals[VALUE_PC] =   dcpu_mem[ register_literals[VALUE_SP]++ ]; /* pop PC */
	ADD_CYCLES(3);
}

void instruction_IAQ(udcpu_t op, udcpu_t *a)
{
	queue_int_not_trigger = *a;
	ADD_CYCLES(2);
}

void instruction_HWN(udcpu_t op, udcpu_t *a)
{
	*a = no_connected_devices;
	ADD_CYCLES(2);
}

void instruction_HWQ(udcpu_t op, udcpu_t *a)
{
	hardware_device *dev = connected_devices[ (*a) ];
	
	register_literals[REGISTER_A] = BOTTOM_WORD(dev->id); 
	register_literals[REGISTER_B] =    TOP_WORD(dev->id);
	
	register_literals[REGISTER_C] = dev->version;
	
	register_literals[REGISTER_X] = BOTTOM_WORD(dev->manufacturer); 
	register_literals[REGISTER_Y] =    TOP_WORD(dev->manufacturer);
	
	ADD_CYCLES(4);
}

void instruction_HWI(udcpu_t op, udcpu_t *a)
{
	connected_devices[ (*a) ]->call_int(a);
	ADD_CYCLES(4);
}
