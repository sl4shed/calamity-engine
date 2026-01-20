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
        std::cout << "start" << std::endl;
        bird = this->getNode();
    }

    void update()
    {
        std::cout << "update" << std::endl;
        bird->transform.angle += 0.1f;
    }
};