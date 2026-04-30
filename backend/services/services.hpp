#pragma once

// Forward declarations
class Graphics;
class Engine;
class Input;
class InputRegistry;
class Physics;
class Audio;

/**
 * # Services
 * "Singleton wrapper" for accessing core services basically anywhere.
 * 
 * Example usage:
 * ```
 * // Example for getting the graphics services
 * #include "backend/services.hpp"
 * #include "backend/graphics.hpp"
 * Graphics *graphics = Services::graphics();
 * ```
 *
 * For now, you can get the following services:
 *  - Graphics
 *  - Physics
 *  - Engine
 *  - Input
 *  - InputRegistry
 *  - Audio
 * 
 * It should be initialized in the main function with a pointer to every service. You can find more details in the [project template's main.cpp](https://github.com/sl4shed/calamity-engine-template/blob/main/main.cpp).
 */
class Services
{
public:
    static void init(Graphics *graphics, Physics *physics, Engine *engine, Input *input, InputRegistry *inputRegistry, Audio *audio);

    static Graphics *graphics();
    static Engine *engine();
    static Input *input();
    static InputRegistry *inputRegistry();
    static Physics *physics();
    static Audio *audio();

    static void exit();

private:
    static Graphics *s_graphics;
    static Engine *s_engine;
    static Input *s_input;
    static InputRegistry *s_inputRegistry;
    static Audio *s_audio;
    static Physics *s_physics;
};