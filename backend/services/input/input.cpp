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
    SDL_PumpEvents(); // update keyboard array i think
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
            
            if (std::abs(event.wheel.x) > std::abs(event.wheel.y)) {
                ev->buttonIndex = event.wheel.x > 0 
                    ? MouseButton::MOUSE_BUTTON_WHEEL_RIGHT 
                    : MouseButton::MOUSE_BUTTON_WHEEL_LEFT;
            } else {
                ev->buttonIndex = event.wheel.y > 0 
                    ? MouseButton::MOUSE_BUTTON_WHEEL_UP 
                    : MouseButton::MOUSE_BUTTON_WHEEL_DOWN;
            }

            ev->factor = {event.motion.x, event.motion.y};
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_QUIT) {
            shouldQuit = true;
        }
    }

    for (auto& e : inputs)
        Services::engine()->root.input(*e);
}

bool Input::isKeyPressed(Keycode key) const {
    return sdlKeyArray[(SDL_Scancode)key];
}

bool Input::isMouseButtonPressed(MouseButton button) const {
    SDL_MouseButtonFlags flags = SDL_GetMouseState(nullptr, nullptr);
    return (flags & SDL_BUTTON_MASK((int)button)) != 0;
}

Vector2 Input::getMousePosition() const {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}
