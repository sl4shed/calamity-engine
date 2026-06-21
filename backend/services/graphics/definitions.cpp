#include "graphics.hpp"
#include "definitions.hpp"
#include "../../utils/logger.hpp"
#include "../../core/node/node.hpp"
#include "../../core/definitions.hpp"
#include "../../core/node/node.hpp"
#include "../../core/node/node.hpp"
#include "../../core/node/components.hpp"

#if TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

Window::Window(std::string name, Rect dimensions, RenderLogicalPresentation presentation, WindowFlags flags, Color clearColor, bool fullscreen) : title(name), dimensions(dimensions), presentation(presentation), flags(flags), clearColor(clearColor), fullscreen(fullscreen)
{
    root = std::make_unique<Node>("root");
    root->transform.position = {0, 0};

    SDL_PropertiesID props = SDL_CreateProperties();

    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, name.c_str());
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, dimensions.size.x);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, dimensions.size.y);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, dimensions.position.x);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, dimensions.position.y);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, (SDL_WindowFlags)flags);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, fullscreen);

    this->window = SDL_CreateWindowWithProperties(props);
    if (!this->window)
    {
        Logger::error("Failed to create window: {}", SDL_GetError());
    }

    this->renderer = SDL_CreateRenderer(window, nullptr);
    if (!this->renderer)
    {
        Logger::error("Failed to create renderer: {}", SDL_GetError());
    }

    if (!SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(dimensions.size.x), static_cast<int>(dimensions.size.y), static_cast<SDL_RendererLogicalPresentation>(presentation)))
    {
        Logger::error("Failed to set render logical presentation: {}", SDL_GetError());
    }

    if (!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND))
    {
        Logger::error("Failed to set render draw blend mode: {}", SDL_GetError());
    }
}

void Window::setFullscreen(bool value)
{
    this->fullscreen = value;
    
    if(!SDL_SetWindowFullscreen(this->window, value)) {
        Logger::error("Failed to set fullscreen for window: {}", SDL_GetError());
        return;
    }
}

std::vector<WindowMode> Window::getSupportedWindowModes()
{
    std::vector<WindowMode> modes;
    SDL_DisplayID did = SDL_GetDisplayForWindow(this->window);

    if (!did)
    {
        Logger::error("Failed to get display for window: {}", SDL_GetError());
        return modes;
    }

    int count = 0;
    SDL_DisplayMode **sdlModes = SDL_GetFullscreenDisplayModes(did, &count);
    if (!sdlModes)
    {
        Logger::error("Failed to get display modes: {}", SDL_GetError());
        return modes;
    }

    for (int i = 0; i < count; i++)
    {
        const SDL_DisplayMode *m = sdlModes[i];
        WindowMode mode;
        mode.size = {static_cast<float>(m->w), static_cast<float>(m->h)};
        mode.scale = m->pixel_density;
        mode.refreshRate = m->refresh_rate;

        mode.refreshRateDenominator = m->refresh_rate_denominator;
        mode.refreshRateNumerator = m->refresh_rate_numerator;

        modes.push_back(mode);
    }

    SDL_free(sdlModes);
    return modes;
}

void Window::setFullscreenWindowMode(WindowMode mode)
{
    SDL_DisplayMode sdlMode = {};
    sdlMode.w = static_cast<int>(mode.size.x);
    sdlMode.h = static_cast<int>(mode.size.y);
    sdlMode.pixel_density = mode.scale;
    sdlMode.refresh_rate = mode.refreshRate;
    sdlMode.refresh_rate_numerator = mode.refreshRateNumerator;
    sdlMode.refresh_rate_denominator = mode.refreshRateDenominator;

    if (!SDL_SetWindowFullscreenMode(this->window, &sdlMode))
    {
        Logger::error("Failed to set fullscreen mode: {}", SDL_GetError());
    }
}

void Window::changeSize(Rect dim) {
    this->dimensions = dim;

    if(!SDL_SetWindowSize(this->window, dimensions.size.x, dimensions.size.y)) {
        Logger::error("Failed to set window size: {}", SDL_GetError());
        return;
    }

    if(!SDL_SetWindowPosition(this->window, dimensions.position.x, dimensions.position.y)) {
        Logger::error("Failed to set window position: {}", SDL_GetError());
        return;
    }
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Window::preRender() const
{
#if TRACY_ENABLED
    ZoneScoped;
#endif

    SDL_SetRenderDrawColor(this->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(this->renderer);
}

void Window::postRender() const
{
#if TRACY_ENABLE
    ZoneScoped;
#endif

    SDL_RenderPresent(this->renderer);
}

void Window::update(float deltaTime)
{
    root->update(deltaTime);
}

void Window::render(Graphics &graphics, Engine *engine)
{
#if TRACY_ENABLE
    ZoneScoped;
#endif

    preRender();

    root->render(graphics, engine, shared_from_this());

    postRender();
}

void Window::exit()
{
    root->exit();
    root->children.clear();
    root->components.clear();
}

void Window::initialize()
{
    root->setWindow(shared_from_this());
    root->initialize();
}

void Window::physicsUpdate()
{
    root->physicsUpdate();
}

void Window::resetLogicalPresentation()
{
    if (!SDL_SetRenderLogicalPresentation(renderer, static_cast<int>(dimensions.size.x), static_cast<int>(dimensions.size.y), static_cast<SDL_RendererLogicalPresentation>(presentation)))
    {
        Logger::error("Failed to set render logical presentation: {}", SDL_GetError());
    }
}

void Window::setActiveCamera(Camera *camera)
{
    if (activeCamera != nullptr)
    {
        activeCamera->active = false;
    }
    activeCamera = camera;
}

Camera *Window::getActiveCamera() const
{
    return activeCamera;
}

template <class Archive>
void Window::load(Archive &ar)
{
    ar(CEREAL_NVP(title), CEREAL_NVP(flags), CEREAL_NVP(presentation), CEREAL_NVP(dimensions), CEREAL_NVP(fullscreen), CEREAL_NVP(id), CEREAL_NVP(root), CEREAL_NVP(mode));

    for (auto &child : root->children)
    {
        child->setWindow(shared_from_this());
    }

    // TODO: idfk i don't even set fullscreen or anything and i don't know if im supposed to

    root->postLoad();
}
