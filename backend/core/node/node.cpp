#include "node.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/logger.hpp"
#include "../../services/input/input.hpp"
#include "../../services/graphics.hpp"
#include "../ui/label.hpp"
#include <iostream>
#include <cmath>

Node::Node(std::string name)
    : parent(nullptr)
{
    this->name = name;
}

Node::~Node()
{
    for (auto &child : children)
        child->parent = nullptr;
    children.clear();
    components.clear();
}

void Node::free() {
    if (parent) {
        parent->removeChild(shared_from_this());
    } else {
        Logger::warn("Trying to free a node {} with no parent.", name);
    }
}

void Node::addChild(std::shared_ptr<Node> child)
{
    if (!child)
        return;

    child->parent = this;
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

    if (Sprite *sprite = dynamic_cast<Sprite *>(component.get()))
    {
        if (currentSprite)
        {
            // todo throw an error here...
        }
        currentSprite = sprite;
    }
}

void Node::removeComponent(std::shared_ptr<Component> component)
{
    for (size_t i = 0; i < components.size(); ++i)
    {
        if (components[i] == component)
        {
            if (Sprite *sprite = dynamic_cast<Sprite *>(components[i].get()))
            {
                if (currentSprite == sprite)
                    currentSprite = nullptr;
            }
            components.erase(components.begin() + i);
            return;
        }
    }
}

void Node::render(Graphics &graphics, Engine *engine)
{
    for (size_t i = 0; i < this->components.size(); i++)
    {
        Sprite *sprite = dynamic_cast<Sprite *>(components[i].get());
        if (sprite && sprite->visible)
        {
            graphics.renderSprite(*this, engine);
        }

        ShapeSprite *polygonSprite = dynamic_cast<ShapeSprite *>(components[i].get());
        if (polygonSprite && polygonSprite->visible) {
            graphics.renderPolygonSprite(*this, engine);
        }

        Label *label = dynamic_cast<Label *>(components[i].get());
        if(label && label->visible) {
            graphics.renderLabel(label);
        }
    }

    for (size_t i = 0; i < this->children.size(); i++)
    {
        children[i]->render(graphics, engine);
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

    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->update(deltaTime);
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->update(deltaTime);
    }
}

void Node::exit()
{
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->exit();
    }

    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->exit();
    }
}

void Node::physicsUpdate() {
    //Logger::debug("physicsUpdate - {}", name);
    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->physicsUpdate();
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->physicsUpdate();
    }
}

void Node::initialize()
{
    // copied from update
    if (parent)
    {
        globalTransform = parent->globalTransform.applyTo(transform);
    }
    else
    {
        globalTransform.transformation = transform.transformation;
        globalTransform.position = transform.position;
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->initialize();
    }

    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->initialize();
    }
}

void Node::input(InputEvent& event) {
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->input(event);
    }

    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->input(event);
    }
}

std::shared_ptr<Node> Node::getChild(std::string name)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->name == name)
        {
            return children[i];
        }
    }
    return nullptr;
}

std::shared_ptr<Node> Node::getChildByIndex(int index)
{
    if (index < 0 || index >= children.size())
    {
        return nullptr;
    }
    return children[index];
}

std::shared_ptr<Component> Node::getComponentByIndex(int index)
{
    if (index < 0 || index >= components.size())
    {
        return nullptr;
    }
    return components[index];
}