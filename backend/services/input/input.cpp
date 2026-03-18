#include "input.hpp"
#include "../../utils/logger.hpp"
#include "../services.hpp"
#include "../engine.hpp"
#include "../graphics.hpp"
#include "keycode.hpp"
#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>

void Input::update(float deltaTime)
{
    inputs.clear();

    // bullshit
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_KEY_DOWN) {
            auto ev = std::make_unique<InputEventKey>();
            ev->pressed = true;
            ev->keycode = (Keycode)event.key.scancode;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_KEY_UP) {
            auto ev = std::make_unique<InputEventKey>();
            ev->pressed = false;
            ev->keycode = (Keycode)event.key.scancode;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_MOUSE_MOTION) {
            // todo fucking optimize this bullshit
            auto ev = std::make_unique<InputEventMouseMotion>();
            auto camera = Services::engine()->getActiveCamera();
            auto cameraT = camera->getNode()->globalTransform;
            auto screen = Services::graphics()->screenSize;

            ev->position = (Vector2){event.motion.x, event.motion.y} + cameraT.position - (screen * camera->origin);
            ev->relative = (Vector2){event.motion.xrel, event.motion.yrel};
            
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            auto ev = std::make_unique<InputEventMouseButton>();
            ev->pressed = true;
            ev->buttonIndex = (MouseButton)event.button.button;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            auto ev = std::make_unique<InputEventMouseButton>();
            ev->pressed = false;
            ev->buttonIndex = (MouseButton)event.button.button;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_MOUSE_WHEEL) {
            auto ev = std::make_unique<InputEventMouseButton>();
            ev->pressed = true;
            
            // i write this with the assumption that nobody who uses calamity engine has a fucking scroll ball mouse
            // maybe i should fix this somehow?? idk how i would fix it but i think if i were to add support for scroll balls then a separate
            // InputEventMouseScroll would be appropriate with a direction enum and x and y variables.
            if(event.motion.x > 0) {
                ev->buttonIndex = MouseButton::MOUSE_BUTTON_WHEEL_RIGHT;
                ev->factor = event.motion.x;
            } else if (event.motion.x < 0)
            {
                ev->buttonIndex = MouseButton::MOUSE_BUTTON_WHEEL_LEFT;
                ev->factor = -event.motion.x;
            } else if (event.motion.y < 0) 
            {
                ev->buttonIndex = MouseButton::MOUSE_BUTTON_WHEEL_DOWN;
                ev->factor = -event.motion.y;
            } else if (event.motion.y > 0)
            {
                ev->buttonIndex = MouseButton::MOUSE_BUTTON_WHEEL_UP;
                ev->factor = event.motion.y;
            }
        }

        if(event.type == SDL_EVENT_QUIT) {
            shouldQuit = true;
        }
    }

    // // keyboard bullshit
    // const bool* state = SDL_GetKeyboardState(nullptr);

    // for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
    // {
    //     bool curr = state[i];
    //     bool prev = prevKeyboardInputs[i];

    //     auto ev = std::make_unique<InputEventKey>();
    //     ev->keycode = (Keycode)i;
    //     if (curr && !prev)
    //         ev->pressed = true;
    //     else if (!curr && prev)
    //         ev->pressed = false;
        
    //     inputs.push_back(std::move(ev));
    //     prevKeyboardInputs[i] = curr;
    // }

    for (auto& e : inputs)
        Services::engine()->root.input(*e);
}