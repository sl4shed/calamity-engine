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

    SDL_PumpEvents(); // update keyboard array i think
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
                ev->keycode = (Keycode)event.key.key;
                ev->scancode = (Keycode)event.key.scancode;
                ev->keyLabel = SDL_GetKeyName(event.key.key);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_KEY_UP:
            {
                auto ev = std::make_unique<InputEventKey>();
                ev->pressed = false;
                ev->keycode = (Keycode)event.key.key;
                ev->scancode = (Keycode)event.key.scancode;
                ev->keyLabel = SDL_GetKeyName(event.key.key);
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_MOTION:
            {
                auto ev = std::make_unique<InputEventMouseMotion>();
                auto camera = Services::engine()->getActiveCamera();
                auto cameraT = camera->getNode()->globalTransform;
                auto screen = Services::graphics()->screenSize;

                float lx, ly;
                SDL_RenderCoordinatesFromWindow(Services::graphics()->getRenderer(), event.motion.x, event.motion.y, &lx, &ly);

                ev->position = (Vector2){lx, ly} + cameraT.position - (screen * camera->origin);
                ev->relative = (Vector2){event.motion.xrel, event.motion.yrel};
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                auto ev = std::make_unique<InputEventMouseButton>();
                ev->pressed = true;
                ev->buttonIndex = (MouseButton)event.button.button;
                if(event.button.clicks > 1) ev->doubleClick = true;
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                auto ev = std::make_unique<InputEventMouseButton>();
                ev->pressed = false;
                ev->buttonIndex = (MouseButton)event.button.button;
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
                ev->device = controllerMap.at((int)event.gdevice.which); // find calamity controller id;
                ev->button = (ControllerButton)event.gbutton.button;
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            {
                auto ev = std::make_unique<InputEventControllerButton>();
                ev->pressed = false;
                ev->device = controllerMap.at((int)event.gdevice.which); // find calamity controller id;
                ev->button = (ControllerButton)event.gbutton.button;
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
                ev->device = controllerMap.at((int)event.gdevice.which); // find calamity controller id;
                reversedControllerMap.erase(controllerMap.at((int)event.gdevice.which));
                controllerMap.erase((int)event.gdevice.which); // remove the controller from the list
                inputs.push_back(std::move(ev));
                break;
            }
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                auto ev = std::make_unique<InputEventControllerMotion>();
                ev->device = controllerMap.at((int)event.gdevice.which); // find calamity controller id;
                ev->motion = (float)event.gaxis.value / 32767.0f;
                ev->axis = (ControllerAxis)event.gaxis.axis;
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
    for (auto& e : inputs) {
        Services::engine()->root.input(*e);    
    }

    for (auto& e : inputs) {
        if (auto* motion = dynamic_cast<InputEventControllerMotion*>(e.get())) {
            for (auto& [name, action] : *actionsArrayPointer)
                if (Services::inputRegistry()->eventIsAction(e.get(), name, true))
                {
                    actionStrength[name] = std::max(actionStrength.count(name) ? actionStrength[name] : 0.0f, std::abs(motion->getStrength()));
                }
        } else {
            for (auto& [name, action] : *actionsArrayPointer) {
                if (Services::inputRegistry()->eventIsAction(e.get(), name, true)) {
                    if (e->isPressed()) heldActions.insert(name);
                    else heldActions.erase(name);
                }
            }
        }
    }

    // poll analog bindings directly from hardware
    for (auto& [name, action] : *actionsArrayPointer) {
        for (auto& stored : action.events) {
            if (auto* motion = dynamic_cast<InputEventControllerMotion*>(stored.get())) {
                float val = getControllerAxis(motion->device, motion->axis);
                // only activate if val is in the same direction as the binding
                float directed = val * motion->motion; // positive if same direction
                if (directed > action.deadzone)
                    actionStrength[name] = std::max(actionStrength.count(name) ? actionStrength[name] : 0.0f, directed);
            }
        }
    }

    for (auto& [name, action] : *actionsArrayPointer) {
        if (!actionStrength.count(name))
            actionStrength[name] = heldActions.count(name) ? 1.0f : 0.0f;
    }
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
    auto camera = Services::engine()->getActiveCamera();
    auto cameraT = camera->getNode()->globalTransform;
    auto screen = Services::graphics()->screenSize;
    float wx, wy;
    SDL_GetMouseState(&wx, &wy);

    // map from window coords -> logical renderer coords
    float lx, ly;
    SDL_RenderCoordinatesFromWindow(Services::graphics()->getRenderer(), wx, wy, &lx, &ly);

    Vector2 pos = (Vector2){lx, ly} + cameraT.position - (screen * camera->origin);
    return pos;
}

// controller stuff

float Input::getControllerAxis(int device, ControllerAxis axis) const {
    if (!reversedControllerMap.count(device)) return 0.0f;
    float val = (float)SDL_GetGamepadAxis(SDL_GetGamepadFromID(reversedControllerMap.at(device)), (SDL_GamepadAxis)axis) / 32768.0f;
    if (axis == ControllerAxis::LEFT_Y || axis == ControllerAxis::RIGHT_Y)
        val = -val; // stupid sdl3
    return val;
}

std::string Input::getControllerGUID(int device) const {
    if (!reversedControllerMap.count(device)) return "";
    char buf[33];
    SDL_GUIDToString(SDL_GetGamepadGUIDForID(reversedControllerMap.at(device)), buf, sizeof(buf));
    return std::string(buf);
}

std::string Input::getControllerName(int device) const {
    if (!reversedControllerMap.count(device)) return "";
    return std::string(SDL_GetGamepadNameForID(reversedControllerMap.at(device)));
}

std::vector<int> Input::getConnectedControllers() {
    std::vector<int> controllers;
    for (auto pair : controllerMap)
    {
        controllers.push_back(pair.second);
    }
    return controllers;
}

void Input::startControllerVibration(int device, float weakMagnitude, float strongMagnitude, int durationMs) {
    SDL_RumbleGamepad(SDL_GetGamepadFromID(reversedControllerMap.at(device)), 0xFFFF * weakMagnitude, 0xFFFF * strongMagnitude, durationMs);
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
    actions.at(name).events.push_back(std::move(event));
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
bool InputRegistry::eventIsAction(const InputEvent* event, std::string name, bool identityCheck) {
    if(identityCheck) {
        auto& events = actions.at(name).events;
        return std::any_of(events.begin(), events.end(), [&](const std::unique_ptr<InputEvent>& stored) {
            return *event <= *stored;
        });
    } else {
        auto& events = actions.at(name).events;
        return std::any_of(events.begin(), events.end(), [&](const std::unique_ptr<InputEvent>& stored) {
            return *event == *stored;
        });
    }
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

bool InputEvent::isAction(std::string name) const {
    return Services::inputRegistry()->eventIsAction(this, name);
}

bool InputEvent::isActionPressed(std::string name) const {
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isPressed());
}

bool InputEvent::isActionReleased(std::string name) const {
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isReleased());
}

bool Input::isActionPressed(std::string name) const {
    return actionStrength.at(name) > Services::inputRegistry()->actionGetDeadzone(name);
}

bool Input::isActionJustPressed(std::string name) const {
    auto dz = Services::inputRegistry()->actionGetDeadzone(name);
    float prev = prevActionStrength.count(name) ? prevActionStrength.at(name) : 0.0f;
    return actionStrength.at(name) > dz && prev <= dz;
}

bool Input::isActionJustReleased(std::string name) const {
    auto dz = Services::inputRegistry()->actionGetDeadzone(name);
    float prev = prevActionStrength.count(name) ? prevActionStrength.at(name) : 0.0f;
    return actionStrength.at(name) <= dz && prev > dz;
}

float Input::getAxis(std::string minAction, std::string maxAction) const {
    return actionStrength.at(maxAction) - actionStrength.at(minAction);
}

Vector2 Input::getVector(std::string minX, std::string maxX, std::string minY, std::string maxY, float deadzone) const {
    Vector2 v = {getAxis(minX, maxX), getAxis(minY, maxY)};
    // normalize if outside deadzone
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (deadzone < 0) deadzone = 0.2f; // fallback default
    if (len <= deadzone) return {0, 0};
    return {v.x / len * ((len - deadzone) / (1.0f - deadzone)),
            v.y / len * ((len - deadzone) / (1.0f - deadzone))};
}