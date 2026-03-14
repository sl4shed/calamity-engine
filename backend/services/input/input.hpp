#pragma once

#include "../../core/definitions.hpp"
#include "keycode.hpp"

// multiple per frame
class InputEvent
{
public:
    InputEvent(Keycode keycode);

    bool isKeycode(Keycode compare);
private:
    Keycode keycode;
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
private:
    std::vector<InputEvent> inputs;
    std::vector<InputEvent> prevInputs;
};