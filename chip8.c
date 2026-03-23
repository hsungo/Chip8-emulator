#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

// CHIP-8 build-in hexadecimal font set, each character composed of 4*5 pixels
const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// initialize
void init_chip8(Chip8 *chip8) {
    //reset the entire CHIP‑8 structure to zero.
    memset(chip8, 0, sizeof(Chip8));

    // PC start at 0x200 
    chip8->pc = 0x200;

    // set fontset start from 0x000
    for (int i = 0; i < 80; ++i) {
        chip8->memory[i] = fontset[i];
    }
}

// read ROM and load into memory
bool load_rom(Chip8 *chip8, const char *filename) {
    // use "rb" (read binary) to open binary file
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Unable to open ROM file %s\n", filename);
        return false;
    }

    // get file size
    fseek(file, 0, SEEK_END);      
    long file_size = ftell(file); 
    rewind(file);                  

    // Check whether the ROM file size exceeds the remaining memory capacity
    if (file_size > (4096 - 512)) {
        printf("Error: ROM file too large, unable to load into memory!\n");
        fclose(file);
        return false;
    }

    fread(&chip8->memory[0x200], 1, file_size, file);
    fclose(file);

    return true;
}

uint16_t fetch_instruction(Chip8 *chip8) {
    uint8_t front_instr = chip8->memory[chip8->pc];    // get first instr bytes
    uint8_t back_instr = chip8->memory[chip8->pc + 1]; // get second instr bytes
    uint16_t instr = (front_instr << 8) | back_instr;     // merge instr
    chip8->pc += 2;                                     //update pc
    return instr;
}

void emulate_cycle(Chip8 *chip8) {
    uint16_t instr = fetch_instruction(chip8);

    switch (instr & 0xF000) {   
        case 0x0000: {
            if (instr == 0x00E0) {
                memset(chip8->display, 0, sizeof(chip8->display));
                chip8->draw_flag = true;
            } else if (instr == 0x00EE) {
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
            } else{
               printf("Error: Unknown instruction 0x%04X\n", instr);
            }
            break;} 
        case 0x1000:{
            chip8->pc = instr & 0x0FFF; 
            break;}
        case 0x2000:{
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;
            chip8->pc = instr & 0x0FFF;
            break;}
        case 0x3000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t NN = instr & 0x00FF;
            if(chip8->V[X] == NN) chip8->pc +=2; 
            break;}
        case 0x4000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t NN = instr & 0x00FF;
            if(chip8->V[X] != NN) chip8->pc +=2; 
            break;}
        case 0x5000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t Y = (instr & 0x00F0) >> 4;
            if(chip8->V[X] == chip8->V[Y]) chip8->pc +=2; 
            break;}
        case 0x6000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            chip8->V[X] = instr & 0x00FF; 
            break;}
        case 0x7000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t NN = instr & 0x00FF;
            chip8->V[X] += NN;
            break;}
        case 0x8000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t Y = (instr & 0x00F0) >> 4;
            switch(instr & 0x000F){
                case 0x0000:{
                    chip8->V[X] = chip8->V[Y];
                    break;}
                case 0x0001:{
                    chip8->V[X] |= chip8->V[Y];
                    break;}
                case 0x0002:{
                    chip8->V[X] &= chip8->V[Y];
                    break;}
                case 0x0003:{
                    chip8->V[X] ^= chip8->V[Y];
                    break;}
                case 0x0004:{
                    uint16_t flag = chip8->V[X] + chip8->V[Y];   
                    chip8->V[X] = flag & 0xFF;
                    chip8->V[0xF] = (flag > 0xFF);
                    break;}
                case 0x0005:{
                    uint16_t flag = (chip8->V[X] >= chip8->V[Y]);
                    chip8->V[X] -= chip8->V[Y];
                    chip8->V[0xF] = flag;
                    break;}
                case 0x0006:{
                    uint8_t flag = (chip8->V[X] & 0x1);
                    chip8->V[X] >>= 1;
                    chip8->V[0xF] = flag;
                    break;}
                case 0x0007:{
                    uint16_t flag = (chip8->V[Y] >= chip8->V[X]);
                    chip8->V[X] = chip8->V[Y] - chip8->V[X];
                    chip8->V[0xF] = flag;
                    break;}
                case 0x000E:{
                    uint8_t flag = (chip8->V[X] >> 7)& 0x1;
                    chip8->V[X] <<= 1;
                    chip8->V[0xF] = flag;
                    break;}
                default:{
                    printf("Error: Unknown instruction 0x%04X\n", instr);
                    break;}
            }
            break;
        }
        case 0x9000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t Y = (instr & 0x00F0) >> 4;
            if(chip8->V[X] != chip8->V[Y]) chip8->pc +=2; 
            break;}
        case 0xA000:{
            chip8->I = instr & 0x0FFF; 
            break;}
        case 0xB000:{
            uint16_t NNN= instr & 0x0FFF;
            chip8->pc = NNN + chip8->V[0x0];
            break;}
        case 0xC000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t NN = instr & 0x00FF;
            chip8->V[X] = (rand() % 256) & NN; 
            break;}   
        case 0xD000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            uint8_t Y = (instr & 0x00F0) >> 4;
            uint8_t N = instr & 0x000F;
            uint8_t x = chip8->V[X];
            uint8_t y = chip8->V[Y];
            chip8->V[0xF] = 0;

            for(int row = 0; row < N; row++){
                uint8_t sprite_byte = chip8->memory[chip8->I + row];
                for (int col = 0; col < 8; col++){
                    if ((sprite_byte & (0x80 >> col)) != 0) {
                        int px = (x + col) % 64;
                        int py = (y + row) % 32;
                        int index = py * 64 + px;

                        if (chip8->display[index] == 1) {
                            chip8->V[0xF] = 1;
                        }       
                        chip8->display[index] ^= 1;
                    }
                }
            }
            chip8->draw_flag = true;
            break;}   
        case 0xE000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            switch (instr & 0x00FF){
                case 0x009E:
                    if(chip8->keypad[chip8->V[X]]) chip8->pc += 2;
                    break;
                case 0x00A1:
                    if(!(chip8->keypad[chip8->V[X]])) chip8->pc += 2;
                    break;
                default:
                    printf("Error: Unknown instruction 0x%04X\n", instr);
                    break;
                }
            break;}
        case 0xF000:{
            uint8_t X = (instr & 0x0F00) >> 8;
            switch (instr & 0x00FF){
                case 0x0007:
                    chip8->V[X] = chip8->delay_timer;
                    break;
                case 0x000A:
                    if (chip8->wait_key == false) {
                        for (int i = 0; i < 16; i++) {
                            if (chip8->keypad[i]) {
                                chip8->wait_key = i;
                                break;
                            }
                        }
                        chip8->pc -= 2;
                        } else {
                        if (!chip8->keypad[chip8->wait_key]) { 
                            chip8->V[X] = chip8->wait_key; 
                            chip8->wait_key = false; 
                        } else {
                            chip8->pc -= 2; 
                            }
                        }
                    break;
                case 0x0015:
                    chip8->delay_timer = chip8->V[X];
                    break;
                case 0x0018:
                    chip8->sound_timer = chip8->V[X];
                    break;
                case 0x001E:
                    chip8->I += chip8->V[X];
                    break;
                case 0x0029:
                    chip8->I = chip8->V[X] * 5;
                    break;
                case 0x0033:
                    chip8->memory[chip8->I] = chip8->V[X] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->V[X] / 10) % 10;
                    chip8->memory[chip8->I + 2] = chip8->V[X] % 10;
                    break;
                case 0x0055:
                    for(int i = 0; i <= X; i++){
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                case 0x0065:
                    for(int i = 0; i <= X; i++){
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
                default:
                    printf("Error: Unknown instruction 0x%04X\n", instr);
                    break;
            }
            break;}   
        default:{
            printf("Error: Unknown instruction 0x%04X\n", instr);
            break;}
    }
}
