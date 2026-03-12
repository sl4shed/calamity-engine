#include "input.hpp"
#include "logger.hpp"
#include "services.hpp"
#include "engine.hpp"
#include <iostream>
#include <SDL3/SDL.h>

void Input::update(float deltaTime)
{
    int numKeys;
    const bool *key_states = SDL_GetKeyboardState(&numKeys);

    for (int i = 0; i < numKeys; i++)
    {
        if (key_states[i])
        {
            SDL_Scancode scancode = (SDL_Scancode)i;
            SDL_Keycode sdlKeycode = SDL_GetKeyFromScancode(scancode, SDL_KMOD_NONE, false);
            Keycode keycode = (Keycode)sdlKeycode;
            Services::engine()->root.input(keycode);
        }
    }
}