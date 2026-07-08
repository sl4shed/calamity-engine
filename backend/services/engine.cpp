#include "engine.hpp"

#include <utility>

#include "audio.hpp"
#include "services.hpp"
#include "input/input.hpp"
#include "graphics/graphics.hpp"
#include "physics/physics.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#if TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

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
#ifdef PSP
    SetupCallbacks();
#endif
}

Engine::~Engine()
{
    // idk
}

void Engine::initialize() {
    for (auto &window : windows)
    {
        window->initialize();
    }

    Services::audio()->initialize();
}

void Engine::update()
{
#if TRACY_ENABLE
    ZoneScoped;
#endif

    // frame limiter
    if (maxFps > 0.0f) {
        const float targetFrameTime = 1.0f / maxFps;
        const Uint64 targetNS = static_cast<Uint64>(targetFrameTime * 1e9f);
        const Uint64 elapsed = SDL_GetPerformanceCounter() - now;
        const Uint64 elapsedNS = elapsed * 1000000000ULL / SDL_GetPerformanceFrequency();
        if (elapsedNS < targetNS) {
            SDL_DelayNS(targetNS - elapsedNS);
        }
    }

    last = now;
    now = SDL_GetPerformanceCounter();
    const float deltaTime = static_cast<float>(now - last) / SDL_GetPerformanceFrequency();

    Services::input()->update(deltaTime);
    for(auto &window : windows) {
        window->update(deltaTime);
    }

    accumulator += deltaTime;
    while (accumulator >= physicsTimestep) {
        Services::physics()->physicsUpdate(physicsTimestep);

        for (auto &window : windows)
        {
            window->physicsUpdate();
        }

        accumulator -= physicsTimestep;
    }
#if TRACY_ENABLE
    FrameMark;
#endif
}

void Engine::exit()
{

    for(auto &window : windows) {
        window->exit();
    }

    Services::exit();
    Logger::exit();

#ifdef PSP
    sceKernelSleepThread();
    sceKernelExitGame();
#endif
}

SDL_AppResult processInput(SDL_Event *event) {
    
}

void Engine::render(Graphics &graphics)
{
#if TRACY_ENABLE
    ZoneScoped;
#endif

    for(auto &window : windows) {
        window->render(graphics, this);
    }
}

int Engine::appendWindow(std::shared_ptr<Window> window) {
    int id = SDL_GetWindowID(window->window);
    window->id = id;
    windows.emplace_back(window);
    return id;
}

std::vector<std::shared_ptr<Window>> Engine::getWindows() {
    return windows;
}

void Engine::removeWindow(int id) {
    // TODO: implement
}

std::shared_ptr<Window> Engine::getWindow(int id) {
    for (auto &window : windows) {
        if (window->id == id) return window;
    }
    return nullptr;
}