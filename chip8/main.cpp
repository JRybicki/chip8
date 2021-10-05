#include <iostream>

#include "chip8.h"

//0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
//0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
//0x200 - 0xFFF - Program ROM and work RAM

void SetupGraphics() { };
void SetupInput() { };
void DrawGraphics() { };

int main()
{
    // Set up render system and register input callbacks
    SetupGraphics();
    SetupInput();

    chip8 myChip8;

    // Initialize the Chip8 system and load the game into the memory  
    myChip8.Initialize();
    myChip8.LoadGame(); //game name

    // Emulation loop
    for (;;)
    {
        // Emulate one cycle
        myChip8.EmulateCycle();

        // If the draw flag is set, update the screen
        if (myChip8.GetDrawFlag())
        {
            DrawGraphics();
        }

        // Store key press state (Press and Release)
        myChip8.SetKeys();
    }

    return 0;
}