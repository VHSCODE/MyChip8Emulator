#ifndef CPU_H
#define CPU_H


#include<cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <iomanip>
#include <sstream>
#include <string>
#include "logger.h"
#include "SDL2/SDL.h"

class CPU
{
public:
    CPU();

    int loadRom(char* filePath);

    bool isRunning() {return running;}

    void shutdown();


    void start_log();
    void end_log();
    void emulateCycle();

    void handle_key_pressed(SDL_Scancode key_pressed);
    void handle_key_released(SDL_Scancode key_released);

    bool draw() {return draw_flag;}

    void draw_screen();


private:

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

    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short stack[16];
    unsigned short sp;

    unsigned short pc;
    unsigned char delay_timer, sound_timer;

    unsigned char keys[16];

    unsigned char gfx[64* 32];

    unsigned short opcode;

    bool running;
    bool draw_flag;


    void init_ram();
    void init_gfx();



    void update_timers();
    Logger logger;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture * texture = nullptr;


};

#endif // CPU_H
