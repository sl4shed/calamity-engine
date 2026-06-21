#pragma once

#include <iostream>
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
#include "backend/services/physics/definitions.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/core/ui/definitions.hpp"
#include "backend/core/ui/label.hpp"

class ButtonNode : public Node
{
    std::string text;
    Vector2 size;
    bool hovered;
    Vector2 origin;

public:
    std::string name;

    void setHovered(bool v)
    {
        hovered = v;
    }

    ButtonNode(const std::string &name = "ButtonNode", const std::string &text = "Button Text", Vector2 size = {100.0f, 50.0f}, Vector2 origin = {0.5f, 0.5f}) : name(name), text(text), size(size), origin(origin)
    {
        std::shared_ptr<Shape> shape = std::make_shared<BoxShape>(size, origin);

        shape->setSensor(true);
        auto staticBody = std::make_shared<StaticBody>(shape);

        staticBody->mouseEntered.connect([this, staticBody]()
                                         { this->setHovered(true); staticBody->mouseEntered.reset(); this->mouseEntered.fire(); this->getComponent<ShapeSprite>()->modulate = Color(233, 233, 233); });

        staticBody->mouseExited.connect([this, staticBody]()
                                        { this->setHovered(false); staticBody->mouseExited.reset(); this->mouseExited.fire(); this->getComponent<ShapeSprite>()->modulate = Color(255, 255, 255); });

        auto shapeSprite = std::make_shared<ShapeSprite>(shape);
        shapeSprite->modulate = Color(255, 255, 255);

        auto font = std::make_shared<Font>("res://calamity/default.ttf");
        font->setAlignment(FontAlignment::CENTER);
        font->setSize(14);

        auto labelNode = std::make_shared<Node>("labelNode");
        auto label = std::make_shared<Label>(text, font);
        label->origin = origin;
        label->size = size;
        labelNode->transform.position = Vector2(-size.x * origin.x, -size.y * origin.y);
        label->setColor(Color::BLACK);
        label->font->setAlignment(FontAlignment::CENTER);

        addComponent(staticBody);
        addComponent(shapeSprite);
        labelNode->addComponent(label);
        addChild(labelNode);
    }

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Node>(this), CEREAL_NVP(text), CEREAL_NVP(size), CEREAL_NVP(origin));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Node>(this), CEREAL_NVP(text), CEREAL_NVP(size), CEREAL_NVP(origin));
    }

    Signal<> mouseEntered;
    Signal<> mouseExited;
    Signal<> clicked;

    void input(InputEvent &event)
    {
        Node::input(event);

        InputEventMouseButton *mouseButtonEvent = dynamic_cast<InputEventMouseButton *>(&event);
        if (hovered && mouseButtonEvent && mouseButtonEvent->pressed && mouseButtonEvent->button == MouseButton::LEFT)
        {
            this->getComponent<ShapeSprite>()->modulate = Color(200, 200, 200);
            this->clicked.fire();
        }
        else if (mouseButtonEvent && mouseButtonEvent->pressed == false && mouseButtonEvent->button == MouseButton::LEFT)
        {
            if (hovered)
            {
                this->getComponent<ShapeSprite>()->modulate = Color(233, 233, 233);
            }
            else
            {
                this->getComponent<ShapeSprite>()->modulate = Color(255, 255, 255);
            }
        }
    }
};

CEREAL_REGISTER_TYPE(ButtonNode);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Node, ButtonNode);