#pragma once
#include <SDL3/SDL.h>

namespace bluff {
    class Client {
        public:
            Client();
            int run();
            ~Client();
        private:
            bool init();
            SDL_Surface* gMainMenu;
            SDL_Surface* gGlow;
            SDL_Surface* gFog;
            SDL_Surface* gVignette;
            
            SDL_Window* gWindow;
            SDL_Surface* gScreenSurface;
            static constexpr const char* kWindowName = "Veil of Deceit"; // client class constructor k means constant here and S
    };
}