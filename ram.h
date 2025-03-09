#ifndef RAM_H
#define RAM_H

#include <stdint.h>

typedef struct {
    uint8_t* data;
    uint32_t size;
    uint16_t base_addr;
} EMU6502_Ram;

EMU6502_Ram* emu6502_ram_create(uint16_t base_addr, uint32_t size);
void emu6502_ram_destroy(EMU6502_Ram* ram);
uint8_t emu6502_ram_read(EMU6502_Ram* ram, uint16_t addr);
void emu6502_ram_write(EMU6502_Ram* ram, uint16_t addr, uint8_t data);

#endif