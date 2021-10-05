#include <cstring>

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

}

void chip8::LoadGame()
{

}

void chip8::EmulateCycle()
{

}

void chip8::SetKeys()
{

}