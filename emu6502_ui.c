#include "emu6502_ui.h"
#include "emu6502_cpu.h"
#include <stdio.h>
#include <string.h>

EMU6502_UI* emu6502_ui_create(SDL_Renderer* renderer, TTF_Font* font) {
    EMU6502_UI* ui = (EMU6502_UI*)malloc(sizeof(EMU6502_UI));
    if (ui) {
        ui->renderer = renderer;
        ui->font = font;
        ui->bg_color = (SDL_Color){0, 0, 255, 255}; // Blue background
        ui->text_color = (SDL_Color){255, 255, 255, 255}; // White text
    }
    return ui;
}

void emu6502_ui_destroy(EMU6502_UI* ui) {
    if (ui) free(ui);
}

void render_text(EMU6502_UI* ui, const char* text, int x, int y, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(ui->font, text, ui->text_color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static void print_state(EMU6502_UI* ui, EMU6502_CPU* cpu, int y_offset) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "PC: $%04X  A: $%02X  X: $%02X  Y: $%02X  Stack P: $%02X",
             cpu->pc, cpu->a, cpu->x, cpu->y, cpu->sp);
    render_text(ui, buffer, 10, y_offset, ui->renderer);

    snprintf(buffer, sizeof(buffer), "STATUS: %c%c%c%c%c%c%c",
             (cpu->status & FLAG_N) ? 'N' : '-',
             (cpu->status & FLAG_V) ? 'V' : '-',
             (cpu->status & FLAG_B) ? 'B' : '-',
             (cpu->status & FLAG_D) ? 'D' : '-',
             (cpu->status & FLAG_I) ? 'I' : '-',
             (cpu->status & FLAG_Z) ? 'Z' : '-',
             (cpu->status & FLAG_C) ? 'C' : '-');
    render_text(ui, buffer, 10, y_offset + 20, ui->renderer);
}

static void print_memory(EMU6502_UI* ui, EMU6502_Memory* mem, uint16_t start, uint16_t end, int x_offset, int y_offset, const char* label) {
    char buffer[256];
    render_text(ui, label, x_offset, y_offset, ui->renderer);
    y_offset += 20;

    for (uint16_t addr = start; addr <= end && addr < 0x10000; addr += 16) {
        snprintf(buffer, sizeof(buffer), "$%04X: ", addr);
        for (int i = 0; i < 16; i++) {
            if (addr + i <= end) {
                char temp[4];
                snprintf(temp, sizeof(temp), "%02X ", emu6502_memory_read(mem, addr + i));
                strcat(buffer, temp);
            }
        }
        render_text(ui, buffer, x_offset, y_offset, ui->renderer);
        y_offset += 20;
    }
}

static void disassemble(EMU6502_UI* ui, EMU6502_CPU* cpu, uint16_t addr, int y_offset) {
    char buffer[256];
    uint8_t opcode = emu6502_bus_read(cpu->bus, addr);
    switch (opcode) {
        case 0x00: snprintf(buffer, sizeof(buffer), "BRK"); break;
        case 0xA9: snprintf(buffer, sizeof(buffer), "LDA #$%02X", emu6502_bus_read(cpu->bus, addr + 1)); break;
        case 0xAD: snprintf(buffer, sizeof(buffer), "LDA $%04X", addr_absolute(cpu)); break;
        case 0xA5: snprintf(buffer, sizeof(buffer), "LDA $%02X", emu6502_bus_read(cpu->bus, addr + 1)); break;
        case 0xA2: snprintf(buffer, sizeof(buffer), "LDX #$%02X", emu6502_bus_read(cpu->bus, addr + 1)); break;
        case 0x8D: snprintf(buffer, sizeof(buffer), "STA $%04X", addr_absolute(cpu)); break;
        case 0x8E: snprintf(buffer, sizeof(buffer), "STX $%04X", addr_absolute(cpu)); break;
        case 0x9D: snprintf(buffer, sizeof(buffer), "STA $%04X, X", addr_absolute(cpu)); break;
        case 0x4C: snprintf(buffer, sizeof(buffer), "JMP $%04X", addr_absolute(cpu)); break;
        case 0xEA: snprintf(buffer, sizeof(buffer), "NOP"); break;
        case 0x78: snprintf(buffer, sizeof(buffer), "SEI"); break;
        case 0xD8: snprintf(buffer, sizeof(buffer), "CLD"); break;
        case 0xE8: snprintf(buffer, sizeof(buffer), "INX"); break;
        case 0xF0: snprintf(buffer, sizeof(buffer), "BEQ $%02X", emu6502_bus_read(cpu->bus, addr + 1)); break;
        default: snprintf(buffer, sizeof(buffer), "???"); break;
    }
    printf("Disassembled: %s at y=%d\n", buffer, y_offset); // Debug print
    render_text(ui, buffer, 10, y_offset, ui->renderer);
}

void emu6502_ui_render(EMU6502_UI* ui, EMU6502_CPU* cpu, EMU6502_Memory* mem) {
    SDL_SetRenderDrawColor(ui->renderer, ui->bg_color.r, ui->bg_color.g, ui->bg_color.b, ui->bg_color.a);
    SDL_RenderClear(ui->renderer);

    print_state(ui, cpu, 10);
    print_memory(ui, mem, 0x0400, 0x041F, 10, 50, "RAM:");
    print_memory(ui, mem, 0x8000, 0x801F, 450, 50, "ROM:");
    disassemble(ui, cpu, cpu->pc - 1, 200); // Moved to y=200 for better visibility
    render_text(ui, "SPACE = Step  R = RESET  I = IRQ  N = NMI  B = Next Bank  Q = Quit", 10, 350, ui->renderer);

    char bank_info[64];
    snprintf(bank_info, sizeof(bank_info), "Current Bank: %d / %d", mem->current_bank, mem->num_banks - 1);
    render_text(ui, bank_info, 10, 370, ui->renderer);

    SDL_RenderPresent(ui->renderer);
}