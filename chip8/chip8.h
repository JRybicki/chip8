#pragma once

//#DEFINES
#define DEBUGPRINT

class chip8
{
public:

    chip8();
    ~chip8();

    void Initialize();
    bool LoadGame(char* romPath);

    void EmulateCycle();

    bool GetDrawFlag() { return drawFlag; }
    void ResetDrawFlag() { drawFlag = false; }

    void SetKeys();

    //Make graphics data public for openGL usage
    static const unsigned int SCREEN_WIDTH = 64;
    static const unsigned int SCREEN_HEIGHT = 32;
    unsigned char  gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; //64 * 32 = 2048

    //keypad input
    unsigned char key[16];

private:
    //Some system settings variables
    static const unsigned int SYSTEM_MEMORY = 4096;
    static const unsigned int SYSEM_REGISTER = 16;

    //System meory and registers
    unsigned char  memory[SYSTEM_MEMORY];
    unsigned char  V[SYSEM_REGISTER];

    //Stack information (size = 16 deep)
    unsigned short stack[16];
    unsigned short sp;

    //Current opcode
    unsigned short opcode;

    //Index register and Program counter
    unsigned short I;
    unsigned short pc;

    //Timer registers
    unsigned char delay_timer;
    unsigned char sound_timer;

    //Update scren flag
    bool drawFlag;

    //Hex values for drawing text, these numbers are 4 wide x 5 tall
    unsigned char chip8_fontset[80] =
    {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
};