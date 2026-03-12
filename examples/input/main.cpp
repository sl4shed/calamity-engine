#include "backend/engine.hpp"
#include "backend/node.hpp"
#include "backend/definitions.hpp"
#include "backend/input.hpp"
#include "backend/audio.hpp"
#include "backend/graphics.hpp"
#include "backend/logger.hpp"
#include "backend/services.hpp"
#include "backend/components.hpp"

#include "inputScript.hpp"

int main() {
    Logger::init();
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics();
    Services::init(&graphics, &engine, &input);

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    engine.root.addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
    sprite->texture = Texture("assets/glass.png");
    node->addComponent(sprite);

    node->addComponent(std::make_shared<InputScript>());
    engine.root.addChild(node);
    engine.root.initialize();

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        engine.update();
        engine.render(graphics);
    }
}
