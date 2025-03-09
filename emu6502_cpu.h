#ifndef EMU6502_CPU_H
#define EMU6502_CPU_H

#include <stdint.h>
#include "bus.h"

typedef struct {
    // Registers
    uint8_t  a;
    uint8_t  x;
    uint8_t  y;
    uint8_t  sp;
    uint16_t pc;
    uint8_t  status;

    // Internal state
    EMU6502_Bus* bus;
    uint8_t opcode;
    uint8_t cycles;
    uint16_t addr_abs;
    uint16_t addr_rel;
    uint8_t fetched;

    // Status flags
    #define FLAG_C (1 << 0) // Carry
    #define FLAG_Z (1 << 1) // Zero
    #define FLAG_I (1 << 2) // Interrupt Disable
    #define FLAG_D (1 << 3) // Decimal Mode
    #define FLAG_B (1 << 4) // Break
    #define FLAG_U (1 << 5) // Unused
    #define FLAG_V (1 << 6) // Overflow
    #define FLAG_N (1 << 7) // Negative
} EMU6502_CPU;

EMU6502_CPU* emu6502_cpu_create();
void emu6502_cpu_destroy(EMU6502_CPU* cpu);
void emu6502_cpu_connect_bus(EMU6502_CPU* cpu, EMU6502_Bus* bus);
void emu6502_cpu_clock(EMU6502_CPU* cpu);
void emu6502_cpu_reset(EMU6502_CPU* cpu);
void emu6502_cpu_irq(EMU6502_CPU* cpu);
void emu6502_cpu_nmi(EMU6502_CPU* cpu);

// Addressing mode functions
uint16_t addr_immediate(EMU6502_CPU* cpu);
uint16_t addr_absolute(EMU6502_CPU* cpu);
uint16_t addr_zero_page(EMU6502_CPU* cpu);

#endif