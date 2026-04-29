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
#include "backend/core/ui/label.hpp"
#include "playerScript.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
PSP_MODULE_INFO("texture", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

static Physics physics = Physics({0.0f, 98.1f * 3});
static Engine engine = Engine("Platformer Example");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

int main() {
    Logger::init();

    graphics = new Graphics({480, 272}, "Platformer Example", RenderLogicalPresentation::LETTERBOX, Color(0x00b7ff));
    Input input;
    InputRegistry inputRegistry;
    Audio audio;

    Services::init(graphics, &physics, &engine, &input, &inputRegistry, &audio);

    // input stuff

    inputRegistry.addAction("left", 0.2f);
    auto leftEvent = std::make_unique<InputEventControllerButton>();
    leftEvent->device = 0;
    leftEvent->button = ControllerButton::DPAD_LEFT;
    inputRegistry.actionAddEvent("left", std::move(leftEvent));

    auto leftEventK = std::make_unique<InputEventKey>();
    leftEventK->pressed = true;
    leftEventK->scancode = Keycode::A;
    inputRegistry.actionAddEvent("left", std::move(leftEventK));

    inputRegistry.addAction("right", 0.2f);
    auto rightEvent = std::make_unique<InputEventControllerButton>();
    rightEvent->device = 0;
    rightEvent->button = ControllerButton::DPAD_RIGHT;
    inputRegistry.actionAddEvent("right", std::move(rightEvent));

    auto rightEventK = std::make_unique<InputEventKey>();
    rightEventK->pressed = true;
    rightEventK->scancode = Keycode::D;
    inputRegistry.actionAddEvent("right", std::move(rightEventK));

    inputRegistry.addAction("up", 0.2f);
    auto upEvent = std::make_unique<InputEventControllerButton>();
    upEvent->device = 0;
    upEvent->button = ControllerButton::DPAD_UP;
    inputRegistry.actionAddEvent("up", std::move(upEvent));

    auto upEventK = std::make_unique<InputEventKey>();
    upEventK->pressed = true;
    upEventK->scancode = Keycode::W;
    inputRegistry.actionAddEvent("up", std::move(upEventK));

    // node stuff

    std::shared_ptr<Node> cameraNode = std::make_shared<Node>();
    cameraNode->transform.setScale({3, 3});
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    cameraNode->addComponent(camera);

    // player node
    auto node = std::make_shared<Node>("playerNode");
    node->transform.position = {0, -40};
    auto sprite = std::make_shared<AnimatedSprite>();

    // player animations
    auto idle = Animation("idle", 5, Vector2{13, 19} * 4, true, true, TextureScaling::PIXELART);
    idle.texturePath = "res://assets/knight.png";
    idle.addFrames(
        Frame(Rect{{9, 9}, {13, 19}}, {0.5, 0.5}),
        Frame(Rect{{41, 9}, {13, 19}}, {0.5, 0.5}),
        Frame(Rect{{73, 9}, {13, 19}}, {0.5, 0.5}),
        Frame(Rect{{105, 9}, {13, 19}}, {0.5, 0.5})
    );
    sprite->addAnimation(idle);

    // rest of player node code
    node->addComponent(sprite);
    auto shape = std::make_shared<BoxShape>(Vector2{13, 19} * 4);
    Material mat = Material();
    mat.density = 10.0f;
    shape->applyMaterial(mat);
    auto rbody = std::make_shared<RigidBody>(shape);
    rbody->fixRotation(true);

    node->addComponent(rbody);
    node->addComponent(std::make_shared<PlayerScript>());
    node->addComponent(camera);

    // other nodes
    auto map = std::make_shared<Node>("map");
    map->transform.position = {0, 0};
    auto tilemap = std::make_shared<Tilemap>("res://assets/world_tileset.png", TextureScaling::PIXELART, Vector2{64, 64});
    // 896x192
    tilemap->addTiles(
        // grass
        Tile(Vector2{-7, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-6, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-5, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-4, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-3, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-2, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{-1, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{0, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{1, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{2, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{3, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{4, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{5, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{6, 0}, Rect{{0, 0}, {16, 16}}),
        Tile(Vector2{7, 0}, Rect{{0, 0}, {16, 16}}),

        // dirt layer 1
        Tile(Vector2{-7, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-6, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-5, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-4, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-3, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-2, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-1, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{0, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{1, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{2, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{3, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{4, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{5, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{6, 1}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{7, 1}, Rect{{0, 16}, {16, 16}}),

        // dirt layer 2
        Tile(Vector2{-7, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-6, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-5, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-4, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-3, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-2, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{-1, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{0, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{1, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{2, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{3, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{4, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{5, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{6, 2}, Rect{{0, 16}, {16, 16}}),
        Tile(Vector2{7, 2}, Rect{{0, 16}, {16, 16}})
    );
    auto mshape = std::make_shared<BoxShape>(Vector2{896.0f, 192.0f}, Vector2{0.5f, 1.0f});
    map->addComponent(std::make_shared<StaticBody>(mshape));
    map->addComponent(std::make_shared<ShapeSprite>(mshape));


    map->addComponent(tilemap);

    engine.root.addChild(map);
    engine.root.addChild(node);

    engine.initialize();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit) {
        loop();
    }
    engine.shutdown();
    delete graphics;
#endif

    return 0;
}
