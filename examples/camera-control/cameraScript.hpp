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

class CameraScript : public Script
{
    const int SPEED = 500;
    const int ROTATE_SPEED = 50;

    Input *pe;
    Node *node;

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

    void update(float dt)
    {
        auto rot = pe->getAxis("rotateLeft", "rotateRight");
        auto vec = pe->getVector("left", "right", "up", "down");

        // Rotate the movement vector by the node's current angle
        float angle = node->transform.getAngleRadians();
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);

        // Assuming vec has x and y components
        float rotatedX = vec.x * cos_a - vec.y * sin_a;
        float rotatedY = vec.x * sin_a + vec.y * cos_a;

        node->transform.position.x += rotatedX * dt * SPEED;
        node->transform.position.y += rotatedY * dt * SPEED;

        node->transform.rotate(rot * dt * ROTATE_SPEED);
    }

    void input(InputEvent& event)
    {
        if (event.isActionPressed("rotateReset"))
        {
            node->transform.setAngle(0.0f);
        }
    }
};

CEREAL_REGISTER_TYPE(CameraScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, CameraScript);