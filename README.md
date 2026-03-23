![C](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

# CHIP-8 Emulator

A CHIP-8 emulator written in C and SDL2 library. 

```
chip8/
├── Makefile               # Compilation script for GCC
├── SDL2.dll               # Required runtime library for Windows
├── chip8.c                # Core emulator logic implementation
├── chip8.h                # Hardware specifications & definitions
├── main.c                 # Entry point & SDL2 window management
├── README.md              # Project documentation
├── ROM/                   # Game ROM storage
│   ├── Pong.ch8           # Pong ROM
│   ├── Space_Invaders.ch8 # Space Invaders ROM
│   └── Tetris.ch8         # Tetris ROM
└── SDL2/                  # SDL2 development framework
    ├── include/           # SDL2 header files
    └── lib/               # SDL2 static libraries 
```

## ⚙️ Specifications

* **Language**: C
* **Graphics & Input**: SDL2 (Simple DirectMedia Layer)
* **Memory**: 4096 bytes (4KB)
* **Registers**: 16x 8-bit general-purpose registers (`V0` - `VF`), one 16-bit index register (`I`)
* **Stack**: 16 levels of nested subroutines
* **Display**: 64 x 32 monochrome pixels (Scaled up 10x for modern monitors)
* **Timers**: 60Hz Delay timer and Sound timer

## ⌨️ Key Mapping

| Original CHIP-8 Keypad | Modern Keyboard Mapping |
| :---: | :---: |
| `1` `2` `3` `C` | `1` `2` `3` `4` |
| `4` `5` `6` `D` | `Q` `W` `E` `R` |
| `7` `8` `9` `E` | `A` `S` `D` `F` |
| `A` `0` `B` `F` | `Z` `X` `C` `V` |

## CHIP-8 Instruction Set

| Symbol | Description |
|--------|------------|
| N      | 4-bit value (0 ~ 15) |
| NN     | 8-bit value (0 ~ 255) |
| NNN    | 12-bit address (0 ~ 4095) |
| vx, vy | Registers (V0 ~ VF) |
| i      | Memory index register |

| No | Machine Code |  Instruction        | Description |
|----|-------------|------------------------|-------------|
| 1  | 00E0        | clear                  | Clear the display |
| 2  | 00EE        | return                 | Return from subroutine |
| 3  | 1NNN        | jump NNN               | Jump to address NNN |
| 4  | 2NNN        | call NNN               | Call subroutine at NNN |
| 5  | 3XNN        | if vx == NN then       | Skip next if vx == NN |
| 6  | 4XNN        | if vx != NN then       | Skip next if vx != NN |
| 7  | 5XY0        | if vx == vy then       | Skip next if vx == vy |
| 8  | 6XNN        | vx := NN               | Set vx = NN |
| 9  | 7XNN        | vx += NN               | Set vx = vx + NN |
| 10 | 8XY0        | vx := vy               | Set vx = vy |
| 11 | 8XY1        | vx \|= vy              | Set vx = vx OR vy |
| 12 | 8XY2        | vx &= vy               | Set vx = vx AND vy |
| 13 | 8XY3        | vx ^= vy               | Set vx = vx XOR vy |
| 14 | 8XY4        | vx += vy               | vx += vy, vf = carry |
| 15 | 8XY5        | vx -= vy               | vx -= vy, vf = NOT borrow |
| 16 | 8XY6        | vx >>= 1               | Shift right, vf = LSB |
| 17 | 8XY7        | vx = vy - vx           | vx = vy - vx, vf = NOT borrow |
| 18 | 8XYE        | vx <<= 1               | Shift left, vf = MSB |
| 19 | 9XY0        | if vx != vy then       | Skip next if vx != vy |
| 20 | ANNN        | i := NNN               | Set i = NNN |
| 21 | BNNN        | jump0 NNN              | Jump to NNN + v0 |
| 22 | CXNN        | vx := random & NN      | Random AND NN |
| 23 | DXYN        | sprite vx vy N         | Draw sprite, vf = collision |
| 24 | EX9E        | if key == vx then      | Skip if key == vx |
| 25 | EXA1        | if key != vx then      | Skip if key != vx |
| 26 | FX07        | vx := delay            | vx = delay timer |
| 27 | FX0A        | vx := key              | Wait for key press |
| 28 | FX15        | delay := vx            | Set delay timer |
| 29 | FX18        | sound := vx            | Set sound timer |
| 30 | FX1E        | i += vx                | i += vx |
| 31 | FX29        | i := hex vx            | Set i to font sprite |
| 32 | FX33        | bcd vx                 | Store BCD of vx |
| 33 | FX55        | save v0-vx             | Store v0 to vx in memory |
| 34 | FX65        | load v0-vx             | Load v0 to vx from memory |

---

## 🚀 Quick start

* ### Get `.exe` file
```bash
    git clone https://github.com/hsungo/Chip8-emulator.git
    cd Chip8-emulator
    make build
```
* ### Start game 
> put `.ch8` file in `ROM/` and modify makefile
```bash
make pong    # play pong game
# keys 1 and Q move left player and 4 and E move right player.

make tetris  # play tetris game
# left rotate with Q, left move with W, right move with E, drop with S

make space   # play space invaders game
# Shoot with W, move with Q and E. Press W to begin a game.

make <game_name> # play the game put in ROM/
```
