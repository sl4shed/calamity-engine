#include "services.hpp"

Graphics *Services::s_graphics = nullptr;
Engine *Services::s_engine = nullptr;
Input *Services::s_input = nullptr;
InputRegistry* Services::s_inputRegistry = nullptr;
Physics* Services::s_physics = nullptr;

void Services::init(Graphics *graphics, Engine *engine, Input *input, InputRegistry *inputRegistry, Physics *physics)
{
    s_graphics = graphics;
    s_engine = engine;
    s_input = input;
    s_inputRegistry = inputRegistry;
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

void Services::shutdown()
{
    s_graphics = nullptr;
    s_engine = nullptr;
    s_input = nullptr;
    s_inputRegistry = nullptr;
    s_physics = nullptr;
}
