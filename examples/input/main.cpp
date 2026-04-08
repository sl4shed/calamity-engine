#include "backend/services/engine.hpp"
#include "backend/core/node/node.hpp"
#include "backend/core/definitions.hpp"
#include "backend/services/input/input.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/graphics.hpp"
#include "backend/utils/logger.hpp"
#include "backend/services/services.hpp"
#include "backend/core/node/components.hpp"
#include "backend/services/physics/physics.hpp"
#include "inputScript.hpp"

#ifdef PSP
// if you want psp support you have to have the psp module info thing
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("texture", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

int main() {
    Logger::init();
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics({480, 272}, "Input Example", RenderLogicalPresentation::LETTERBOX, {0, 0, 0});
    InputRegistry inputRegistry = InputRegistry();
    Physics physics = Physics();
    Services::init(&graphics, &engine, &input, &inputRegistry, &physics);

    inputRegistry.addAction("left", 0.2f);
    auto leftEvent = std::make_unique<InputEventControllerMotion>();
    leftEvent->device = 0;
    leftEvent->axis = ControllerAxis::LEFT_X;
    leftEvent->motion = -1.0f;
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>();
    leftEventK->pressed = true;
    leftEventK->scancode = Keycode::A;
    inputRegistry.actionAddEvent("left", std::move(leftEventK));

    inputRegistry.addAction("right", 0.2f);
    auto rightEvent = std::make_unique<InputEventControllerMotion>();
    rightEvent->device = 0;
    rightEvent->axis = ControllerAxis::LEFT_X;
    rightEvent->motion = 1.0f;
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>();
    rightEventK->pressed = true;
    rightEventK->scancode = Keycode::D;
    inputRegistry.actionAddEvent("right", std::move(rightEventK));

    inputRegistry.addAction("up", 0.2f);
    auto upEvent = std::make_unique<InputEventControllerMotion>();
    upEvent->device = 0;
    upEvent->axis = ControllerAxis::LEFT_Y;
    upEvent->motion = 1.0f;
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>();
    upEventK->pressed = true;
    upEventK->scancode = Keycode::W;
    inputRegistry.actionAddEvent("up", std::move(upEventK));

    inputRegistry.addAction("down", 0.2f);
    auto downEvent = std::make_unique<InputEventControllerMotion>();
    downEvent->device = 0;
    downEvent->axis = ControllerAxis::LEFT_Y;
    downEvent->motion = -1.0f;
    inputRegistry.actionAddEvent("down", std::move(downEvent));

    auto downEventK = std::make_unique<InputEventKey>();
    downEventK->pressed = true;
    downEventK->scancode = Keycode::S;
    inputRegistry.actionAddEvent("down", std::move(downEventK));

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);
    engine.root.addChild(cameraNode);

    std::shared_ptr<Node> node = std::make_shared<Node>();
    node->transform.scale({4, 4});
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
    sprite->texture = Texture("assets/glass.png");
    node->addComponent(sprite);

    std::shared_ptr<Node> lnode = std::make_shared<Node>();
    std::shared_ptr<Label> label = std::make_shared<Label>("WASD/Left Joystick - Move the glass around");
    label->font->setSize(12);
    label->size = {100, 500};
    lnode->transform.position = {20, 20};
    lnode->addComponent(label);
    engine.root.addChild(lnode);

    node->addComponent(std::make_shared<InputScript>());
    engine.root.addChild(node);
    engine.initialize();

    while(!input.shouldQuit) {
        engine.update();
        engine.render(graphics);
    }

    engine.shutdown();
}
