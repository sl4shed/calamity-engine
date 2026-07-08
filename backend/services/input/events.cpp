#include "events.hpp"
#include "input.hpp"

// event operators

bool InputEventKey::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventKey *>(&other);
    if (!o)
        return false;
    return (scancode == o->scancode && pressed == o->pressed && altPressed == o->altPressed && ctrlPressed == o->ctrlPressed && metaPressed == o->metaPressed && shiftPressed == o->shiftPressed);
}

bool InputEventMouseButton::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventMouseButton *>(&other);
    if (!o)
        return false;
    return (button == o->button && pressed == o->pressed && factor == o->factor && doubleClick == o->doubleClick);
}

bool InputEventMouseMotion::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventMouseMotion *>(&other);
    if (!o)
        return false;
    return (position == o->position && relative == o->relative);
}

bool InputEventControllerButton::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerButton *>(&other);
    if (!o)
        return false;
    return (device == o->device && button == o->button && pressed == o->pressed && pressure == o->pressure);
}

bool InputEventControllerStatus::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerStatus *>(&other);
    if (!o)
        return false;
    return (device == o->device && connected == o->connected);
}

bool InputEventControllerMotion::operator==(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerMotion *>(&other);
    if (!o)
        return false;
    return (device == o->device && axis == o->axis && motion == o->motion);
}

bool InputEventKey::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventKey *>(&other);
    if (!o)
        return false;
    return (scancode == o->scancode);
}

bool InputEventMouseButton::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventMouseButton *>(&other);
    if (!o)
        return false;
    return (button == o->button);
}

bool InputEventMouseMotion::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventMouseMotion *>(&other);
    if (!o)
        return false;
    return (position == o->position && relative == o->relative);
}

bool InputEventControllerButton::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerButton *>(&other);
    if (!o)
        return false;
    return (device == o->device && button == o->button);
}

bool InputEventControllerStatus::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerStatus *>(&other);
    if (!o)
        return false;
    return (device == o->device);
}

bool InputEventControllerMotion::operator<=(const InputEvent &other) const
{
    const auto *o = dynamic_cast<const InputEventControllerMotion *>(&other);
    if (!o)
        return false;
    return (device == o->device && axis == o->axis);
}

// action functions

bool InputEvent::isAction(const std::string &name) const
{
    return Services::inputRegistry()->eventIsAction(this, name);
}

bool InputEvent::isActionPressed(const std::string &name) const
{
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isPressed());
}

bool InputEvent::isActionReleased(const std::string &name) const
{
    return (Services::inputRegistry()->eventIsAction(this, name) && this->isReleased());
}