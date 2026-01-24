#pragma once

#include <iostream>
#include "../backend/definitions.hpp"
#include "../backend/node.hpp"
#include "../backend/components.hpp"

class BirdScript : public Script
{
    Node *bird;

public:
    void start()
    {
        bird = this->getNode();
    }

    void update(float deltaTime)
    {
        float angle = bird->transform.getAngle();
        // printf("angle: %f\n", angle);
        bird->transform.rotate(0.001f * deltaTime);
        // printf("new angle: %f\n", bird->transform.getAngle());
    }
};