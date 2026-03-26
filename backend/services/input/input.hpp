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

// enums //

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

// input events //

class InputEvent
{
public:
    virtual ~InputEvent() = default;
    virtual bool operator==(const InputEvent& other) const = 0;

    bool isActionPressed(std::string action) const;
    bool isActionReleased(std::string action) const;
    bool isAction(std::string action) const;
    // float getActionStrength(std::string action) const;
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
    bool operator==(const InputEventKey& other) const;

    bool echo = false;
    Keycode scancode = Keycode::UNKNOWN;
    Keycode keycode = Keycode::UNKNOWN;
    const char * keyLabel;
    bool pressed = false;
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
    bool operator==(const InputEventMouseButton& other) const;

    MouseButton buttonIndex = MouseButton::MOUSE_BUTTON_NONE;
    bool double_click = false;
    Vector2 factor = {0, 0};
    bool pressed = false;
};

class InputEventMouseMotion : public InputEventMouse {
public:
    bool operator==(const InputEventMouseMotion& other) const;

    // this also has some stuff to do with drawing pens so todo that i guess
    // should be easy with SDL pen events
    Vector2 relative = {0, 0};
    Vector2 position = {0, 0};
};

class InputEventControllerButton : public InputEvent {
public:
    bool operator==(const InputEventControllerButton& other) const;
    ControllerButton button;
    bool pressed;
    float pressure;
    int device;
};

class InputEventControllerStatus : public InputEvent {
public:
    bool operator==(const InputEventControllerStatus& other) const;
    int device;
    bool connected;
};

class InputEventControllerMotion : public InputEvent {
public:
    bool operator==(const InputEventControllerMotion& other) const;
    int device;
    float motion;
    ControllerAxis axis;
};

#include "../../core/node/components.hpp"
// input stuff //

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
    bool isControllerButtonPressed(int device, ControllerButton button) const;
    bool isKeyLabelPressed(const char * label) const;
    bool isMouseButtonPressed(MouseButton button) const;
    Vector2 getMousePosition() const;
    bool isActionPressed() const; // big todo

    // controllers, todo
    float getControllerAxis(int device, ControllerAxis axis) const;
    std::string getControllerGUID(int device) const;
    std::string getControllerName(int device) const;
    // todo these two functions but i think theyre useless
    //float getControllerVibrationDuration(int device); 
    //Vector2 getControllerVibrationStrength(int device);

    std::vector<int> getConnectedControllers();
    void startControllerVibration(int device, float weakMagnitude, float strongMagnitude, int durationMs = 0);
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
    std::vector<int> controllers;
};

// input registry stuff //

struct InputRegistryAction {
    std::string name;
    std::vector<std::unique_ptr<InputEvent>> events;
    float deadzone;
};

class InputRegistry {
public:
    int actionAddEvent(std::string name, std::unique_ptr<InputEvent> event);
    void actionRemoveEvent(std::string action, int index);
    void actionRemoveEvents(std::string action);
    float actionGetDeadzone(std::string action);
    std::vector<InputEvent> actionGetEvents(std::string action);
    // bool actionHasEvent(std::string action, InputEvent event);
    void actionSetDeadzone(std::string action, float deadzone);
    void addAction(std::string action, float deadzone = 0.2f);
    void removeAction(std::string action);
    bool eventIsAction(std::unique_ptr<InputEvent> event, std::string action);
    std::vector<std::string> getActions();
    bool hasAction(std::string action) const;
private:
    std::unordered_map<std::string, InputRegistryAction> actions;
};