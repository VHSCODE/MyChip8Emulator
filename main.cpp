#include <iostream>
#include "cpu.h"
#include<chrono>
#include <thread>
#include "SDL2/SDL.h"

void init_sdl();

int main()
{
    CPU emu =  CPU();

    if(emu.loadRom("INVADERS.ch8") != 0)
        return -1;

    while( emu.isRunning())
    {
        emu.emulateCycle();
        SDL_Event e;

        if(SDL_PollEvent(&e))
        {
            switch (e.type) {
               case SDL_QUIT:
                std::cout << "Bye!" << std::endl;
                emu.shutdown();
                break;
            case SDL_KEYDOWN:
                emu.handle_key_pressed(e.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                emu.handle_key_released(e.key.keysym.scancode);
                break;
            default:
                break;
            }
        }

        if(emu.draw() && emu.isRunning()) //In case we exit the emulator and the draw flag is set, we check if the emu will be running in the next iteration.
        {
            emu.draw_screen();
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }
    return 0;
}
