#include <cstring>
#include <stdio.h>

#include "chip8.h"

chip8::chip8() : 
	memory{0},
	V{0},
	gfx{0},
	stack{0},
	sp(0),
	opcode(0),
	I(0),
	pc(0),
	delay_timer(0),
	sound_timer(0),
	drawFlag(0),
	key{0}
{
	//constructor
}

chip8::~chip8()
{
	//destructor
}

void chip8::Initialize()
{
	//Initialize memory and registers
	pc = 0x200;

	opcode = 0;
	I      = 0;
	sp     = 0;

	//Load font into memory (5 X 16 = 80)
	memcpy(memory, chip8_fontset, sizeof(char) * 80);

	// This is a test remove later - 
	// Assume the following:
	memory[pc] = 0xA2;
	memory[pc + 1] = 0xF0;
}

void chip8::LoadGame(char* fileName)
{
	printf("fileName %s\n", fileName);


}

void chip8::EmulateCycle()
{
	//Load opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	//Decode opcode 
	//https://en.wikipedia.org/wiki/CHIP-8 has a list of the opcodes
	switch (opcode & 0xF000)
	{
		//Special opcodes
		case 0x0000:
			switch (opcode & 0x000F)
			{
				case 0x0000: //0x00E0: Clears the screen        
					//Execute opcode
					break;

				case 0x000E: //0x00EE: Returns from subroutine          
					//Execute opcode
					break;

				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
					break;
			}
			break;

		case 0x2000: //2NNN: calls subroutine at NNN
			stack[sp] = pc; //store the pc in stack and increment stack pointer index
			sp++;
			pc = opcode & 0xFFF; //set the PC to NNN
			

		case 0xA000: //ANNN: Sets Index register to the address NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;



		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
	}

}

void chip8::SetKeys()
{

}