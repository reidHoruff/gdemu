#ifndef _DCPU_H_
#define _DCPU_H_

typedef int16_t   sdcpu_t;    /*signed two byte word*/
typedef u_int16_t udcpu_t;    /*signed two byte word*/
typedef int32_t   ldcpu_t;    /*long, for overflow detection*/
typedef u_int32_t uldcpu_t;   /*long, for overflow detection*/

typedef int bool;
#define true  1
#define false 0

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

#define MAX_DCPU_T       0xFFFF
#define MIN_DCPU_T       0x0000
#define SIGN(x) ((sdcpu_t)(x))

#define OVERFLOW_FLAG    0x0001
#define NO_OVERFLOW_FLAG 0x0000
#define UNDERFLOW_FLAG   0xffff

extern udcpu_t register_literals[13];
/* 
 * registers are indexed by their machine value 
 * other values (PC, SP, EX, IA) are also indexed
 */
 
extern udcpu_t cycles_wait;

#define ADD_CYCLES(x) (cycles_wait += (x-1))

extern udcpu_t code_size;
extern bool if_skip_flag;

#define INT_QUEUE_SIZE 256
extern udcpu_t interrupt_queue_ptr;

extern udcpu_t *dcpu_mem, *dcpu_rom, *interrupt_queue;

void dcpu_read_file(FILE*);
void disp_info(void);
void dcpu_cycle(void);
void dcpu_step(void);
void init_dcpu(void);
void queue_interrupt(udcpu_t);
void reset_dcpu(void);
udcpu_t *get_value(udcpu_t, udcpu_t*, bool);

#endif
