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
            auto children = node->getChild("catNode")->children;
            for (auto n : children)
            {
                n->free();
            }
        }

        InputEventKey *keyEvent = dynamic_cast<InputEventKey*>(&event);
        if(keyEvent && keyEvent->pressed && keyEvent->scancode == Keycode::Z) {
            std::string exp = exportNodeTree(node->getChild("catNode"));
            File *file = File::open("user://save.data", "w");
            Logger::info("Save path: {}", file->getAbsolutePath());
            file->storeString(exp);
            file->flush();
            file->close();
        } else if (keyEvent && keyEvent->pressed && keyEvent->scancode == Keycode::V)
        {
            if (!File::fileExists("user://save.data")) return;
            File *file = File::open("user://save.data", "r");

            std::string saveData = file->getAsText();
            if (saveData != "")
            {
                auto children = node->getChild("catNode")->children;
                for (auto n : children)
                {
                    n->free();
                }
                loadNodeTree(node->getChild("catNode"), saveData);
            } else
            {
                Logger::info("no save data to load");
            }
        } else if (keyEvent && keyEvent->pressed && keyEvent->scancode == Keycode::B)
        {
            Logger::info("changing camera zoom");
            auto& transform = node->getChild("cameraNode")->transform;
            Logger::debug("x - {}, y - {}", transform.getScale().x, transform.getScale().y);
            if (transform.getScale() == Vector2{3.0f, 3.0f})
            {
                transform.setScale({1.0f, 1.0f});
            } else
            {
                transform.setScale({3.0f, 3.0f});
            }

            Logger::debug("x - {}, y - {}", transform.getScale().x, transform.getScale().y);
        }
    }

    void update(float dt) {
        if(Services::input()->isActionJustPressed("add")) {
            std::shared_ptr<Node> fallingNode = std::make_shared<Node>("fallingNode");
            fallingNode->transform.position = Services::input()->getMousePosition();
            fallingNode->transform.setAngle(rand() % 100 / 100.0f - 0.5f); // random angle between -0.5 and 0.5 radians
            Vector2 size = Vector2{(float)(rand() % 75 + 25), (float)(rand() % 75 + 25)};
            auto fallingShape = std::make_shared<BoxShape>(size);

            Material mat = Material();
            mat.friction = 1.0f;
            mat.density = 2.0f;
            fallingShape->applyMaterial(mat);

            std::shared_ptr<RigidBody> rigidBody = std::make_shared<RigidBody>(fallingShape);
            fallingNode->addComponent(rigidBody);

            std::shared_ptr<ShapeSprite> shapeSprite = std::make_shared<ShapeSprite>(fallingShape);
            shapeSprite->modulate = Color::RED;
            fallingNode->addComponent(shapeSprite);

            std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
            sprite->texture = Texture("res://assets/cat.png");
            sprite->texture.width = size.x;
            sprite->texture.height = size.y;
            sprite->origin = {0.0f, 0.0f};
            fallingNode->addComponent(sprite);

            node->getChild("catNode")->addChild(fallingNode);
            fallingNode->initialize();
        }
    }
};

CEREAL_REGISTER_TYPE(RootScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, RootScript);