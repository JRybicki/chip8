#pragma once
class chip8
{
public:

    chip8();
    ~chip8();

    void Initialize();
    void LoadGame();

    void EmulateCycle();

    bool GetDrawFlag() { return drawFlag; }

    void SetKeys();

private:
    //Some system settings variables
    static const unsigned int SYSTEM_MEMORY = 4096;
    static const unsigned int SYSEM_REGISTER = 16;

    static const unsigned int SCREEN_WIDTH = 64;
    static const unsigned int SCREEN_HEIGHT = 32;

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
    
    //Update scren flag
    bool drawFlag;
};