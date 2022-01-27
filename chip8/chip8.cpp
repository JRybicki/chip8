#include <string>
#include <stdio.h>
#include <iostream>

#include "chip8.h"

chip8::chip8() :
    memory{ 0 },
    V{ 0 },
    gfx{ 0 },
    stack{ 0 },
    sp(0),
    opcode(0),
    I(0),
    pc(0),
    delay_timer(0),
    sound_timer(0),
    drawFlag(0),
    key{ 0 }
{
    //constructor
}

chip8::~chip8()
{
    //destructor
}

//Initialize memory and registers
void chip8::Initialize()
{
    //ROM is loaded at address 0x200
    pc = 0x200;

    opcode = 0;
    I = 0;
    sp = 0;

    //Load font into memory (5 X 16 = 80)
    memcpy(memory, chip8_fontset, sizeof(char) * 80);

    // This is a test remove later - 
    // Assume the following:
    memory[pc] = 0xA2;
    memory[pc + 1] = 0xF0;
}

bool chip8::LoadGame(char* romPath)
{
    //Load the file into memory at the program counter (0x0020)
    FILE* romFP = fopen(romPath, "rb");
    if (!romFP)
    { 
        printf("Error: could not open file %s\n", romPath);
        return false;
    }

    unsigned int bytesRead = fread(&memory[pc], sizeof(char), sizeof(memory), romFP);

    if (bytesRead < 0)
    {
        return false;
    }

    printf("fileName: %s opened %d bytes\n", romPath, bytesRead);
    return true;
}

void chip8::EmulateCycle()
{
    //Load opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    std::cout << "opcode: " << std::hex << opcode << " - ";

    //Decode opcode 
    //https://en.wikipedia.org/wiki/CHIP-8 has a list of the opcodes
    //Remember - Stack and PC are unsigned shorts (2 bytes that represent numbers from 0x0000 to OxFFFF)
    //Using brackets on case statement to scope variables to help debugging
    switch (opcode & 0xF000)
    {
    //Special opcodes
    case 0x0000:
    {
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
    }

    case 0x1000: //1NNN: Go to address NNN
    {
        pc = opcode & 0x0FFF;
        break;
    }

    case 0x2000: //2NNN: calls subroutine at NNN
    {
        stack[sp] = pc; //store the pc in stack and increment stack pointer index
        sp++;
        pc = opcode & 0x0FFF; //set the PC to NNN
        break;
    }

    case 0x6000: //6XNN: Sets VX to NN
    {
        unsigned short regIndex = (opcode & 0x0F00) >> 8; //Shift by byte to get only X value
        unsigned short value    = opcode & 0x00FF;
        V[regIndex] = value;
        pc += 2;
        break;
    }

    case 0xA000: //ANNN: Sets Index register to the address NNN
    {
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    }

    //0xD8B4
    case 0xD000: //DXYN: Draw sprite at index Vx, Vy of height N (Always 8 pixel wide)
    {
        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

        unsigned short Vx = V[xRegIndex];
        unsigned short Vy = V[yRegIndex];
        unsigned short N  = (opcode & 0x000F);

        //Note: VF is set to 1 if pixels are turned on and 0 if turned off
        V[0xF] = 0;

        unsigned short pixelValue = 0;

        //Note: N is height
        for (unsigned short yIndex = 0; yIndex < N; yIndex++)
        {
            pixelValue = memory[I + yIndex];
            //Always 8 pixels wide
            for (unsigned short xIndex = 0; xIndex < 8; xIndex++)
            {
                //Check if this pixel should be set according to memory (shift xIndex to match bit values in short)
                if((pixelValue & (0x80 >> xIndex)) != 0)
                {
                    //Check if the pixel value is already set to true or not
                    unsigned short pixelIndex = (Vx + xIndex) + ((Vy + yIndex) * SCREEN_WIDTH);
                    if (gfx[pixelIndex] == 1)
                    {
                        V[0xF] = 1; //Collision of sprites, another sprite is already in this location
                    }
                    gfx[pixelIndex] ^= 1; //XOR current value with new value, this will blank out double writes
                }
            }
        }
        //Set the screen redraw update flag
        drawFlag = true;
        pc += 2;
        break;
    }

    default:
    {
        printf("Unknown opcode: 0x%X\n", opcode);
        break;
    }
    }
}

void chip8::SetKeys()
{

}