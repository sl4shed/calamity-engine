#include "input.hpp"
#include "../../utils/logger.hpp"
#include "../services.hpp"
#include "../engine.hpp"
#include "keycode.hpp"
#include <iostream>
#include <SDL3/SDL.h>

void Input::update(float deltaTime)
{
    prevInputs = inputs;
    inputs.clear();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            Keycode keycode = (Keycode)event.key.key;
            Logger::debug("{}", (uint)keycode);
            inputs.emplace_back(keycode);
        }
    }

    for (const InputEvent& e : inputs)
    {
        Services::engine()->root.input(e);
    }
}

bool InputEvent::isKeycode(Keycode compare) {
    bool condition = (this->keycode == compare);
    Logger::debug("test: {}", condition);
    return condition;
}

InputEvent::InputEvent(Keycode k) {
    this->keycode = k;
}