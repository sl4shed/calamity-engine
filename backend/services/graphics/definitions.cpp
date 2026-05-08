#include "graphics.hpp"
#include "definitions.hpp"
#include "../../utils/logger.hpp"
#include "../../core/node/node.hpp"
#include "../../core/definitions.hpp"
#include "../../core/node/node.hpp"
#include "../../core/node/node.hpp"
#include "../../core/node/components.hpp"

Window::Window(std::string name, Rect dimensions, RenderLogicalPresentation presentation, WindowFlags flags, Color clearColor, bool fullscreen) : title(title), dimensions(dimensions), presentation(presentation), flags(flags), clearColor(clearColor), fullscreen(fullscreen) {
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
}

Window::~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Window::preRender() const
{
    SDL_SetRenderDrawColor(this->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(this->renderer);
}

void Window::postRender() const
{
    SDL_RenderPresent(this->renderer);
}

void Window::update(float deltaTime) {
    root->update(deltaTime);
}

void Window::render(Graphics &graphics, Engine *engine) {
    preRender();

    root->render(graphics, engine, shared_from_this());

    postRender();
}

void Window::exit() {
    root->exit();
    root->children.clear();
    root->components.clear();
}

void Window::initialize() {
    root->setWindow(shared_from_this());
    root->initialize();
}

void Window::physicsUpdate() {
    root->physicsUpdate();
}

void Window::resetLogicalPresentation() {
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
