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

class AudioScript : public Script
{
    Node *node;
    std::shared_ptr<AudioSource> src;

    int frameCounter = 0;
public:
    template <class Archive>
    void save(Archive &ar) const{}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        src = node->getComponent<AudioSource>();
    }

    void update(float deltaTime)
    {
        if (src->getPlaying())
        {
            node->transform.rotate(60 * deltaTime);
        }
    }

    void input(InputEvent& event) {
        InputEventKey *inputKey = dynamic_cast<InputEventKey*>(&event);
        if (inputKey && inputKey->pressed == true)
        {
            switch (inputKey->scancode)
            {
            case Keycode::SPACE:
                if (src->getPlaying())
                {
                    src->stop();
                } else
                {
                    src->play();
                }
                break;
            default:
                break;
            }
        }
    }
};

CEREAL_REGISTER_TYPE(AudioScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, AudioScript);