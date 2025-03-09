#include "emu6502_cpu.h"
#include "opcodes.h"
#include <stdlib.h>

uint16_t addr_immediate(EMU6502_CPU* cpu) {
    return cpu->pc++;
}

uint16_t addr_absolute(EMU6502_CPU* cpu) {
    uint16_t lo = emu6502_bus_read(cpu->bus, cpu->pc++);
    uint16_t hi = emu6502_bus_read(cpu->bus, cpu->pc++);
    return (hi << 8) | lo;
}

uint16_t addr_zero_page(EMU6502_CPU* cpu) {
    return emu6502_bus_read(cpu->bus, cpu->pc++);
}

EMU6502_CPU* emu6502_cpu_create() {
    EMU6502_CPU* cpu = (EMU6502_CPU*)malloc(sizeof(EMU6502_CPU));
    if (cpu) {
        cpu->a = 0x00;
        cpu->x = 0x00;
        cpu->y = 0x00;
        cpu->sp = 0xFD;
        cpu->pc = 0x0000;
        cpu->status = 0x00 | FLAG_U;
        cpu->bus = NULL;
        cpu->opcode = 0x00;
        cpu->cycles = 0;
    }
    return cpu;
}

void emu6502_cpu_destroy(EMU6502_CPU* cpu) {
    if (cpu) free(cpu);
}

void emu6502_cpu_connect_bus(EMU6502_CPU* cpu, EMU6502_Bus* bus) {
    cpu->bus = bus;
}

void emu6502_cpu_clock(EMU6502_CPU* cpu) {
    if (cpu->cycles == 0) {
        cpu->opcode = emu6502_bus_read(cpu->bus, cpu->pc);
        printf("Fetched opcode: %02X at PC: $%04X\n", cpu->opcode, cpu->pc); // Debug print
        cpu->pc++;
        emu6502_opcodes_execute(cpu, cpu->opcode);
    }
    cpu->cycles--;
}


void emu6502_cpu_reset(EMU6502_CPU* cpu) {
    uint16_t lo = emu6502_bus_read(cpu->bus, 0xFFFC);
    uint16_t hi = emu6502_bus_read(cpu->bus, 0xFFFD);
    cpu->pc = (hi << 8) | lo;
    printf("Reset vector: $%04X\n", cpu->pc); // Debug print

    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->sp = 0xFD;
    cpu->status = 0x00 | FLAG_U;

    cpu->cycles = 8;
}

void emu6502_cpu_irq(EMU6502_CPU* cpu) {
    if (!(cpu->status & FLAG_I)) {
        emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, (cpu->pc >> 8) & 0x00FF);
        cpu->sp--;
        emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->pc & 0x00FF);
        cpu->sp--;
        emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->status);
        cpu->sp--;

        cpu->status |= FLAG_I;
        uint16_t lo = emu6502_bus_read(cpu->bus, 0xFFFE);
        uint16_t hi = emu6502_bus_read(cpu->bus, 0xFFFF);
        cpu->pc = (hi << 8) | lo;

        cpu->cycles = 7;
    }
}

void emu6502_cpu_nmi(EMU6502_CPU* cpu) {
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, (cpu->pc >> 8) & 0x00FF);
    cpu->sp--;
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->pc & 0x00FF);
    cpu->sp--;
    emu6502_bus_write(cpu->bus, 0x0100 + cpu->sp, cpu->status);
    cpu->sp--;

    cpu->status |= FLAG_I;
    uint16_t lo = emu6502_bus_read(cpu->bus, 0xFFFA);
    uint16_t hi = emu6502_bus_read(cpu->bus, 0xFFFB);
    cpu->pc = (hi << 8) | lo;

    cpu->cycles = 8;
}