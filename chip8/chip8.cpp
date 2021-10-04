// chip8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

const unsigned int SYSTEM_MEMORY  = 4096;
const unsigned int SYSEM_REGISTER = 16;

const unsigned int SCREEN_WIDTH  = 64;
const unsigned int SCREEN_HEIGHT = 32;

//0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
//0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
//0x200 - 0xFFF - Program ROM and work RAM

int main()
{
    //System meory and registers
    unsigned char  memory[SYSTEM_MEMORY];
    unsigned char  V[SYSEM_REGISTER];
    unsigned char  gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; //64 * 32 = 2048

    //Stack information (size = 16 deep)
    unsigned short stack[16];
    unsigned short sp;

    //Current opcode
    unsigned short opcode;
    
    //Index register and Program counter
    unsigned short I;
    unsigned short PC;

    //Timer registers
    unsigned char delay_timer;
    unsigned char sound_timer;

    //keypad input
    unsigned char key[16];

    std::cout << "Hello World!\n";

    return 0;
}
