#pragma once
#include "definitions.hpp"
#include "node.hpp"

/**
 *
 * test for doxygen
 * # text for doxygen
 *
 */
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

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

private:
    Camera *activeCamera = nullptr;
};