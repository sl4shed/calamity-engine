#include <SDL.h>
#include "backend/definitions.hpp"
#include "backend/graphics.hpp"
#include "backend/node.hpp"
#include "backend/utils.hpp"
#include "backend/engine.hpp"
#include <iostream>

int loop(Graphics &graphics, Engine &engine, bool running, Node &bird)
{
    bird.transform.angle += 0.05f;
    engine.update();
    engine.render(graphics);

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
    bird->transform.scale = {0.6, 0.6};
    bird->transform.origin = {0.5, 0.5};
    bird->transform.angle = 0.0f;
    engine.root.addChild(bird);

    Node *bird2 = new Node();
    bird2->name = std::string("Bird 2");
    Sprite *fireSprite = new Sprite();
    fireSprite->texture = graphics.loadTexture(std::string("assets/flappy.png"));
    fireSprite->visible = true;
    fireSprite->z_index = 1;
    bird2->addComponent(fireSprite);

    bird2->transform.position = {60, 0};
    bird2->transform.scale = {0.8, 0.8};
    bird2->transform.origin = {0.5, 0.5};
    bird2->transform.angle = 0.0f;
    bird->addChild(bird2);

    bool running = true;
    while (running)
    {
        loop(graphics, engine, running, *bird);
    }

    return 0;
}
