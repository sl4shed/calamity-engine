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
#include "backend/services/physics/physics.hpp"

class PlayerScript : public Script
{
    const int SPEED = 5;
    const int JUMP = 50;

    Input *pe;
    Node *node;
    std::shared_ptr<RigidBody> body;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        pe = Services::input();
        body = node->getComponent<RigidBody>();
    }

    void input(InputEvent& event) override
    {
        if (event.isActionPressed("up"))
        {
            Vector2 vel = body->getLinearVelocity();
            body->setLinearVelocity(Vector2{vel.x, vel.y - JUMP});
        }
    }

    void physicsUpdate()
    {
        auto vec = pe->getAxis("left", "right");
        body->setLinearVelocity({vec * SPEED, 0});

    }
};

CEREAL_REGISTER_TYPE(PlayerScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, PlayerScript);