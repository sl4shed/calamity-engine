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
    const int SPEED = 500;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        pe = Services::input();
    }

    void update(float dt) {
        auto vec = Services::input()->getVector("left", "right", "up", "down");
        node->transform.position = node->transform.position + (vec * dt * SPEED);
    }

    void input(InputEvent& event) {
        // nothing for now
    }
};

CEREAL_REGISTER_TYPE(InputScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, InputScript);