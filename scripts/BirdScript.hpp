#pragma once

#include <iostream>
#include "../backend/definitions.hpp"
#include "../backend/node.hpp"
#include "../backend/components.hpp"

class BirdScript : public Script
{
    Node *bird;
    Sprite *sprite;

public:
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