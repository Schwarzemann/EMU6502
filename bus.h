#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include "memory.h"

typedef struct {
    EMU6502_Memory* memory;
} EMU6502_Bus;

EMU6502_Bus* emu6502_bus_create(const char* rom_filename);
void emu6502_bus_destroy(EMU6502_Bus* bus);
uint8_t emu6502_bus_read(EMU6502_Bus* bus, uint16_t addr);
void emu6502_bus_write(EMU6502_Bus* bus, uint16_t addr, uint8_t data);

#endif