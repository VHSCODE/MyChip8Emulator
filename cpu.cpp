#include "cpu.h"

CPU::CPU()
{

    init_ram();
    init_gfx();

    //Load the fontset into memory
    for(long i = 0L; i < 80; i++){

        memory[i] = chip8_fontset[i];
    }

    I = 0;
    sp = 0;

    for(int j = 0; j < 16; ++j)
            stack[j] = 0;

    for(int i = 0; i < 16; ++i)
        keys[i] = V[i] = 0;
    this->pc = 0x200; //512

    this->delay_timer= 0, this->sound_timer = 00;

    running = true;

    draw_flag = true;
    logger = Logger();

    start_log();
}


void CPU::start_log()
{
    logger.start_log();
}
void CPU::end_log()
{
    logger.end_log();
    logger.save_log_to_file();
}


void CPU::draw_screen()
{
    uint32_t pixels[2048];

    SDL_RenderClear(renderer);
    //Code snippet taken from https://github.com/JamesGriffin/CHIP-8-Emulator/blob/master/src/main.cpp. Ty James
    for (int i = 0; i < 2048; ++i) {
        uint8_t pixel = this->gfx[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }
    // Update SDL texture
    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
    // Clear screen and render

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);


}



void CPU::update_timers()
{
        if(delay_timer > 0)
            --delay_timer;

        if(sound_timer > 0)
        {
            if(sound_timer == 1)
                printf("BEEP!\n");
            --sound_timer;
        } sound_timer = 60;
}
void CPU::init_ram()
{
    for(long i = 0L; i < 4096; i++){

        memory[i] = 0;
    }
}
void CPU::init_gfx()
{

    if(SDL_Init(SDL_INIT_EVERYTHING)!=0){

        std::cout << "Error while loading SDL" << std::endl;
    }
    window = SDL_CreateWindow("Chip8Emulator",0,0,640,320, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                64, 32);

    SDL_RenderSetScale(renderer,10,10);
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    for(int x = 0; x < 64 * 32; x++)
            this->gfx[x] = 0;
}



void CPU::emulateCycle()
{

    opcode = this->memory[pc] << 8 | this->memory[pc+1];

    std::stringstream  hexOpcode;
    hexOpcode <<  "0x"<< std::hex << opcode;

    logger.update("Current Opcode:" + hexOpcode.str());
    switch (opcode & 0xF000)
    {
        case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000: //Clear display
            for(int x = 0; x < 64 * 32; x++)
                    gfx[x] = 0x0;

            draw_flag = true;
            pc+=2;
            break;

        case 0x000E: //Return from subroutine
            --sp;
            pc = stack[sp];
            pc+=2;
            break;
        }
        break;

    case 0x1000: //Jump to location
        pc = opcode & 0x0FFF;
        break;
    case 0x2000: //Jump to subroutine
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;

    case 0x3000: //If V[x] == kk, skip next instruction
        if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
            pc +=4;
            break;
        }
         pc+=2;
        break;

    case 0x4000: //If V[x] != kk, skip next instruction
        if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
            pc +=4;
            break;
        }
         pc+=2;
        break;

    case 0x5000: // if V[x] == V[y], skip next instruction
        if(V[(opcode & 0x0F00) >> 8] == (V[(opcode & 0x00F0) >> 4 ])){
            pc +=4;
            break;
        }
         pc+=2;
        break;

    case 0x6000: // Set V[x] to kk
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc+=2;
        break;


    case 0x7000: // Add kk to V[x]
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc+=2;
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {

        case 0x0000: // Set V[x] to V[y]
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4 ]);
            pc+=2;
            break;
        case 0x0001: // Set V[x] to V[x] OR V[y]
            V[(opcode & 0x0F00) >> 8] |= (V[(opcode & 0x00F0) >> 4 ]);
            pc+=2;
            break;
        case 0x0002: // Set V[x] to V[x] AND V[y]
            V[(opcode & 0x0F00) >> 8] &= (V[(opcode & 0x00F0) >> 4 ]);
            pc+=2;
            break;
        case 0x0003: // Set V[x] to V[x] XOR V[y]
            V[(opcode & 0x0F00) >> 8] ^= (V[(opcode & 0x00F0) >> 4 ]);
            pc+=2;
            break;
        case 0x0004: // Set V[x] to V[x] + V[y]. If the result is greater than 8 bits , VF is set to 1, otherwise 0
            if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1; //carry
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0005: // Set Vx = Vx - Vy, set VF = NOT borrow, If Vx > Vy
            if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 0; // there is a borrow
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0006: // Set Vx = Vx SHR 1
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
            break;
        case 0x0007:  // Set Vx = Vy - Vx, set VF = NOT borrow If Vy > Vx
            if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])	// VY-VX
                V[0xF] = 0; // there is a borrow
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x000E: // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
            break;
        }
        break;
    case 0x9000:
        if(V[(opcode & 0x0F00) >> 8] != (V[(opcode & 0x00F0) >> 4 ])){
            pc +=4;
            break;
        }
         pc+=2;
        break;
    case 0xA000:
        I = opcode & 0x0FFF;
        pc+=2;
        break;
    case 0xB000:
        pc = V[0x0] + (opcode & 0x0FFF);
        break;
    case 0xC000:

        uint8_t rand;
        {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist255(0,255);
        rand = dist255(rng);
        }
        V[(opcode & 0x0F00) >> 8] = rand & (opcode & 0x00FF);
        pc+=2;
        break;

    case 0xD000:
    {
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x000F;
        unsigned short pixel;

        V[0xF] = 0;
        for (int yline = 0; yline < height; yline++)
        {
            pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++)
            {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                    {
                        V[0xF] = 1;
                    }
                    gfx[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
        }

        draw_flag = true;
        pc += 2;
    }break;
    case 0xE000:
        switch (opcode & 0x000F)
        {
            case 0x000E: // Skip next instruction if key with the value of Vx is pressed.
                if(keys[V[(opcode & 0x0F00) >> 8]] == 1)
                    pc+=4;
                else
                    pc+=2;
                break;
            case 0x0001: //  Skip next instruction if key with the value of Vx is not pressed.
            if(keys[V[(opcode & 0x0F00) >> 8]] == 0)
                pc+=4;
            else
                pc+=2;
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
               V[(opcode & 0x0F00) >> 8] = delay_timer;
               pc+=2;
                break;
        case 0x000A:
        {
            bool keyPressed = false;
            for(int i = 0; i < 16; i++)
            {
                if(keys[i] == 1)
                {
                    V[(opcode & 0x0F00) >> 8] = i;
                    keyPressed = true;
                    break;
                }
            }

            if(!keyPressed)
                return;
            pc+=2;
            break;
        }
        case 0x0015:
            delay_timer = V[(opcode & 0x0F00) >> 8];
            pc+=2;
            break;
        case 0x0018:
            sound_timer = V[(opcode & 0x0F00) >> 8];
            pc+=2;
            break;
        case 0x001E:
            if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
                V[0xF] = 1;
            else
                V[0xF] = 0;
            I += V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0029:
            I= V[(opcode & 0x0F00) >> 8] * 0x5;
            pc += 2;
            break;
        case 0x0033:
            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
            pc += 2;
            break;
        case 0x0055:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                memory[I + i] = V[i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        case 0x0065:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                V[i] = memory[I + i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        }

        update_timers();

        std::stringstream  hexOpcode;
        hexOpcode <<  "0x"<< std::hex;

        logger.update("Current Opcode:" + hexOpcode.str());
    }
}


void CPU::handle_key_pressed(SDL_Scancode key_pressed)
{
    switch (key_pressed)
    {
    case SDL_SCANCODE_1:
        keys[0x0] = 1;
        break;
    case SDL_SCANCODE_2:
        keys[0x1] = 1;
        break;
    case SDL_SCANCODE_3:
        keys[0x2] = 1;
        break;
    case SDL_SCANCODE_4:
        keys[0x3] = 1;
        break;
    case SDL_SCANCODE_Q:
        keys[0x4] = 1;
        break;
    case SDL_SCANCODE_W:
        keys[0x5] = 1;
        break;
    case SDL_SCANCODE_E:
        keys[0x6] = 1;
        break;
    case SDL_SCANCODE_R:
        keys[0x7] = 1;
        break;
    case SDL_SCANCODE_A:
        keys[0x8] = 1;
        break;
    case SDL_SCANCODE_S:
        keys[0x9] = 1;
        break;
    case SDL_SCANCODE_D:
        keys[0xA] = 1;
        break;
    case SDL_SCANCODE_F:
        keys[0xB] = 1;
        break;
    case SDL_SCANCODE_Z:
        keys[0xC] = 1;
        break;
    case SDL_SCANCODE_X:
        keys[0xD] = 1;
        break;
    case SDL_SCANCODE_C:
        keys[0xE] = 1;
        break;
    case SDL_SCANCODE_V:
        keys[0xF] = 1;
        break;
    default:
        break;
    }
}

void CPU::handle_key_released(SDL_Scancode key_released)
{
    switch (key_released)
    {
    case SDL_SCANCODE_1:
        keys[0x0] = 0;
        break;
    case SDL_SCANCODE_2:
        keys[0x1] = 0;
        break;
    case SDL_SCANCODE_3:
        keys[0x2] = 0;
        break;
    case SDL_SCANCODE_4:
        keys[0x3] = 0;
        break;
    case SDL_SCANCODE_Q:
        keys[0x4] = 0;
        break;
    case SDL_SCANCODE_W:
        keys[0x5] = 0;
        break;
    case SDL_SCANCODE_E:
        keys[0x6] = 0;
        break;
    case SDL_SCANCODE_R:
        keys[0x7] = 0;
        break;
    case SDL_SCANCODE_A:
        keys[0x8] = 0;
        break;
    case SDL_SCANCODE_S:
        keys[0x9] = 0;
        break;
    case SDL_SCANCODE_D:
        keys[0xA] = 0;
        break;
    case SDL_SCANCODE_F:
        keys[0xB] = 0;
        break;
    case SDL_SCANCODE_Z:
        keys[0xC] = 0;
        break;
    case SDL_SCANCODE_X:
        keys[0xD] = 0;
        break;
    case SDL_SCANCODE_C:
        keys[0xE] = 0;
        break;
    case SDL_SCANCODE_V:
        keys[0xF] = 0;
        break;
    default:
        break;
    }
}








void CPU::shutdown()
{
    running = false;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    end_log();
}




/**
 * @brief CPU::loadRom loads the rom file into memory
 * @param filePath path of rom file
 * @return 0 if the load is succesfull, 1 if not
 */
int CPU::loadRom(char *filePath){

    std::ifstream file;
    std::streampos size;

    file.open(filePath, std::ios::binary | std::ios::ate);

    if(!file.is_open()){
        return 1;
    }

    size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size];

    file.read(buffer,size);

    file.close();

    for(long i = 0L; i < size; i++){

        this->memory[0x200 + i] = buffer[i];
    }

    logger.update("Rom loaded succesfully!");

    delete[] buffer;
    return 0;

}
