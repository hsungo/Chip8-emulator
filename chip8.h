#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t memory[4096];       // 4KB memory
    uint8_t V[16];              // 16x 8-bit registers
    uint16_t I;                 // 16-bit index register
    uint16_t pc;                // 16-bit program counter

    uint16_t stack[16];        
    uint8_t sp;                 // stack pointer

    uint8_t delay_timer;        
    uint8_t sound_timer;        

    uint8_t keypad[16];         // 16 bottons status
    uint32_t display[64 * 32];  // 64x32 pixels display

    bool wait_key;              // whether key pressed
    bool draw_flag;             // whether renew display
} Chip8;


void init_chip8(Chip8 *chip8); //initialize
bool load_rom(Chip8 *chip8, const char *filename);
uint16_t fetch_instruction(Chip8 *chip8);
void emulate_cycle(Chip8 *chip8);

#endif