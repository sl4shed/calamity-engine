#include "engine.hpp"

#include "audio.hpp"
#include "services.hpp"
#include "input/input.hpp"
#include "graphics.hpp"
#include "physics/physics.hpp"

#ifdef PSP
#include <pspuser.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

// these callbacks are for that menu that pops up when u press HOME to exit the game
int SetupCallbacks(void)
{
    int thid = 0;
    // a bunch of random magic numbers and I don't know what they do
    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}
#endif

Engine::Engine(std::string _appName) : appName(_appName)
{
    root = Node();
    root.transform.position = {0, 0};

#ifdef PSP
    SetupCallbacks();
#endif
}

Engine::~Engine()
{
    // idk
}

void Engine::initialize() {
    root.initialize();
    Services::audio()->initialize();
}

void Engine::update()
{
    last = now;
    now = SDL_GetPerformanceCounter();
    float deltaTime = (float)(now - last) / SDL_GetPerformanceFrequency();

    accumulator += deltaTime;
    while (accumulator >= physicsTimestep) {
        root.physicsUpdate();
        Services::physics()->physicsUpdate(physicsTimestep);
        accumulator -= physicsTimestep;
    }

    Services::input()->update(deltaTime);
    root.update(deltaTime);
}

void Engine::shutdown()
{
    for (const auto child : root.children)
    {
        root.removeChild(child);
    }

#ifdef PSP
    sceKernelSleepThread();
    sceKernelExitGame();
#endif
}

void Engine::render(Graphics &graphics)
{
    graphics.preRender();
    root.render(graphics, this);
    graphics.postRender();
}

void Engine::setActiveCamera(Camera *camera)
{
    if (activeCamera != nullptr)
    {
        activeCamera->active = false;
    }
    activeCamera = camera;
}

Camera *Engine::getActiveCamera()
{
    return activeCamera;
}