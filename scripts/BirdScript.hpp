#pragma once

#include <iostream>
#include "../backend/definitions.hpp"
#include "../backend/node.hpp"
#include "../backend/components.hpp"

class BirdScript : public Script
{
    Node *bird;

public:
    void Start()
    {
        std::cout << "start" << std::endl;
        bird = this->getNode();
    }

    void Update()
    {
        bird->transform.angle += 0.1f;
    }
};