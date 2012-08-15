#ifndef _disassembler_h_
#define _disassembler_h_

#include "dcpu.h"

void init_disassembler(void);
void dump_all(void);
void desc_value(udcpu_t, udcpu_t**, bool, char*);
void describe_live_instruction(udcpu_t);
void desc_inst(udcpu_t **);

const char *reg_names = "ABCXYZIJ";
const char *reg_inst_names[0x20];
const char *spec_inst_names[0x20];

#include "disassembler.c"
#endif
