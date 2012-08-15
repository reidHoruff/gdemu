#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "dcpu.h"

/*basic instructions */

void instruction_SET(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to a */

void instruction_ADD(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b+a, sets EX to 0x0001 if there's an overflow, 0x0 otherwise */

void instruction_SUB(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b-a, sets EX to 0xffff if there's an underflow, 0x0 otherwise */

void instruction_MUL(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b*a, sets EX to ((b*a)>>16)&0xffff (treats b, a as unsigned) */

void instruction_MLI(udcpu_t, udcpu_t*, udcpu_t*);
/* like MUL, but treat b, a as signed */

void instruction_DIV(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b/a, sets EX to ((b<<16)/a)&0xffff. if a==0, sets b and EX to 0 instead. (treats b, a as unsigned) */

void instruction_DVI(udcpu_t, udcpu_t*, udcpu_t*);
/* like DIV, but treat b, a as signed. Rounds towards 0 */

void instruction_MOD(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b%a. if a==0, sets b to 0 instead. */

void instruction_MDI(udcpu_t, udcpu_t*, udcpu_t*);
/* like MOD, but treat b, a as signed. (MDI -7, 16 == -7) */

void instruction_AND(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b&a */

void instruction_BOR(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b|a */

void instruction_XOR(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b^a */

void instruction_SHR(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b>>>a, sets EX to ((b<<16)>>a)&0xffff  */

void instruction_ASR(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b>>a, sets EX to ((b<<16)>>>a)&0xffff (arithmetic shift) (treats b as signed) */

void instruction_SHL(udcpu_t, udcpu_t*, udcpu_t*);
/*sets b to b<<a, sets EX to ((b<<a)>>16)&0xffff*/

void instruction_IFB(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if (b&a)!=0 */

void instruction_IFC(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if (b&a)==0 */

void instruction_IFE(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b==a  */

void instruction_IFN(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b!=a  */

void instruction_IFG(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b>a  */

void instruction_IFA(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b>a (signed)  */

void instruction_IFL(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b<a  */

void instruction_IFU(udcpu_t, udcpu_t*, udcpu_t*);
/* performs next instruction only if b<a (signed) */

void instruction_ADX(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b+a+EX, sets EX to 0x0001 if there is an over - flow, 0x0 otherwise*/

void instruction_SBX(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to b-a+EX, sets EX to 0xFFFF if there is an under - flow, 0x0 otherwise */

void instruction_STI(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to a, then increases I and J by 1 */

void instruction_STD(udcpu_t, udcpu_t*, udcpu_t*);
/* sets b to a, then decreases I and J by 1 */



/* Special OP Codes */
void instruction_JSR(udcpu_t, udcpu_t*);
/*  pushes the address of the next instruction to the stack, then sets PC to a */

void instruction_INT(udcpu_t, udcpu_t*);
/* triggers a software interrupt with message a */

void instruction_IAG(udcpu_t, udcpu_t*);
/* sets a to IA  */

void instruction_IAS(udcpu_t, udcpu_t*);
/* sets IA to a  */

void instruction_RFI(udcpu_t, udcpu_t*);
/* disables interrupt queueing, pops A from the stack, then, pops PC from the stack */

void instruction_IAQ(udcpu_t, udcpu_t*);
/* 
 * if a is nonzero, interrupts will be added to the queue
 * instead of triggered. if a is zero, interrupts will be
 * triggered as normal again
 */

void instruction_HWN(udcpu_t, udcpu_t*);
/* sets a to number of connected hardware devices */

void instruction_HWQ(udcpu_t, udcpu_t*);
/*
 * sets A, B, C, X, Y registers to information about hardware a
 * A+(B<<16) is a 32 bit word identifying the hardware id
 * C is the hardware version
 * X+(Y<<16) is a 32 bit word identifying the manufacturer
 */

void instruction_HWI(udcpu_t, udcpu_t*);
/* sends an interrupt to hardware a */

#define HAS_LOOKUP(x) (((x>=NEXT_WORD_PLUS_REG_A_MEM)&&(x<=NEXT_WORD_PLUS_REG_J_MEM))||(x==NEXT_WORD_LITERAL)||(x==NEXT_WORD_MEM))
udcpu_t total_instruction_size(udcpu_t);


extern bool interrupt_queueing;
#define ENABLED  true
#define DISABLED false

extern bool queue_int_not_trigger;

#endif
