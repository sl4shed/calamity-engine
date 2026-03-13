#include "input.hpp"
#include "logger.hpp"
#include "services.hpp"
#include "engine.hpp"
#include <iostream>
#include <SDL3/SDL.h>

void Input::update(float deltaTime)
{
    prevInputs = inputs;
    inputs.clear();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        Keycode keycode = (Keycode)event.key.key;
        inputs.emplace_back(keycode);
    }

    for (const InputEvent& e : inputs)
    {
        Services::engine()->root.input(e);
    }
}

bool InputEvent::isKeycode(Keycode compare) {
    return this->keycode == compare;
}