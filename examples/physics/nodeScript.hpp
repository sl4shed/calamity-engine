#pragma once

#include <iostream>
#include <cstdlib>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/input/keycode.hpp"
#include "backend/services/physics/physics.hpp"

class NodeScript : public Script
{
    Node *node;
    std::shared_ptr<Node> objects;

    bool controller = false;
    const int SPEED = 500;
    std::shared_ptr<Shape> shape;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        objects = node->getWindow()->root->getChild("objects");

        randomize();
    }

    void input(InputEvent &event)
    {
        auto *status = dynamic_cast<InputEventControllerStatus *>(&event);
        if (status)
        {
            controller = status->connected;
        }

        if (event.isActionPressed("add"))
        {
            // add a random object
            place();
            randomize();
        }

        if (event.isActionPressed("clear"))
        {
            // clear all objects
            for (auto child : objects->children)
            {
                child->free();
            }
        }
    }

    void update(float dt)
    {
        // if a controller is connected, enable directional input with the joystick
        if (controller)
        {
            auto vector = Services::input()->getVector("left", "right", "up", "down");
            node->transform.position = node->transform.position + (vector * dt * SPEED);
        }
        else
        {
            auto mousePos = Services::input()->getMousePosition();
            if (!mousePos.has_value())
                return;
            node->transform.position = mousePos.value();
        }
    }

    void randomize()
    {
        auto which = rand() % 3;
        if (which == 0)
        {
            shape = std::make_shared<CircleShape>(static_cast<float>(rand() % 25 + 15));
        }
        else if (which == 1)
        {
            node->transform.setAngle(rand() % 100);
            shape = std::make_shared<BoxShape>(Vector2{static_cast<float>(rand() % 25 + 15), static_cast<float>(rand() % 25 + 15)});
        }
        else if (which == 2)
        {
            float radius = rand() % 10 + 15;
            node->transform.setAngle(rand() % 100);
            shape = std::make_shared<CapsuleShape>(Vector2{0, -10}, Vector2{0, 10}, radius, Vector2{0.5f, 0.5f});
        }

        auto sprite = node->getComponent<ShapeSprite>();
        if (sprite)
        { // does the node have a shapesprite?
            sprite->shape = shape;
        }
        else
        {
            auto sprite = std::make_shared<ShapeSprite>(shape);
            sprite->modulate = Color(255, 255, 255, 100);
            node->addComponent(sprite);
        }
    }

    void place()
    {
        auto fallingNode = std::make_shared<Node>("fallingNode");
        auto sprite = std::make_shared<ShapeSprite>(shape);
        sprite->modulate = Color::RED;

        auto body = std::make_shared<RigidBody>(shape);

        fallingNode->transform = node->transform;
        fallingNode->addComponent(sprite);
        fallingNode->addComponent(body);

        objects->addChild(fallingNode);
        fallingNode->initialize();
    }
};

CEREAL_REGISTER_TYPE(NodeScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, NodeScript);