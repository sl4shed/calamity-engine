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
#include "backend/utils/file.hpp"

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

        InputEventControllerButton* controllerButtonEvent = dynamic_cast<InputEventControllerButton*>(&event);
        if(controllerButtonEvent && controllerButtonEvent->pressed && controllerButtonEvent->button == ControllerButton::SOUTH) {
            std::shared_ptr<Node> fallingNode = std::make_shared<Node>("fallingNode");
            fallingNode->transform.position = {0, -100};
            fallingNode->transform.setAngle(rand() % 100 / 100.0f - 0.5f); // random angle between -0.5 and 0.5 radians
            auto fallingShape = std::make_shared<BoxShape>(Vector2{(float)(rand() % 75 + 25), (float)(rand() % 75 + 25)});
            fallingNode->addComponent(std::make_shared<RigidBody>(fallingShape));
            std::shared_ptr<ShapeSprite> fallingSprite = std::make_shared<ShapeSprite>(fallingShape->polygon);
            fallingSprite->color = Color::RED;
            fallingNode->addComponent(fallingSprite);

            fallingNodes.push_back(fallingNode);
            node->addChild(fallingNode);
            fallingNode->initialize();
        }

        InputEventKey *keyEvent = dynamic_cast<InputEventKey*>(&event);
        if(keyEvent && keyEvent->isPressed() && keyEvent->keycode == Keycode::Z) {
            // guh why doesnt get child return a shared ptr
            // todo fix. bad.
            exportNodeTree(node->getChild("catNode"));
        }
    }

    void update(float dt) {
        if(Services::input()->isActionJustPressed("add")) {
            std::shared_ptr<Node> fallingNode = std::make_shared<Node>("fallingNode");
            fallingNode->transform.position = Services::input()->getMousePosition();
            fallingNode->transform.setAngle(rand() % 100 / 100.0f - 0.5f); // random angle between -0.5 and 0.5 radians
            Vector2 size = Vector2{(float)(rand() % 75 + 25), (float)(rand() % 75 + 25)};
            auto fallingShape = std::make_shared<BoxShape>(size);

            std::shared_ptr<RigidBody> rigidBody = std::make_shared<RigidBody>(fallingShape);
            // TODO: add friction setting
            fallingNode->addComponent(rigidBody);
            
            std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
            sprite->texture = Texture("res://assets/cat.png");
            sprite->texture.width = size.x;
            sprite->texture.height = size.y;
            sprite->origin = {0.0f, 0.0f};
            fallingNode->addComponent(sprite);

            fallingNodes.push_back(fallingNode);
            node->getChild("catNode")->addChild(fallingNode);
            fallingNode->initialize();
        }
    }
};

CEREAL_REGISTER_TYPE(RootScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RootScript);