#ifndef EMU6502_UI_H
#define EMU6502_UI_H

#include "emu6502_cpu.h"
#include "memory.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color bg_color;
    SDL_Color text_color;
} EMU6502_UI;

EMU6502_UI* emu6502_ui_create(SDL_Renderer* renderer, TTF_Font* font);
void emu6502_ui_destroy(EMU6502_UI* ui);
void emu6502_ui_render(EMU6502_UI* ui, EMU6502_CPU* cpu, EMU6502_Memory* mem);
void render_text(EMU6502_UI* ui, const char* text, int x, int y, SDL_Renderer* renderer);

#endif