#include "emu6502_cpu.h"
#include "bus.h"
#include "emu6502_ui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 800
#define GRAPHICS_WIDTH 640
#define GRAPHICS_HEIGHT 400

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        return 1;
    }

    // Debug Window
    SDL_Window* debug_window = SDL_CreateWindow("EMU6502 Debug", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!debug_window) {
        printf("Debug window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* debug_renderer = SDL_CreateRenderer(debug_window, -1, SDL_RENDERER_ACCELERATED);
    if (!debug_renderer) {
        printf("Debug renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    // Graphics Window
    SDL_Window* graphics_window = SDL_CreateWindow("EMU6502 Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                   GRAPHICS_WIDTH, GRAPHICS_HEIGHT, SDL_WINDOW_SHOWN);
    if (!graphics_window) {
        printf("Graphics window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* graphics_renderer = SDL_CreateRenderer(graphics_window, -1, SDL_RENDERER_ACCELERATED);
    if (!graphics_renderer) {
        printf("Graphics renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("fonts/VTS.TTF", 16);
    if (!font) {
        printf("Font loading failed: %s\n", TTF_GetError());
        return 1;
    }

    EMU6502_UI* ui = emu6502_ui_create(debug_renderer, font);
    EMU6502_Bus* bus = emu6502_bus_create(argc > 1 ? argv[1] : "test.rom");
    EMU6502_CPU* cpu = emu6502_cpu_create();
    emu6502_cpu_connect_bus(cpu, bus);

    emu6502_cpu_reset(cpu);

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) running = 0;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE: emu6502_cpu_clock(cpu); break;
                    case SDLK_r: emu6502_cpu_reset(cpu); break;
                    case SDLK_i: emu6502_cpu_irq(cpu); break;
                    case SDLK_n: emu6502_cpu_nmi(cpu); break;
                    case SDLK_b: emu6502_memory_switch_bank(bus->memory, bus->memory->current_bank + 1); break;
                    case SDLK_q: running = 0; break;
                }
            }
        }

        // Render Debug Window
        emu6502_ui_render(ui, cpu, bus->memory);

        // Render Graphics Window
        SDL_SetRenderDrawColor(graphics_renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(graphics_renderer);
        uint8_t* framebuffer = emu6502_memory_get_framebuffer(bus->memory);
        printf("Framebuffer at $0400: ");
        for (int i = 0; i < 16; i++) {
            printf("%02X ", framebuffer[i]);
        }
        printf("\n");
        for (int y = 0; y < 25; y++) {
            for (int x = 0; x < 40; x++) {
                char c = framebuffer[y * 40 + x];
                if (c >= 32 && c <= 126) { // Printable ASCII characters
                    char text[2] = {c, 0};
                    render_text(ui, text, x * 16, y * 16, graphics_renderer);
                }
            }
        }
        SDL_RenderPresent(graphics_renderer);

        SDL_Delay(16);
    }

    emu6502_ui_destroy(ui);
    emu6502_cpu_destroy(cpu);
    emu6502_bus_destroy(bus);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(debug_renderer);
    SDL_DestroyRenderer(graphics_renderer);
    SDL_DestroyWindow(debug_window);
    SDL_DestroyWindow(graphics_window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}