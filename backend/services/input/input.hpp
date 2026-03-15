#pragma once
#include <SDL3/SDL.h>
#include "../../core/definitions.hpp"
#include "keycode.hpp"

enum class InputAction {
    KeyPressed = SDL_EVENT_KEY_DOWN,
    KeyReleased = SDL_EVENT_KEY_UP,
    MouseButtonPressed = SDL_EVENT_MOUSE_BUTTON_DOWN,
    MouseButtonReleased = SDL_EVENT_MOUSE_BUTTON_UP,
    GamepadButtonPressed = SDL_EVENT_GAMEPAD_BUTTON_DOWN,
    GamepadButtonReleased = SDL_EVENT_GAMEPAD_BUTTON_UP,
};

// multiple per frame
class InputEvent
{
public:
    InputEvent(Keycode keycode, InputAction action);

    bool isKeycode(Keycode compare);
private:
    Keycode keycode;
    InputAction action;
};


#include "../../core/node/components.hpp"


/**
 * # Input class
 *
 * yo phone ringing
 */
class Input
{
public:
    void update(float deltaTime);
private:
    std::vector<InputEvent> inputs;
    std::vector<InputEvent> prevInputs;
};

