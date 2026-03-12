#include "input.hpp"
#include "logger.hpp"

void Input::update(float deltaTime)
{
    // update input state here

    const bool *key_states = SDL_GetKeyboardState();
    Logger::debug("key states: {}", key_states);
}