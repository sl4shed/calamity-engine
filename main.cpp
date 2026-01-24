#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "backend/definitions.hpp"
#include "backend/graphics.hpp"
#include "backend/node.hpp"
#include "backend/utils.hpp"
#include "backend/engine.hpp"
#include "backend/components.hpp"
#include "backend/file.hpp"

#include "scripts/BirdScript.hpp"
#include "scripts/CameraScript.hpp"
#include <iostream>

int loop(Graphics &graphics, Engine &engine, bool &running)
{
    engine.update();
    engine.render(graphics);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    printf("hello worlsd\n");
    Graphics graphics = Graphics();
    Engine engine = Engine();

    // bird 1
    Node *bird = new Node();
    bird->name = std::string("Bird");

    Sprite *birdSprite = new Sprite();
    birdSprite->texture = graphics.loadTexture(std::string("assets/flappy.png"));
    printf("penis\n");
    birdSprite->visible = true;
    birdSprite->zIndex = 1;
    birdSprite->origin = {0, 0};
    bird->addComponent(birdSprite);

    bird->transform.position = {200, 100};

    BirdScript *birdScript = new BirdScript();
    bird->addComponent(birdScript);
    engine.root.addChild(bird);

    // camera node
    Node *cameraNode = new Node();
    cameraNode->name = std::string("Main Camera");
    Camera *cameraComponent = new Camera();
    cameraNode->addComponent(cameraComponent);
    engine.root.addChild(cameraNode);
    CameraScript *cameraScript = new CameraScript();
    cameraNode->addComponent(cameraScript);
    cameraComponent->setActive(engine);

    // bird 2
    Node *bird2 = new Node();
    bird2->name = std::string("Bird 2");
    bird2->addComponent(birdSprite);

    bird2->transform.position = {200, 0};
    bird2->transform.scale({0.8, 0.8});
    bird->addChild(bird2);

    // bird 3
    Node *bird3 = new Node();
    bird3->name = std::string("Bird 3");
    bird3->addComponent(birdSprite);

    bird3->transform.position = {120, 0};
    bird3->transform.scale({0.6, 0.6});
    bird2->addChild(bird3);

    // script start functions
    cameraScript->start();
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
