#include "services.hpp"

Graphics *Services::s_graphics = nullptr;
Engine *Services::s_engine = nullptr;

void Services::init(Graphics *graphics, Engine *engine)
{
    s_graphics = graphics;
    s_engine = engine;
}

Graphics *Services::graphics()
{
    return s_graphics;
}

Engine *Services::engine()
{
    return s_engine;
}

void Services::shutdown()
{
    s_graphics = nullptr;
    s_engine = nullptr;
}
