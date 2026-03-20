#pragma once

#include <iostream>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/input/keycode.hpp"

class InputScript : public Script
{
    Node *node;
    Input *pe;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void start()
    {
        node = this->getNode();
        pe = Services::input();
    }

    void update(float dt) {
        if(pe->isKeyPressed(Keycode::W)) {
            node->transform = node->transform.inverse();
        }
    }

    void input(InputEvent& event) {
        if(auto* ev = dynamic_cast<InputEventMouseButton*>(&event)) {
            if(ev->buttonIndex == MouseButton::MOUSE_BUTTON_LEFT && ev->pressed == false) {
                // left mouse button released
                node->transform.rotate(0.1f);
            } else if (ev->buttonIndex == MouseButton::MOUSE_BUTTON_MIDDLE && ev->pressed == true)
            {
                // middle mouse pressed
                auto scale = node->transform.getScale();
                node->transform.scale({1.1f, 1.1f});
            }

            // factor is only different from 0 when the mouse scrolls
            node->transform.position = node->transform.position + ev->factor;
        }
    }
};

CEREAL_REGISTER_TYPE(InputScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, InputScript);