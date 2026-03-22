game = ./chip8.exe ROM/

build:
	gcc main.c chip8.c -o chip8.exe -I ./SDL2/include -L ./SDL2/lib -lmingw32 -lSDL2main -lSDL2
clean:
	rm *.exe

pong:
	${game}Pong.ch8
tetris:
	${game}Tetris.ch8
space:
	${game}Space_Invaders.ch8
