#pragma once
#include <SDL3/SDL.h>
#include "../../core/definitions.hpp"
#include "keycode.hpp"
#include <map>

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
    virtual bool operator==(const InputEvent& other) const { return false; };
    virtual bool operator<=(const InputEvent& other) const { return false; };

    bool isActionPressed(const std::string& action) const;
    bool isActionReleased(const std::string& action) const;
    bool isAction(const std::string& action) const;
    
    // float getActionStrength(std::string action) const;
    virtual bool isPressed() const { return false; }
    virtual bool isReleased() const { return false; }
    virtual float getStrength() const { return 0.0f; }
};

class InputEventAction : public InputEvent {
public:
    std::string action = "";
    int eventIndex = -1;
    bool pressed = false;
    float strength = 1.0;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return strength; };
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
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;

    bool echo = false; // todo
    Keycode scancode = Keycode::UNKNOWN;
    Keycode keycode = Keycode::UNKNOWN;
    const char * keyLabel;
    bool pressed = false;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; }
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
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;

    MouseButton buttonIndex = MouseButton::NONE;
    bool doubleClick = false;
    Vector2 factor = {0, 0};
    bool pressed = false;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; };
};

class InputEventMouseMotion : public InputEventMouse {
public:
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;

    // this also has some stuff to do with drawing pens so todo that i guess
    // should be easy with SDL pen events
    Vector2 relative = {0, 0};
    Vector2 position = {0, 0};
};

class InputEventControllerButton : public InputEvent {
public:
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;

    ControllerButton button;
    bool pressed;
    float pressure = 1.0f;
    int device;

    bool isPressed() const { return pressed; };
    bool isReleased() const { return !pressed; };
    float getStrength() const { return (float)pressed; } // for now its this because sdl doesnt do gamepad button pressure :(
};

class InputEventControllerStatus : public InputEvent {
public:
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;

    int device;
    bool connected;
};

class InputEventControllerMotion : public InputEvent {
public:
    bool operator==(const InputEvent& other) const;
    bool operator<=(const InputEvent& other) const;
    
    int device;
    float motion;
    ControllerAxis axis;

    float getStrength() const { return motion; };
};

#include "../../core/node/components.hpp"

// input registry stuff //

struct InputRegistryAction {
    std::string name;
    std::vector<std::unique_ptr<InputEvent>> events;
    float deadzone;
};

class InputRegistry {
public:
    int actionAddEvent(const std::string& name, std::unique_ptr<InputEvent> event);
    void actionRemoveEvent(const std::string& action, int index);
    void actionRemoveEvents(const std::string& action);
    float actionGetDeadzone(const std::string& action) const;
    std::vector<InputEvent> actionGetEvents(std::string action);
    void actionSetDeadzone(const std::string& action, float deadzone);
    void addAction(const std::string& action, float deadzone = 0.1f);
    void removeAction(const std::string& action);
    bool eventIsAction(const InputEvent* event, const std::string& name, bool identityCheck = false);
    std::vector<std::string> getActions();
    bool hasAction(const std::string& action) const;
    std::unordered_map<std::string, InputRegistryAction> *getActionsArray();
private:
    std::unordered_map<std::string, InputRegistryAction> actions;
};

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

    // controllers //
    float getControllerAxis(int device, ControllerAxis axis) const;
    std::string getControllerGUID(int device) const;
    std::string getControllerName(int device) const;
    // todo these two functions but i think theyre useless
    //float getControllerVibrationDuration(int device); 
    //Vector2 getControllerVibrationStrength(int device);

    std::vector<int> getConnectedControllers();
    void startControllerVibration(int device, float weakMagnitude, float strongMagnitude, int durationMs = 0) const;
    //void vibrateHandheld(int durationMs = 500, float amplitude = -1.0f); // no mobile support, yet :)

    // input getting methods //
    Vector2 getVector(const std::string& minX, const std::string& maxX, const std::string& minY, const std::string& maxY, float deadzone = -1.0f) const;
    float getAxis(const std::string& minAction, const std::string& maxAction) const;
    bool isActionJustPressed(const std::string& name) const;
    bool isActionJustReleased(const std::string& name) const;
    bool isActionPressed(const std::string& name) const;

    bool shouldQuit = false;
private:
    std::vector<std::unique_ptr<InputEvent>> inputs;
    int sdlKeyNum = 0;
    const bool * sdlKeyArray = SDL_GetKeyboardState(&sdlKeyNum);

    std::unordered_map<std::string, float> actionStrength;
    std::unordered_map<std::string, float> prevActionStrength;
    std::unordered_map<std::string, InputRegistryAction>* actionsArrayPointer = nullptr;
    std::unordered_set<std::string> heldActions;

    // sdl controller id -> sequential controller id (calamity controller id)
    std::map<int, int> controllerMap;
    std::map<int, int> reversedControllerMap; // its stupid, but it works, so its not stupid
};