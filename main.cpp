#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "backend/definitions.hpp"
#include "backend/graphics.hpp"
#include "backend/node.hpp"
#include "backend/utils.hpp"
#include "backend/engine.hpp"
#include "backend/components.hpp"
#include "backend/file.hpp"
#include "backend/logger.hpp"
#include <cereal/archives/json.hpp>

#include "scripts/BirdScript.hpp"
#include "scripts/CameraScript.hpp"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __psp__
#include <pspkernel.h>
#include <pspdebug.h>

PSP_MODULE_INFO("calamity-engine", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(20480);
#endif

int loop(Graphics &graphics, Engine &engine, bool &running)
{
    engine.update();
    engine.render(graphics);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
#ifdef __EMSCRIPTEN
            emscripten_cancel_main_loop();
#else
            running = false;
            return 0;
#endif
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    Logger::init();
    Graphics graphics = Graphics();
    Engine engine = Engine();

    Logger::info("Calamity Engine v1.0");

    // bird 1
    Node *bird = new Node();
    bird->name = std::string("Bird");

    Sprite *birdSprite = new Sprite();
    birdSprite->texture = Texture(&graphics, std::string("assets/flappy.png"));

    cereal::JSONOutputArchive archive(std::cout);
    archive(bird);
    archive(birdSprite->texture);

    birdSprite->visible = true;
    birdSprite->zIndex = 1;
    birdSprite->origin = {0.5, 0.5};
    bird->addComponent(birdSprite);

    bird->transform.position = {240, 136};

    BirdScript *birdScript = new BirdScript();
    bird->addComponent(birdScript);
    engine.root.addChild(bird);

    // camera node
    Node *cameraNode = new Node();
    cameraNode->name = std::string("Main Camera");
    cameraNode->transform.position = {240, 136};
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
    // Logger::debug("{}", readFileText("assets/clug.txt"));

// main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void *arg)
        {
            auto params = static_cast<std::pair<Graphics *, Engine *> *>(arg);
            Graphics &graphics = *(params->first);
            Engine &engine = *(params->second);
            bool running = true;
            loop(graphics, engine, running);
        },
        new std::pair<Graphics *, Engine *>(&graphics, &engine),
        0,
        1);
#else
    bool running = true;
    while (running)
    {
        loop(graphics, engine, running);
    }
#endif

    Logger::shutdown();
    return 0;
}
