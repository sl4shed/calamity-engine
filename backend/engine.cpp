#include "engine.hpp"

Engine::Engine()
{
    root = Node();
    root.transform.position = {0, 0};
    root.transform.scale = {1, 1};
    root.transform.angle = 0;
    root.transform.origin = {0, 0};
}

Engine::~Engine()
{
    // idk
}

void Engine::update()
{
    root.update();
}

void Engine::render(Graphics &graphics)
{
    graphics.preRender();
    root.render(graphics);
    graphics.postRender();
}

void Engine::setActiveCamera(Camera *camera)
{
    if (activeCamera)
    {
        activeCamera->active = false;
    }
    activeCamera = camera;
}

Camera *Engine::getActiveCamera()
{
    return activeCamera;
}