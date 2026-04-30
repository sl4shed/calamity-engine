#include "engine.hpp"

#include <utility>

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

Engine::Engine(std::string _appName) : appName(std::move(_appName))
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
    const float deltaTime = static_cast<float>(now - last) / SDL_GetPerformanceFrequency();

    Services::input()->update(deltaTime);
    root.update(deltaTime);

    accumulator += deltaTime;
    while (accumulator >= physicsTimestep) {
        Services::physics()->physicsUpdate(physicsTimestep);
        root.physicsUpdate();
        accumulator -= physicsTimestep;
    }
}

void Engine::exit()
{
    root.exit();
    root.children.clear();
    root.components.clear();

    Services::exit();
    Logger::exit();

#ifdef PSP
    Logger::debug("exiting");
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

Camera *Engine::getActiveCamera() const
{
    return activeCamera;
}
