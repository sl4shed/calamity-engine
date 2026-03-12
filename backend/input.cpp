#include "input.hpp"
#include "logger.hpp"
#include <iostream>

void Input::update(float deltaTime)
{
    // update input state here

    const bool *key_states = SDL_GetKeyboardState(nullptr);
    Logger::debug("W key: {}", (bool)key_states[SDL_SCANCODE_W]);
}