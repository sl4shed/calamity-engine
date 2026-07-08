#pragma once
#include <SDL3/SDL.h>
#include "../graphics/definitions.hpp"
#include "keycode.hpp"

enum class ControllerButton
{
    INVALID = -1,
    SOUTH, /**< Bottom face button (e.g. Xbox A button) */
    EAST,  /**< Right face button (e.g. Xbox B button) */
    WEST,  /**< Left face button (e.g. Xbox X button) */
    NORTH, /**< Top face button (e.g. Xbox Y button) */
    BACK,
    GUIDE,
    START,
    LEFT_STICK,
    RIGHT_STICK,
    LEFT_SHOULDER,
    RIGHT_SHOULDER,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    MISC1,         /**< Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button) */
    RIGHT_PADDLE1, /**< Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1, DualSense Edge RB button, Right Joy-Con SR button) */
    LEFT_PADDLE1,  /**< Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3, DualSense Edge LB button, Left Joy-Con SL button) */
    RIGHT_PADDLE2, /**< Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2, DualSense Edge right Fn button, Right Joy-Con SL button) */
    LEFT_PADDLE2,  /**< Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4, DualSense Edge left Fn button, Left Joy-Con SR button) */
    TOUCHPAD,      /**< PS4/PS5 touchpad button */
    COUNT
};

enum class ControllerAxis
{
    INVALID = -1,
    LEFT_X,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    TRIGGER_LEFT,
    TRIGGER_RIGHT,
    SDL_MAX
};

enum class MouseButton
{
    NONE,
    LEFT,
    MIDDLE,
    RIGHT,
    XBUTTON1,
    XBUTTON2,
    WHEEL_UP,
    WHEEL_DOWN,
    WHEEL_LEFT,
    WHEEL_RIGHT
};

// input events //

class InputEvent
{
public:
    virtual ~InputEvent() = default;
    virtual bool operator==(const InputEvent &other) const { return false; };
    virtual bool operator<=(const InputEvent &other) const { return false; };

    bool isActionPressed(const std::string &action) const;
    bool isActionReleased(const std::string &action) const;
    bool isAction(const std::string &action) const;

    // float getActionStrength(std::string action) const;
    virtual bool isPressed() const { return false; }
    virtual bool isReleased() const { return false; }
    virtual float getStrength() const { return 0.0f; }

    std::shared_ptr<Window> window = nullptr;
};

class InputEventAction : public InputEvent
{
public:
    std::string action = "";
    int eventIndex = -1;
    bool pressed = false;
    float strength = 1.0;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return strength; };
};

class InputEventWithModifiers : public InputEvent
{
public:
    bool altPressed = false;
    bool ctrlPressed = false;
    bool metaPressed = false;
    bool shiftPressed = false;

    void fromSDLmod(SDL_Keymod mod) {
        altPressed = (mod & SDL_KMOD_ALT) != 0;
        ctrlPressed = (mod & SDL_KMOD_CTRL) != 0;
        shiftPressed = (mod & SDL_KMOD_SHIFT) != 0;
        metaPressed = (mod & SDL_KMOD_GUI) != 0;
    }
};

// TODO add keylabel and shit to this from SDL key
class InputEventKey : public InputEventWithModifiers
{
public:
    InputEventKey(bool pressed = false, Keycode scancode = Keycode::UNKNOWN, Keycode keycode = Keycode::UNKNOWN, bool echo = false) : pressed(pressed), scancode(scancode), keycode(keycode), echo(echo) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    bool echo = false; // todo
    Keycode scancode = Keycode::UNKNOWN;
    Keycode keycode = Keycode::UNKNOWN;
    const char *keyLabel;
    bool pressed = false;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; }
};

class InputEventMouse : public InputEventWithModifiers
{
public:
    Vector2 position = {0, 0};
    Vector2 relative = {0, 0};
};

// here's one thing that i dont like how godot does.
// if its a mouse wheel event i'll just put like the entire Vector2 of factor because
// then if you're using a touchpad (for example) you dont actually get the high resolution 2d scroll or whatever
class InputEventMouseButton : public InputEventMouse
{
public:
    InputEventMouseButton(bool pressed = false, MouseButton button = MouseButton::NONE, bool doubleClick = false, Vector2 factor = {0, 0}) : pressed(pressed), button(button), doubleClick(doubleClick), factor(factor) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    MouseButton button = MouseButton::NONE;
    bool doubleClick = false;
    Vector2 factor = {0, 0};
    bool pressed = false;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; };
};

class InputEventMouseMotion : public InputEventMouse
{
public:
    InputEventMouseMotion(Vector2 relative = {0, 0}, Vector2 position = {0, 0}) : relative(relative), position(position) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    // this also has some stuff to do with drawing pens so todo that i guess
    // should be easy with SDL pen events
    Vector2 relative = {0, 0};
    Vector2 position = {0, 0};
};

class InputEventControllerButton : public InputEvent
{
public:
    InputEventControllerButton(int device = 0, bool pressed = false, ControllerButton button = ControllerButton::INVALID) : device(device), pressed(pressed), button(button) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    ControllerButton button = ControllerButton::INVALID;
    bool pressed = false;
    float pressure = 1.0f;
    int device = 0;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; } // for now its this because sdl doesnt do gamepad button pressure :(
};

class InputEventControllerStatus : public InputEvent
{
public:
    InputEventControllerStatus(int device = 0, bool connected = false) : device(device), connected(connected) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    int device = 0;
    bool connected = false;
};

class InputEventControllerMotion : public InputEvent
{
public:
    InputEventControllerMotion(int device = 0, float motion = 0.0f, ControllerAxis axis = ControllerAxis::INVALID) : device(device), motion(motion), axis(axis) {};

    bool operator==(const InputEvent &other) const;
    bool operator<=(const InputEvent &other) const;

    int device = 0;
    float motion = 0.0f;
    ControllerAxis axis = ControllerAxis::INVALID;

    float getStrength() const { return motion; };
};

//#include "../../core/node/components.hpp"

// input registry stuff //

struct InputRegistryAction
{
    std::string name;
    std::vector<std::unique_ptr<InputEvent>> events;
    float deadzone;
};