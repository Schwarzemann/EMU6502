#include <stdlib.h>
#include "bus.h"

EMU6502_Bus* emu6502_bus_create(const char* rom_filename) {
    EMU6502_Bus* bus = (EMU6502_Bus*)malloc(sizeof(EMU6502_Bus));
    if (bus) {
        bus->memory = emu6502_memory_create(0x0000, 0x8000, 0x8000, rom_filename);
    }
    return bus;
}

void emu6502_bus_destroy(EMU6502_Bus* bus) {
    if (bus) {
        emu6502_memory_destroy(bus->memory);
        free(bus);
    }
}

uint8_t emu6502_bus_read(EMU6502_Bus* bus, uint16_t addr) {
    return emu6502_memory_read(bus->memory, addr);
}

void emu6502_bus_write(EMU6502_Bus* bus, uint16_t addr, uint8_t data) {
    emu6502_memory_write(bus->memory, addr, data);
}