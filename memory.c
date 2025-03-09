#include "memory.h"
#include <stdlib.h>
#include <string.h>

EMU6502_Memory* emu6502_memory_create(uint16_t ram_base, uint32_t ram_size, uint16_t rom_base, const char* rom_filename) {
    EMU6502_Memory* mem = (EMU6502_Memory*)malloc(sizeof(EMU6502_Memory));
    if (mem) {
        mem->ram = emu6502_ram_create(ram_base, ram_size);
        mem->rom = emu6502_rom_create(rom_base, rom_filename);
        mem->rom_base_addr = rom_base;
        mem->current_bank = 0;
        mem->num_banks = 0;
        memset(mem->framebuffer, 0, FRAMEBUFFER_SIZE); // Initialize framebuffer

        if (mem->rom) {
            mem->num_banks = (mem->rom->size + BANK_SIZE - 1) / BANK_SIZE;
            if (mem->num_banks > MAX_BANKS) mem->num_banks = MAX_BANKS;
        }
    }
    return mem;
}

void emu6502_memory_destroy(EMU6502_Memory* mem) {
    if (mem) {
        emu6502_ram_destroy(mem->ram);
        emu6502_rom_destroy(mem->rom);
        free(mem);
    }
}

uint8_t emu6502_memory_read(EMU6502_Memory* mem, uint16_t addr) {
    if (addr >= mem->ram->base_addr && addr < (mem->ram->base_addr + mem->ram->size)) {
        return emu6502_ram_read(mem->ram, addr);
    }
    if (mem->rom && addr >= mem->rom_base_addr && addr < (mem->rom_base_addr + BANK_SIZE)) {
        uint32_t rom_offset = (addr - mem->rom_base_addr) + (mem->current_bank * BANK_SIZE);
        if (rom_offset < mem->rom->size) {
            return mem->rom->data[rom_offset];
        }
    }
    if (addr >= FRAMEBUFFER_START && addr < (FRAMEBUFFER_START + FRAMEBUFFER_SIZE)) {
        return mem->framebuffer[addr - FRAMEBUFFER_START];
    }
    return 0x00; // Unmapped memory
}

void emu6502_memory_write(EMU6502_Memory* mem, uint16_t addr, uint8_t data) {
    if (addr >= mem->ram->base_addr && addr < (mem->ram->base_addr + mem->ram->size)) {
        emu6502_ram_write(mem->ram, addr, data);
        if (addr >= 0x0400 && addr <= 0x041F) {
            printf("Wrote %02X to RAM at $%04X\n", data, addr);
        }
    }
    if (addr >= FRAMEBUFFER_START && addr < (FRAMEBUFFER_START + FRAMEBUFFER_SIZE)) {
        mem->framebuffer[addr - FRAMEBUFFER_START] = data;
    }
    if (addr == 0xFF00) {
        emu6502_memory_switch_bank(mem, data);
    }
}

void emu6502_memory_switch_bank(EMU6502_Memory* mem, uint8_t bank) {
    if (bank < mem->num_banks) {
        mem->current_bank = bank;
    }
}

uint8_t* emu6502_memory_get_framebuffer(EMU6502_Memory* mem) {
    return mem->framebuffer;
}