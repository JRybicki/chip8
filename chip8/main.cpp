#include <iostream>

#include "chip8.h"

//0x000 - 0x1FF - This would be the chip 8 interpreter but doesn't really matter, keep with history and leave empty
//0x000 - 0x050 - Use this for the built in 4x5 pixel font set(0 - F)
//0x200 - 0xFFF - Program ROM and work RAM

void SetupGraphics() { };
void SetupInput() { };
void DrawGraphics() { };

int main(int argc, char** argv)
{
    //Set up render system and register input callbacks
    SetupGraphics();
    SetupInput();

    chip8 myChip8;

    //Load the game passed into program
    if (argc > 1)
    {
        //Initialize the Chip8 system and load the game into the memory  
        myChip8.Initialize();

        //Error if game can't be read, probably don't quit the program in a final version
        if (!myChip8.LoadGame(argv[1])) //game name
        {
            return 0;
        }
    }
    else
    {
        //No input given for the rom
        return 0;
    }


    //Emulation loop
    //Don't use vsync in this solution, TODO: just keep track of the time and run at 60Hz
    for (;;)
    {
        //Emulate one cycle
        myChip8.EmulateCycle();

        //If the draw flag is set, update the screen
        if (myChip8.GetDrawFlag())
        {
            DrawGraphics();
        }

        //Store key press state (Press and Release)
        myChip8.SetKeys();
    }

    return 0;
}