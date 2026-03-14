#include "services.hpp"

Graphics *Services::s_graphics = nullptr;
Engine *Services::s_engine = nullptr;
Input *Services::s_input = nullptr;

void Services::init(Graphics *graphics, Engine *engine, Input *input)
{
    s_graphics = graphics;
    s_engine = engine;
    s_input = input;
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

void Services::shutdown()
{
    s_graphics = nullptr;
    s_engine = nullptr;
    s_input = nullptr;
}
