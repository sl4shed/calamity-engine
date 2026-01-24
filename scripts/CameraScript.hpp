#pragma once

#include <iostream>
#include "../backend/definitions.hpp"
#include "../backend/node.hpp"
#include "../backend/components.hpp"

class CameraScript : public Script
{
    Node *camera;

public:
    void start()
    {
        camera = this->getNode();
    }

    void update(float deltaTime)
    {
        // camera->transform.position.x += 0.05f * deltaTime;
        // camera->transform.rotate(0.001f * deltaTime);
        // camera->transform.scale({1.0f + 0.0001f * deltaTime, 1.0f + 0.0001f * deltaTime});
    }
};