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
        bird->transform.angle += 0.01f;
    }
};