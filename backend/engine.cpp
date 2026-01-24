#include "engine.hpp"

Engine::Engine()
{
    root = Node();
    root.transform.position = {0, 0};
}

Engine::~Engine()
{
    // idk
}

void Engine::update()
{
    last = now;
    now = SDL_GetPerformanceCounter();
    float deltaTime = (float)(now - last) * 1000 / SDL_GetPerformanceFrequency();

    root.update(deltaTime);
}

void Engine::render(Graphics &graphics)
{
    graphics.preRender();
    root.render(graphics, this);
    graphics.postRender();
}

void Engine::setActiveCamera(Camera *camera)
{
    if (activeCamera != nullptr)
    {
        activeCamera->active = false;
    }
    activeCamera = camera;
}

Camera *Engine::getActiveCamera()
{
    return activeCamera;
}