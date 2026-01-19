#include "engine.hpp"

Engine::Engine() {
    root = Node();
    root.transform.position = {0, 0};
    root.transform.scale = {1, 1};
    root.transform.angle = 0;
    root.transform.origin = {0, 0};
}

Engine::~Engine() {
    // idk
}

void Engine::update() {
    root.update();
}

void Engine::render(Graphics &graphics) {
    graphics.preRender();
    root.render(graphics);
    graphics.postRender();
}
