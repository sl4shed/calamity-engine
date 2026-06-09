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

class RootScript : public Script
{
    Node *node;
    std::vector<std::shared_ptr<Node>> fallingNodes;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
    }

    void input(InputEvent &event)
    {
        InputEventMouseButton *mouseButtonEvent = dynamic_cast<InputEventMouseButton *>(&event);
        if (mouseButtonEvent && mouseButtonEvent->pressed && mouseButtonEvent->button == MouseButton::RIGHT)
        {
            for (auto node : fallingNodes)
            {
                node->free();
            }
            fallingNodes.clear();
        }
    }

    void update(float dt)
    {
        if (Services::input()->isActionJustPressed("add"))
        {
            std::shared_ptr<Node> fallingNode = std::make_shared<Node>("fallingNode");
#ifdef PSP
            fallingNode->transform.position = Vector2{rand() % 25 + 15, rand() % 25 + 15};
#else
            auto mousePos = Services::input()->getMousePosition();
            if (!mousePos.has_value()) return;
            fallingNode->transform.position = mousePos.value();
#endif

            std::shared_ptr<Shape> fallingShape;
            auto which = rand() % 4;
            if (which == 0)
            {
                fallingShape = std::make_shared<CircleShape>(static_cast<float>(rand() % 25 + 15));
            }
            else if (which == 1)
            {
                fallingNode->transform.setAngle(rand() % 100); // random angle between -0.5 and 0.5 radians
                fallingShape = std::make_shared<BoxShape>(Vector2{static_cast<float>(rand() % 25 + 15), static_cast<float>(rand() % 25 + 15)});
            }
            else if (which == 2)
            {
                float radius = rand() % 10 + 15;
                fallingNode->transform.setAngle(rand() % 100); // random angle between -0.5 and 0.5 radians
                fallingShape = std::make_shared<CapsuleShape>(Vector2{0, -10}, Vector2{0, 10}, radius, Vector2{0.5f, 0.5f});
            }
            else if (which == 3) {
                fallingShape = std::make_shared<SegmentShape>(Vector2{static_cast<float>(rand() % 25 + 15), static_cast<float>(rand() % 25 + 15)}, Vector2{static_cast<float>(rand() % 25 + 15), static_cast<float>(rand() % 25 + 15)});
            }
            fallingNode->addComponent(std::make_shared<RigidBody>(fallingShape));
            std::shared_ptr<ShapeSprite> fallingSprite = std::make_shared<ShapeSprite>(fallingShape);
            fallingSprite->modulate = Color::RED;
            if(which == 3) fallingSprite->modulate = Color::GREEN;
            fallingNode->addComponent(fallingSprite);

            fallingNodes.push_back(fallingNode);
            node->addChild(fallingNode);
            fallingNode->initialize();
        }
    }
};

CEREAL_REGISTER_TYPE(RootScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RootScript);