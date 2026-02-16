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
#include "backend/services.hpp"
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>

#include "scripts/BirdScript.hpp"
#include "scripts/CameraScript.hpp"
#include <iostream>
#include <fstream>

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

CEREAL_REGISTER_ARCHIVE(cereal::JSONOutputArchive)
CEREAL_REGISTER_ARCHIVE(cereal::JSONInputArchive)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Script)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Sprite)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Camera)

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
    Services::init(&graphics, &engine);

    Logger::info("Calamity Engine v1.0");

    // bird 1
    std::shared_ptr<Node> bird = std::make_shared<Node>();
    bird->name = std::string("Bird");

    std::shared_ptr<Sprite> birdSprite = std::make_shared<Sprite>();
    birdSprite->texture = Texture("assets/flappy.png");
    birdSprite->visible = true;
    birdSprite->zIndex = 1;
    birdSprite->origin = {0.5, 0.5};
    bird->addComponent(birdSprite);

    bird->transform.position = {240, 136};
    std::shared_ptr<BirdScript> birdScript = std::make_shared<BirdScript>();
    bird->addComponent(birdScript);
    engine.root.addChild(bird);

    // camera node
    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    cameraNode->name = std::string("Main Camera");
    cameraNode->transform.position = {240, 136};
    std::shared_ptr<Camera> cameraComponent = std::make_shared<Camera>();
    cameraNode->addComponent(cameraComponent);
    engine.root.addChild(cameraNode);
    std::shared_ptr<CameraScript> cameraScript = std::make_shared<CameraScript>();
    cameraNode->addComponent(cameraScript);
    cameraComponent->setActive();

    // bird 2
    std::shared_ptr<Node> bird2 = std::make_shared<Node>();
    bird2->name = std::string("Bird 2");
    bird2->addComponent(birdSprite);

    bird2->transform.position = {200, 0};
    bird2->transform.scale({0.8, 0.8});
    bird->addChild(bird2);

    // bird 3
    std::shared_ptr<Node> bird3 = std::make_shared<Node>();
    bird3->name = std::string("Bird 3");
    bird3->addComponent(birdSprite);

    bird3->transform.position = {120, 0};
    bird3->transform.scale({0.6, 0.6});
    bird2->addChild(bird3);

    // script start functions
    cameraScript->start();
    birdScript->start();

    std::ofstream file("out.json");
    cereal::JSONOutputArchive archive(file);
    try
    {
        archive(engine.root);
    }
    catch (const cereal::Exception &e)
    {
        Logger::error("Cereal error: {}", e.what());
    }

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
