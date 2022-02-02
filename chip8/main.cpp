#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include "chip8.h"
#include <windows.h>
#include <GL/glut.h>

//Use chrono high resolution clock if setting framerate
typedef std::chrono::high_resolution_clock Clock;

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

//Input keys (Compile with defaults)
static const unsigned int NUM_KEYS = 16;
unsigned char keyConfig[NUM_KEYS] = { '1', '2', '3', '4' , 'q', 'w', 'e', 'r', 'a', 's', 'd', 'f', 'z', 'x', 'c', 'v' };

//Timer Variables
//Frames per second, TODO: Make this variable in the future (Probably default to 60)
unsigned int frameRate = 600;  //don't make this const framerate could change

std::chrono::nanoseconds deltaTime;
std::chrono::nanoseconds accumulator;
std::chrono::nanoseconds frameTime;
std::chrono::time_point<Clock> oldTime;


void SetupInput(char* configPath);
void SetupGraphics();
void reshape_window(GLsizei w, GLsizei h);
void keyboardUp(unsigned char key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);

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
        //Emulate one cycle
        myChip8.EmulateCycle();

#ifdef DEBUGPRINT
        //This will affect the framerate timing
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(accumulator);
        std::cout << "Frame timer: " << std::dec << ms.count() << " ms";
#endif // DEBUGPRINT

        accumulator -= frameTime;

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

//TODO: Change the input arugoments to have selectors (like -f for framerate) so you don't need all of them
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

    //Optional input configuration file
    if (argc > 2)
    {
        SetupInput(argv[2]);
    }

    //Optioanl framerate override
    if (argc > 3)
    {
        //Update the framerate
        frameRate = atoi(argv[3]);
    }

    //Init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(display_width, display_height);
    glutCreateWindow("Chip-8 Emulator");

    //Set up render system and register input callbacks
    SetupGraphics();

    glutDisplayFunc(runLoop);
    glutIdleFunc(runLoop);
    glutReshapeFunc(reshape_window);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();

    return 0;
}

//Read in the config file for input keys, for now just assume every input is here in order
//TODO: Something fancy with pairs
void SetupInput(char* configPath)
{
    std::ifstream infile(configPath);

    //Override the default
    if (infile.is_open())
    {
        std::string line;
        unsigned char index = 0;
        while (std::getline(infile, line))
        {
            //This is a bit hacky but it works
            std::stringstream line_stream(line);

            std::string substr;
            getline(line_stream, substr, ',');
            getline(line_stream, substr, ',');  //Second paramter is input key

            keyConfig[index] = *substr.c_str();
            index++;
        }
        infile.close();
    }
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

    //Set the frame timer based on the frameRate
    frameTime = std::chrono::nanoseconds(1000000000 / frameRate);
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

//Keyboard input pretty much from the emulator tutorial code as well, pretty simple working design
void keyboardDown(unsigned char key, int x, int y)
{
    //Escape to exit
    if (key == 27)
    {
        exit(0);
    }

    if (key == keyConfig[0])        myChip8.key[0x1] = 1;
    else if (key == keyConfig[1])	myChip8.key[0x2] = 1;
    else if (key == keyConfig[2])	myChip8.key[0x3] = 1;
    else if (key == keyConfig[3])	myChip8.key[0xC] = 1;

    else if (key == keyConfig[4])	myChip8.key[0x4] = 1;
    else if (key == keyConfig[5])	myChip8.key[0x5] = 1;
    else if (key == keyConfig[6])	myChip8.key[0x6] = 1;
    else if (key == keyConfig[7])	myChip8.key[0xD] = 1;

    else if (key == keyConfig[8])	myChip8.key[0x7] = 1;
    else if (key == keyConfig[9])	myChip8.key[0x8] = 1;
    else if (key == keyConfig[10])	myChip8.key[0x9] = 1;
    else if (key == keyConfig[11])	myChip8.key[0xE] = 1;

    else if (key == keyConfig[12])	myChip8.key[0xA] = 1;
    else if (key == keyConfig[13])	myChip8.key[0x0] = 1;
    else if (key == keyConfig[14])	myChip8.key[0xB] = 1;
    else if (key == keyConfig[15])	myChip8.key[0xF] = 1;
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (key == keyConfig[0])        myChip8.key[0x1] = 0;
    else if (key == keyConfig[1])	myChip8.key[0x2] = 0;
    else if (key == keyConfig[2])	myChip8.key[0x3] = 0;
    else if (key == keyConfig[3])	myChip8.key[0xC] = 0;

    else if (key == keyConfig[4])	myChip8.key[0x4] = 0;
    else if (key == keyConfig[5])	myChip8.key[0x5] = 0;
    else if (key == keyConfig[6])	myChip8.key[0x6] = 0;
    else if (key == keyConfig[7])	myChip8.key[0xD] = 0;

    else if (key == keyConfig[8])	myChip8.key[0x7] = 0;
    else if (key == keyConfig[9])	myChip8.key[0x8] = 0;
    else if (key == keyConfig[10])	myChip8.key[0x9] = 0;
    else if (key == keyConfig[11])	myChip8.key[0xE] = 0;

    else if (key == keyConfig[12])	myChip8.key[0xA] = 0;
    else if (key == keyConfig[13])	myChip8.key[0x0] = 0;
    else if (key == keyConfig[14])	myChip8.key[0xB] = 0;
    else if (key == keyConfig[15])	myChip8.key[0xF] = 0;
}