#ifndef _DCPU_H_
#define _DCPU_H_

#define MEM_SIZE_WORDS      0x10000
#define STACK_MEM_START_LOC 0xFFFF

#define INSTRUCTION_OP(x) (x & 0x1F)          /* first 5 bits of word */
#define INSTRUCTION_B(x)  ( (x>>5) & 0x1F )   /* next 5 bits of word */
#define INSTRUCTION_A(x)  ( (x>>10) & 0x3F )  /* last 6 bits of word */

#define SPECIAL_OP_OP(x) INSTRUCTION_B(x)
#define SPECIAL_OP_A(x)  INSTRUCTION_A(x)

#define IS_BRANCH_OP(x) (x>=OP_IFB && x<=OP_IFU)
#define IS_SPECIAL_OP(x) ((x&0x1F)==0)

#define TOP_WORD(x)    ( (udcpu_t)(x>>16) )
#define BOTTOM_WORD(x) ( (udcpu_t)(x&0xFFFF) )

/*values*/

#define REGISTER_A               0x00
#define REGISTER_B               0x01
#define REGISTER_C               0x02
#define REGISTER_X               0x03
#define REGISTER_Y               0x04
#define REGISTER_Z               0x05
#define REGISTER_I               0x06
#define REGISTER_J               0x07

/* indexs of values in register_literals[] */
#define VALUE_PC                 0x08
#define VALUE_SP                 0x09
#define VALUE_EX                 0x0A
#define VALUE_IA                 0x0B

#define REGISTER_A_MEM           0x08
#define REGISTER_B_MEM           0x09
#define REGISTER_C_MEM           0x0A
#define REGISTER_X_MEM           0x0B
#define REGISTER_Y_MEM           0x0C
#define REGISTER_Z_MEM           0x0D
#define REGISTER_I_MEM           0x0E
#define REGISTER_J_MEM           0x0F

#define NEXT_WORD_PLUS_REG_A_MEM 0x10
#define NEXT_WORD_PLUS_REG_B_MEM 0x11
#define NEXT_WORD_PLUS_REG_C_MEM 0x12
#define NEXT_WORD_PLUS_REG_X_MEM 0x13
#define NEXT_WORD_PLUS_REG_Y_MEM 0x14
#define NEXT_WORD_PLUS_REG_Z_MEM 0x15
#define NEXT_WORD_PLUS_REG_I_MEM 0x16
#define NEXT_WORD_PLUS_REG_J_MEM 0x17

#define PUSH_POP                 0x18
#define PEEK                     0x19
#define ADDR_SP_NEXT_WORD        0x1A

#define STACK_POINTER            0x1B
#define PROGRAM_COUNTER          0x1C
#define OVERFLOW                 0x1D

#define NEXT_WORD_MEM            0x1E
#define NEXT_WORD_LITERAL        0x1F
#define LITERAL_VALUE_MIN        0x20
#define LITERAL_VALUE_MAX        0x3f

#define IS_LITERAL(x) ((x>=LITERAL_VALUE_MIN) && (x<=LITERAL_VALUE_MAX))
#define LITERAL_VALUE_OFFSET(x) ((udcpu_t)(x-LITERAL_VALUE_MIN-1))

/* basic OP codes */
#define OP_SET 0x1
#define OP_ADD 0x2
#define OP_SUB 0x3
#define OP_MUL 0x4
#define OP_MLI 0x5
#define OP_DIV 0x6
#define OP_DVI 0x7
#define OP_MOD 0x8
#define OP_MDI 0x9
#define OP_AND 0xA
#define OP_BOR 0xB
#define OP_XOR 0xC
#define OP_SHR 0xD
#define OP_ASR 0xE
#define OP_SHL 0xF
#define OP_IFB 0x10
#define OP_IFC 0x11
#define OP_IFE 0x12
#define OP_IFN 0x13
#define OP_IFG 0x14
#define OP_IFA 0x15
#define OP_IFL 0x16
#define OP_IFU 0x17
#define OP_ADX 0x1A
#define OP_SBX 0x1B
#define OP_STI 0x1E
#define OP_STD 0x1F

/*special OP codes */
#define NON_BASIC 0x00
#define OP_JSR    0x01
#define OP_INT    0x08
#define OP_IAG    0x09
#define OP_IAS    0x0A
#define OP_RFI    0x0B
#define OP_IAQ    0x0C
#define OP_HWN    0x10
#define OP_HWQ    0x11
#define OP_HWI    0x12

typedef int16_t   sdcpu_t;    /*signed two byte word*/
typedef u_int16_t udcpu_t;    /*signed two byte word*/
typedef int32_t   ldcpu_t;    /*long, for overflow detection*/
typedef u_int32_t uldcpu_t;   /*long, for overflow detection*/

#define MAX_DCPU_T       0xFFFF
#define MIN_DCPU_T       0x0000
#define SIGN(x) ((sdcpu_t)(x))

typedef int bool;
#define true  1
#define false 0

#define OVERFLOW_FLAG    0x0001
#define NO_OVERFLOW_FLAG 0x0000
#define UNDERFLOW_FLAG   0xffff

udcpu_t register_literals[13];
/* 
 * registers are indexed by their machine value 
 * other values (PC, SP, EX, IA) are also indexed
 */

udcpu_t *dcpu_mem, *dcpu_rom;

#define ADD_CYCLES(x) (cycles_wait += (x-1))

#define INT_QUEUE_SIZE 256
		
udcpu_t *get_value(udcpu_t, udcpu_t*, bool);
void disp_info(void);
void dcpu_cycle(void);
void dcpu_step(void);
void init_dcpu(void);
void queue_interrupt(udcpu_t);
void reset_dcpu(void);

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

#define ENABLED  true
#define DISABLED false

#include "dcpu.c"
#endif
