#include <iostream>
#include "client.hpp"
#include <cmath>
#include <vector>
#include <SDL3/SDL.h>

namespace bluff {

struct GothicButton {
        SDL_FRect rect;
        bool is_hovered = false;
        bool is_clicked = false;
    };

    static void DrawDiamond(SDL_Renderer* renderer, float cx, float cy, float size, SDL_FColor color) {
        // create a basic structure
        SDL_SetRenderDrawColorFloat(renderer, color.r, color.g, color.b, color.a);
        for (float dy = -size; dy <= size; dy += 1.0f) {
            float width = size - std::abs(dy);
            SDL_RenderLine(renderer, cx - width, cy + dy, cx + width, cy + dy);
        }
    }

    static void DrawGothicButton(SDL_Renderer* renderer, const GothicButton& btn) {
        float x = btn.rect.x;
        float y = btn.rect.y;
        float w = btn.rect.w;
        float h = btn.rect.h;
        float tip = h * 0.28f; 

        // colors
        SDL_FColor borderColor = btn.is_hovered ? SDL_FColor{0.96f, 0.80f, 0.47f, 1.0f}  : SDL_FColor{0.58f, 0.45f, 0.23f, 0.85f}; 

        SDL_FColor cCenter = btn.is_hovered 
            ? SDL_FColor{0.50f, 0.08f, 0.10f, 0.95f}  // deep crimson/wine glow
            : SDL_FColor{0.11f, 0.08f, 0.10f, 0.90f}; // smoky charcoal/slate

        SDL_FColor cEdge = SDL_FColor{0.04f, 0.03f, 0.04f, 0.95f}; // dark vignette 

        // 7 vetices 1 center 6 perimeter tipped
        SDL_Vertex verts[7] = {
            { {x + w * 0.5f, y + h * 0.5f}, cCenter, {0, 0} }, // 0: Center
            { {x + tip,     y},             cEdge,   {0, 0} }, // 1: Top-Left
            { {x + w - tip, y},             cEdge,   {0, 0} }, // 2: Top-Right
            { {x + w,       y + h * 0.5f},  cEdge,   {0, 0} }, // 3: Right Tip
            { {x + w - tip, y + h},         cEdge,   {0, 0} }, // 4: Bottom-Right
            { {x + tip,     y + h},         cEdge,   {0, 0} }, // 5: Bottom-Left
            { {x,           y + h * 0.5f},  cEdge,   {0, 0} }  // 6: Left Tip
        };

        int indices[18] = {
            0, 1, 2,  // Top segment
            0, 2, 3,  // Top-right tip
            0, 3, 4,  // Bottom-right tip
            0, 4, 5,  // Bottom segment
            0, 5, 6,  // Bottom-left tip
            0, 6, 1   // Top-left tip
        };

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderGeometry(renderer, nullptr, verts, 7, indices, 18);
        SDL_FPoint outline[7] = {
            {x + tip,     y},
            {x + w - tip, y},
            {x + w,       y + h * 0.5f},
            {x + w - tip, y + h},
            {x + tip,     y + h},
            {x,           y + h * 0.5f},
            {x + tip,     y}
        };

        SDL_SetRenderDrawColorFloat(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderLines(renderer, outline, 7);
        DrawDiamond(renderer, x + w * 0.5f, y, 3.5f, borderColor);
        DrawDiamond(renderer, x + w * 0.5f, y + h, 3.5f, borderColor);
    }

    Client::Client() 
    {
        gWindow = nullptr; // the window we will be rendering
        gScreenSurface = nullptr; // the surface in the window
        gMainMenu = nullptr; // image we will be loading
        gFog = nullptr;
        gVignette = nullptr;
        gGlow = nullptr;

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
        gGlow = SDL_LoadBMP("assets/candle_glow.bmp");
        gFog = SDL_LoadBMP("assets/fog.bmp");
        gVignette = SDL_LoadBMP("assets/vignette.bmp");

        if(gMainMenu == nullptr) SDL_Log("Error Loading The Main Menu Image is Not there %s", SDL_GetError());
        if(gFog) SDL_SetSurfaceBlendMode(gFog, SDL_BLENDMODE_BLEND);
        if(gVignette) SDL_SetSurfaceBlendMode(gVignette, SDL_BLENDMODE_MOD);
        if(gGlow) SDL_SetSurfaceBlendMode(gGlow, SDL_BLENDMODE_ADD);

        std::vector<GothicButton> buttons(3);
        bool is_running = true;
        bool fullscreen_applied = false;
        SDL_Event event;
        SDL_zero(event); // initalize the memory reigon to zero
        while(is_running)
        {
            float mouseX = 0.0f, mouseY = 0.0f;
            SDL_GetMouseState(&mouseX, &mouseY);
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
                if(gGlow != nullptr) {
                    // normalized wick centers in main_menu.bmp (x / 1672, y / 941). , (x / width_of_image, y / height_of_image)
                    constexpr SDL_FPoint glow_points[] = {
                        {0.017f, 0.391f}, // far-left candle
                        {0.054f, 0.404f}, // foreground candle
                        {0.081f, 0.176f}, // shelf candle
                        {0.147f, 0.433f}, // small left candle
                        {0.430f, 0.318f}, // center candle
                        {0.477f, 0.420f}, // candle in center cluster
                        {0.961f, 0.271f}  // right candle
                    };
                    SDL_Rect glow_point;
                    for(int i = 0; i < std::size(glow_points); i++)
                    {
                        // fun fact blit means bit block transfer
                        constexpr uint8_t GLOW_TRANSPARENCY = 64;
                        constexpr float PI = 3.14f;
                        constexpr float speed = 3.25f;
                        
                        float current_time = SDL_GetTicks()/1000.0f ; // convert to millisecond
                        float phase = current_time * speed  + (i  * 0.75f  * PI); // wt + randomoffset * i; 3/4pi
                        float wave = std::sin(phase); // since sin output is from -1 to 1 we cant have intensity b/w -1 to 1 so we normalize it 0 to 1
                        float scale = 0.5f + (wave*wave)*0.5f; // scaling down the wave to 0.5 so we can have a base intensity of 0.5 total range is now 0.5 to 1.0

                        glow_point.w = gGlow->w;
                        glow_point.h = gGlow->h;
                        glow_point.x = (glow_points[i].x * gScreenSurface->w) - (gGlow->w / 2) ;
                        glow_point.y = (glow_points[i].y * gScreenSurface->h) - (gGlow->h / 2);

                        SDL_SetSurfaceAlphaMod(gGlow, GLOW_TRANSPARENCY * scale);
                        SDL_BlitSurfaceScaled(gGlow , nullptr,gScreenSurface, &glow_point, SDL_SCALEMODE_LINEAR);   
                        
                         // 0 to 255 alpha transparency
                        // sine wave = Asin(wt + phase); set A = 1
                    }

                }   
                //buttons multiplied by their floating points
                float btnW = gScreenSurface->w * 0.18f;
                float btnH = gScreenSurface->h * 0.055f;
                float startX = gScreenSurface->w * 0.138f;
                float startY = gScreenSurface->h * 0.415f;
                float gap = btnH * 0.24f;

                for (size_t i = 0; i < buttons.size(); ++i) {
                    buttons[i].rect = { startX, startY + i * (btnH + gap), btnW, btnH };
                    SDL_Point mouse_pt = { (int)mouseX, (int)mouseY };
                    SDL_Rect btn_r = { (int)buttons[i].rect.x, (int)buttons[i].rect.y, (int)buttons[i].rect.w, (int)buttons[i].rect.h };
                    buttons[i].is_hovered = SDL_PointInRect(&mouse_pt, &btn_r);
                }

                // Render the 3 buttons directly onto the surface
                SDL_Renderer* surface_renderer = SDL_CreateSoftwareRenderer(gScreenSurface);
                if (surface_renderer != nullptr) {
                    for (const auto& btn : buttons) {
                        DrawGothicButton(surface_renderer, btn);
                    }
                    SDL_DestroyRenderer(surface_renderer);
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
        if(gFog) {
            SDL_DestroySurface(gFog);
            gFog = nullptr;
        }
        if(gVignette) {
            SDL_DestroySurface(gVignette);
            gVignette = nullptr;
        }
        if(gGlow) {
            SDL_DestroySurface(gGlow);
            gGlow = nullptr;
        }
        gScreenSurface = nullptr;  // this owned by window internally so freeing up window will free this as well
        SDL_Quit(); // quits the sdl3 library
    }
}