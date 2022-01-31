#include <string>
#include <stdio.h>
#include <iostream>
#include <iomanip>

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

    std::cout << "opcode: " << std::setw(4) << std::hex << opcode << " - ";

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
            case 0x0000: //0x00E0: Clears the screen (No other 0x0NNN instruction ends in 0 so just check for this)
            {
                memset(gfx, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(char));
                drawFlag = true;
                pc += 2;
                break;
            }
            case 0x000E: //0x00EE: Returns from subroutine        
            {
                sp--;
                pc = stack[sp]; //Set the program counter to the top address on the stack
                pc += 2;        //Increment by 2 or we'll be stuck in an infinite loop
                break;
            }
            default:
            {
                printf("Unknown opcode: 0x%X\n", opcode);
                break;
            }
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

    case 0x3000: //3XNN Skips the next instruction if V[X] == NN
    {
        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        unsigned short Vx = V[xRegIndex];

        unsigned short NN = opcode & 0x00FF;

        if (Vx == NN)
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    }

    case 0x4000: //4XNN: Skips the next instruction if V[X] != NN
    {
        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        unsigned short Vx = V[xRegIndex];

        unsigned short NN = opcode & 0x00FF;

        if (Vx != NN)
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    }

    case 0x5000: //5XY0: Skips the next instruction if V[X] == V[Y]
    {
        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        unsigned short Vx = V[xRegIndex];

        unsigned short yRegIndex = (opcode & 0x00F0) >> 4;
        unsigned short Vy = V[yRegIndex];

        if (Vx == Vy)
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    }

    case 0x6000: //6XNN: Sets V[X] to NN
    {
        unsigned short regIndex = (opcode & 0x0F00) >> 8; //Shift by byte to get only X value
        unsigned short value    = (opcode & 0x00FF);
        V[regIndex] = value;
        pc += 2;
        break;
    }

    case 0x7000: //0x7XNN: Adds NN to V[X]
    {
        unsigned short NN = opcode & 0x00FF;

        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        V[xRegIndex] += NN;

        pc += 2;
        break;
    }
    
    //There are multiple 0x8NNN opcodes
    case 0x8000:
    {
        switch (opcode & 0x000F)
        {
            case 0x0000: //0x8XY0: Sets the value of V[X] to V[Y] (V[X] = V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                V[xRegIndex] = V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0001: //0x8XY1: Sets the value of Vx to (V[X] OR V[Y]) (V[X] |= V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                V[xRegIndex] |= V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0002: //0x8XY2: Sets the value of Vx to (Vx AND V[Y]) (Vx & V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                V[xRegIndex] &= V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0003: //0x8XY3: Sets the value of Vx to (V[X] XOR V[Y]) (V[X] ^= V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                V[xRegIndex] ^= V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0004: //0x8XY4: Sets the value of Vx to (V[X] + V[Y]) (V[X] += V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                //Check for overflow
                if ((V[xRegIndex] + V[yRegIndex]) > 0xFF)
                {
                    V[0xF] = 1;
                }
                else
                {
                    V[0xF] = 0;
                }
                
                V[xRegIndex] += V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0005: //0x8XY5: Sets the value of Vx to (V[X] - V[Y]) (V[X] -= V[Y])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                unsigned short yRegIndex = (opcode & 0x00F0) >> 4;

                //Check for underflow
                if (V[yRegIndex] > V[xRegIndex] )
                {
                    V[0xF] = 0;
                }
                else
                {
                    V[0xF] = 1;
                }

                V[xRegIndex] -= V[yRegIndex];

                pc += 2;
                break;
            }
            case 0x0006: //0x8XY6: Store the LSB of Vx in V[0xF] and shift Vx to the right by 1 (V[X] >>= 1)
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;

                V[0xF] = V[xRegIndex] & 0x01;

                V[xRegIndex] >>= 1;

                pc += 2;
                break;
            }
            case 0x000E: //0x8XYE: Store the MSB of Vx in V[0xF] and shift Vx to the left by 1 (V[X] <<= 1)
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                
                V[0xF] = V[xRegIndex] & 0x80;
                
                V[xRegIndex] <<= 1;

                pc += 2;
                break;
            }
            default:
            {
                printf("Unknown opcode: 0x%X\n", opcode);
                break;
            }
        }
        break;
    }

    case 0x9000: //0x9XY0: Skips the next instruction if V[X] != V[Y]
    {
        unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
        unsigned short Vx = V[xRegIndex];

        unsigned short yRegIndex = (opcode & 0x00F0) >> 4;
        unsigned short Vy = V[yRegIndex];

        if (Vx != Vy)
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    }

    case 0xA000: //ANNN: Sets Index register to the address NNN
    {
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    }

    case 0xD000: //DXYN: Draw sprite at index V[X], V[Y] of height N (Always 8 pixel wide)
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

    //TODO:
    case 0xE000:
    {
        switch (opcode & 0x000F)
        {
            case 0x000E: //0xEX9E: Skips the next instruction if key() == Vx
                //Execute opcode
                break;

            case 0x0001: //0xEXA1: Skips the next instruction if key() != Vx  
                //Execute opcode
                break;

            default:
                printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
                break;
        }
        break;
    }

    //TODO: F155
    case 0xF000:
    {
        switch (opcode & 0x00FF)
        {
            case 0x0015: //0xFX15: Set the delay timer to V[X]
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;

                delay_timer = V[xRegIndex];

                pc += 2;
                break;
            }
            case 0x0018: //0xFX15: Set the sound timer to V[X]
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;

                sound_timer = V[xRegIndex];

                pc += 2;
                break;
            }
            case 0x0055: //0xFX55: Store V[0] to V[x] (including V[X]) in memory starting at i
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;

                memcpy(memory + I, V, (xRegIndex + 1) * sizeof(char));

                pc += 2;
                break;
            }
            case 0x0033: //0xFX55: Store binary coded decimal of V[X] at I
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;
                //137
                unsigned char x1 = V[xRegIndex] / 100;        //Char only goes to 255
                unsigned char x2 = (V[xRegIndex] / 10) % 10;  //Remove last place and mod 10
                unsigned char x3 = (V[xRegIndex] % 100) % 10; //Mod by 100 and then 10

                memory[I]     = x1;
                memory[I + 1] = x2;
                memory[I + 2] = x3;

                //memcpy(memory + I, V, (xRegIndex + 1) * sizeof(char));

                pc += 2;
                break;
            }
            case 0x0065: //0xFX55: Copy memory at I V[0] to V[X] (including V[X])
            {
                unsigned short xRegIndex = (opcode & 0x0F00) >> 8;

                memcpy(V, memory + I, (xRegIndex + 1) * sizeof(char));

                pc += 2;
                break;
            }
            default:
            {
                printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
                break;
            }
            break;
        }
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