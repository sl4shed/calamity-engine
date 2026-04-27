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
#include "backend/services/graphics.hpp"
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

    void input(InputEvent& event) {
        InputEventMouseButton* mouseButtonEvent = dynamic_cast<InputEventMouseButton*>(&event);
        if(mouseButtonEvent && mouseButtonEvent->pressed && mouseButtonEvent->buttonIndex == MouseButton::RIGHT) {
            for(auto node : fallingNodes) {
                node->free();
            }
            fallingNodes.clear();
        }
    }

    void update(float dt) {
        if(Services::input()->isActionJustPressed("add")) {
            Logger::info("Adding circle");
            std::shared_ptr<Node> fallingNode = std::make_shared<Node>("fallingNode");
            fallingNode->transform.position = Services::input()->getMousePosition();
            //fallingNode->transform.setAngle(rand() % 100 / 100.0f - 0.5f); // random angle between -0.5 and 0.5 radians

            auto fallingShape = std::make_shared<CircleShape>((float)(rand() % 25 + 15));
            fallingNode->addComponent(std::make_shared<RigidBody>(fallingShape));
            std::shared_ptr<ShapeSprite> fallingSprite = std::make_shared<ShapeSprite>(fallingShape);
            fallingSprite->modulate = Color::RED;
            fallingNode->addComponent(fallingSprite);

            fallingNodes.push_back(fallingNode);
            node->addChild(fallingNode);
            fallingNode->initialize();
        }
    }
};

CEREAL_REGISTER_TYPE(RootScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RootScript);