#include <SDL.h>
#include "backend/definitions.hpp"
#include "backend/graphics.hpp"
#include "backend/node.hpp"
#include "backend/utils.hpp"
#include "backend/engine.hpp"
#include "backend/components.hpp"
#include "backend/file.hpp"

#include "scripts/BirdScript.hpp"
#include <iostream>

int loop(Graphics &graphics, Engine &engine, bool &running)
{
    engine.update();
    engine.render(graphics);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    Graphics graphics = Graphics();
    Engine engine = Engine();

    Node *bird = new Node();
    bird->name = std::string("Bird");

    Sprite *birdSprite = new Sprite();
    birdSprite->texture = graphics.loadTexture(std::string("assets/flappy.png"));
    birdSprite->visible = true;
    birdSprite->z_index = 1;
    bird->addComponent(birdSprite);

    bird->transform.position = {200, 100};
    bird->transform.scale = {100, 100};
    bird->transform.origin = {0.5, 0.5};
    bird->transform.angle = 0.0f;

    BirdScript *birdScript = new BirdScript();
    bird->addComponent(birdScript);
    engine.root.addChild(bird);

    Node *bird2 = new Node();
    bird2->name = std::string("Bird 2");
    bird2->addComponent(birdSprite);

    bird2->transform.position = {100, 0};
    bird2->transform.scale = {80, 80};
    bird2->transform.origin = {0, 0};
    bird2->transform.angle = 0.0f;
    bird->addChild(bird2);

    Node *bird3 = new Node();
    bird3->name = std::string("Bird 3");
    bird3->addComponent(birdSprite);

    bird3->transform.position = {80, 0};
    bird3->transform.scale = {60, 60};
    bird3->transform.origin = {0.5, 0.5};
    bird3->transform.angle = 0.0f;
    bird2->addChild(bird3);

    // script start functions
    birdScript->start();

    // exportNodeTree(&engine.root);
    // std::cout << readFileText("assets/clug.txt") << std::endl;

    // main loop
    bool running = true;
    while (running)
    {
        loop(graphics, engine, running);
    }

    return 0;
}
