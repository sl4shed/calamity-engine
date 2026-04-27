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
    if (!actionsArrayPointer)
        actionsArrayPointer = Services::inputRegistry()->getActionsArray();

    SDL_PumpEvents(); // update keyboard array I think
    inputs.clear();

    // bullshit
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            {
                auto ev = std::make_unique<InputEventKey>();
                ev->pressed = true;
                ev->keycode = static_cast<Keycode>(event.key.key);
                ev->scancode = static_cast<Keycode>(event.key.scancode);
                ev->keyLabel = SDL_GetKeyName(event.key.key);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_KEY_UP:
            {
                auto ev = std::make_unique<InputEventKey>();
                ev->pressed = false;
                ev->keycode = static_cast<Keycode>(event.key.key);
                ev->scancode = static_cast<Keycode>(event.key.scancode);
                ev->keyLabel = SDL_GetKeyName(event.key.key);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_MOTION:
            {
                auto ev = std::make_unique<InputEventMouseMotion>();
                const auto camera = Services::engine()->getActiveCamera();

                float lx, ly;
                SDL_RenderCoordinatesFromWindow(Services::graphics()->getRenderer(), event.motion.x, event.motion.y, &lx, &ly);

                ev->position = camera->screenToWorld({lx, ly});
                ev->relative = Vector2{event.motion.xrel, event.motion.yrel};
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                auto ev = std::make_unique<InputEventMouseButton>();
                ev->pressed = true;
                ev->buttonIndex = static_cast<MouseButton>(event.button.button);
                if(event.button.clicks > 1) ev->doubleClick = true;
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                auto ev = std::make_unique<InputEventMouseButton>();
                ev->pressed = false;
                ev->buttonIndex = static_cast<MouseButton>(event.button.button);
                if(event.button.clicks > 1) ev->doubleClick = true;
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_WHEEL:
            {
                auto ev = std::make_unique<InputEventMouseButton>();
                ev->pressed = true;

                if (std::abs(event.wheel.x) > std::abs(event.wheel.y)) {
                    ev->buttonIndex = event.wheel.x > 0
                        ? MouseButton::WHEEL_RIGHT
                        : MouseButton::WHEEL_LEFT;
                } else {
                    ev->buttonIndex = event.wheel.y > 0
                        ? MouseButton::WHEEL_UP
                        : MouseButton::WHEEL_DOWN;
                }

                ev->factor = {event.motion.x, event.motion.y};
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_QUIT:
            {
                shouldQuit = true;
                break;
            }
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            {
                auto ev = std::make_unique<InputEventControllerButton>();
                ev->pressed = true;
                ev->device = controllerMap.at(static_cast<int>(event.gdevice.which)); // find calamity controller id;
                ev->button = static_cast<ControllerButton>(event.gbutton.button);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            {
                auto ev = std::make_unique<InputEventControllerButton>();
                ev->pressed = false;
                ev->device = controllerMap.at(static_cast<int>(event.gdevice.which)); // find calamity controller id;
                ev->button = static_cast<ControllerButton>(event.gbutton.button);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_ADDED:
            {
                SDL_OpenGamepad(event.gdevice.which);
                auto ev = std::make_unique<InputEventControllerStatus>();
                ev->connected = true;

                int c = controllerMap.size();
                controllerMap.insert({event.gdevice.which, c});
                reversedControllerMap.insert({c, event.gdevice.which});
                ev->device = c;

                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_REMOVED:
            {
                auto ev = std::make_unique<InputEventControllerStatus>();
                ev->connected = false;
                ev->device = controllerMap.at(static_cast<int>(event.gdevice.which)); // find calamity controller id;
                reversedControllerMap.erase(controllerMap.at(static_cast<int>(event.gdevice.which)));
                controllerMap.erase(static_cast<int>(event.gdevice.which)); // remove the controller from the list
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                auto ev = std::make_unique<InputEventControllerMotion>();
                ev->device = controllerMap.at(static_cast<int>(event.gdevice.which)); // find calamity controller id;
                ev->motion = static_cast<float>(event.gaxis.value) / 32768.0f;
                ev->axis = static_cast<ControllerAxis>(event.gaxis.axis);
                if (ev->axis == ControllerAxis::LEFT_Y || ev->axis == ControllerAxis::RIGHT_Y)
                    ev->motion = -ev->motion; // stupid sdl3
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_WINDOW_RESIZED:
            {
                Services::graphics()->resetLogicalPresentation();
                break;
            }
        default:
            break;
        }
    }

    prevActionStrength = actionStrength;
    actionStrength.clear();

    // send events and pull action strengths and held actions
    for (auto& e : inputs) {
        Services::engine()->root.input(*e);

        if (const auto* motion = dynamic_cast<InputEventControllerMotion*>(e.get())) {
            for (auto& [name, action] : *actionsArrayPointer)
                if (Services::inputRegistry()->eventIsAction(e.get(), name, true))
                    actionStrength[name] = std::max(actionStrength.count(name) ? actionStrength[name] : 0.0f, std::abs(motion->getStrength()));
        } else {
            for (auto& [name, action] : *actionsArrayPointer) {
                if (Services::inputRegistry()->eventIsAction(e.get(), name, true)) {
                    if (e->isPressed()) heldActions.insert(name);
                    else heldActions.erase(name);
                }
            }
        }
    }

    // special cases for action strengths and held actions
    for (auto& [name, action] : *actionsArrayPointer) {
        float evStrength = actionStrength.count(name) ? actionStrength[name] : 0.0f;

        float pollStrength = 0.0f;
        for (auto& stored : action.events) {
            if (const auto* motion = dynamic_cast<InputEventControllerMotion*>(stored.get())) {
                const float val = getControllerAxis(motion->device, motion->axis);
                if (float directed = val * motion->motion; directed > action.deadzone)
                    pollStrength = std::max(pollStrength, directed);
            }
        }

        float held = heldActions.count(name) ? 1.0f : 0.0f;
        actionStrength[name] = std::max({evStrength, pollStrength, held});
    }
}

bool Input::isKeyPressed(Keycode key) const {
    return sdlKeyArray[static_cast<SDL_Scancode>(key)];
}

bool Input::isKeyLabelPressed(const char * label) const {
    SDL_Keycode key = SDL_GetKeyFromName(label);
    return sdlKeyArray[static_cast<SDL_Scancode>(key)];
}

bool Input::isMouseButtonPressed(MouseButton button) const {
    const SDL_MouseButtonFlags flags = SDL_GetMouseState(nullptr, nullptr);
    return (flags & SDL_BUTTON_MASK(static_cast<int>(button))) != 0;
}

bool Input::isControllerButtonPressed(const int device, ControllerButton button) const {
    return SDL_GetGamepadButton(SDL_GetGamepadFromID(device), static_cast<SDL_GamepadButton>(button));
}

Vector2 Input::getMousePosition() const {
    float wx, wy;
    SDL_GetMouseState(&wx, &wy);

    float lx, ly;
    SDL_RenderCoordinatesFromWindow(
        Services::graphics()->getRenderer(),
        wx, wy, &lx, &ly
    );

    return Services::engine()
        ->getActiveCamera()
        ->screenToWorld({lx, ly});
}

// controller stuff

float Input::getControllerAxis(const int device, ControllerAxis axis) const {
    if (!reversedControllerMap.count(device)) return 0.0f;
    float val = static_cast<float>(SDL_GetGamepadAxis(SDL_GetGamepadFromID(reversedControllerMap.at(device)), static_cast<SDL_GamepadAxis>(axis))) / 32768.0f;
    if (axis == ControllerAxis::LEFT_Y || axis == ControllerAxis::RIGHT_Y)
        val = -val; // stupid sdl3
    return val;
}

std::string Input::getControllerGUID(const int device) const {
    if (!reversedControllerMap.count(device)) return "";
    char buf[33];
    SDL_GUIDToString(SDL_GetGamepadGUIDForID(reversedControllerMap.at(device)), buf, sizeof(buf));
    return std::string(buf);
}

std::string Input::getControllerName(const int device) const {
    if (!reversedControllerMap.count(device)) return "";
    return std::string(SDL_GetGamepadNameForID(reversedControllerMap.at(device)));
}

std::vector<int> Input::getConnectedControllers() {
    std::vector<int> controllers;
    for (auto [sdlId, calId] : controllerMap)
    {
        controllers.push_back(calId);
    }
    return controllers;
}

void Input::startControllerVibration(const int device, const float weakMagnitude, const float strongMagnitude, const int durationMs) const
{
    SDL_RumbleGamepad(SDL_GetGamepadFromID(reversedControllerMap.at(device)), 0xFFFF * weakMagnitude, 0xFFFF * strongMagnitude, durationMs);
}

// input registry stuff

void InputRegistry::addAction(const std::string& name, const float deadzone) {
    actions[name] = InputRegistryAction{name, {}, deadzone};
}

void InputRegistry::removeAction(const std::string& name) {
    actions.erase(name);
}

bool InputRegistry::hasAction(const std::string& action) const {
    return actions.count(action) > 0;
}

// input registry action event management

int InputRegistry::actionAddEvent(const std::string& name, std::unique_ptr<InputEvent> event) {
    actions.at(name).events.push_back(std::move(event));
    return actions.at(name).events.size() - 1;
}

void InputRegistry::actionRemoveEvent(const std::string& name, const int index) {
    auto& events = actions.at(name).events;
    events.erase(events.begin() + index);
}

void InputRegistry::actionRemoveEvents(const std::string& name) {
    auto& events = actions.at(name).events;
    events.erase(events.begin(), events.end());
}

float InputRegistry::actionGetDeadzone(const std::string& name) const
{
    return actions.at(name).deadzone;
}

void InputRegistry::actionSetDeadzone(const std::string& name, float deadzone) {
    actions.at(name).deadzone = deadzone;
}

std::vector<std::string> InputRegistry::getActions() {
    std::vector<std::string> act;
    for (auto& [name, action] : actions) {
        act.push_back(name);
    }
    return act;
}

std::unordered_map<std::string, InputRegistryAction>* InputRegistry::getActionsArray() {
    return &actions;
}

// darwin help me
bool InputRegistry::eventIsAction(const InputEvent* event, const std::string& name, const bool identityCheck) {
    if(identityCheck) {
        auto& events = actions.at(name).events;
        return std::any_of(events.begin(), events.end(), [&](const std::unique_ptr<InputEvent>& stored) {
            return *event <= *stored;
        });
    }

    auto& events = actions.at(name).events;
    return std::any_of(events.begin(), events.end(), [&](const std::unique_ptr<InputEvent>& stored) {
        return *event == *stored;
    });
}

// event operators

bool InputEventKey::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventKey*>(&other);
    if(!o) return false;
    return (scancode == o->scancode && pressed == o->pressed);
}

bool InputEventMouseButton::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventMouseButton*>(&other);
    if(!o) return false;
    return (buttonIndex == o->buttonIndex && pressed == o->pressed && factor == o->factor && doubleClick == o->doubleClick);
}

bool InputEventMouseMotion::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventMouseMotion*>(&other);
    if(!o) return false;
    return (position == o->position && relative == o->relative);
}

bool InputEventControllerButton::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerButton*>(&other);
    if(!o) return false;
    return (device == o->device && button == o->button && pressed == o->pressed && pressure == o->pressure);
}

bool InputEventControllerStatus::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerStatus*>(&other);
    if(!o) return false;
    return (device == o->device && connected == o->connected);
}

bool InputEventControllerMotion::operator==(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerMotion*>(&other);
    if(!o) return false;
    return (device == o->device && axis == o->axis && motion == o->motion);
}

bool InputEventKey::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventKey*>(&other);
    if(!o) return false;
    return (scancode == o->scancode);
}

bool InputEventMouseButton::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventMouseButton*>(&other);
    if(!o) return false;
    return (buttonIndex == o->buttonIndex);
}

bool InputEventMouseMotion::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventMouseMotion*>(&other);
    if(!o) return false;
    return (position == o->position && relative == o->relative);
}

bool InputEventControllerButton::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerButton*>(&other);
    if(!o) return false;
    return (device == o->device && button == o->button);
}

bool InputEventControllerStatus::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerStatus*>(&other);
    if(!o) return false;
    return (device == o->device);
}

bool InputEventControllerMotion::operator<=(const InputEvent& other) const {
    const auto* o = dynamic_cast<const InputEventControllerMotion*>(&other);
    if(!o) return false;
    return (device == o->device && axis == o->axis);
}

// action functions

bool InputEvent::isAction(const std::string& name) const {
    return Services::inputRegistry()->eventIsAction(this, name);
}

bool InputEvent::isActionPressed(const std::string& name) const {
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isPressed());
}

bool InputEvent::isActionReleased(const std::string& name) const {
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isReleased());
}

bool Input::isActionPressed(const std::string& name) const {
    return actionStrength.at(name) > Services::inputRegistry()->actionGetDeadzone(name);
}

bool Input::isActionJustPressed(const std::string& name) const {
    auto dz = Services::inputRegistry()->actionGetDeadzone(name);
    float prev = prevActionStrength.count(name) ? prevActionStrength.at(name) : 0.0f;
    return actionStrength.at(name) > dz && prev <= dz;
}

bool Input::isActionJustReleased(const std::string& name) const {
    auto dz = Services::inputRegistry()->actionGetDeadzone(name);
    float prev = prevActionStrength.count(name) ? prevActionStrength.at(name) : 0.0f;
    return actionStrength.at(name) <= dz && prev > dz;
}

float Input::getAxis(const std::string& minAction, const std::string& maxAction) const {
    return actionStrength.at(maxAction) - actionStrength.at(minAction);
}

Vector2 Input::getVector(const std::string& minX, const std::string& maxX, const std::string& minY, const std::string& maxY, float deadzone) const {
    const Vector2 v = {getAxis(minX, maxX), getAxis(minY, maxY)};
    // normalize if outside deadzone
    const float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (deadzone < 0) deadzone = 0.2f; // fallback default
    if (len <= deadzone) return {0, 0};
    return {v.x / len * ((len - deadzone) / (1.0f - deadzone)),
            v.y / len * ((len - deadzone) / (1.0f - deadzone))};
}