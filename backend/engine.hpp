#pragma once
#include "definitions.hpp"
#include "node.hpp"

class Engine
{
public:
    Engine();
    ~Engine();
    Node root;
    void update();
    void render(Graphics &graphics);

    void setActiveCamera(Camera *camera);
    Camera *getActiveCamera();

private:
    Camera *activeCamera;
};