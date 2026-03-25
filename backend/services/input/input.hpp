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

// controller shit //

enum class ControllerButton {
    INVALID = -1,
    SOUTH,           /**< Bottom face button (e.g. Xbox A button) */
    EAST,            /**< Right face button (e.g. Xbox B button) */
    WEST,            /**< Left face button (e.g. Xbox X button) */
    NORTH,           /**< Top face button (e.g. Xbox Y button) */
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
    MISC1,           /**< Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button) */
    RIGHT_PADDLE1,   /**< Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1, DualSense Edge RB button, Right Joy-Con SR button) */
    LEFT_PADDLE1,    /**< Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3, DualSense Edge LB button, Left Joy-Con SL button) */
    RIGHT_PADDLE2,   /**< Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2, DualSense Edge right Fn button, Right Joy-Con SL button) */
    LEFT_PADDLE2,    /**< Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4, DualSense Edge left Fn button, Left Joy-Con SR button) */
    TOUCHPAD,        /**< PS4/PS5 touchpad button */
    COUNT
};

enum class ControllerAxis {
    INVALID = -1,
    LEFT_X,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    TRIGGER_LEFT,
    TRIGGER_RIGHT,
    SDL_MAX
};

class InputEventControllerButton {
public:
    ControllerButton buttonIndex;
    bool pressed;
    float pressure;
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
    bool isControllerButtonPressed(ControllerButton button) const;
    bool isKeyLabelPressed(Keycode key) const; // todo this shit with sdl keycodes!!!
    bool isMouseButtonPressed(MouseButton button) const;
    Vector2 getMousePosition() const;
    bool isActionPressed() const; // big todo

    // controllers, todo
    float getControllerAxis(int device, ControllerAxis axis) const;
    std::string getControllerGUID(int device) const;
    std::string getControllerName(int device) const;
    float getControllerVibrationDuration(int device);
    Vector2 getControllerVibrationStrength(int device);
    std::vector<int> getConnectedControllers(int device);
    void startControllerVibration(int device, float weakMagnitude, float strongMagnitude, float duration = 0);
    void stopControllerVibration(int device);
    // void vibrateHandheld(int durationMs = 500, float amplitude = -1.0f);
    // no mobile support, yet :)

    // input getting methods, todo for input registry

    Vector2 getVector(std::string minX, std::string maxX, std::string minY, std::string maxY, float deadzone = -1.0f) const;
    float getAxis(std::string minAction, std::string maxAction) const;

    bool shouldQuit = false;
private:
    std::vector<std::unique_ptr<InputEvent>> inputs;
    int sdlKeyNum;
    const bool * sdlKeyArray = SDL_GetKeyboardState(&sdlKeyNum);
};


class InputRegistry {

};