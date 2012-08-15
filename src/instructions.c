#include "instructions.h"
#include "dcpu.h"
#include "hardware.h"

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
