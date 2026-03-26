#pragma once

// Forward declarations
class Graphics;
class Engine;
class Input;
class InputRegistry;

/**
 * # Services class
 * Singleton for accessing core services basically anywhere. Use as follows:
 * ```
 * // Example for getting the graphics services
 * #include "backend/services.hpp"
 * #include "backend/graphics.hpp"
 * Graphics graphics = Services::graphics();
 * ```
 *
 * For now, you can get the following services:
 *  - Graphics
 *  - Engine
 *  - Input
 *  - InputRegistry
 */
class Services
{
public:
    static void init(Graphics *graphics, Engine *engine, Input *input, InputRegistry *inputRegistry);

    static Graphics *graphics();
    static Engine *engine();
    static Input *input();
    static InputRegistry *inputRegistry();

    static void shutdown();

private:
    static Graphics *s_graphics;
    static Engine *s_engine;
    static Input *s_input;
    static InputRegistry *s_inputRegistry;
};
