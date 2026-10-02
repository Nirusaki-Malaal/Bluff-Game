#define SDL_MAIN_HANDLED // custom main point entry
#include <SDL3/SDL_main.h>
#include <iostream> 
#include "client.hpp"

int main() {
    bluff::Client client;
    client.run(); 
    return 0;
}