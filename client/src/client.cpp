#include <iostream>
#include "client.hpp"
#include <SDL3/SDL.h>

namespace bluff {
    Client::Client() 
    {
        gWindow = nullptr; // the window we will be rendering
        gScreenSurface = nullptr; // the surface in the window
        gMainMenu = nullptr; // image we will be loading
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

        gMainMenu = SDL_LoadBMP("assets/main_menu.bmp");
        if(gMainMenu == nullptr) {
            SDL_Log("Error Loading The Main Menu Image is Not there %s", SDL_GetError());
        }

        bool is_running = true;
        bool fullscreen_applied = false;
        SDL_Event event;
        SDL_zero(event); // initalize the memory reigon to zero
        while(is_running)
        {
            while(SDL_PollEvent(&event) == true) {
                if(event.type == SDL_EVENT_QUIT)
                {
                    is_running = false;
                }
                else if(event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED || event.type == SDL_EVENT_WINDOW_RESIZED) {
                    gScreenSurface = nullptr;
                }
                else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                {
                    is_running = false;
                }
            }
            if (gScreenSurface == nullptr) {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
            }

            if (gScreenSurface != nullptr) {
                SDL_ClearSurface(gScreenSurface, 0.0f, 0.0f, 0.0f, 1.0f); // background colour filling
                if(gMainMenu != nullptr) {
                    SDL_Rect image_rect;
                    image_rect.w = gScreenSurface->w; // image width 
                    image_rect.h = gScreenSurface->h; // image height
                    image_rect.x = 0; // i want to fill stretch the image
                    image_rect.y = 0;
                    SDL_BlitSurfaceScaled(gMainMenu , nullptr,gScreenSurface, &image_rect, SDL_SCALEMODE_LINEAR);
                }
                SDL_UpdateWindowSurface(gWindow);
            
            if (!fullscreen_applied) {
                SDL_SetWindowFullscreen(gWindow, true);
                fullscreen_applied = true;
            }
            }

        SDL_Delay(16);
        }
        return 0; // success 

    }

    Client::~Client() {

        // this is my object
        if(gMainMenu) {
            SDL_DestroySurface(gMainMenu);
            gMainMenu = nullptr;
        }

        if(gWindow) {
            SDL_DestroyWindow(gWindow);
            gWindow = nullptr;
        }
        gScreenSurface = nullptr;  // this owned by window internally so freeing up window will free this as well
        SDL_Quit(); // quits the sdl3 library
    }
}