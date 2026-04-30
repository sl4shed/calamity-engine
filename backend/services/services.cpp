#include "services.hpp"
#include "backend/services/audio.hpp"
#include "backend/services/physics/physics.hpp"
#include "backend/services/graphics.hpp"

Graphics *Services::s_graphics = nullptr;
Engine *Services::s_engine = nullptr;
Input *Services::s_input = nullptr;
InputRegistry* Services::s_inputRegistry = nullptr;
Physics* Services::s_physics = nullptr;
Audio* Services::s_audio = nullptr;

void Services::init(Graphics *graphics, Physics *physics, Engine *engine, Input *input, InputRegistry *inputRegistry, Audio *audio)
{
    s_graphics = graphics;
    s_engine = engine;
    s_input = input;
    s_inputRegistry = inputRegistry;
    s_audio = audio;
    s_physics = physics;
}

Graphics *Services::graphics()
{
    return s_graphics;
}

Engine *Services::engine()
{
    return s_engine;
}

Input *Services::input()
{
    return s_input;
}

InputRegistry *Services::inputRegistry()
{
    return s_inputRegistry;
}

Physics *Services::physics() {
    return s_physics;
}

Audio *Services::audio() {
    return s_audio;
}

void Services::exit()
{
    s_audio->exit();
    s_physics->exit();
    s_graphics->exit();

    s_graphics = nullptr;
    s_engine = nullptr;
    s_input = nullptr;
    s_inputRegistry = nullptr;
    s_audio = nullptr;
    s_physics = nullptr;
}
