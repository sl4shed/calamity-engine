#pragma once

// Forward declarations
class Graphics;
class Engine;
class Input;

/**
 * Global services singleton for accessing engine classes.
 * Initialize in main() before creating any game objects.
 */
class Services
{
public:
    static void init(Graphics *graphics, Engine *engine, Input *input);

    static Graphics *graphics();
    static Engine *engine();
    static Input *input();

    static void shutdown();

private:
    static Graphics *s_graphics;
    static Engine *s_engine;
    static Input *s_input;
};
