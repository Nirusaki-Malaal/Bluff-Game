#include <iostream>
#include "client.hpp"
#include <SDL3/SDL.h>
namespace bluff {
    Client::Client() 
    {
        gWindow = nullptr; // the window we will be rendering
        gScreenSurface = nullptr; // the surface in the window
        gHelloWorld = nullptr; // image we will be loading
        // g means global here these variable are global
        std::cout << "Client initialized!\n";
         // NAME Of the game
    }

    bool Client::init() 
    {
        bool success = true;
        if (!SDL_Init(SDL_INIT_VIDEO)) 
        {
            success = false;
            SDL_Log("[SDL] [ERROR] Can't initalize SDL3");
        }
        else 
        {
            gWindow = SDL_CreateWindow(kWindowName, 0, 0, SDL_WINDOW_FULLSCREEN); // 0 , 0 means automatic height and width
            if(gWindow == nullptr) 
            {
                SDL_Log("[SDL] [ERROR] Window could not be created");
                success = false;
            }
            else {
                gScreenSurface = SDL_GetWindowSurface(gWindow); // get window surface
            }
        }
        return success;
    }

    int Client::run() { // client class function run
    std::cout << "Client running...\n";
        if(!init()) {
            std::cerr << "Initialization Failed";
            return 1;
        }
        // creating an event loop which will look this thing
        bool is_running = true;
        SDL_Event event;
        SDL_zero(event); // initalize the memory reigon to zero
        while(is_running)
        {
            while(SDL_PollEvent(&event) == true) {
                if(event.type == SDL_EVENT_QUIT)
                {
                    is_running = false;
                }
            }
        SDL_ClearSurface(gScreenSurface, 0.12f, 0.16f, 0.24f, 1.0f);
        SDL_UpdateWindowSurface(gWindow);
        SDL_Delay(16);
        }
        return 0; // success 

    }

    Client::~Client() {

        // this is my object
        if(gHelloWorld) {
            SDL_DestroySurface(gHelloWorld);
            gHelloWorld = nullptr;
        }

        if(gWindow) {
            SDL_DestroyWindow(gWindow);
            gWindow = nullptr;
        }
        gScreenSurface = nullptr;  // this owned by window internally so freeing up window will free this as well
        SDL_Quit(); // quits the sdl3 library
    }
}