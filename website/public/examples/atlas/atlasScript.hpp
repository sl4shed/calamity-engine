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

class AtlasScript : public Script
{
    Node *node;
    Sprite *spr;


public:
    template <class Archive>
    void save(Archive &ar) const
    {

    }

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        spr = node->currentSprite;
    }

    void input(InputEvent& event) {
        InputEventKey *inputKey = dynamic_cast<InputEventKey*>(&event);
        if (inputKey && inputKey->pressed == true)
        {
            switch (inputKey->scancode)
            {
            case Keycode::W:
                spr->sourceTransform.position = {0, 0};
                break;
            case Keycode::A:
                spr->sourceTransform.position = {160, 0};
                break;
            default:
                break;
            }
        }
    }
};

CEREAL_REGISTER_TYPE(AtlasScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, AtlasScript);