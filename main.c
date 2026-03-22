#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "SDL2/include/SDL.h"
#include "chip8.h"

const int SCALE = 10;

void handle_keys(SDL_Event *event, Chip8 *chip8) {
    bool is_pressed = (event->type == SDL_KEYDOWN);
    
    switch (event->key.keysym.sym) {
        case SDLK_1: chip8->keypad[0x1] = is_pressed; break;
        case SDLK_2: chip8->keypad[0x2] = is_pressed; break;
        case SDLK_3: chip8->keypad[0x3] = is_pressed; break;
        case SDLK_4: chip8->keypad[0xC] = is_pressed; break;
        case SDLK_q: chip8->keypad[0x4] = is_pressed; break;
        case SDLK_w: chip8->keypad[0x5] = is_pressed; break;
        case SDLK_e: chip8->keypad[0x6] = is_pressed; break;
        case SDLK_r: chip8->keypad[0xD] = is_pressed; break;
        case SDLK_a: chip8->keypad[0x7] = is_pressed; break;
        case SDLK_s: chip8->keypad[0x8] = is_pressed; break;
        case SDLK_d: chip8->keypad[0x9] = is_pressed; break;
        case SDLK_f: chip8->keypad[0xE] = is_pressed; break;
        case SDLK_z: chip8->keypad[0xA] = is_pressed; break;
        case SDLK_x: chip8->keypad[0x0] = is_pressed; break;
        case SDLK_c: chip8->keypad[0xB] = is_pressed; break;
        case SDLK_v: chip8->keypad[0xF] = is_pressed; break;
    }
}

void audio_callback(void *userdata, uint8_t *stream, int len) {
    static int phase = 0;
    int16_t *buffer = (int16_t *)stream;
    int length = len / 2;
    int volume = 3000; 
    for (int i = 0; i < length; i++) {
        buffer[i] = ((phase / 100) % 2 == 0) ? volume : -volume; 
        phase++;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Error: please provide ROM file path!\n");
        return -1;
    }

    Chip8 chip8;
    init_chip8(&chip8);
    bool is_running = true;
    if (!load_rom(&chip8, argv[1])) {
        return -1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL initial error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("My CHIP-8 Emulator", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          64 * SCALE, 32 * SCALE, 0);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;        
    want.format = AUDIO_S16SYS; 
    want.channels = 1;          
    want.samples = 2048;       
    want.callback = audio_callback; 
    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_Event event;
    uint32_t last_time = SDL_GetTicks();

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                handle_keys(&event, &chip8);
            }
        }

        emulate_cycle(&chip8);

        if (chip8.draw_flag) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    if (chip8.display[y * 64 + x] == 1) {
                        SDL_Rect rect = {x * SCALE, y * SCALE, SCALE, SCALE};
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }
            SDL_RenderPresent(renderer);
            chip8.draw_flag = false; 
        }
        uint32_t current_time = SDL_GetTicks();
        if(current_time - last_time >= 16){
            if(chip8.delay_timer){
                chip8.delay_timer--;
            }
            if(chip8.sound_timer){
                SDL_PauseAudioDevice(audio_device, 0);
                chip8.sound_timer--;
            }
            else{
                SDL_PauseAudioDevice(audio_device, 1);
            }
            last_time = current_time;
        }
        SDL_Delay(3);
    }
    SDL_CloseAudioDevice(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}