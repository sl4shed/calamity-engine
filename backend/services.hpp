#pragma once

// Forward declarations
class Graphics;
class Engine;

/**
 * Global services singleton for accessing engine systems.
 * Similar to Roblox Studio's global services.
 * Initialize in main() before creating any game objects.
 */
class Services
{
public:
    /**
     * Initialize all services. Call this once in main() before creating game objects.
     */
    static void init(Graphics *graphics, Engine *engine);

    /**
     * Get the global Graphics instance
     */
    static Graphics *graphics();

    /**
     * Get the global Engine instance
     */
    static Engine *engine();

    /**
     * Shutdown services (optional cleanup)
     */
    static void shutdown();

private:
    static Graphics *s_graphics;
    static Engine *s_engine;
};
