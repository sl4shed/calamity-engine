#include "input.hpp"
#include "../../utils/logger.hpp"
#include "../services.hpp"
#include "../engine.hpp"
#include "keycode.hpp"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

void Input::update(float deltaTime)
{
    prevInputs = inputs;
    inputs.clear();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {}

    const bool* state = SDL_GetKeyboardState(nullptr);

    for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
    {
        bool curr = state[i];
        bool prev = std::any_of(prevInputs.begin(), prevInputs.end(),
            [i](InputEvent& e) { return e.isKeycode((Keycode)i); });

        if (curr && !prev)
            inputs.emplace_back((Keycode)i, InputAction::KeyPressed);
        else if (!curr && prev)
            inputs.emplace_back((Keycode)i, InputAction::KeyReleased);
    }

    for (InputEvent& e : inputs)
        Services::engine()->root.input(e);
}

bool InputEvent::isKeycode(Keycode compare) {
    bool condition = (this->keycode == compare);
    Logger::debug("test: {}", condition);
    return condition;
}

InputEvent::InputEvent(Keycode k, InputAction ac) {
    this->keycode = k;
    this->action = ac;
}