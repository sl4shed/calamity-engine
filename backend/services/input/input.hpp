#pragma once
#include "events.hpp"
#include <SDL3/SDL.h>
#include "../../core/definitions.hpp"
#include "keycode.hpp"
#include <map>
#include <optional>

/**
 * # InputRegistry
 * The InputRegistry allows you to define actions which are binded to specific events (like, for example InputEventKey). Once you add an action, you can bind an event to it using `actionAddEvent()`.
 *
 * ```cpp
 * // Add an action called "left" which is binded to a InputEventControllerMotion event. This action has a deadzone of 0.2f
 * InputRegistry.addAction("left", 0.2f);
 * auto leftEvent = std::make_unique<InputEventControllerMotion>();
 * leftEvent->device = 0;
 * leftEvent->axis = ControllerAxis::LEFT_X;
 * leftEvent->motion = -1.0f;
 * inputRegistry.actionAddEvent("left", std::move(leftEvent));
 * ```
 *
 * Once you do that, you can check on a scripts input function if the event is a specific action:
 * ```cpp
 * void input(InputEvent &event)
 * {
 *      if(event.isActionPressed("left")) { // Since the action has
 *           // code here
 *      }
 * }
 * ```
 *
 * Make sure to also check out the [input example](https://calamity.sl4shed.xyz/example-input)!
 */
class InputRegistry
{
public:
    int actionAddEvent(const std::string &name, std::unique_ptr<InputEvent> event);
    void actionRemoveEvent(const std::string &action, int index);
    void actionRemoveEvents(const std::string &action);
    float actionGetDeadzone(const std::string &action) const;
    std::vector<InputEvent> actionGetEvents(std::string action);
    void actionSetDeadzone(const std::string &action, float deadzone);
    void addAction(const std::string &action, float deadzone = 0.1f);
    void removeAction(const std::string &action);
    bool eventIsAction(const InputEvent *event, const std::string &name, bool identityCheck = false);
    std::vector<std::string> getActions();
    bool hasAction(const std::string &action) const;
    std::unordered_map<std::string, InputRegistryAction> *getActionsArray();

private:
    std::unordered_map<std::string, InputRegistryAction> actions;
};

// input stuff //

/**
 * # Input class
 *
 * The Input service lets you obtain information about \ref Action "Actions", current state of the mouse, current state of controller buttons, keys, etc...
 * It's basically the same thing as the [Godot Input class](https://docs.godotengine.org/en/stable/classes/class_input.html). A lot of the functions are of the same name, and are pretty self explanatory.
 *
 * Make sure to also check out the [input example](https://calamity.sl4shed.xyz/example-input)!
 */
class Input
{
public:
    void update(float deltaTime);

    bool isKeyPressed(Keycode key) const;
    bool isControllerButtonPressed(int device, ControllerButton button) const;
    bool isKeyLabelPressed(const char *label) const;
    bool isMouseButtonPressed(MouseButton button) const;
    std::optional<Vector2> getMousePosition() const;

    SDL_AppResult processInput(SDL_Event *event);
    void dispatch(InputEvent &event);

    // controllers //
    float getControllerAxis(int device, ControllerAxis axis) const;
    std::string getControllerGUID(int device) const;
    std::string getControllerName(int device) const;

    std::vector<int> getConnectedControllers();
    void startControllerVibration(int device, float weakMagnitude, float strongMagnitude, int durationMs = 0) const;
    // void vibrateHandheld(int durationMs = 500, float amplitude = -1.0f); // no mobile support, yet :)

    // input getting methods //
    Vector2 getVector(const std::string &minX, const std::string &maxX, const std::string &minY, const std::string &maxY, float deadzone = -1.0f) const;
    float getAxis(const std::string &minAction, const std::string &maxAction) const;
    bool isActionJustPressed(const std::string &name) const;
    bool isActionJustReleased(const std::string &name) const;
    bool isActionPressed(const std::string &name) const;

    bool shouldQuit = false;

private:
    std::vector<std::unique_ptr<InputEvent>> inputs;
    int sdlKeyNum = 0;
    const bool *sdlKeyArray = SDL_GetKeyboardState(&sdlKeyNum);

    std::unordered_map<std::string, float> actionStrength;
    std::unordered_map<std::string, float> prevActionStrength;
    std::unordered_map<std::string, InputRegistryAction> *actionsArrayPointer = nullptr;
    std::unordered_set<std::string> heldActions;

    // sdl controller id -> sequential controller id (calamity controller id)
    std::map<int, int> controllerMap;
    std::map<int, int> reversedControllerMap; // its stupid, but it works, so its not stupid
};