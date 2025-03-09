#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "ram.h"
#include "rom.h"

#define MAX_BANKS 16
#define BANK_SIZE 0x4000 // 16KB per bank
#define FRAMEBUFFER_START 0x0400
#define FRAMEBUFFER_SIZE 0x0400 // 1KB for a 40x25 text screen

typedef struct {
    EMU6502_Ram* ram;
    EMU6502_Rom* rom;
    uint8_t current_bank;
    uint8_t num_banks;
    uint16_t rom_base_addr;
    uint8_t framebuffer[FRAMEBUFFER_SIZE]; // Add framebuffer
} EMU6502_Memory;

EMU6502_Memory* emu6502_memory_create(uint16_t ram_base, uint32_t ram_size, uint16_t rom_base, const char* rom_filename);
void emu6502_memory_destroy(EMU6502_Memory* mem);
uint8_t emu6502_memory_read(EMU6502_Memory* mem, uint16_t addr);
void emu6502_memory_write(EMU6502_Memory* mem, uint16_t addr, uint8_t data);
void emu6502_memory_switch_bank(EMU6502_Memory* mem, uint8_t bank);
uint8_t* emu6502_memory_get_framebuffer(EMU6502_Memory* mem); // New function

#endif