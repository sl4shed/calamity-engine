#pragma once

#include <iostream>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include "backend/definitions.hpp"
#include "backend/node.hpp"
#include "backend/components.hpp"
#include "backend/input.hpp"
#include "backend/keycode.hpp"
#include "backend/logger.hpp"
#include "backend/definitions.hpp"

class InputScript : public Script
{
    Node* node;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void start() {
        node = this->getNode();
    }

    void input(Keycode keycode) {
        if(keycode == Keycode::W) {
            node->transform.position.x += 1;
        }
    }
};

CEREAL_REGISTER_TYPE(InputScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, InputScript);