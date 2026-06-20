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
#include "backend/services/graphics/definitions.hpp"
#include "backend/services/graphics/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/input/keycode.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/utils/file.hpp"

class BoxScript : public Script
{
    Node *node;

    std::shared_ptr<Node> catNode;
    std::shared_ptr<Window> window;

    Vector2 size;
    float angle;

    bool controller = false;
    const int SPEED = 500;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        node = this->getNode();
        window = node->getWindow();
        catNode = window->root->getChild("catNode");

        randomize();
    }

    void input(InputEvent &event)
    {
        // when the controller connects/disconnects, change the input mode
        InputEventControllerStatus *status = dynamic_cast<InputEventControllerStatus *>(&event);
        if (status)
        {
            controller = status->connected;
            return;
        }

        // add event
        if (event.isActionPressed("add"))
        {
            place();
            randomize();
        }

        // clear event
        if (event.isActionPressed("clear"))
        {
            auto children = catNode->children;
            for (auto child : children)
            {
                child->free();
            }
        }

        // save event
        if (event.isActionPressed("save"))
        {
            auto nodeTree = exportNodeTree(catNode);

            File *file = File::open("user://save.data", "w");
            Logger::info("Save path: {}", file->getAbsolutePath());
            file->storeString(nodeTree);
            file->flush();
            file->close();
        }

        // load event
        if (event.isActionPressed("load"))
        {
            if (!File::fileExists("user://save.data"))
                return;
            File *file = File::open("user://save.data", "r");

            std::string saveData = file->getAsText();
            if (saveData != "")
            {
                auto children = catNode->children;
                for (auto n : children)
                {
                    n->free();
                }
                loadNodeTree(catNode, saveData);
            }
            else
            {
                Logger::warn("No save data to load!");
            }

            file->close();
        }
    }

    void update(float dt)
    {
        if (controller)
        {
            Vector2 vec = Services::input()->getVector("left", "right", "up", "down");
            node->transform.position = node->transform.position + (vec * dt * SPEED);
        }
        else
        {
            std::optional<Vector2> mousePos = Services::input()->getMousePosition();
            if (!mousePos.has_value())
                return;

            node->transform.position = mousePos.value();
        }
    }

    void randomize()
    {
        angle = rand() % 100;
        node->transform.setAngle(angle);
        size = Vector2{(float)(rand() % 75 + 25), (float)(rand() % 75 + 25)};

        auto sprite = node->getComponent<Sprite>();
        sprite->texture.width = size.x;
        sprite->texture.height = size.y;
    }

    void place()
    {
        auto fallingNode = std::make_shared<Node>("fallingNode");
        fallingNode->transform = node->transform;

        auto fallingShape = std::make_shared<BoxShape>(size);

        Material mat = Material();
        mat.friction = 1.0f;
        mat.density = 2.0f;
        fallingShape->applyMaterial(mat);

        std::shared_ptr<RigidBody> rigidBody = std::make_shared<RigidBody>(fallingShape);
        fallingNode->addComponent(rigidBody);

        std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>("res://assets/cat.png", window);
        sprite->texture.width = size.x;
        sprite->texture.height = size.y;
        fallingNode->addComponent(sprite);

        catNode->addChild(fallingNode);
        fallingNode->initialize();
    }
};

CEREAL_REGISTER_TYPE(BoxScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, BoxScript);