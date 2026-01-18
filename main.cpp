#include <SDL.h>
#include "backend/definitions.hpp"
#include "backend/graphics.hpp"
#include "backend/node.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    Node node = Node();
    Graphics graphics = Graphics();

    Sprite* component = new Sprite();
    component->texture = graphics.loadTexture(std::string("assets/flappy.png"));
    component->visible = true;
    component->z_index = 1;
    node.addComponent(component);

    // Initialize node transform
    node.transform.position = {100, 100};
    node.transform.scale = {150, 150};
    node.transform.origin = {0.5, 0.5};
    node.transform.angle = 0.0f;

    SDL_Event event;
    bool running = true;
    while (running) {
        graphics.preRender();
        node.transform.angle += 0.1f;
        node.render(graphics);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        graphics.postRender();
    }

    return 0;
}
