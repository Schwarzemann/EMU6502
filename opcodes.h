#ifndef OPCODES_H
#define OPCODES_H

#include "emu6502_cpu.h"

typedef struct {
    uint8_t cycles;
    void (*execute)(EMU6502_CPU*);
} Instruction;

void emu6502_opcodes_init(Instruction* table);
void emu6502_opcodes_execute(EMU6502_CPU* cpu, uint8_t opcode);

#endif