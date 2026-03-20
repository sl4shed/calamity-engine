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
    bool isActionPressed(std::string action) const;
    bool isActionReleased(std::string action) const;
    bool isAction(std::string action) const;
    float getActionStrength(std::string action) const;

    bool isPressed() const;
    bool isReleased() const;

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

// here's one thing that i dont like how godot does.
// if its a mouse wheel event i'll just put like the entire Vector2 of factor because
// then if you're using a touchpad (for example) you dont actually get the high resolution 2d scroll or whatever
class InputEventMouseButton : public InputEventMouse {
public:
    MouseButton buttonIndex = MouseButton::MOUSE_BUTTON_NONE;
    bool double_click = false;
    Vector2 factor = {0, 0};
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

    bool isKeyPressed(Keycode key) const;
    bool isKeyLabelPressed(Keycode key) const; // todo thisz
    bool isMouseButtonPressed(MouseButton button) const;
    Vector2 getMousePosition() const;

    // todo
    bool isActionPressed() const;
    bool isActionJustPressed() const;
    bool isActionJustReleased() const;

    bool shouldQuit = false;
private:
    std::vector<std::unique_ptr<InputEvent>> inputs;
};


class InputRegistry {

};