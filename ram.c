#include "ram.h"
#include <stdlib.h>
#include <string.h>

EMU6502_Ram* emu6502_ram_create(uint16_t base_addr, uint32_t size) {
    EMU6502_Ram* ram = (EMU6502_Ram*)malloc(sizeof(EMU6502_Ram));
    if (ram) {
        ram->data = (uint8_t*)malloc(size);
        if (ram->data) {
            memset(ram->data, 0, size);
            ram->size = size;
            ram->base_addr = base_addr;
        } else {
            free(ram);
            ram = NULL;
        }
    }
    return ram;
}

void emu6502_ram_destroy(EMU6502_Ram* ram) {
    if (ram) {
        free(ram->data);
        free(ram);
    }
}

uint8_t emu6502_ram_read(EMU6502_Ram* ram, uint16_t addr) {
    if (addr >= ram->base_addr && addr < (ram->base_addr + ram->size)) {
        return ram->data[addr - ram->base_addr];
    }
    return 0x00; // Out of range
}

void emu6502_ram_write(EMU6502_Ram* ram, uint16_t addr, uint8_t data) {
    if (addr >= ram->base_addr && addr < (ram->base_addr + ram->size)) {
        ram->data[addr - ram->base_addr] = data;
    }
}