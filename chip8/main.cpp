#include <iostream>
#include <chrono>
#include "chip8.h"
#include <windows.h>
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

//Screen buffer for openGL
static const unsigned int SCREEN_WIDTH  = 64;
static const unsigned int SCREEN_HEIGHT = 32;
unsigned char screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3];
unsigned int display_width  = SCREEN_WIDTH  * 10;
unsigned int display_height = SCREEN_HEIGHT * 10;

//Timer Variables
//Frames per second, TODO: Make this variable in the future (Probably default to 60)
static const unsigned int frameRate = 120;

std::chrono::nanoseconds deltaTime;
std::chrono::nanoseconds accumulator;
std::chrono::nanoseconds frameTime = std::chrono::nanoseconds(1000000000 / frameRate); //don't make this const framerate could change
std::chrono::time_point<Clock> oldTime;


void SetupInput() { };
void SetupGraphics();
void reshape_window(GLsizei w, GLsizei h);

void updateTexture()
{
    // Update pixels
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (myChip8.gfx[(y * 64) + x] == 0)
            {
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;	// Disabled
            }
            else
            {
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled
            }
        }
    }

    // Update Texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);		glVertex2d(0.0, 0.0);
    glTexCoord2d(1.0, 0.0); 	glVertex2d(display_width, 0.0);
    glTexCoord2d(1.0, 1.0); 	glVertex2d(display_width, display_height);
    glTexCoord2d(0.0, 1.0); 	glVertex2d(0.0, display_height);
    glEnd();
}

void DrawGraphics() 
{
    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT);

    updateTexture();

    // Swap buffers!
    glutSwapBuffers();

    myChip8.ResetDrawFlag();
};

void runLoop()
{
    //Emulation loop
    //Don't use vsync in this solution yet, TODO: just keep track of the time and run at 60Hz
    deltaTime = Clock::now() - oldTime;
    oldTime = Clock::now();

    accumulator += deltaTime;

    while (accumulator.count() > frameTime.count())
    {
        accumulator -= frameTime;

        //Emulate one cycle
        myChip8.EmulateCycle();

#ifdef DEBUGPRINT
        //This will affect the framerate timing
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(accumulator);
        std::cout << "Frame timer: " << std::dec << ms.count();
#endif // DEBUGPRINT

        //If the draw flag is set, update the screen
        if (myChip8.GetDrawFlag())
        {
            DrawGraphics();

#ifdef DEBUGPRINT
            std::cout << " - Update Screen";
#endif // DEBUGPRINT

        }

#ifdef DEBUGPRINT
        std::cout << std::endl;  //flush the buffer
#endif // DEBUGPRINT

        //Store key press state (Press and Release)
        myChip8.SetKeys();
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
    glutInitWindowSize(display_width, display_height);
    glutCreateWindow("Chip-8 Emulator");

    //Set up render system and register input callbacks
    SetupGraphics();
    SetupInput();

    glutDisplayFunc(runLoop);
    glutIdleFunc(runLoop);
    glutReshapeFunc(reshape_window);

    glutMainLoop();

    return 0;
}

//Setup Texture (this code and the reshape_window is really from the how to write an emulator example)
void SetupGraphics()
{
    //Clear screen
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            screenData[y][x][0] = 0;
            screenData[y][x][1] = 0;
            screenData[y][x][2] = 0;
        }
    }

    //Create a texture 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

    //Set up the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //Enable textures
    glEnable(GL_TEXTURE_2D);

    //Setup accumulator to 0 so on first main call we draw a frame
    accumulator = std::chrono::seconds(0);
    oldTime = Clock::now();
}

void reshape_window(GLsizei w, GLsizei h)
{
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

    // Resize quad
    display_width = w;
    display_height = h;
}