#pragma once

#include <iostream>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include "../backend/definitions.hpp"
#include "../backend/node.hpp"
#include "../backend/components.hpp"

class BirdScript : public Script
{
    Node *bird;
    Sprite *sprite;

public:
    template <class Archive>
    void save(Archive &ar) const
    {
        ar(cereal::base_class<Script>(this));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(cereal::base_class<Script>(this));
    }

    void start()
    {
        bird = this->getNode();
        sprite = bird->currentSprite;
    }

    void update(float deltaTime)
    {
        // float angle = bird->transform.getAngle();
        //  printf("angle: %f\n", angle);
        // bird->transform.rotate(0.001f * deltaTime);
        //  printf("new angle: %f\n", bird->transform.getAngle());
        // sprite->origin.y += 0.0005f * deltaTime;
        // sprite->origin.x += 0.0005f * deltaTime;
    }
};