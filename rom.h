#ifndef ROM_H
#define ROM_H

#include <stdint.h>

typedef struct {
    uint8_t* data;
    uint32_t size;
    uint16_t base_addr;
} EMU6502_Rom;

EMU6502_Rom* emu6502_rom_create(uint16_t base_addr, const char* filename);
void emu6502_rom_destroy(EMU6502_Rom* rom);
uint8_t emu6502_rom_read(EMU6502_Rom* rom, uint16_t addr);

#endif