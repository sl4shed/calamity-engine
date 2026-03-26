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
            ev->keycode = (Keycode)event.key.key;
            ev->scancode = (Keycode)event.key.scancode;
            ev->keyLabel = SDL_GetKeyName(event.key.key);
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_KEY_UP) {
            auto ev = std::make_unique<InputEventKey>();
            ev->pressed = false;
            ev->keycode = (Keycode)event.key.key;
            ev->scancode = (Keycode)event.key.scancode;
            ev->keyLabel = SDL_GetKeyName(event.key.key);
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

        if(event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
            auto ev = std::make_unique<InputEventControllerButton>();
            ev->pressed = true;
            ev->device = event.gbutton.which;
            inputs.push_back(std::move(ev));
        }
        
        if(event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
            auto ev = std::make_unique<InputEventControllerButton>();
            ev->pressed = false;
            ev->device = event.gbutton.which;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_GAMEPAD_ADDED) {
            auto ev = std::make_unique<InputEventControllerStatus>();
            ev->connected = true;
            ev->device = event.gdevice.which;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            auto ev = std::make_unique<InputEventControllerStatus>();
            ev->connected = false;
            ev->device = event.gdevice.which;
            inputs.push_back(std::move(ev));
        }

        if(event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
            auto ev = std::make_unique<InputEventControllerMotion>();
            ev->device = event.gaxis.which;
            ev->motion = event.gaxis.value;
            ev->axis = (ControllerAxis)event.gaxis.axis;
            inputs.push_back(std::move(ev));
        }
    }

    for (auto& e : inputs)
        Services::engine()->root.input(*e);
}

bool Input::isKeyPressed(Keycode key) const {
    return sdlKeyArray[(SDL_Scancode)key];
}

bool Input::isKeyLabelPressed(const char * label) const {
    SDL_Keycode key = SDL_GetKeyFromName(label);
    return sdlKeyArray[(SDL_Scancode)key];
}

bool Input::isMouseButtonPressed(MouseButton button) const {
    SDL_MouseButtonFlags flags = SDL_GetMouseState(nullptr, nullptr);
    return (flags & SDL_BUTTON_MASK((int)button)) != 0;
}

bool Input::isControllerButtonPressed(int device, ControllerButton button) const {
    return SDL_GetGamepadButton(SDL_GetGamepadFromID(device), (SDL_GamepadButton)button);
}

Vector2 Input::getMousePosition() const {
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

// controller stuff

float Input::getControllerAxis(int device, ControllerAxis axis) const {
    return (float)SDL_GetGamepadAxis(SDL_GetGamepadFromID(device), (SDL_GamepadAxis)axis) / 32768.0f; // clamp to float :)
}

std::string Input::getControllerGUID(int device) const {
    char buf[33];
    SDL_GUIDToString(SDL_GetGamepadGUIDForID(device), buf, sizeof(buf));
    return std::string(buf);
}

std::string Input::getControllerName(int device) const {
    return std::string(SDL_GetGamepadNameForID(device));
}

std::vector<int> Input::getConnectedControllers() {
    return controllers;
}

void Input::startControllerVibration(int device, float weakMagnitude, float strongMagnitude, int durationMs) {
    SDL_RumbleGamepad(SDL_GetGamepadFromID(device), 0xFFFF * weakMagnitude, 0xFFFF * strongMagnitude, durationMs);
}

// input registry stuff

void InputRegistry::addAction(std::string name, float deadzone) {
    actions[name] = InputRegistryAction{name, {}, deadzone};
}

void InputRegistry::removeAction(std::string name) {
    actions.erase(name);
}

bool InputRegistry::hasAction(std::string name) const {
    return actions.count(name) > 0;
}

// input registry action event management

int InputRegistry::actionAddEvent(std::string name, std::unique_ptr<InputEvent> event) {
    actions.at(name).events.push_back(event);
    return actions.at(name).events.size() - 1;
}

void InputRegistry::actionRemoveEvent(std::string name, int index) {
    auto& events = actions.at(name).events;
    events.erase(events.begin() + index);
}

void InputRegistry::actionRemoveEvents(std::string name) {
    auto& events = actions.at(name).events;
    events.erase(events.begin(), events.end());
}

float InputRegistry::actionGetDeadzone(std::string name) {
    return actions.at(name).deadzone;
}

void InputRegistry::actionSetDeadzone(std::string name, float deadzone) {
    actions.at(name).deadzone = deadzone;
}

// darwin help me
bool InputRegistry::eventIsAction(std::unique_ptr<InputEvent> event, std::string name) {
    auto& events = actions.at(name).events;
    return std::any_of(events.begin(), events.end(), [&](const std::unique_ptr<InputEvent> stored) {
        return event == stored;
    });
}

// event operators

bool InputEventKey::operator==(const InputEventKey& other) const {
    return (scancode == other.scancode && pressed == other.pressed);
}

bool InputEventMouseButton::operator==(const InputEventMouseButton& other) const {
    return (buttonIndex == other.buttonIndex && pressed == other.pressed && factor == other.factor && double_click == other.double_click);
}

bool InputEventMouseMotion::operator==(const InputEventMouseMotion& other) const {
    return (position == other.position && relative == other.relative);
}

bool InputEventControllerButton::operator==(const InputEventControllerButton& other) const {
    return (device == other.device && button == other.button && pressed == other.pressed && pressure == other.pressure);
}

bool InputEventControllerStatus::operator==(const InputEventControllerStatus& other) const {
    return (device == other.device && connected == other.connected);
}

bool InputEventControllerMotion::operator==(const InputEventControllerMotion& other) const {
    return (device == other.device && axis == other.axis && motion == other.motion);
}