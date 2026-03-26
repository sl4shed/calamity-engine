#include "services.hpp"

Graphics *Services::s_graphics = nullptr;
Engine *Services::s_engine = nullptr;
Input *Services::s_input = nullptr;
InputRegistry* Services::s_inputRegistry = nullptr;

void Services::init(Graphics *graphics, Engine *engine, Input *input, InputRegistry *inputRegistry)
{
    s_graphics = graphics;
    s_engine = engine;
    s_input = input;
    s_inputRegistry = inputRegistry;
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

void Services::shutdown()
{
    s_graphics = nullptr;
    s_engine = nullptr;
    s_input = nullptr;
    s_inputRegistry = nullptr;
}
