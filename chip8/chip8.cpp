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
	PC(0),
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
	PC = 0x200;


	// This is a test remove later - 
	// Assume the following:
	memory[PC] = 0xA2;
	memory[PC + 1] = 0xF0;
}

void chip8::LoadGame()
{

}

void chip8::EmulateCycle()
{
	//Load opcode
	opcode = memory[PC] << 8 | memory[PC + 1];

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

		case 0xA000: //ANNN: Sets Index register to the address NNN
			I = opcode & 0x0FFF;
			PC += 2;
			break;



		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
	}

}

void chip8::SetKeys()
{

}