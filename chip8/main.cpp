#include <iostream>
#include <chrono>
#include "chip8.h"

#include <GL/glut.h>

//Use chrono high resolution clock if setting framerate
typedef std::chrono::high_resolution_clock Clock;

//#DEFINES
#define DEBUGPRINT

//PROGRAM INFORMATION
//0x000 - 0x1FF - This would be the chip 8 interpreter but doesn't really matter, keep with history and leave empty
//0x000 - 0x050 - Use this for the built in 4x5 pixel font set(0 - F)
//0x200 - 0xFFF - Program ROM and work RAM

//Main class
chip8 myChip8;

bool SetupGraphics() 
{ 
    return true;
};
void SetupInput() { };
void DrawGraphics() { };

void runLoop()
{
    //Frames per second, TODO: Make this variable in the future (Probably default to 60)
    const unsigned int frameRate = 60;

    std::chrono::nanoseconds deltaTime;
    std::chrono::nanoseconds accumulator = std::chrono::seconds(0);
    std::chrono::nanoseconds frameTime = std::chrono::nanoseconds(1000000000 / frameRate);

    //Store this outside for loop so value keeps
    std::chrono::time_point<Clock> oldTime = Clock::now();
    //Emulation loop
    //Don't use vsync in this solution yet, TODO: just keep track of the time and run at 60Hz
    for (;;)
    {
        deltaTime = Clock::now() - oldTime;
        oldTime = Clock::now();

        accumulator += deltaTime;

        while (accumulator.count() > frameTime.count())
        {
#ifdef DEBUGPRINT
            //This will affect the framerate timing
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(accumulator);
            std::cout << "Frame timer: " << ms.count() << std::endl;
#endif // DEBUGPRINT

            accumulator -= frameTime;

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
    }
}

int main(int argc, char** argv)
{
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

    //Init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(640, 320);
    glutCreateWindow("Chip-8 Emulator");

    //Set up render system and register input callbacks
    if (!SetupGraphics())
    {
        return 0;
    }
    SetupInput();

    glutDisplayFunc(runLoop);
    glutIdleFunc(runLoop);

    glutMainLoop();

    return 0;
}