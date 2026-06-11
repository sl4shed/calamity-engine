#include "node.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/logger.hpp"
#include "../../services/input/input.hpp"
#include "../../services/graphics/graphics.hpp"
#include "../ui/label.hpp"
#include <iostream>
#include <cmath>

#if TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

Node::Node(const std::string &_name) : name(_name), parent(nullptr), window(nullptr) {}

Node::~Node()
{
    for (const auto &child : children)
        child->parent = nullptr;
    children.clear();
    components.clear();
}

void Node::free()
{
    if (parent)
    {
        parent->removeChild(shared_from_this());
    }
    else
    {
        Logger::warn("Trying to free a node {} with no parent.", name);
    }
}

void Node::addChild(std::shared_ptr<Node> child)
{
    if (!child)
        return;

    child->parent = this;
    child->window = this->window;
    children.push_back(child);
}

void Node::removeChild(std::shared_ptr<Node> child)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i] == child)
        {
            children[i]->parent = nullptr;
            children[i]->exit();
            children.erase(children.begin() + i);
            return;
        }
    }
}

void Node::addComponent(std::shared_ptr<Component> component)
{
    if (!component)
        return;
    component->setNode(this);
    components.push_back(component);
}

void Node::removeComponent(std::shared_ptr<Component> component)
{
    for (size_t i = 0; i < components.size(); ++i)
    {
        if (components[i] == component)
        {
            components.erase(components.begin() + i);
            return;
        }
    }
}

void Node::render(Graphics &graphics, Engine *engine, std::shared_ptr<Window> window) const
{
#if TRACY_ENABLE
    ZoneScoped;
#endif

    for(size_t i = 0; i < this->components.size(); i++) {
        components[i]->render(window);
    }

    for (size_t i = 0; i < this->children.size(); i++)
    {
        children[i]->render(graphics, engine, window);
    }
}

void Node::update(float deltaTime)
{
    if (parent)
    {
        globalTransform = parent->globalTransform.applyTo(transform);
    }
    else
    {
        globalTransform.transformation = transform.transformation;
        globalTransform.position = transform.position;
    }

    for (const auto &component : components)
    {
        component->update(deltaTime);
    }

    for (const auto &i : children)
    {
        i->update(deltaTime);
    }
}

void Node::exit() const
{
    for (auto &i : children)
    {
        i->exit();
    }

    for (const auto &component : components)
    {
        component->exit();
    }
}

void Node::physicsUpdate() const
{
    for (const auto &component : components)
    {
        component->physicsUpdate();
    }

    for (const auto &i : children)
    {
        i->physicsUpdate();
    }
}

void Node::initialize()
{
    if (parent)
    {
        globalTransform = parent->globalTransform.applyTo(transform);
    }
    else
    {
        globalTransform.transformation = transform.transformation;
        globalTransform.position = transform.position;
    }

    for (const auto &i : children)
    {
        i->initialize();
    }

    for (const auto &component : components)
    {
        component->initialize();
    }
}

void Node::input(InputEvent &event)
{
    for (const auto &i : children)
    {
        i->input(event);
    }

    for (const auto &component : components)
    {
        component->input(event);
    }
}

void Node::postLoad()
{
    Logger::debug("POST LOAD POST LOAD NODE: {}", name);

    for (const auto &i : children)
    {
        i->postLoad();
    }

    for (const auto &component : components)
    {
        component->postLoad();
    }
}

std::shared_ptr<Node> Node::getChild(std::string name)
{
    for (auto &i : children)
    {
        if (i->name == name)
        {
            return i;
        }
    }
    return nullptr;
}

std::shared_ptr<Node> Node::getChildByIndex(const int index)
{
    if (index < 0 || index >= children.size())
    {
        return nullptr;
    }
    return children[index];
}

std::shared_ptr<Component> Node::getComponentByIndex(const int index)
{
    if (index < 0 || index >= components.size())
    {
        return nullptr;
    }
    return components[index];
}

Node *Node::getOwner()
{
    if (parent)
    {
        return parent->getOwner();
    }

    return this;
}

std::shared_ptr<Window> Node::getWindow()
{
    if (window)
    {
        return window;
    }
    else if (parent)
    {
        Node *root = getOwner();
        return root->getWindow();
    }

    return nullptr;
}

void Node::setWindow(std::shared_ptr<Window> win)
{
    if (!win)
        return;
    this->window = win;
}