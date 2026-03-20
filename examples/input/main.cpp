#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"

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

    std::shared_ptr<Node> bgNode = std::make_shared<Node>();
    std::shared_ptr<Sprite> bgSprite = std::make_shared<Sprite>();
    bgSprite->texture = Texture("assets/dash.png");
    bgNode->addComponent(bgSprite);
    engine.root.addChild(bgNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
    sprite->texture = Texture("assets/glass.png");
    node->addComponent(sprite);

    node->addComponent(std::make_shared<InputScript>());
    engine.root.addChild(node);
    engine.root.initialize();

    while(!input.shouldQuit) {
        engine.update();
        engine.render(graphics);
    }
}
