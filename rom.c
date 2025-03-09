#include "rom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EMU6502_Rom* emu6502_rom_create(uint16_t base_addr, const char* filename) {
    EMU6502_Rom* rom = (EMU6502_Rom*)malloc(sizeof(EMU6502_Rom));
    if (rom) {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            printf("Failed to open ROM file: %s\n", filename);
            free(rom);
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        rom->size = ftell(file);
        rewind(file);

        rom->data = (uint8_t*)malloc(rom->size);
        if (rom->data) {
            fread(rom->data, 1, rom->size, file);
            rom->base_addr = base_addr;
        } else {
            free(rom);
            rom = NULL;
        }

        fclose(file);
    }
    return rom;
}

void emu6502_rom_destroy(EMU6502_Rom* rom) {
    if (rom) {
        free(rom->data);
        free(rom);
    }
}

uint8_t emu6502_rom_read(EMU6502_Rom* rom, uint16_t addr) {
    if (addr >= rom->base_addr && addr < (rom->base_addr + rom->size)) {
        return rom->data[addr - rom->base_addr];
    }
    return 0x00; // Out of range
}