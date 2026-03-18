#pragma once
#include <SDL3/SDL.h>
#include "../../core/definitions.hpp"
#include "keycode.hpp"

// hello. here i just copied godot's homework because i love how godot input works its really good

// massive todo kinda
// if i ever continue working on this i need to encapsulate a lot of these values
// such that they either only get set in the constructor
// or they can only get set once
// and theyre constants or whatever
// just like in godot...
class InputEvent
{
public:
    virtual ~InputEvent() = default;
    bool is_action_pressed(std::string action) const;
    bool is_action_released(std::string action) const;
    bool is_action(std::string action) const;
    float get_action_strength(std::string action) const;

    bool is_pressed() const;
    bool is_released() const;

};

class InputEventAction : public InputEvent {
public:
    std::string action = "";
    int eventIndex = -1;
    bool pressed = false;
    float strength = 1.0;
};

class InputEventWithModifiers : public InputEvent {
public:
    bool altPressed = false;
    bool commandOrControlAutoremap = false;
    bool ctrlPressed = false;
    bool metaPressed = false;
    bool shiftPressed = false;

    // TODO make this use keymodifiermask or whatever godot does
    std::vector<bool> getModifiersMask() const;
    bool isCommandOrControlPressed() const;
};

// TODO add keylabel and shit to this from SDL key
class InputEventKey : public InputEventWithModifiers {
public:
    bool echo = false;
    Keycode keycode = Keycode::UNKNOWN;
    bool pressed = false;
};

enum class MouseButton {
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_XBUTTON1,
    MOUSE_BUTTON_XBUTTON2,
    MOUSE_BUTTON_WHEEL_UP,
    MOUSE_BUTTON_WHEEL_DOWN,
    MOUSE_BUTTON_WHEEL_LEFT,
    MOUSE_BUTTON_WHEEL_RIGHT
};

class InputEventMouse : public InputEventWithModifiers {
public:
    Vector2 position = {0, 0};
    Vector2 relative = {0, 0};
};

class InputEventMouseButton : public InputEventMouse {
public:
    MouseButton buttonIndex = MouseButton::MOUSE_BUTTON_NONE;
    bool double_click = false;
    float factor = 1.0; // todo whatever the fuck this has to do with high-resolution scroll;
    bool pressed = false;
};

class InputEventMouseMotion : public InputEventMouse {
public:
    // this also has some stuff to do with drawing pens so todo that i guess
    // should be easy with SDL pen events
    Vector2 relative = {0, 0};
    Vector2 position = {0, 0};
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
    bool shouldQuit = false;
private:
    std::vector<bool> prevKeyboardInputs = std::vector<bool>(SDL_SCANCODE_COUNT, false);
    std::vector<std::unique_ptr<InputEvent>> inputs;
};


class InputRegistry {

};