#include "opcodes.h"
#include "emu6502_cpu.h"
#include <stdio.h>

static void op_brk(EMU6502_CPU* cpu) {
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, (cpu->pc >> 8) & 0x00FF);
    cpu->sp--;
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->pc & 0x00FF);
    cpu->sp--;
    cpu->status |= FLAG_B;
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->status);
    cpu->sp--;
    cpu->status |= FLAG_I;
    uint16_t lo = emu6502_bus_read(cpu->bus, 0xFFFE);
    uint16_t hi = emu6502_bus_read(cpu->bus, 0xFFFF);
    cpu->pc = (hi << 8) | lo;
}

static void op_lda_imm(EMU6502_CPU* cpu) {
    uint16_t addr = addr_immediate(cpu);
    cpu->a = emu6502_bus_read(cpu->bus, addr);
    cpu->status &= ~(FLAG_Z | FLAG_N);
    if (cpu->a == 0) cpu->status |= FLAG_Z;
    if (cpu->a & 0x80) cpu->status |= FLAG_N;
}

static void op_lda_abs(EMU6502_CPU* cpu) {
    uint16_t addr = addr_absolute(cpu);
    cpu->a = emu6502_bus_read(cpu->bus, addr);
    cpu->status &= ~(FLAG_Z | FLAG_N);
    if (cpu->a == 0) cpu->status |= FLAG_Z;
    if (cpu->a & 0x80) cpu->status |= FLAG_N;
}

static void op_ldx_imm(EMU6502_CPU* cpu) {
    uint16_t addr = addr_immediate(cpu);
    cpu->x = emu6502_bus_read(cpu->bus, addr);
    cpu->status &= ~(FLAG_Z | FLAG_N);
    if (cpu->x == 0) cpu->status |= FLAG_Z;
    if (cpu->x & 0x80) cpu->status |= FLAG_N;
}

static void op_sta_abs(EMU6502_CPU* cpu) {
    uint16_t addr = addr_absolute(cpu);
    emu6502_bus_write(cpu->bus, addr, cpu->a);
}

static void op_stx_abs(EMU6502_CPU* cpu) {
    uint16_t addr = addr_absolute(cpu);
    emu6502_bus_write(cpu->bus, addr, cpu->x);
}

static void op_jmp_abs(EMU6502_CPU* cpu) {
    uint16_t addr = addr_absolute(cpu);
    cpu->pc = addr;
}

static void op_nop(EMU6502_CPU* cpu) {
    // No operation
}

void emu6502_opcodes_init(Instruction* table) {
    for (int i = 0; i < 256; i++) {
        table[i].cycles = 2; // Default cycles
        table[i].execute = NULL;
    }

    table[0x00].cycles = 7; table[0x00].execute = op_brk; // BRK
    table[0xA9].cycles = 2; table[0xA9].execute = op_lda_imm; // LDA imm
    table[0xAD].cycles = 4; table[0xAD].execute = op_lda_abs; // LDA abs
    table[0xA2].cycles = 2; table[0xA2].execute = op_ldx_imm; // LDX imm
    table[0x8D].cycles = 4; table[0x8D].execute = op_sta_abs; // STA abs
    table[0x8E].cycles = 4; table[0x8E].execute = op_stx_abs; // STX abs
    table[0x4C].cycles = 3; table[0x4C].execute = op_jmp_abs; // JMP abs
    table[0xEA].cycles = 2; table[0xEA].execute = op_nop; // NOP
}

void emu6502_opcodes_execute(EMU6502_CPU* cpu, uint8_t opcode) {
    static Instruction table[256];
    static int initialized = 0;
    if (!initialized) {
        emu6502_opcodes_init(table);
        initialized = 1;
    }

    if (table[opcode].execute) {
        cpu->cycles = table[opcode].cycles;
        table[opcode].execute(cpu);
    } else {
        printf("Unimplemented opcode: 0x%02X at PC=0x%04X\n", opcode, cpu->pc - 1);
        cpu->cycles = 2;
    }
}